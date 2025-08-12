#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json.hpp>
#include <optimizer/optimizer.hpp>
#include <random>
#include <sstream>

using namespace shorelark;
using Catch::Matchers::WithinAbs;

namespace {
// Helper function to create a test network
Network
create_test_network() {
    return Network(2, {3, 1});
}

// Helper function to create a test optimizer
Optimizer
create_test_optimizer(std::vector<OptLog> *logs = nullptr) {
    OptConfig config{.population_size = 10,
                     .mutation_rate = 0.1f,
                     .mutation_range = 0.5f,
                     .selection_count = 3};

    OptLogCallback log_callback =
        logs ? [logs](const OptLog &log) { logs->push_back(log); }
             : OptLogCallback{};

    return Optimizer(config, log_callback);
}

// Helper function for basic fitness evaluation
float
basic_fitness_fn(const Network &network) {
    std::vector<float> input{1.0f, 1.0f};
    auto output = network.compute(input);
    return 1.0f - std::abs(output[0] - 1.0f);
}
}   // namespace

TEST_CASE("Optimizer basic functionality", "[optimizer]") {
    auto template_network = create_test_network();
    std::vector<OptLog> logs;
    auto optimizer = create_test_optimizer(&logs);

    SECTION("Training produces valid results") {
        auto result = optimizer.train(template_network, basic_fitness_fn, 5);
        REQUIRE(result.has_value());

        const auto &networks = *result;
        REQUIRE(networks.size() == 10);   // population_size

        // Verify logs were generated
        REQUIRE(logs.size() == 5);   // One log per generation

        // Check log contents
        for (const auto &log : logs) {
            // Config should match what we set
            REQUIRE(log.config.population_size == 10);
            REQUIRE(log.config.mutation_rate == 0.1f);
            REQUIRE(log.config.mutation_range == 0.5f);
            REQUIRE(log.config.selection_count == 3);

            // Statistics should be valid
            REQUIRE(log.stats.min_fitness >= 0.0f);
            REQUIRE(log.stats.max_fitness <= 1.0f);
            REQUIRE(log.stats.min_fitness <= log.stats.max_fitness);
            REQUIRE(log.stats.avg_fitness >= log.stats.min_fitness);
            REQUIRE(log.stats.avg_fitness <= log.stats.max_fitness);
            REQUIRE(log.stats.median_fitness >= log.stats.min_fitness);
            REQUIRE(log.stats.median_fitness <= log.stats.max_fitness);
        }

        // Verify fitness improves over generations
        REQUIRE(logs.front().stats.max_fitness <=
                logs.back().stats.max_fitness);
    }
}

TEST_CASE("Optimizer configuration validation", "[optimizer]") {
    SECTION("Invalid population size") {
        OptConfig config{.population_size = 0};
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);
    }

    SECTION("Invalid mutation rate") {
        OptConfig config{.population_size = 10, .mutation_rate = -0.1f};
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);

        config.mutation_rate = 1.1f;
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);
    }

    SECTION("Invalid mutation range") {
        OptConfig config{.population_size = 10, .mutation_range = -0.1f};
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);
    }

    SECTION("Invalid selection count") {
        OptConfig config{.population_size = 10, .selection_count = 0};
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);

        config.selection_count = 11;   // Greater than population size
        REQUIRE_THROWS_AS(Optimizer(config), std::invalid_argument);
    }
}

