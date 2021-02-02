set(COMMON_H
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cptreewidget.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cvmatandqimage.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/geometryengine.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/objectcontroller.cpp
)

set(COMMON_SRC
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cptreewidget.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cvmatandqimage.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/geometryengine.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/objectcontroller.h
)

add_library(CMP_GUI_Common STATIC ${COMMON_H} ${COMMON_SRC})

target_include_directories(CMP_GUI_Common PRIVATE
  ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
  ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/qpropertypages
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
  ${OpenCV_INCLUDE_DIRS}
  )

target_link_libraries(CMP_GUI_Common CMP_Compressonator Qt5::Widgets ${OpenCV_LIBS})

set_target_properties(CMP_GUI_Common PROPERTIES
    AUTOMOC ON
    FOLDER "Libs")
