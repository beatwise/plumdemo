cmake_minimum_required(VERSION 3.7.2)

project(plumdemo)

include(${external_libs})


add_subdirectory("host")
add_subdirectory("plugin")

