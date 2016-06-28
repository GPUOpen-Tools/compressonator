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

#include "opcode.h"

#include <assert.h>
#include <string.h>

#include <cstdlib>

#include "instruction.h"
#include "spirv-tools/libspirv.h"
#include "spirv_constant.h"
#include "spirv_endian.h"
#include "spirv_operands.h"

namespace {

// Descriptions of each opcode.  Each entry describes the format of the
// instruction that follows a particular opcode.
//
// Most fields are initialized statically by including an automatically
// generated file.
// The operandTypes fields are initialized during spvOpcodeInitialize().
//
// TODO(dneto): Some of the macros are quite unreadable.  We could make
// good use of constexpr functions, but some compilers don't support that yet.
const spv_opcode_desc_t opcodeTableEntries[] = {
#define EmptyList \
  {}
#define List(...) \
  { __VA_ARGS__ }
#define Capability(X) SPV_CAPABILITY_AS_MASK(SpvCapability##X)
#define Capability2(X, Y) Capability(X) | Capability(Y)
#define SpvCapabilityNone \
  0  // Needed so Capability(None) still expands to valid syntax.
#define Instruction(Name, HasResult, HasType, NumLogicalOperands,        \
                    NumCapabilities, CapabilityRequired, IsVariable,     \
                    LogicalArgsList)                                     \
  {#Name,     SpvOp##Name, (NumCapabilities) ? (CapabilityRequired) : 0, \
   0,         {}, /* Filled in later. Operand list, including            \
                     result id and type id, if needed */                 \
   HasResult, HasType,     LogicalArgsList},
#include "opcode.inc"
#undef EmptyList
#undef List
#undef Capability
#undef Capability2
#undef CapabilityNone
#undef Instruction
};

// Opcode API

