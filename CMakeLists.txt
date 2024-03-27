cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

set(PROJECT_NAME font_render)
project(${PROJECT_NAME} C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-std=gnu99")

cmake_policy(SET CMP0069 NEW)

find_package(Threads REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -s -Os -ffunction-sections -Wl,--gc-sections -DNDEBUG -Wextra -Wno-builtin-macro-redefined")
else()
    set (CMAKE_BUILD_TYPE "Debug")
    if(BUILD_SHARED_LIBS)
        set(CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS} -O1 -g -fsanitize=address -fno-omit-frame-pointer -Wno-builtin-macro-redefined")
    endif()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(
    -DCONFIG_FONT_FIXED_WIDTH_HEIGHT=1
    -DCONFIG_FONT_ENC=1
    -DCONFIG_BPP=2
)

include_directories(
    src
)

add_executable(${PROJECT_NAME}
    src/main.c
    src/render.c
    src/utf8.c
    src/monaco32_fixedsize_rawbb_2bpp.c
)

target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    dl gcc_s)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/bin)

set_target_properties(${PROJECT} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
