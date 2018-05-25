#########
# CMAKE #
#########

cmake_minimum_required(VERSION 2.6.2)

project(ls_system)

## set compilation flags
set(CMAKE_C_FLAGS "-W -Wall -pedantic -std=c99 -std=gnu99 -g")

## set the target name and source
add_executable(
    lss
    src/main.c
    src/get_info_api.c
)

## set the needed library
find_package(X11 REQUIRED)
target_link_libraries(lss X11)

