# =======================================================================

add_library(Plugin_UserInterface)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../Common)

target_sources(Plugin_UserInterface PRIVATE

	${LEGACY_LOCATION}/UserInterface.h
	${LEGACY_LOCATION}/UserInterface.cpp
)

target_include_directories(Plugin_UserInterface PUBLIC
	./
	${LEGACY_LOCATION}
)

set_target_properties(Plugin_UserInterface PROPERTIES FOLDER ${FOLDER_NAME})
