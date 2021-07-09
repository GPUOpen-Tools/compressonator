
add_executable(cmp_testcore)

# CTestDashboardTargets for catch2 (enable for ref only!)
# add_subdirectory(
#                 ${PROJECT_EXTERNAL_LIBDIR}/catch2
#                 ${PROJECT_EXTERNAL_LIBDIR}/catch2/bin
#                 )

target_sources(cmp_testcore
                PRIVATE
                testsmain.cpp
                compressonatortests.cpp
                compressonatortests.h
                blockconstants.h
                 ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.cpp
                 ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.h
                )

target_include_directories(cmp_testcore PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders
    ${PROJECT_SOURCE_DIR}/cmp_core/source 
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/catch2
    )

target_link_libraries(cmp_testcore 
                CMP_Core
                )

set_target_properties(cmp_testcore PROPERTIES 
    FOLDER "Tests"
    OUTPUT_NAME "cmp_testcore" 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
    )
