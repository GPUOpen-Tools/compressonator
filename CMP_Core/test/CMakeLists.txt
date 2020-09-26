
add_executable(CMP_Core_Tests_BIN)

target_sources(CMP_Core_Tests_BIN 
    PRIVATE
        BlockConstants.h
        CompressonatorTests.cpp
        CompressonatorTests.h
        TestsMain.cpp
)

target_link_libraries(CMP_Core_Tests_BIN 
    PRIVATE 
        ExtCatch2 
        CMP_Core
        Plugin_Common_UtilFuncs
)

set_target_properties(CMP_Core_Tests_BIN PROPERTIES FOLDER Tests)

add_test(NAME CMP_Core_Tests
    COMMAND $<TARGET_FILE:CMP_Core_Tests_BIN>
)
