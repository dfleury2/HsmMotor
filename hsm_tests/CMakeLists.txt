add_executable(entry_exit_actions
        entry_exit_actions.cpp
        orthogonal_regions.cpp
        )
target_link_libraries(entry_exit_actions PUBLIC simple-hsm::simple-hsm doctest::doctest)

add_test(NAME entry_exit_actions_test COMMAND entry_exit_actions)
