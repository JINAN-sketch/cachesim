include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(cachesim_tests
    test_address_decomposition.cpp
    test_lru_policy.cpp
    test_cache.cpp
    test_memory_hierarchy.cpp
    test_memory_hierarchy_deep.cpp
)
target_link_libraries(cachesim_tests PRIVATE cachesim_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cachesim_tests)