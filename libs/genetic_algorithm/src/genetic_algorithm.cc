#include "genetic_algorithm/genetic_algorithm.h"

#include <algorithm>
#include <memory>
#include <tl/expected.hpp>
#include <utility>
#include <vector>

#include "genetic_algorithm/crossover.h"
#include "genetic_algorithm/mutation.h"
#include "genetic_algorithm/selection.h"

namespace cshorelark::genetic {}  // namespace cshorelark::genetic