// Converts the given operand class enum (from the SPIR-V document generation
// logic) to the operand type required by the parser.  The SPV_OPERAND_TYPE_NONE
// value indicates there is no current operand and no further operands.
// This only applies to logical operands.
spv_operand_type_t convertOperandClassToType(SpvOp opcode,
                                             OperandClass operandClass) {
  // The spec document generator uses OptionalOperandLiteral for several kinds
  // of repeating values.  Our parser needs more specific information about
  // what is being repeated.
  if (operandClass == OperandOptionalLiteral) {
    switch (opcode) {
      case SpvOpExecutionMode:
        // An OpExecutionMode only takes a single ExecutionMode operand and the
        // operands for that execution mode.  The OperandOptionalLiteral in the
        // grammar from the spec is only used to generate the text "Optional
        // literal(s)".  But we've already recorded the
        // SPV_OPERAND_TYPE_EXECUTION_MODE which will absorb those extra
        // literals.  Use a NONE operand type here to terminate the operands
        // to the instruction.
        return SPV_OPERAND_TYPE_NONE;
      default:
        break;
    }
  } else if (operandClass == OperandVariableLiterals) {
    switch (opcode) {
      case SpvOpConstant:
      case SpvOpSpecConstant:
        // The number type is determined by the type Id operand.
        return SPV_OPERAND_TYPE_TYPED_LITERAL_NUMBER;
      case SpvOpDecorate:
      case SpvOpMemberDecorate:
        // The operand types at the end of the instruction are
        // determined instead by the decoration kind.
        return SPV_OPERAND_TYPE_NONE;
      default:
        break;
    }
  }

  switch (operandClass) {
    case OperandNone:
      return SPV_OPERAND_TYPE_NONE;
    case OperandId:
      return SPV_OPERAND_TYPE_ID;
    case OperandOptionalId:
      return SPV_OPERAND_TYPE_OPTIONAL_ID;
    case OperandVariableIds:
      if (opcode == SpvOpSpecConstantOp) {
        // These are the operands to the specialization constant opcode.
        // The assembler and binary parser set up the extra Id and literal
        // arguments when processing the opcode operand.  So don't add
        // an operand type for them here.
        return SPV_OPERAND_TYPE_NONE;
      }
      return SPV_OPERAND_TYPE_VARIABLE_ID;
    // The spec only uses OptionalLiteral for an optional literal number.
    case OperandOptionalLiteral:
      return SPV_OPERAND_TYPE_OPTIONAL_LITERAL_INTEGER;
    case OperandOptionalLiteralString:
      return SPV_OPERAND_TYPE_OPTIONAL_LITERAL_STRING;
    // This is only used for sequences of literal numbers.
    case OperandVariableLiterals:
      return SPV_OPERAND_TYPE_VARIABLE_LITERAL_INTEGER;
    case OperandLiteralNumber:
      if (opcode == SpvOpExtInst) {
        // We use a special operand type for the extension instruction number.
        // For now, we assume there is only one LiteraNumber argument to
        // OpExtInst, and it is the extension instruction argument.
        // See the ExtInst entry in opcode.inc
        // TODO(dneto): Use a function to confirm the assumption, and to verify
        // that the index into the operandClass is 1, as expected.
        return SPV_OPERAND_TYPE_EXTENSION_INSTRUCTION_NUMBER;
      } else if (opcode == SpvOpSpecConstantOp) {
        // Use a special operand type for the opcode operand, so we can
        // use mnemonic names instead of the numbers.  For example, the
        // assembler should accept "IAdd" instead of the numeric value of
        // SpvOpIAdd.
        return SPV_OPERAND_TYPE_SPEC_CONSTANT_OP_NUMBER;
      }
      return SPV_OPERAND_TYPE_LITERAL_INTEGER;
    case OperandLiteralString:
      return SPV_OPERAND_TYPE_LITERAL_STRING;
    case OperandSource:
      return SPV_OPERAND_TYPE_SOURCE_LANGUAGE;
    case OperandExecutionModel:
      return SPV_OPERAND_TYPE_EXECUTION_MODEL;
    case OperandAddressing:
      return SPV_OPERAND_TYPE_ADDRESSING_MODEL;
    case OperandMemory:
      return SPV_OPERAND_TYPE_MEMORY_MODEL;
    case OperandExecutionMode:
      return SPV_OPERAND_TYPE_EXECUTION_MODE;
    case OperandStorage:
      return SPV_OPERAND_TYPE_STORAGE_CLASS;
    case OperandDimensionality:
      return SPV_OPERAND_TYPE_DIMENSIONALITY;
    case OperandSamplerAddressingMode:
      return SPV_OPERAND_TYPE_SAMPLER_ADDRESSING_MODE;
    case OperandSamplerFilterMode:
      return SPV_OPERAND_TYPE_SAMPLER_FILTER_MODE;
    case OperandSamplerImageFormat:
      return SPV_OPERAND_TYPE_SAMPLER_IMAGE_FORMAT;
    case OperandImageChannelOrder:
      // This is only used to describe the value generated by OpImageQueryOrder.
      // It is not used as an operand.
      break;
    case OperandImageChannelDataType:
      // This is only used to describe the value generated by
      // OpImageQueryFormat. It is not used as an operand.
      break;
    case OperandImageOperands:
      return SPV_OPERAND_TYPE_IMAGE;
    case OperandOptionalImageOperands:
      return SPV_OPERAND_TYPE_OPTIONAL_IMAGE;
    case OperandFPFastMath:
      return SPV_OPERAND_TYPE_FP_FAST_MATH_MODE;
    case OperandFPRoundingMode:
      return SPV_OPERAND_TYPE_FP_ROUNDING_MODE;
    case OperandLinkageType:
      return SPV_OPERAND_TYPE_LINKAGE_TYPE;
    case OperandAccessQualifier:
      return SPV_OPERAND_TYPE_ACCESS_QUALIFIER;
    case OperandOptionalAccessQualifier:
      return SPV_OPERAND_TYPE_OPTIONAL_ACCESS_QUALIFIER;
    case OperandFuncParamAttr:
      return SPV_OPERAND_TYPE_FUNCTION_PARAMETER_ATTRIBUTE;
    case OperandDecoration:
      return SPV_OPERAND_TYPE_DECORATION;
    case OperandBuiltIn:
      return SPV_OPERAND_TYPE_BUILT_IN;
    case OperandSelect:
      return SPV_OPERAND_TYPE_SELECTION_CONTROL;
    case OperandLoop:
      return SPV_OPERAND_TYPE_LOOP_CONTROL;
    case OperandFunction:
      return SPV_OPERAND_TYPE_FUNCTION_CONTROL;
    case OperandMemorySemantics:
      return SPV_OPERAND_TYPE_MEMORY_SEMANTICS_ID;
    case OperandMemoryAccess:
      // This case does not occur in the table for SPIR-V 0.99 Rev 32.
      // We expect that it will become SPV_OPERAND_TYPE_OPTIONAL_MEMORY_ACCESS,
      // and we can remove the special casing above for memory operation
      // instructions.
      break;
    case OperandOptionalMemoryAccess:
      // Expect an optional mask.  When the Aligned bit is set in the mask,
      // we will later add the expectation of a literal number operand.
      return SPV_OPERAND_TYPE_OPTIONAL_MEMORY_ACCESS;
    case OperandScope:
      return SPV_OPERAND_TYPE_SCOPE_ID;
    case OperandGroupOperation:
      return SPV_OPERAND_TYPE_GROUP_OPERATION;
    case OperandKernelEnqueueFlags:
      return SPV_OPERAND_TYPE_KERNEL_ENQ_FLAGS;
    case OperandKernelProfilingInfo:
      return SPV_OPERAND_TYPE_KERNEL_PROFILING_INFO;
    case OperandCapability:
      return SPV_OPERAND_TYPE_CAPABILITY;

    // Used by GroupMemberDecorate
    case OperandVariableIdLiteral:
      return SPV_OPERAND_TYPE_VARIABLE_ID_LITERAL_INTEGER;

    // Used by Switch
    case OperandVariableLiteralId:
      return SPV_OPERAND_TYPE_VARIABLE_LITERAL_INTEGER_ID;

    // These exceptional cases shouldn't occur.
    case OperandCount:
    default:
      break;
  }
  assert(0 && "Unexpected operand class");
  return SPV_OPERAND_TYPE_NONE;
}

}  // anonymous namespace

