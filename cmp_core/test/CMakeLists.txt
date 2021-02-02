
add_executable(test_core)

# CTestDashboardTargets for catch2 (enable for ref only!)
# add_subdirectory(
#                 ${PROJECT_EXTERNAL_LIBDIR}/catch2
#                 ${PROJECT_EXTERNAL_LIBDIR}/catch2/bin
#                 )

target_sources(test_core
                PRIVATE
                testsmain.cpp
                compressonatortests.cpp
                compressonatortests.h
                blockconstants.h
                 ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.cpp
                 ${PROJECT_SOURCE_DIR}/applications/_plugins/common/utilfuncs.h
                )

target_include_directories(test_core PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders
    ${PROJECT_SOURCE_DIR}/cmp_core/source 
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/catch2
    )

target_link_libraries(test_core 
                CMP_Core
                )

set_target_properties(test_core 
    PROPERTIES FOLDER "Tests"
    )
