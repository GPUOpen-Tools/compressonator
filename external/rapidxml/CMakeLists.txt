message(STATUS "++++++++++++++++++ External-RAPIDXML" )


set(RAPIDXML_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/rapidxml/)

add_library(ExtRapidXML INTERFACE)

target_include_directories(ExtRapidXML INTERFACE

    ${RAPIDXML_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
)
