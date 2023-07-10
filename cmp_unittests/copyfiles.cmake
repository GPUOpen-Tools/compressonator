
macro(cmp_unittests_copy_to_output source dest)
    cmp_copy_to_output(cmp_unittests ${source} ${dest})
endmacro()

set(ASSETS_PATH $<TARGET_FILE_DIR:cmp_unittests>)

file(GLOB_RECURSE TEST_DATA ${PROJECT_SOURCE_DIR}/cmp_unittests/test_data/*)

foreach(rsc ${TEST_DATA})
     file(RELATIVE_PATH asset ${PROJECT_SOURCE_DIR}/cmp_unittests/test_data ${rsc})
     cmp_unittests_copy_to_output(${rsc} ${ASSETS_PATH}/test_data/${asset})
 endforeach()