// Finish populating the opcodeTableEntries array.
void spvOpcodeTableInitialize(spv_opcode_desc_t* entries,
                              uint32_t num_entries) {
  // Compute the operandTypes field for each entry.
  for (uint32_t i = 0; i < num_entries; ++i) {
    spv_opcode_desc_t& opcode = entries[i];
    opcode.numTypes = 0;
    // Type ID always comes first, if present.
    if (opcode.hasType)
      opcode.operandTypes[opcode.numTypes++] = SPV_OPERAND_TYPE_TYPE_ID;
    // Result ID always comes next, if present
    if (opcode.hasResult)
      opcode.operandTypes[opcode.numTypes++] = SPV_OPERAND_TYPE_RESULT_ID;
    const uint16_t maxNumOperands = static_cast<uint16_t>(
        sizeof(opcode.operandTypes) / sizeof(opcode.operandTypes[0]));
    const uint16_t maxNumClasses = static_cast<uint16_t>(
        sizeof(opcode.operandClass) / sizeof(opcode.operandClass[0]));
    for (uint16_t classIndex = 0;
         opcode.numTypes < maxNumOperands && classIndex < maxNumClasses;
         classIndex++) {
      const OperandClass operandClass = opcode.operandClass[classIndex];
      const auto operandType =
          convertOperandClassToType(opcode.opcode, operandClass);
      opcode.operandTypes[opcode.numTypes++] = operandType;
      // The OperandNone value is not explicitly represented in the .inc file.
      // However, it is the zero value, and is created via implicit value
      // initialization.  It converts to SPV_OPERAND_TYPE_NONE.
      // The SPV_OPERAND_TYPE_NONE operand type indicates no current or futher
      // operands.
      if (operandType == SPV_OPERAND_TYPE_NONE) {
        opcode.numTypes--;
        break;
      }
    }

    // We should have written the terminating SPV_OPERAND_TYPE_NONE entry, but
    // also without overflowing.
    assert((opcode.numTypes < maxNumOperands) &&
           "Operand class list is too long.  Expand "
           "spv_opcode_desc_t.operandClass");
  }
}

