add_library(CMP_GUI_BIN_QPropertyPages INTERFACE)

target_sources(CMP_GUI_BIN_QPropertyPages INTERFACE
    qtbuttonpropertybrowser.cpp
    qteditorfactory.cpp
    qtgroupboxpropertybrowser.cpp
    qtpropertybrowser.cpp
    qtpropertybrowserutils.cpp
    qtpropertymanager.cpp
    qttreepropertybrowser.cpp
    qtvariantproperty.cpp
)

target_sources(CMP_GUI_BIN_QPropertyPages INTERFACE
    qtbuttonpropertybrowser.h
    qteditorfactory.h
    qtgroupboxpropertybrowser.h
    qtpropertybrowser.h
    qtpropertybrowserutils_p.h
    qtpropertymanager.h
    qttreepropertybrowser.h
    qtvariantproperty.h
)

target_include_directories(CMP_GUI_BIN_QPropertyPages INTERFACE

    ./
)

