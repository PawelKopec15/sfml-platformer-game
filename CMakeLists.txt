cmake_minimum_required(VERSION 3.21)
project(randomProject2 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY leveldata DESTINATION ${CMAKE_BINARY_DIR})

add_executable(randomProject2 src/main.cpp)
target_link_libraries(randomProject2 PRIVATE sfml-graphics)
target_compile_features(randomProject2 PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET randomProject2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:randomProject2> $<TARGET_FILE_DIR:randomProject2> COMMAND_EXPAND_LISTS)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

install(TARGETS randomProject2)
