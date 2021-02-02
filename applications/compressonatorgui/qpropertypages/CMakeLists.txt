set(Q_PROPERTY_PAGES_SRC
    qtbuttonpropertybrowser.cpp
    qteditorfactory.cpp
    qtgroupboxpropertybrowser.cpp
    qtpropertybrowser.cpp
    qtpropertybrowserutils.cpp
    qtpropertymanager.cpp
    qttreepropertybrowser.cpp
    qtvariantproperty.cpp
)

set(Q_PROPERTY_PAGES_H
    qtbuttonpropertybrowser.h
    qteditorfactory.h
    qtgroupboxpropertybrowser.h
    qtpropertybrowser.h
    qtpropertybrowserutils_p.h
    qtpropertymanager.h
    qttreepropertybrowser.h
    qtvariantproperty.h
)

add_library(CMP_GUI_QPropertyPages STATIC ${Q_PROPERTY_PAGES_H} ${Q_PROPERTY_PAGES_SRC})

target_link_libraries(CMP_GUI_QPropertyPages Qt5::Widgets)

set_target_properties(CMP_GUI_QPropertyPages PROPERTIES
        AUTOMOC ON
        FOLDER "Libs")
