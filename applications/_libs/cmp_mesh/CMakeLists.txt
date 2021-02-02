
add_library(CMP_Mesh)

file(GLOB_RECURSE MATH_JRT_SRC
     jrt/*.cpp
     jrt/*.h
 )

target_sources(CMP_Mesh 
    PRIVATE
        ${MATH_JRT_SRC}
        aligned_malloc.cpp
        aligned_malloc.h
        bbox.h
        cloud.h
        clustering.cpp
        clustering.h
        cmp_mesh.h
        color.h
        error.c
        error.h
        feedback.cpp
        feedback.h
        fit.cpp
        fit.h
        gdiwindow.h
        gdiwm.cpp
        gdiwm.h
        heap.c
        heap.h
        matrix.h
        mesh.h
        option.h
        quaternion.h
        scalar.h
        soup.cpp
        soup.h
        souptomesh.cpp
        souptomesh.h
        stripifier.cpp
        stripifier.h
        timer.cpp
        timer.h
        tootlepch.h
        tootleraytracer.cpp
        tootleraytracer.h
        triorder.cpp
        triorder.h
        vector.h
        viewpoints.h
        window.h
)

target_include_directories(CMP_Mesh PUBLIC
    ./
    ./jrt/
)

target_link_libraries(CMP_Mesh PRIVATE
    CMP_Math
)

if (OPTION_CMP_DIRECTX)
    target_sources(CMP_Mesh PRIVATE
        overdraw.cpp
        overdraw.h
        tootlelib.cpp
        tootlelib.h
    )

    target_include_directories(CMP_Mesh PUBLIC
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include  # using d3dx9 for cmp_mesh
    )

    set(SOFTWARE_VERSION_ONLY ON)

    if (SOFTWARE_VERSION_ONLY)
        target_compile_definitions(CMP_Mesh PUBLIC -D_SOFTWARE_ONLY_VERSION=1)
    else()
        target_sources(CMP_Mesh PRIVATE
            d3doverdrawwindow.cpp
            d3doverdrawwindow.h
            d3dwindow.h
            d3dwm.cpp
            d3dwm.h
        )
    endif()
    
endif()

set_target_properties(CMP_Mesh PROPERTIES 
    FOLDER "Libs"
    )
