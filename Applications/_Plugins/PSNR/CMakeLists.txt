# =======================================================================

add_library(Plugin_PSNR)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../Common)

target_sources(Plugin_PSNR PRIVATE

	${LEGACY_LOCATION}/PSNR.cpp
	${LEGACY_LOCATION}/PSNR.h
	${LEGACY_LOCATION}/SSIM.cpp
	${LEGACY_LOCATION}/SSIM.h
)

target_include_directories(Plugin_PSNR PUBLIC
    ./
	${LEGACY_LOCATION}
)

target_link_libraries(Plugin_PSNR PRIVATE

	CompressonatorLIB

	Plugin_Common_TestReport
	ExtOpenCV
)

set_target_properties(Plugin_PSNR PROPERTIES FOLDER ${FOLDER_NAME})
