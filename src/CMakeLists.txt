add_subdirectory(test)
add_executable(
    hack-assembler
    main.c
    symbols.c
    hash.c
    parsing/parsing.c
    parsing/a_instruction.c
    parsing/c_instruction.c
    parsing/label_declaration.c
    encoding.c
    translate.c
    utils.c
)

target_link_libraries(hack-assembler PRIVATE project_warnings)
