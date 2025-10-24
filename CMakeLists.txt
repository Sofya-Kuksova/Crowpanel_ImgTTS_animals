# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(COMPONENTS "main")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(proj)

set(SPIFFS_IMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
spiffs_create_partition_image(spiffs ${SPIFFS_IMG_DIR} FLASH_IN_PROJECT)




