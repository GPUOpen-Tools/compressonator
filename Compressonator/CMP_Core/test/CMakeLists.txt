cmake_minimum_required(VERSION 3.5)
project(CMP_Core_Tests)

add_executable(Tests TestsMain.cpp)
add_subdirectory(../../../Common/Lib/Ext/Catch2
                Common/Lib/Ext/Catch2/bin)
target_sources(Tests 
                PRIVATE
                CompressonatorTests.cpp
                CompressonatorTests.h
                BlockConstants.h
		../../Applications/_Plugins/Common/UtilFuncs.cpp
		../../Applications/_Plugins/Common/UtilFuncs.h
                )
target_link_libraries(Tests Catch2::Catch2 CMP_Core)
