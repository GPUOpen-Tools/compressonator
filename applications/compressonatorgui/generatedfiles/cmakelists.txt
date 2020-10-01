
add_library(CMP_GUI_BIN_GeneratedFiles INTERFACE)

target_sources(CMP_GUI_BIN_GeneratedFiles INTERFACE
    qteditorfactory.moc
    qtpropertymanager.moc
    qttreepropertybrowser.moc
)

target_link_libraries(CMP_GUI_BIN_GeneratedFiles INTERFACE

    ExtQt5OpenGL
    ExtQt5ForGUI
)

target_include_directories(CMP_GUI_BIN_GeneratedFiles INTERFACE

    ./
)

# set_target_properties(CMP_GUI_BIN_GeneratedFiles PROPERTIES FOLDER ${FOLDER_NAME})
