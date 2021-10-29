cmake_minimum_required(VERSION 3.7.2)

project(plumhost)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
pkg_check_modules(JACK2 REQUIRED jack)

add_executable(plumhost
    src/plumhost.cpp
    src/plugincatalog.cpp
    src/pluginview.cpp
    src/controller.cpp
    src/audio.cpp
    src/engine.cpp
)

target_compile_options(plumhost PRIVATE -g -Wall )
set (CMAKE_EXE_LINKER_FLAGS -Wl,-rpath=.)

target_include_directories(plumhost
    PRIVATE
		${plum_path}
		${dylib_path}
		${GTKMM3_LIBRARY_DIRS}
		${GTKMM3_INCLUDE_DIRS}
		${JACK2_LIBRARY_DIRS}
		${JACK2_INCLUDE_DIRS}

)

target_link_libraries(plumhost ${GTKMM3_LIBRARIES} ${JACK2_LIBRARIES} -ldl)


install(TARGETS plumhost RUNTIME DESTINATION bin)


