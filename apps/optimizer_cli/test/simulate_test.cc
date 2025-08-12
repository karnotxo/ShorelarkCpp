#include <catch2/catch_test_macros.hpp>
#include <optimizer/simulate.hpp>
#include <neural-network/network.hpp>

using namespace shorelark;

TEST_CASE("Simulate can run a simulation", "[simulate]") {
    // Create a simple network template
    NeuralNetwork network({2, 3, 1}); // 2 inputs, 3 hidden, 1 output
    
    // Simple fitness function that always returns a constant
    auto fitness_fn = [](const NeuralNetwork&) { return 1.0f; };
    
    // Configure simulation
    Simulate::Config config;
    config.iterations = 5;
    config.optimizer_config.population_size = 10;
    
    Simulate simulator(config);
    
    // Run simulation
    auto stats = simulator.run(network, fitness_fn);
    
    // Check results
    REQUIRE(stats.stats_history.size() == config.iterations);
    REQUIRE(stats.final_population.size() == config.optimizer_config.population_size);
    CHECK(stats.best_individual_idx < config.optimizer_config.population_size);
}

TEST_CASE("Simulate tracks fitness improvement", "[simulate]") {
    NeuralNetwork network({1, 1}); // Simple 1-1 network
    
    // Fitness function that improves over time
    size_t generation = 0;
    auto fitness_fn = [&generation](const NeuralNetwork&) { 
        return static_cast<float>(generation++); 
    };
    
    Simulate::Config config;
    config.iterations = 3;
    config.optimizer_config.population_size = 5;
    
    Simulate simulator(config);
    
    auto stats = simulator.run(network, fitness_fn);
    
    // Check that fitness improves over time
    REQUIRE(stats.stats_history.size() == 3);
    CHECK(stats.stats_history[0].max_fitness < stats.stats_history[1].max_fitness);
    CHECK(stats.stats_history[1].max_fitness < stats.stats_history[2].max_fitness);
}

TEST_CASE("Simulate preserves network architecture", "[simulate]") {
    std::vector<size_t> architecture = {3, 4, 2};
    NeuralNetwork network(architecture);
    
    auto fitness_fn = [](const NeuralNetwork&) { return 0.0f; };
    
    Simulate::Config config;
    config.iterations = 1;
    config.optimizer_config.population_size = 5;
    
    Simulate simulator(config);
    
    auto stats = simulator.run(network, fitness_fn);
    
    // Check that all networks in final population have same architecture
    for (const auto& net : stats.final_population) {
        auto layers = net->get_layer_sizes();
        REQUIRE(layers == architecture);
    }
} 