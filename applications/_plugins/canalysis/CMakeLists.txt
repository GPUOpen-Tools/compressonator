cmake_minimum_required(VERSION 3.10)

add_library(Image_Analysis STATIC "")

if (OPTION_CMP_QT)
    set(ANALYSIS_QT_FILES
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cvmatandqimage.h
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common/cvmatandqimage.cpp
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageloader.h
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components/cpimageloader.cpp
    )
else()
    set(ANALYSIS_QT_FILES)
endif()


target_sources(Image_Analysis
               PRIVATE
               analysis/canalysis.cpp
               analysis/canalysis.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/atiformats.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/pluginmanager.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/textureio.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/textureio.cpp
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/ssim.h
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common/ssim.cpp
               ${ANALYSIS_QT_FILES}
               )

target_include_directories(Image_Analysis
               PRIVATE
               ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
               ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
               ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/common
               ${PROJECT_SOURCE_DIR}/applications/compressonatorgui/components
               ${PROJECT_SOURCE_DIR}/../common/lib/ext/rapidxml

               ${OpenEXR_INCLUDE_DIRS}
               ${OpenCV_INCLUDE_DIRS}
               ${Qt5Gui_INCLUDE_DIRS}
               )

if (OPTION_CMP_QT)
target_include_directories(Image_Analysis
               PRIVATE
               ${Qt5Gui_INCLUDE_DIRS}
               )
endif()

if (CMP_HOST_WINDOWS)
    # update this and enable for plugin builds
    # target_link_libraries(Image_Analysis  
    #      PRIVATE
    #          CMP_Compressonator
    #          Plugin_TCPluginAPI
    #          Plugin_PluginManager
    #          Plugin_CImage_EXR
    #          ExtOpenCV
    #          ExtOpenEXR
    #          ExtRapidXML
    #  
    #      PUBLIC
    #          Plugin_Common_TestReport
    #  )

else()

    find_package(OpenCV) 
    if (OpenCV_FOUND)
        target_include_directories(Image_Analysis
                                   PRIVATE
                                   ${OpenCV_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "Package OpenCV are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install libopencv-dev to install the libs.")
    endif()
    
    set_property(TARGET Image_Analysis PROPERTY POSITION_INDEPENDENT_CODE TRUE)

    # Qt5 include path - users install required
    if (OPTION_CMP_QT)
        find_package(Qt5Gui)
        if(Qt5Gui_FOUND)
            target_include_directories(Image_Analysis
                                       PRIVATE 
                                       "${Qt5Gui_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "Package Qt5 (Qt5Gui) are required, but not found. In Unix, run initsetup_unix.sh or sudo apt-get install qtdeclarative5-dev. If is window, please make sure install qt and add the bin path (i.e. C:\Qt\5.7\msvc2015_64\bin) to environment PATH.")
        endif()
    endif()

    if(APPLE)
        target_include_directories(Image_Analysis
                                   PRIVATE
                                   /usr/local/include/openexr/)
    endif()

    if (UNIX)
        target_compile_definitions(Image_Analysis PRIVATE _LINUX)
    endif()
endif()

set_target_properties(Image_Analysis PROPERTIES FOLDER "Plugin_Static/Analysis")

