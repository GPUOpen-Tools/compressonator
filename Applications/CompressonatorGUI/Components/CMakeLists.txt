add_library(CMP_GUI_BIN_Components INTERFACE)

target_sources(CMP_GUI_BIN_Components INTERFACE
    ac3DMeshAnalysis.h
    acAboutDlg.h
    acCustomDockWidget.h
    acCustomGraphics.h
    acDiffImage.h
    acDockWidgetTitleBar.h
    acEXRTool.h
    acImageView.h
    acOGLWidget.h
    acProgressAnimationWidget.h
    acProgressDlg.h
    acTableImageDataModel.h
    cp3DModelCompare.h
    cp3DModelConvert.h
    cp3DModelView.h
    cpCompressStatus.h
    cpGenMips.h
    cpImageAnalysis.h
    cpImageCompare.h
    cpImageFileData.h
    cpImagePropertyView.h
    cpImageView.h
    cpNewProject.h
    cpProjectData.h
    cpProjectView.h
    cpSetApplicationOptions.h
    cpSetCompressOptions.h
    cpSetMeshOptions.h
    cpStartupPage.h
    cpWelcomePage.h
)

target_sources(CMP_GUI_BIN_Components INTERFACE
    ac3DMeshAnalysis.cpp
    acAboutDlg.cpp
    acCustomDockWidget.cpp
    acCustomGraphics.cpp
    acDiffImage.cpp
    acDockWidgetTitleBar.cpp
    acEXRTool.cpp
    acImageView.cpp
    acOGLWidget.cpp
    acProgressAnimationWidget.cpp
    acProgressDlg.cpp
    acTableImageDataModel.cpp
    cp3DModelCompare.cpp
    cp3DModelConvert.cpp
    cp3DModelView.cpp
    cpCompressStatus.cpp
    cpGenMips.cpp
    cpImageAnalysis.cpp
    cpImageCompare.cpp
    cpImagePropertyView.cpp
    cpImageView.cpp
    cpNewProject.cpp
    cpProjectView.cpp
    cpSetApplicationOptions.cpp
    cpSetCompressOptions.cpp
    cpSetMeshOptions.cpp
    cpStartupPage.cpp
    cpWelcomePage.cpp
)

target_link_libraries(CMP_GUI_BIN_Components INTERFACE

    CMP_GUI
    CMP_Mesh
    CMP_MeshOptimizer

    Plugin_CAnalysis
    Plugin_Common_ATIFormats
    Plugin_Json

    ExtOpenGL
    ExtQt5ForGUI
    ExtQt5OpenGL
    ExtRapidXML
)

target_include_directories(CMP_GUI_BIN_Components INTERFACE

    ./
)