const char* spvGeneratorStr(uint32_t generator) {
  switch (generator) {
    case SPV_GENERATOR_KHRONOS:
      return "Khronos";
    case SPV_GENERATOR_LUNARG:
      return "LunarG";
    case SPV_GENERATOR_VALVE:
      return "Valve";
    case SPV_GENERATOR_CODEPLAY:
      return "Codeplay Software Ltd.";
    case SPV_GENERATOR_NVIDIA:
      return "NVIDIA";
    case SPV_GENERATOR_ARM:
      return "ARM";
    case SPV_GENERATOR_KHRONOS_LLVM_TRANSLATOR:
      return "Khronos LLVM/SPIR-V Translator";
    case SPV_GENERATOR_KHRONOS_ASSEMBLER:
      return "Khronos SPIR-V Tools Assembler";
    case SPV_GENERATOR_KHRONOS_GLSLANG:
      return "Khronos Glslang Reference Front End";
    default:
      return "Unknown";
  }
}

uint32_t spvOpcodeMake(uint16_t wordCount, SpvOp opcode) {
  return ((uint32_t)opcode) | (((uint32_t)wordCount) << 16);
}

void spvOpcodeSplit(const uint32_t word, uint16_t* pWordCount, SpvOp* pOpcode) {
  if (pWordCount) {
    *pWordCount = (uint16_t)((0xffff0000 & word) >> 16);
  }
  if (pOpcode) {
    *pOpcode = (SpvOp)(0x0000ffff & word);
  }
}

spv_result_t spvOpcodeTableGet(spv_opcode_table* pInstTable) {
  if (!pInstTable) return SPV_ERROR_INVALID_POINTER;

  const uint32_t size = sizeof(opcodeTableEntries);
  spv_opcode_desc_t* copied_entries =
      static_cast<spv_opcode_desc_t*>(::malloc(size));
  if (!copied_entries) return SPV_ERROR_OUT_OF_MEMORY;
  ::memcpy(copied_entries, opcodeTableEntries, size);

  const uint32_t count = static_cast<uint32_t>(sizeof(opcodeTableEntries) /
                                               sizeof(spv_opcode_desc_t));
  spv_opcode_table_t* table = new spv_opcode_table_t{count, copied_entries};

  spvOpcodeTableInitialize(copied_entries, count);

  *pInstTable = table;

  return SPV_SUCCESS;
}

