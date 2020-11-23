set(COMPONENTS_H
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/ac3dmeshanalysis.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acaboutdlg.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/accustomdockwidget.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/accustomgraphics.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acdiffimage.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acdockwidgettitlebar.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acexrtool.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acimageview.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acoglwidget.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acprogressanimationwidget.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acprogressdlg.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/actableimagedatamodel.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelcompare.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelconvert.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelview.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpcompressstatus.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpgenmips.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageanalysis.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimagecompare.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimagefiledata.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageloader.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimagepropertyview.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageview.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpnewproject.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpprojectdata.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpprojectview.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetapplicationoptions.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetcompressoptions.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetmeshoptions.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpstartuppage.h
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpwelcomepage.h
)

set(COMPONENTS_SRC
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/ac3dmeshanalysis.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acaboutdlg.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/accustomdockwidget.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/accustomgraphics.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acdiffimage.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acdockwidgettitlebar.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acexrtool.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acimageview.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acoglwidget.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acprogressanimationwidget.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/acprogressdlg.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/actableimagedatamodel.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelcompare.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelconvert.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cp3dmodelview.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpcompressstatus.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpgenmips.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageanalysis.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimagecompare.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageloader.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimagepropertyview.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageview.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpnewproject.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpprojectview.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetapplicationoptions.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetcompressoptions.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpsetmeshoptions.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpstartuppage.cpp
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpwelcomepage.cpp
)

add_library(CMP_GUI_Components STATIC ${COMPONENTS_H} ${COMPONENTS_SRC})

target_include_directories(CMP_GUI_Components PRIVATE
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/source
  ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/qpropertypages
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
  ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_mesh
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common
  ${PROJECT_SOURCE_DIR}/applications/_plugins/canalysis/analysis
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf/
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common/json/
  ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
  ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/rapidxml
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
  ${OpenCV_INCLUDE_DIRS}
  ${OpenEXR_INCLUDE_DIRS}
  ${draco_INCLUDE_DIRS}
  )

target_link_libraries(CMP_GUI_Components 
                      Qt5::Widgets 
                      Qt5::OpenGL 
                      Qt5::Qml 
                      Qt5::WebEngineWidgets
                      )

set_target_properties(CMP_GUI_Components PROPERTIES 
        AUTOMOC ON
        FOLDER "Libs")
