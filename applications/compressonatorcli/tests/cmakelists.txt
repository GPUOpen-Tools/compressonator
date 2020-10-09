# Setup test environment
set(TESTS_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${TESTS_WORKING_DIR})
file(GLOB CMP_CLI_TEST_COMPONENTS "${CMAKE_CURRENT_LIST_DIR}/../../../runtime/*" LIST_DIRECTORIES true)
foreach(CMP_TEST_FILE ${CMP_CLI_TEST_COMPONENTS})
    get_filename_component(_CMP_TEST_FILE_NAME ${CMP_TEST_FILE} NAME)
    file(COPY ${CMP_TEST_FILE} DESTINATION ${TESTS_WORKING_DIR})
endforeach()

# Test definition helpers
macro(add_cmp_cli_test FORMAT)
    set(TEST_NAME CompressonatorCLI_${FORMAT})
    add_test(
        NAME ${TEST_NAME}
        COMMAND CompressonatorCLI-bin -fd ${FORMAT} ./images ./results_${TEST_NAME}
        WORKING_DIRECTORY ${TESTS_WORKING_DIR}
    )
    set(TEST_NAME CompressonatorCLI_${FORMAT}_mips)
    add_test(
        NAME ${TEST_NAME}
        COMMAND CompressonatorCLI-bin -fd ${FORMAT} -miplevels 4 ./images ./results_${TEST_NAME}
        WORKING_DIRECTORY ${TESTS_WORKING_DIR}
    )
    set(TEST_NAME CompressonatorCLI_${FORMAT}_block_rate)
    add_test(
        NAME ${TEST_NAME}
        COMMAND CompressonatorCLI-bin -fd ${FORMAT} -BlockRate 12x12 ./images ./results_${TEST_NAME}
        WORKING_DIRECTORY ${TESTS_WORKING_DIR}
    )
    set(TEST_NAME CompressonatorCLI_${FORMAT}_low_quality)
    add_test(
        NAME ${TEST_NAME}
        COMMAND CompressonatorCLI-bin -fd ${FORMAT} -Quality 0.01 ./images ./results_${TEST_NAME}
        WORKING_DIRECTORY ${TESTS_WORKING_DIR}
    )
endmacro()

# Tests all formats
add_cmp_cli_test(ARGB_16)
add_cmp_cli_test(ARGB_16F)
add_cmp_cli_test(ARGB_2101010)
add_cmp_cli_test(ARGB_32F)
add_cmp_cli_test(ARGB_8888)
add_cmp_cli_test(ASTC)
add_cmp_cli_test(ATC_RGB)
add_cmp_cli_test(ATC_RGBA_EXPLICIT)
add_cmp_cli_test(ATC_RGBA_INTERPOLATED)
add_cmp_cli_test(ATI1N)
add_cmp_cli_test(ATI2N_DXT5)
add_cmp_cli_test(ATI2N_XY)
add_cmp_cli_test(ATI2N)
add_cmp_cli_test(BC1)
add_cmp_cli_test(BC2)
add_cmp_cli_test(BC3)
add_cmp_cli_test(BC4)
add_cmp_cli_test(BC5)
add_cmp_cli_test(BC6H_SF)
add_cmp_cli_test(BC6H)
add_cmp_cli_test(BC7)
add_cmp_cli_test(DXT1)
add_cmp_cli_test(DXT3)
add_cmp_cli_test(DXT5_RBXG)
add_cmp_cli_test(DXT5_RGXB)
add_cmp_cli_test(DXT5_RXBG)
add_cmp_cli_test(DXT5_XGBR)
add_cmp_cli_test(DXT5_XGXR)
add_cmp_cli_test(DXT5_XRBG)
add_cmp_cli_test(DXT5)
add_cmp_cli_test(ETC_RGB)
add_cmp_cli_test(ETC2_RGB)
add_cmp_cli_test(ETC2_RGBA)
add_cmp_cli_test(ETC2_RGBA1)
add_cmp_cli_test(ETC2_SRGB)
add_cmp_cli_test(ETC2_SRGBA)
add_cmp_cli_test(ETC2_SRGBA1)
add_cmp_cli_test(GTC)
add_cmp_cli_test(R_16)
add_cmp_cli_test(R_16F)
add_cmp_cli_test(R_32F)
add_cmp_cli_test(R_8)
add_cmp_cli_test(RG_16)
add_cmp_cli_test(RG_16F)
add_cmp_cli_test(RG_32F)
add_cmp_cli_test(RG_8)
add_cmp_cli_test(RGB_888)
