

set(RAPIDXML_DIR ${CMAKE_CURRENT_LIST_DIR}/rapidxml-submodule)

add_library(ExtRapidXML INTERFACE)

target_include_directories(ExtRapidXML INTERFACE

    ${RAPIDXML_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
)
