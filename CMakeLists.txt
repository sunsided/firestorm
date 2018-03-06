cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

# Set default compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g")

# Set module paths for extra functionality.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Required libraries
include(LibraryBoost)
include(LibraryGperftools)
include(LibraryGTest)
include(LibrarySpdlog)
include(LibraryCLI11)
include(LibraryThreads)
include(LibraryConcurrentqueue)

# Options
include(OptionLto)
include(OptionCcache)
include(OptionSimd)
include(OptionFastmath)
include(OptionOpenMP)

# Targets that we develop here
add_subdirectory(src)

# Print a summary of the selected options.
include(FeatureSummary)
feature_summary(WHAT ALL)