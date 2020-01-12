
add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(QPropertyPages)

add_executable(compressonator-gui
  Source/cpMainComponents.cpp
  Source/cpMainComponents.h
  Source/main.cpp
  Source/qtIgnoreCompilerWarnings.h
  CompressonatorGUI.qrc
)

target_link_libraries(compressonator-gui
  CMP_Framework
  CMP_GUI_Components
  CMP_GUI_Common
  QPropertyPages
  PluginCommon
  GpuDecode
  gltf
  EXR
  KTX TGA ASTC
  Analysis
  Compressonator
  Qt5::Widgets Qt5::Qml Qt5::OpenGL Qt5::WebEngineWidgets Qt5::Xml
  ${OpenEXR_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${Boost_LIBRARIES}
  ${draco_LIBRARIES}
  Threads::Threads)

target_include_directories(compressonator-gui PRIVATE
  ${OpenCV_INCLUDE_DIRS}
  ${draco_INCLUDE_DIRS}
  ../../CMP_CompressonatorLib
  ../../CMP_Framework/Common/half
  Components
  QPropertyPages
  Common
  ../_Plugins/Common
  ../_Plugins/CAnalysis/Analysis
  ../_Plugins/Common/gltf/
  ../_Plugins/Common/json/
  ../_Libs/GPU_Decode
  ../_Libs/CMP_Mesh
  ../_Libs/CMP_MeshOptimizer
)

install(TARGETS compressonator-gui)
