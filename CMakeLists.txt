cmake_minimum_required(VERSION 3.21)
project(platformerGame LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY leveldata DESTINATION ${CMAKE_BINARY_DIR})

add_executable(platformerGame src/main.cpp)
target_link_libraries(platformerGame PRIVATE sfml-graphics)
target_link_libraries(platformerGame PRIVATE sfml-audio)
target_compile_features(platformerGame PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET platformerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:platformerGame> $<TARGET_FILE_DIR:platformerGame> COMMAND_EXPAND_LISTS)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

install(TARGETS platformerGame)
