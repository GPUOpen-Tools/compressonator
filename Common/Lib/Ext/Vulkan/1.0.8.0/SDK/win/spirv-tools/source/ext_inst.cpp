// Copyright (c) 2015-2016 The Khronos Group Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
// KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
// SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
//    https://www.khronos.org/registry/
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

#include "ext_inst.h"

#include <string.h>

#include "spirv_definition.h"

/// Generate a spv_ext_inst_desc_t literal for a GLSL std450 extended
/// instruction with one/two/three <id> parameter(s).
#define GLSL450Inst1(name) \
  #name, GLSLstd450::GLSLstd450##name, 0, { SPV_OPERAND_TYPE_ID }
#define GLSL450Inst1Cap(name, cap)                        \
  #name, GLSLstd450::GLSLstd450##name,                    \
          SPV_CAPABILITY_AS_MASK(SpvCapability##cap), { \
    SPV_OPERAND_TYPE_ID                                   \
  }
#define GLSL450Inst2(name)                   \
  #name, GLSLstd450::GLSLstd450##name, 0, {  \
    SPV_OPERAND_TYPE_ID, SPV_OPERAND_TYPE_ID \
  }
#define GLSL450Inst2Cap(name, cap)                  \
  #name, GLSLstd450::GLSLstd450##name,              \
      SPV_CAPABILITY_AS_MASK(SpvCapability##cap), { \
    SPV_OPERAND_TYPE_ID, SPV_OPERAND_TYPE_ID        \
  }
#define GLSL450Inst3(name)                                        \
  #name, GLSLstd450::GLSLstd450##name, 0, {                       \
    SPV_OPERAND_TYPE_ID, SPV_OPERAND_TYPE_ID, SPV_OPERAND_TYPE_ID \
  }

static const spv_ext_inst_desc_t glslStd450Entries[] = {
    {GLSL450Inst1(Round)},
    {GLSL450Inst1(RoundEven)},
    {GLSL450Inst1(Trunc)},
    {GLSL450Inst1(FAbs)},
    {GLSL450Inst1(SAbs)},
    {GLSL450Inst1(FSign)},
    {GLSL450Inst1(SSign)},
    {GLSL450Inst1(Floor)},
    {GLSL450Inst1(Ceil)},
    {GLSL450Inst1(Fract)},
    {GLSL450Inst1(Radians)},
    {GLSL450Inst1(Degrees)},
    {GLSL450Inst1(Sin)},
    {GLSL450Inst1(Cos)},
    {GLSL450Inst1(Tan)},
    {GLSL450Inst1(Asin)},
    {GLSL450Inst1(Acos)},
    {GLSL450Inst1(Atan)},
    {GLSL450Inst1(Sinh)},
    {GLSL450Inst1(Cosh)},
    {GLSL450Inst1(Tanh)},
    {GLSL450Inst1(Asinh)},
    {GLSL450Inst1(Acosh)},
    {GLSL450Inst1(Atanh)},
    {GLSL450Inst2(Atan2)},
    {GLSL450Inst2(Pow)},
    {GLSL450Inst1(Exp)},
    {GLSL450Inst1(Log)},
    {GLSL450Inst1(Exp2)},
    {GLSL450Inst1(Log2)},
    {GLSL450Inst1(Sqrt)},
    {GLSL450Inst1(InverseSqrt)},
    {GLSL450Inst1(Determinant)},
    {GLSL450Inst1(MatrixInverse)},
    {GLSL450Inst2(Modf)},
    {GLSL450Inst1(ModfStruct)},
    {GLSL450Inst2(FMin)},
    {GLSL450Inst2(UMin)},
    {GLSL450Inst2(SMin)},
    {GLSL450Inst2(FMax)},
    {GLSL450Inst2(UMax)},
    {GLSL450Inst2(SMax)},
    {GLSL450Inst3(FClamp)},
    {GLSL450Inst3(UClamp)},
    {GLSL450Inst3(SClamp)},
    {GLSL450Inst3(FMix)},
    {GLSL450Inst3(IMix)},
    {GLSL450Inst2(Step)},
    {GLSL450Inst3(SmoothStep)},
    {GLSL450Inst3(Fma)},
    {GLSL450Inst2(Frexp)},
    {GLSL450Inst1(FrexpStruct)},
    {GLSL450Inst2(Ldexp)},
    {GLSL450Inst1(PackSnorm4x8)},
    {GLSL450Inst1(PackUnorm4x8)},
    {GLSL450Inst1(PackSnorm2x16)},
    {GLSL450Inst1(PackUnorm2x16)},
    {GLSL450Inst1(PackHalf2x16)},
    {GLSL450Inst1Cap(PackDouble2x32, Float64)},
    {GLSL450Inst1(UnpackSnorm2x16)},
    {GLSL450Inst1(UnpackUnorm2x16)},
    {GLSL450Inst1(UnpackHalf2x16)},
    {GLSL450Inst1(UnpackSnorm4x8)},
    {GLSL450Inst1(UnpackUnorm4x8)},
    {GLSL450Inst1(UnpackDouble2x32)},
    {GLSL450Inst1(Length)},
    {GLSL450Inst2(Distance)},
    {GLSL450Inst2(Cross)},
    {GLSL450Inst1(Normalize)},
    {GLSL450Inst3(FaceForward)},
    {GLSL450Inst2(Reflect)},
    {GLSL450Inst3(Refract)},
    {GLSL450Inst1(FindILsb)},
    {GLSL450Inst1(FindSMsb)},
    {GLSL450Inst1(FindUMsb)},
    {GLSL450Inst1Cap(InterpolateAtCentroid, InterpolationFunction)},
    {GLSL450Inst2Cap(InterpolateAtSample, InterpolationFunction)},
    {GLSL450Inst2Cap(InterpolateAtOffset, InterpolationFunction)},
    {GLSL450Inst2(NMin)},
    {GLSL450Inst2(NMax)},
    {GLSL450Inst2(NClamp)},
};

