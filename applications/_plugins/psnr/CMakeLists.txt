# =======================================================================

add_library(Plugin_PSNR)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../common)

target_sources(Plugin_PSNR PRIVATE
    ${LEGACY_LOCATION}/psnr.cpp
    ${LEGACY_LOCATION}/psnr.h
    ${LEGACY_LOCATION}/ssim.cpp
    ${LEGACY_LOCATION}/ssim.h
)

target_include_directories(Plugin_PSNR PUBLIC
    ./
    ${LEGACY_LOCATION}
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
)

target_link_libraries(Plugin_PSNR PRIVATE

    CompressonatorLIB
    Plugin_Common_TestReport
    ExtOpenCV
)

set_target_properties(Plugin_PSNR PROPERTIES FOLDER ${FOLDER_NAME})