TEST_CASE("Optimizer statistics calculation", "[optimizer]") {
    auto template_network = create_test_network();
    std::vector<OptLog> logs;
    auto optimizer = create_test_optimizer(&logs);

    SECTION("Even population size") {
        std::vector<float> fitnesses = {0.1f, 0.2f, 0.3f, 0.4f};
        auto stats = optimizer.calculate_statistics(fitnesses);

        REQUIRE_THAT(stats.min_fitness, WithinAbs(0.1f, 1e-6f));
        REQUIRE_THAT(stats.max_fitness, WithinAbs(0.4f, 1e-6f));
        REQUIRE_THAT(stats.avg_fitness, WithinAbs(0.25f, 1e-6f));
        REQUIRE_THAT(stats.median_fitness, WithinAbs(0.25f, 1e-6f));
    }

    SECTION("Odd population size") {
        std::vector<float> fitnesses = {0.1f, 0.2f, 0.3f};
        auto stats = optimizer.calculate_statistics(fitnesses);

        REQUIRE_THAT(stats.min_fitness, WithinAbs(0.1f, 1e-6f));
        REQUIRE_THAT(stats.max_fitness, WithinAbs(0.3f, 1e-6f));
        REQUIRE_THAT(stats.avg_fitness, WithinAbs(0.2f, 1e-6f));
        REQUIRE_THAT(stats.median_fitness, WithinAbs(0.2f, 1e-6f));
    }

    SECTION("Empty population") {
        std::vector<float> fitnesses;
        auto stats = optimizer.calculate_statistics(fitnesses);

        REQUIRE_THAT(stats.min_fitness, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(stats.max_fitness, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(stats.avg_fitness, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(stats.median_fitness, WithinAbs(0.0f, 1e-6f));
    }
}

TEST_CASE("Optimizer parallel processing", "[optimizer]") {
    auto template_network = create_test_network();
    std::vector<OptLog> logs;
    auto optimizer = create_test_optimizer(&logs);

    SECTION("Parallel fitness evaluation") {
        // Create a computationally intensive fitness function
        auto intensive_fitness_fn = [](const Network &network) {
            float sum = 0.0f;
            std::vector<float> input{1.0f, 1.0f};

            // Perform multiple evaluations to make it CPU intensive
            for (int i = 0; i < 1000; ++i) {
                auto output = network.compute(input);
                sum += output[0];
            }
            return 1.0f - std::abs(sum / 1000.0f - 1.0f);
        };

        // Train with parallel processing
        auto start = std::chrono::high_resolution_clock::now();
        auto result =
            optimizer.train(template_network, intensive_fitness_fn, 5);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        REQUIRE(result.has_value());
        REQUIRE(duration.count() > 0);   // Ensure some time has passed
    }
}

TEST_CASE("OptLog serialization", "[optimizer]") {
    OptLog log{.config = OptConfig{.population_size = 10,
                                   .mutation_rate = 0.1f,
                                   .mutation_range = 0.5f,
                                   .selection_count = 3},
               .context = OptContext{.generation = 5, .iteration = 2},
               .stats = OptStatistics{.min_fitness = 0.2f,
                                      .max_fitness = 0.8f,
                                      .avg_fitness = 0.5f,
                                      .median_fitness = 0.6f}};

    SECTION("JSON serialization") {
        nlohmann::json j = log;
        std::string serialized = j.dump();

        OptLog deserialized = nlohmann::json::parse(serialized);

        REQUIRE(deserialized.config.population_size ==
                log.config.population_size);
        REQUIRE(deserialized.config.mutation_rate == log.config.mutation_rate);
        REQUIRE(deserialized.config.mutation_range ==
                log.config.mutation_range);
        REQUIRE(deserialized.config.selection_count ==
                log.config.selection_count);

        REQUIRE(deserialized.context.generation == log.context.generation);
        REQUIRE(deserialized.context.iteration == log.context.iteration);

        REQUIRE_THAT(deserialized.stats.min_fitness,
                     WithinAbs(log.stats.min_fitness, 1e-6f));
        REQUIRE_THAT(deserialized.stats.max_fitness,
                     WithinAbs(log.stats.max_fitness, 1e-6f));
        REQUIRE_THAT(deserialized.stats.avg_fitness,
                     WithinAbs(log.stats.avg_fitness, 1e-6f));
        REQUIRE_THAT(deserialized.stats.median_fitness,
                     WithinAbs(log.stats.median_fitness, 1e-6f));
    }
}