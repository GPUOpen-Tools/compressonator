
add_library(CMP_Math)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../../CMP_Framework/Common)

target_sources(CMP_Math PRIVATE

    cmp_math_common.cpp
    cmp_math_common.h
    cmp_math_cpuid.cpp
    cmp_math_cpuid.h
    JML.h
    JMLFuncs.cpp
    JMLFuncs.h
    JMLMatrix.h
    JMLScalar.h
    JMLSSEVec.h
    JMLVec2.h
    JMLVec3.h
    JRTCommon.h
    ${LEGACY_LOCATION}/MathMacros.h
    TootlePCH.h
)

target_include_directories(CMP_Math PUBLIC

    ./
)

target_link_libraries(CMP_Math PRIVATE

    CMP_Core
)

set_target_properties(CMP_Math PROPERTIES FOLDER ${FOLDER_NAME})
