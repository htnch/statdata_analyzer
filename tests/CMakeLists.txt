# Unit tests executable
add_executable(unit_tests unit_tests.c)
target_link_libraries(unit_tests statdata)

# System tests executable
add_executable(system_test system_test.c)
target_link_libraries(system_test statdata)
add_dependencies(system_test statdata_analyzer)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME system_tests COMMAND system_test)
