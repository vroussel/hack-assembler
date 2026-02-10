add_executable(test_hash test_hash.c ../hash.c)
target_link_libraries(test_hash unity)
add_test(test_hash test_hash)

add_executable(test_symbols test_symbols.c ../symbols.c ../hash.c)
target_link_libraries(test_symbols unity)
add_test(test_symbols test_symbols)
