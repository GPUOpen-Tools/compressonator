set(COMMON_H
  cpTreeWidget.cpp
  cvmatandqimage.cpp
  geometryengine.cpp
  objectcontroller.cpp
)

set(COMMON_SRC
  cpTreeWidget.h
  cvmatandqimage.h
  geometryengine.h
  objectcontroller.h
)

add_library(CMP_GUI_Common STATIC ${COMMON_H} ${COMMON_SRC})

target_include_directories(CMP_GUI_Common PRIVATE
  ../../../CMP_Framework/Common/half
  ../../../CMP_CompressonatorLib
  ../../_Plugins/Common/
  ../QPropertyPages)

target_link_libraries(CMP_GUI_Common Qt5::Widgets ${OpenCV_LIBS})