spv_result_t spvOpcodeTableNameLookup(const spv_opcode_table table,
                                      const char* name,
                                      spv_opcode_desc* pEntry) {
  if (!name || !pEntry) return SPV_ERROR_INVALID_POINTER;
  if (!table) return SPV_ERROR_INVALID_TABLE;

  // TODO: This lookup of the Opcode table is suboptimal! Binary sort would be
  // preferable but the table requires sorting on the Opcode name, but it's
  // static
  // const initialized and matches the order of the spec.
  const size_t nameLength = strlen(name);
  for (uint64_t opcodeIndex = 0; opcodeIndex < table->count; ++opcodeIndex) {
    if (nameLength == strlen(table->entries[opcodeIndex].name) &&
        !strncmp(name, table->entries[opcodeIndex].name, nameLength)) {
      // NOTE: Found out Opcode!
      *pEntry = &table->entries[opcodeIndex];
      return SPV_SUCCESS;
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}

spv_result_t spvOpcodeTableValueLookup(const spv_opcode_table table,
                                       const SpvOp opcode,
                                       spv_opcode_desc* pEntry) {
  if (!table) return SPV_ERROR_INVALID_TABLE;
  if (!pEntry) return SPV_ERROR_INVALID_POINTER;

  // TODO: As above this lookup is not optimal.
  for (uint64_t opcodeIndex = 0; opcodeIndex < table->count; ++opcodeIndex) {
    if (opcode == table->entries[opcodeIndex].opcode) {
      // NOTE: Found the Opcode!
      *pEntry = &table->entries[opcodeIndex];
      return SPV_SUCCESS;
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}

int32_t spvOpcodeRequiresCapabilities(spv_opcode_desc entry) {
  return entry->capabilities != 0;
}

void spvInstructionCopy(const uint32_t* words, const SpvOp opcode,
                        const uint16_t wordCount, const spv_endianness_t endian,
                        spv_instruction_t* pInst) {
  pInst->opcode = opcode;
  pInst->words.resize(wordCount);
  for (uint16_t wordIndex = 0; wordIndex < wordCount; ++wordIndex) {
    pInst->words[wordIndex] = spvFixWord(words[wordIndex], endian);
    if (!wordIndex) {
      uint16_t thisWordCount;
      SpvOp thisOpcode;
      spvOpcodeSplit(pInst->words[wordIndex], &thisWordCount, &thisOpcode);
      assert(opcode == thisOpcode && wordCount == thisWordCount &&
             "Endianness failed!");
    }
  }
}

const char* spvOpcodeString(const SpvOp opcode) {
// Use the syntax table so it's sure to be complete.
#define Instruction(Name, ...) \
  case SpvOp##Name:            \
    return #Name;
  switch (opcode) {
#include "opcode.inc"
    default:
      assert(0 && "Unreachable!");
  }
  return "unknown";
#undef Instruction
}

int32_t spvOpcodeIsScalarType(const SpvOp opcode) {
  switch (opcode) {
    case SpvOpTypeInt:
    case SpvOpTypeFloat:
    case SpvOpTypeBool:
      return true;
    default:
      return false;
  }
}

int32_t spvOpcodeIsConstant(const SpvOp opcode) {
  switch (opcode) {
    case SpvOpConstantTrue:
    case SpvOpConstantFalse:
    case SpvOpConstant:
    case SpvOpConstantComposite:
    case SpvOpConstantSampler:
    // case SpvOpConstantNull:
    case SpvOpConstantNull:
    case SpvOpSpecConstantTrue:
    case SpvOpSpecConstantFalse:
    case SpvOpSpecConstant:
    case SpvOpSpecConstantComposite:
      // case SpvOpSpecConstantOp:
      return true;
    default:
      return false;
  }
}

int32_t spvOpcodeIsComposite(const SpvOp opcode) {
  switch (opcode) {
    case SpvOpTypeVector:
    case SpvOpTypeMatrix:
    case SpvOpTypeArray:
    case SpvOpTypeStruct:
      return true;
    default:
      return false;
  }
}

int32_t spvOpcodeIsPointer(const SpvOp opcode) {
  switch (opcode) {
    case SpvOpVariable:
    case SpvOpAccessChain:
    case SpvOpPtrAccessChain:
    case SpvOpInBoundsAccessChain:
    case SpvOpInBoundsPtrAccessChain:
    case SpvOpFunctionParameter:
      return true;
    default:
      return false;
  }
}

int32_t spvOpcodeGeneratesType(SpvOp op) {
  switch (op) {
    case SpvOpTypeVoid:
    case SpvOpTypeBool:
    case SpvOpTypeInt:
    case SpvOpTypeFloat:
    case SpvOpTypeVector:
    case SpvOpTypeMatrix:
    case SpvOpTypeImage:
    case SpvOpTypeSampler:
    case SpvOpTypeSampledImage:
    case SpvOpTypeArray:
    case SpvOpTypeRuntimeArray:
    case SpvOpTypeStruct:
    case SpvOpTypeOpaque:
    case SpvOpTypePointer:
    case SpvOpTypeFunction:
    case SpvOpTypeEvent:
    case SpvOpTypeDeviceEvent:
    case SpvOpTypeReserveId:
    case SpvOpTypeQueue:
    case SpvOpTypePipe:
      return true;
    default:
      // In particular, OpTypeForwardPointer does not generate a type,
      // but declares a storage class for a pointer type generated
      // by a different instruction.
      break;
  }
  return 0;
}
