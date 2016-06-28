# Copyright (c) 2015-2016 The Khronos Group Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and/or associated documentation files (the
# "Materials"), to deal in the Materials without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Materials, and to
# permit persons to whom the Materials are furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Materials.
#
# MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
# KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
# SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
#    https://www.khronos.org/registry/
#
# THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

set(SPIRV_SOURCES
  ${spirv-tools_SOURCE_DIR}/include/spirv-tools/libspirv.h
  ${spirv-tools_SOURCE_DIR}/include/spirv/spirv.h
  ${spirv-tools_SOURCE_DIR}/include/spirv/GLSL.std.450.h
  ${spirv-tools_SOURCE_DIR}/include/spirv/OpenCL.std.h
  ${CMAKE_CURRENT_SOURCE_DIR}/util/bitutils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/util/hex_float.h
  ${CMAKE_CURRENT_SOURCE_DIR}/assembly_grammar.h
  ${CMAKE_CURRENT_SOURCE_DIR}/binary.h
  ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ext_inst.h
  ${CMAKE_CURRENT_SOURCE_DIR}/instruction.h
  ${CMAKE_CURRENT_SOURCE_DIR}/opcode.h
  ${CMAKE_CURRENT_SOURCE_DIR}/operand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/print.h
  ${CMAKE_CURRENT_SOURCE_DIR}/spirv_constant.h
  ${CMAKE_CURRENT_SOURCE_DIR}/spirv_definition.h
  ${CMAKE_CURRENT_SOURCE_DIR}/spirv_endian.h
  ${CMAKE_CURRENT_SOURCE_DIR}/table.h
  ${CMAKE_CURRENT_SOURCE_DIR}/text.h
  ${CMAKE_CURRENT_SOURCE_DIR}/text_handler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/validate.h
  ${CMAKE_CURRENT_SOURCE_DIR}/assembly_grammar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/binary.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/disassemble.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ext_inst.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/instruction.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/opcode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/operand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/print.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/spirv_endian.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/text.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/text_handler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_cfg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_id.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_instruction.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_ssa.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/validate_types.cpp)

add_library(${SPIRV_TOOLS} ${SPIRV_SOURCES})
spvtools_default_compile_options(${SPIRV_TOOLS})
target_include_directories(${SPIRV_TOOLS} PUBLIC ${spirv-tools_SOURCE_DIR}/include)

install(TARGETS ${SPIRV_TOOLS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
