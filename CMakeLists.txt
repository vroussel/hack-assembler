cmake_minimum_required(VERSION 3.31)
project(HackAssembler LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
include(CTest)
if(BUILD_TESTING)
    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG
            cbcd08fa7de711053a3deec6339ee89cad5d2697 #2.6.1
        FIND_PACKAGE_ARGS NAMES unity
    )
    FetchContent_MakeAvailable(unity)
    target_compile_definitions(unity PUBLIC UNITY_INCLUDE_DOUBLE)
    target_compile_definitions(unity PUBLIC UNITY_DOUBLE_PRECISION=1e-12)
endif()

add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE -Wall -Wextra)

add_subdirectory(src)