static const spv_ext_inst_desc_t openclEntries[] = {
#define ExtInst(Name, Opcode, OperandList) \
  { #Name, Opcode, 0, OperandList }           \
  ,
#define EmptyList \
  {}
#define List(...) \
  { __VA_ARGS__ }
#define OperandId SPV_OPERAND_TYPE_ID
#define OperandLiteralNumber SPV_OPERAND_TYPE_LITERAL_INTEGER
#define OperandFPRoundingMode SPV_OPERAND_TYPE_FP_ROUNDING_MODE
#define OperandVariableIds SPV_OPERAND_TYPE_VARIABLE_ID
#include "opencl_std_ext_inst.inc"
#undef ExtList
#undef EmptyList
#undef List
#undef OperandId
#undef OperandLiteralNumber
#undef OperandFPRoundingMode
#undef OperandVariableIds
};

spv_result_t spvExtInstTableGet(spv_ext_inst_table* pExtInstTable) {
  if (!pExtInstTable) return SPV_ERROR_INVALID_POINTER;

  static const spv_ext_inst_group_t groups[] = {
      {SPV_EXT_INST_TYPE_GLSL_STD_450,
       static_cast<uint32_t>(sizeof(glslStd450Entries) /
                             sizeof(spv_ext_inst_desc_t)),
       glslStd450Entries},
      {SPV_EXT_INST_TYPE_OPENCL_STD,
       static_cast<uint32_t>(sizeof(openclEntries) /
                             sizeof(spv_ext_inst_desc_t)),
       openclEntries},
  };

  static const spv_ext_inst_table_t table = {
      static_cast<uint32_t>(sizeof(groups) / sizeof(spv_ext_inst_group_t)),
      groups};

  *pExtInstTable = &table;

  return SPV_SUCCESS;
}

spv_ext_inst_type_t spvExtInstImportTypeGet(const char* name) {
  // The names are specified by the respective extension instruction
  // specifications.
  if (!strcmp("GLSL.std.450", name)) {
    return SPV_EXT_INST_TYPE_GLSL_STD_450;
  }
  if (!strcmp("OpenCL.std", name)) {
    return SPV_EXT_INST_TYPE_OPENCL_STD;
  }
  return SPV_EXT_INST_TYPE_NONE;
}

spv_result_t spvExtInstTableNameLookup(const spv_ext_inst_table table,
                                       const spv_ext_inst_type_t type,
                                       const char* name,
                                       spv_ext_inst_desc* pEntry) {
  if (!table) return SPV_ERROR_INVALID_TABLE;
  if (!pEntry) return SPV_ERROR_INVALID_POINTER;

  for (uint32_t groupIndex = 0; groupIndex < table->count; groupIndex++) {
    auto& group = table->groups[groupIndex];
    if (type == group.type) {
      for (uint32_t index = 0; index < group.count; index++) {
        auto& entry = group.entries[index];
        if (!strcmp(name, entry.name)) {
          *pEntry = &table->groups[groupIndex].entries[index];
          return SPV_SUCCESS;
        }
      }
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}

spv_result_t spvExtInstTableValueLookup(const spv_ext_inst_table table,
                                        const spv_ext_inst_type_t type,
                                        const uint32_t value,
                                        spv_ext_inst_desc* pEntry) {
  if (!table) return SPV_ERROR_INVALID_TABLE;
  if (!pEntry) return SPV_ERROR_INVALID_POINTER;

  for (uint32_t groupIndex = 0; groupIndex < table->count; groupIndex++) {
    auto& group = table->groups[groupIndex];
    if (type == group.type) {
      for (uint32_t index = 0; index < group.count; index++) {
        auto& entry = group.entries[index];
        if (value == entry.ext_inst) {
          *pEntry = &table->groups[groupIndex].entries[index];
          return SPV_SUCCESS;
        }
      }
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}
