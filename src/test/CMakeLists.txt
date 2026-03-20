add_executable(test_hash test_hash.c ../hash.c)
target_link_libraries(test_hash unity project_warnings)
add_test(test_hash test_hash)

add_executable(test_symbols test_symbols.c ../symbols.c ../hash.c)
target_link_libraries(test_symbols unity project_warnings)
add_test(test_symbols test_symbols)

add_executable(
    test_parsing
    test_parsing.c
    ../symbols.c
    ../hash.c
    ../parsing/parsing.c
    ../parsing/a_instruction.c
    ../parsing/c_instruction.c
    ../parsing/label_declaration.c
)
target_link_libraries(test_parsing unity project_warnings)
add_test(test_parsing test_parsing)

add_executable(
    test_encoding
    test_encoding.c
    ../encoding.c
    ../symbols.c
    ../hash.c
)
target_link_libraries(test_encoding unity project_warnings)
add_test(test_encoding test_encoding)

add_executable(test_utils test_utils.c ../utils.c)
target_link_libraries(test_utils unity project_warnings)
add_test(test_utils test_utils)
