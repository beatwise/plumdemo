cmake_minimum_required(VERSION 3.7.2)

project(demoplugin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM REQUIRED cairomm-1.0)



add_library(demoplugin SHARED
    src/library.cpp
    src/abcdwindow.cpp
    src/utils.cpp
	${abcd_path}/abcdgui.cpp

	src/demo-gain/gain.cpp
	src/demo-gain/gui.cpp

	src/demo-synth/synth.cpp
	src/demo-synth/gui.cpp
)


set_target_properties(demoplugin PROPERTIES PREFIX "")
set_target_properties(demoplugin PROPERTIES OUTPUT_NAME "demoplugin")

target_compile_options(demoplugin PRIVATE -fPIC -Wall)

target_include_directories(demoplugin
    PRIVATE
		${plum_path}
		${abcd_path}
		${tonic_path}/include/
		${CAIROMM_LIBRARY_DIRS}
		${CAIROMM_INCLUDE_DIRS}
)




target_link_libraries(demoplugin ${tonic_path}/lib/libTonicLib.a)

install(TARGETS demoplugin LIBRARY DESTINATION bin)
