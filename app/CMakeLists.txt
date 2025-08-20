cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app LANGUAGES C)
zephyr_include_directories(${CMAKE_SOURCE_DIR}/../include)
target_sources(app PRIVATE src/main.c src/resources.c)

# Tell Zephyr to use our linker section file
zephyr_linker_sources(SECTIONS sections-rom.ld)
#zephyr_linker_sources(SECTIONS sections-html.ld)

message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}, CMAKE_C_COMPILER = ${CMAKE_C_COMPILER} ,CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

# Path to the HTML file
# set(INDEX_HTML ${CMAKE_SOURCE_DIR}/static/index.html)
# set(INDEX_HTML_OBJ ${CMAKE_CURRENT_BINARY_DIR}/index_html.o)

# Convert HTML to an object file
# add_custom_command(
#     OUTPUT ${INDEX_HTML_OBJ}
#     COMMAND ${CMAKE_OBJCOPY} --input-target=binary --output-target=elf32-littlearm --binary-architecture=arm
#         --rename-section .data=.rodata.index_html
#         ${INDEX_HTML} ${INDEX_HTML_OBJ}
# 
#     DEPENDS ${INDEX_HTML}
# )

# Link the object file into the final app
# target_sources(app PRIVATE ${INDEX_HTML_OBJ})

