add_executable(test_hash test_hash.c ../hash.c)
target_link_libraries(test_hash unity)
add_test(test_hash test_hash)
