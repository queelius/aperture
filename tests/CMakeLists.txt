# Test configuration
include(CTest)

# Test helper library
add_library(test_helpers INTERFACE)
target_include_directories(test_helpers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Helper macro for adding tests
macro(add_aperture_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE aperture_core test_helpers)
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES 
        TIMEOUT 10
        LABELS "unit"
    )
endmacro()

# Unit tests
add_aperture_test(test_arithmetic)
add_aperture_test(test_let_bindings)
add_aperture_test(test_conditionals)
add_aperture_test(test_lambda)
add_aperture_test(test_partial_eval)
add_aperture_test(test_error_handling)
add_aperture_test(test_complex_programs)
add_aperture_test(test_special_forms)

# Integration tests
add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration PRIVATE aperture_core test_helpers)
add_test(NAME test_integration COMMAND test_integration)
set_tests_properties(test_integration PROPERTIES 
    TIMEOUT 30
    LABELS "integration"
)

# Security tests
add_executable(test_security test_security.cpp)
target_link_libraries(test_security PRIVATE aperture_core test_helpers)
add_test(NAME test_security COMMAND test_security)
set_tests_properties(test_security PROPERTIES 
    TIMEOUT 20
    LABELS "security"
)

# Performance benchmarks
add_executable(test_performance test_performance.cpp)
target_link_libraries(test_performance PRIVATE aperture_core test_helpers)
add_test(NAME test_performance COMMAND test_performance)
set_tests_properties(test_performance PROPERTIES 
    TIMEOUT 60
    LABELS "performance"
)

# Add custom target for running only unit tests
add_custom_target(unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    DEPENDS test_arithmetic test_let_bindings test_conditionals test_lambda 
            test_partial_eval test_error_handling test_complex_programs
)

# Add custom target for running all tests with verbose output
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS test_arithmetic test_let_bindings test_conditionals test_lambda 
            test_partial_eval test_error_handling test_complex_programs
            test_integration test_security test_performance
)