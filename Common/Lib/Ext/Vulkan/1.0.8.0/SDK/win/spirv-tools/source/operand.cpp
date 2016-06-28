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

#include "operand.h"

#include <assert.h>
#include <string.h>

static const spv_operand_desc_t sourceLanguageEntries[] = {
    {"Unknown", SpvSourceLanguageUnknown, 0, {SPV_OPERAND_TYPE_NONE}},
    {"ESSL", SpvSourceLanguageESSL, 0, {SPV_OPERAND_TYPE_NONE}},
    {"GLSL", SpvSourceLanguageGLSL, 0, {SPV_OPERAND_TYPE_NONE}},
    {"OpenCL_C", SpvSourceLanguageOpenCL_C, 0, {SPV_OPERAND_TYPE_NONE}},
    {"OpenCL_CPP", SpvSourceLanguageOpenCL_CPP, 0, {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t executionModelEntries[] = {
    {"Vertex",
     SpvExecutionModelVertex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"TessellationControl",
     SpvExecutionModelTessellationControl,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"TessellationEvaluation",
     SpvExecutionModelTessellationEvaluation,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"Geometry",
     SpvExecutionModelGeometry,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry),
     {SPV_OPERAND_TYPE_NONE}},
    {"Fragment",
     SpvExecutionModelFragment,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"GLCompute",
     SpvExecutionModelGLCompute,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Kernel",
     SpvExecutionModelKernel,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t addressingModelEntries[] = {
    {"Logical", SpvAddressingModelLogical, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Physical32",
     SpvAddressingModelPhysical32,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityAddresses),
     {SPV_OPERAND_TYPE_NONE}},
    {"Physical64",
     SpvAddressingModelPhysical64,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityAddresses),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t memoryModelEntries[] = {
    {"Simple",
     SpvMemoryModelSimple,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"GLSL450",
     SpvMemoryModelGLSL450,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"OpenCL",
     SpvMemoryModelOpenCL,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

// Execution mode requiring the given capability and having no operands.
#define ExecMode0(mode, cap)                                                   \
  #mode, SpvExecutionMode##mode, SPV_CAPABILITY_AS_MASK(SpvCapability##cap), { \
    SPV_OPERAND_TYPE_NONE                                                      \
  }
// Execution mode requiring the given capability and having one literal number
// operand.
#define ExecMode1(mode, cap)                                                   \
  #mode, SpvExecutionMode##mode, SPV_CAPABILITY_AS_MASK(SpvCapability##cap), { \
    SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE                    \
  }
static const spv_operand_desc_t executionModeEntries[] = {
    {ExecMode1(Invocations, Geometry)},
    {ExecMode0(SpacingEqual, Tessellation)},
    {ExecMode0(SpacingFractionalEven, Tessellation)},
    {ExecMode0(SpacingFractionalOdd, Tessellation)},
    {ExecMode0(VertexOrderCw, Tessellation)},
    {ExecMode0(VertexOrderCcw, Tessellation)},
    {ExecMode0(PixelCenterInteger, Shader)},
    {ExecMode0(OriginUpperLeft, Shader)},
    {ExecMode0(OriginLowerLeft, Shader)},
    {ExecMode0(EarlyFragmentTests, Shader)},
    {ExecMode0(PointMode, Tessellation)},
    {ExecMode0(Xfb, TransformFeedback)},
    {ExecMode0(DepthReplacing, Shader)},
    {ExecMode0(DepthGreater, Shader)},
    {ExecMode0(DepthLess, Shader)},
    {ExecMode0(DepthUnchanged, Shader)},
    {"LocalSize",
     SpvExecutionModeLocalSize,
     0,
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_LITERAL_INTEGER,
      SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"LocalSizeHint",
     SpvExecutionModeLocalSizeHint,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_LITERAL_INTEGER,
      SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {ExecMode0(InputPoints, Geometry)},
    {ExecMode0(InputLines, Geometry)},
    {ExecMode0(InputLinesAdjacency, Geometry)},
    {"Triangles",
     SpvExecutionModeTriangles,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry) |
         SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {ExecMode0(InputTrianglesAdjacency, Geometry)},
    {ExecMode0(Quads, Tessellation)},
    {ExecMode0(Isolines, Tessellation)},
    {"OutputVertices",
     SpvExecutionModeOutputVertices,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry) |
         SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {ExecMode0(OutputPoints, Geometry)},
    {ExecMode0(OutputLineStrip, Geometry)},
    {ExecMode0(OutputTriangleStrip, Geometry)},
    {ExecMode1(VecTypeHint, Kernel)},
    {ExecMode0(ContractionOff, Kernel)},
};
#undef ExecMode0
#undef ExecMode1

static const spv_operand_desc_t storageClassEntries[] = {
    // TODO(dneto): There are more storage classes in Rev32 and later.
    {"UniformConstant",
     SpvStorageClassUniformConstant,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"Input",
     SpvStorageClassInput,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Uniform",
     SpvStorageClassUniform,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Output",
     SpvStorageClassOutput,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Workgroup", SpvStorageClassWorkgroup, 0, {SPV_OPERAND_TYPE_NONE}},
    {"CrossWorkgroup",
     SpvStorageClassCrossWorkgroup,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"Private",
     SpvStorageClassPrivate,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Function", SpvStorageClassFunction, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Generic",
     SpvStorageClassGeneric,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGenericPointer),
     {SPV_OPERAND_TYPE_NONE}},
    {"PushConstant",
     SpvStorageClassPushConstant,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"AtomicCounter",
     SpvStorageClassAtomicCounter,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityAtomicStorage),
     {SPV_OPERAND_TYPE_NONE}},
    {"Image", SpvStorageClassImage, 0, {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t dimensionalityEntries[] = {
    {"1D",
     SpvDim1D,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampled1D),
     {SPV_OPERAND_TYPE_NONE}},
    {"2D", SpvDim2D, 0, {SPV_OPERAND_TYPE_NONE}},
    {"3D", SpvDim3D, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Cube",
     SpvDimCube,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Rect",
     SpvDimRect,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampledRect),
     {SPV_OPERAND_TYPE_NONE}},
    {"Buffer",
     SpvDimBuffer,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampledBuffer),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubpassData",
     SpvDimSubpassData,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityInputAttachment),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t samplerAddressingModeEntries[] = {
    {"None",
     SpvSamplerAddressingModeNone,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"ClampToEdge",
     SpvSamplerAddressingModeClampToEdge,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Clamp",
     SpvSamplerAddressingModeClamp,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Repeat",
     SpvSamplerAddressingModeRepeat,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"RepeatMirrored",
     SpvSamplerAddressingModeRepeatMirrored,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t samplerFilterModeEntries[] = {
    {"Nearest",
     SpvSamplerFilterModeNearest,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Linear",
     SpvSamplerFilterModeLinear,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t samplerImageFormatEntries[] = {
#define CASE0(NAME)                                           \
  {                                                           \
    #NAME, SpvImageFormat##NAME, 0, { SPV_OPERAND_TYPE_NONE } \
  }
#define CASE(NAME, CAP)                                                        \
  {                                                                            \
    #NAME, SpvImageFormat##NAME, SPV_CAPABILITY_AS_MASK(SpvCapability##CAP), { \
      SPV_OPERAND_TYPE_NONE                                                    \
    }                                                                          \
  }
    // clang-format off
  CASE0(Unknown),
  CASE(Rgba32f, Shader),
  CASE(Rgba16f, Shader),
  CASE(R32f, Shader),
  CASE(Rgba8, Shader),
  CASE(Rgba8Snorm, Shader),
  CASE(Rg32f, StorageImageExtendedFormats),
  CASE(Rg16f, StorageImageExtendedFormats),
  CASE(R11fG11fB10f, StorageImageExtendedFormats),
  CASE(R16f, StorageImageExtendedFormats),
  CASE(Rgba16, StorageImageExtendedFormats),
  CASE(Rgb10A2, StorageImageExtendedFormats),
  CASE(Rg16, StorageImageExtendedFormats),
  CASE(Rg8, StorageImageExtendedFormats),
  CASE(R16, StorageImageExtendedFormats),
  CASE(R8, StorageImageExtendedFormats),
  CASE(Rgba16Snorm, StorageImageExtendedFormats),
  CASE(Rg16Snorm, StorageImageExtendedFormats),
  CASE(Rg8Snorm, StorageImageExtendedFormats),
  CASE(R16Snorm, StorageImageExtendedFormats),
  CASE(R8Snorm, StorageImageExtendedFormats),
  CASE(Rgba32i, Shader),
  CASE(Rgba16i, Shader),
  CASE(Rgba8i, Shader),
  CASE(R32i, Shader),
  CASE(Rg32i, StorageImageExtendedFormats),
  CASE(Rg16i, StorageImageExtendedFormats),
  CASE(Rg8i, StorageImageExtendedFormats),
  CASE(R16i, StorageImageExtendedFormats),
  CASE(R8i, StorageImageExtendedFormats),
  CASE(Rgba32ui, Shader),
  CASE(Rgba16ui, Shader),
  CASE(Rgba8ui, Shader),
  CASE(R32ui, Shader),
  CASE(Rgb10a2ui, StorageImageExtendedFormats),
  CASE(Rg32ui, StorageImageExtendedFormats),
  CASE(Rg16ui, StorageImageExtendedFormats),
  CASE(Rg8ui, StorageImageExtendedFormats),
  CASE(R16ui, StorageImageExtendedFormats),
  CASE(R8ui, StorageImageExtendedFormats),
// clang-format on
#undef CASE
};

// All image channel orders depend on the Kernel capability.
#define CASE(NAME)                                     \
  {                                                    \
    #NAME, SpvImageChannelOrder##NAME,                 \
        SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel), { \
      SPV_OPERAND_TYPE_NONE                            \
    }                                                  \
  }
static const spv_operand_desc_t imageChannelOrderEntries[] = {
    CASE(R),    CASE(A),     CASE(RG),    CASE(RA),        CASE(RGB),
    CASE(RGBA), CASE(BGRA),  CASE(ARGB),  CASE(Intensity), CASE(Luminance),
    CASE(Rx),   CASE(RGx),   CASE(RGBx),  CASE(Depth),     CASE(DepthStencil),
    CASE(sRGB), CASE(sRGBx), CASE(sRGBA), CASE(sBGRA),
};
#undef CASE

// All image channel data types depend on the Kernel capability.
#define CASE(NAME)                                     \
  {                                                    \
    #NAME, SpvImageChannelDataType##NAME,              \
        SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel), { \
      SPV_OPERAND_TYPE_NONE                            \
    }                                                  \
  }
static const spv_operand_desc_t imageChannelDataTypeEntries[] = {
    CASE(SnormInt8),      CASE(SnormInt16),       CASE(UnormInt8),
    CASE(UnormInt16),     CASE(UnormShort565),    CASE(UnormShort555),
    CASE(UnormInt101010), CASE(SignedInt8),       CASE(SignedInt16),
    CASE(SignedInt32),    CASE(UnsignedInt8),     CASE(UnsignedInt16),
    CASE(UnsignedInt32),  CASE(HalfFloat),        CASE(Float),
    CASE(UnormInt24),     CASE(UnormInt101010_2),
};
#undef CASE

// Image operand definitions.  Each enum value is a mask.  When that mask
// bit is set, the instruction should have further ID operands.
// Some mask values depend on a capability.
static const spv_operand_desc_t imageOperandEntries[] = {
// Rev32 and later adds many more enums.
#define CASE(NAME) #NAME, SpvImageOperands##NAME##Mask, 0
#define CASE_CAP(NAME, CAP) #NAME, SpvImageOperands##NAME##Mask, CAP
#define ID SPV_OPERAND_TYPE_ID
#define NONE SPV_OPERAND_TYPE_NONE
    {"None", SpvImageOperandsMaskNone, 0, {NONE}},
    {CASE_CAP(Bias, SPV_CAPABILITY_AS_MASK(SpvCapabilityShader)), {ID, NONE}},
    {CASE(Lod), {ID, NONE}},
    {CASE(Grad), {ID, ID, NONE}},
    {CASE(ConstOffset), {ID, NONE}},
    {CASE_CAP(Offset, SPV_CAPABILITY_AS_MASK(SpvCapabilityImageGatherExtended)),
     {ID, NONE}},
    {CASE(ConstOffsets), {ID, NONE}},
    {CASE(Sample), {ID, NONE}},
    {CASE_CAP(MinLod, SPV_CAPABILITY_AS_MASK(SpvCapabilityMinLod)), {ID, NONE}},
#undef CASE
#undef CASE_CAP
#undef ID
#undef NONE
};

static const spv_operand_desc_t fpFastMathModeEntries[] = {
    {"None", SpvFPFastMathModeMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"NotNaN",
     SpvFPFastMathModeNotNaNMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NotInf",
     SpvFPFastMathModeNotInfMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NSZ",
     SpvFPFastMathModeNSZMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"AllowRecip",
     SpvFPFastMathModeAllowRecipMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Fast",
     SpvFPFastMathModeFastMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t fpRoundingModeEntries[] = {
    {"RTE",
     SpvFPRoundingModeRTE,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"RTZ",
     SpvFPRoundingModeRTZ,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"RTP",
     SpvFPRoundingModeRTP,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"RTN",
     SpvFPRoundingModeRTN,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t linkageTypeEntries[] = {
    {"Export",
     SpvLinkageTypeExport,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityLinkage),
     {SPV_OPERAND_TYPE_NONE}},
    {"Import",
     SpvLinkageTypeImport,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityLinkage),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t accessQualifierEntries[] = {
    {"ReadOnly",
     SpvAccessQualifierReadOnly,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"WriteOnly",
     SpvAccessQualifierWriteOnly,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"ReadWrite",
     SpvAccessQualifierReadWrite,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t functionParameterAttributeEntries[] = {
    {"Zext",
     SpvFunctionParameterAttributeZext,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Sext",
     SpvFunctionParameterAttributeSext,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"ByVal",
     SpvFunctionParameterAttributeByVal,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Sret",
     SpvFunctionParameterAttributeSret,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NoAlias",
     SpvFunctionParameterAttributeNoAlias,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NoCapture",
     SpvFunctionParameterAttributeNoCapture,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NoWrite",
     SpvFunctionParameterAttributeNoWrite,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NoReadWrite",
     SpvFunctionParameterAttributeNoReadWrite,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t decorationEntries[] = {
    {"RelaxedPrecision",
     SpvDecorationRelaxedPrecision,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {
        "SpecId",
        SpvDecorationSpecId,
        SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
        {SPV_OPERAND_TYPE_LITERAL_INTEGER},
    },
    {"Block",
     SpvDecorationBlock,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"BufferBlock",
     SpvDecorationBufferBlock,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"RowMajor",
     SpvDecorationRowMajor,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityMatrix),
     {SPV_OPERAND_TYPE_NONE}},
    {"ColMajor",
     SpvDecorationColMajor,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityMatrix),
     {SPV_OPERAND_TYPE_NONE}},
    {"ArrayStride",
     SpvDecorationArrayStride,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"MatrixStride",
     SpvDecorationMatrixStride,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityMatrix),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"GLSLShared",
     SpvDecorationGLSLShared,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"GLSLPacked",
     SpvDecorationGLSLPacked,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"CPacked",
     SpvDecorationCPacked,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"BuiltIn",
     SpvDecorationBuiltIn,
     0,
     {SPV_OPERAND_TYPE_BUILT_IN, SPV_OPERAND_TYPE_NONE}},
    {"NoPerspective",
     SpvDecorationNoPerspective,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Flat",
     SpvDecorationFlat,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Patch",
     SpvDecorationPatch,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"Centroid",
     SpvDecorationCentroid,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Sample",
     SpvDecorationSample,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampleRateShading),
     {SPV_OPERAND_TYPE_NONE}},
    {"Invariant",
     SpvDecorationInvariant,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"Restrict", SpvDecorationRestrict, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Aliased", SpvDecorationAliased, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Volatile", SpvDecorationVolatile, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Constant",
     SpvDecorationConstant,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Coherent", SpvDecorationCoherent, 0, {SPV_OPERAND_TYPE_NONE}},
    {"NonWritable", SpvDecorationNonWritable, 0, {SPV_OPERAND_TYPE_NONE}},
    {"NonReadable", SpvDecorationNonReadable, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Uniform",
     SpvDecorationUniform,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"SaturatedConversion",
     SpvDecorationSaturatedConversion,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"Stream",
     SpvDecorationStream,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometryStreams),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Location",
     SpvDecorationLocation,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Component",
     SpvDecorationComponent,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Index",
     SpvDecorationIndex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Binding",
     SpvDecorationBinding,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"DescriptorSet",
     SpvDecorationDescriptorSet,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Offset",
     SpvDecorationOffset,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"XfbBuffer",
     SpvDecorationXfbBuffer,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTransformFeedback),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"XfbStride",
     SpvDecorationXfbStride,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTransformFeedback),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"FuncParamAttr",
     SpvDecorationFuncParamAttr,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_FUNCTION_PARAMETER_ATTRIBUTE, SPV_OPERAND_TYPE_NONE}},
    {"FPRoundingMode",
     SpvDecorationFPRoundingMode,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_FP_ROUNDING_MODE, SPV_OPERAND_TYPE_NONE}},
    {"FPFastMathMode",
     SpvDecorationFPFastMathMode,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_FP_FAST_MATH_MODE, SPV_OPERAND_TYPE_NONE}},
    {"LinkageAttributes",
     SpvDecorationLinkageAttributes,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityLinkage),
     {SPV_OPERAND_TYPE_LITERAL_STRING, SPV_OPERAND_TYPE_LINKAGE_TYPE,
      SPV_OPERAND_TYPE_NONE}},
    {"NoContraction",
     SpvDecorationNoContraction,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"InputAttachmentIndex",
     SpvDecorationInputAttachmentIndex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityInputAttachment),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
    {"Alignment",
     SpvDecorationAlignment,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t builtInEntries[] = {
    {"Position",
     SpvBuiltInPosition,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"PointSize",
     SpvBuiltInPointSize,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"ClipDistance",
     SpvBuiltInClipDistance,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityClipDistance),
     {SPV_OPERAND_TYPE_NONE}},
    {"CullDistance",
     SpvBuiltInCullDistance,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityCullDistance),
     {SPV_OPERAND_TYPE_NONE}},
    {"VertexId",
     SpvBuiltInVertexId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"InstanceId",
     SpvBuiltInInstanceId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"PrimitiveId",
     SpvBuiltInPrimitiveId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry) |
         SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"InvocationId",
     SpvBuiltInInvocationId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry) |
         SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"Layer",
     SpvBuiltInLayer,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityGeometry),
     {SPV_OPERAND_TYPE_NONE}},
    {"ViewportIndex",
     SpvBuiltInViewportIndex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityMultiViewport),
     {SPV_OPERAND_TYPE_NONE}},
    {"TessLevelOuter",
     SpvBuiltInTessLevelOuter,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"TessLevelInner",
     SpvBuiltInTessLevelInner,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"TessCoord",
     SpvBuiltInTessCoord,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"PatchVertices",
     SpvBuiltInPatchVertices,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityTessellation),
     {SPV_OPERAND_TYPE_NONE}},
    {"FragCoord",
     SpvBuiltInFragCoord,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"PointCoord",
     SpvBuiltInPointCoord,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"FrontFacing",
     SpvBuiltInFrontFacing,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"SampleId",
     SpvBuiltInSampleId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampleRateShading),
     {SPV_OPERAND_TYPE_NONE}},
    {"SamplePosition",
     SpvBuiltInSamplePosition,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampleRateShading),
     {SPV_OPERAND_TYPE_NONE}},
    {"SampleMask",
     SpvBuiltInSampleMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilitySampleRateShading),
     {SPV_OPERAND_TYPE_NONE}},
    {"FragDepth",
     SpvBuiltInFragDepth,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"HelperInvocation",
     SpvBuiltInHelperInvocation,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"NumWorkgroups", SpvBuiltInNumWorkgroups, 0, {SPV_OPERAND_TYPE_NONE}},
    {"WorkgroupSize", SpvBuiltInWorkgroupSize, 0, {SPV_OPERAND_TYPE_NONE}},
    {"WorkgroupId", SpvBuiltInWorkgroupId, 0, {SPV_OPERAND_TYPE_NONE}},
    {"LocalInvocationId",
     SpvBuiltInLocalInvocationId,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"GlobalInvocationId",
     SpvBuiltInGlobalInvocationId,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"LocalInvocationIndex",
     SpvBuiltInLocalInvocationIndex,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"WorkDim",
     SpvBuiltInWorkDim,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"GlobalSize",
     SpvBuiltInGlobalSize,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"EnqueuedWorkgroupSize",
     SpvBuiltInEnqueuedWorkgroupSize,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"GlobalOffset",
     SpvBuiltInGlobalOffset,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"GlobalLinearId",
     SpvBuiltInGlobalLinearId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubgroupSize",
     SpvBuiltInSubgroupSize,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubgroupMaxSize",
     SpvBuiltInSubgroupMaxSize,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NumSubgroups",
     SpvBuiltInNumSubgroups,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"NumEnqueuedSubgroups",
     SpvBuiltInNumEnqueuedSubgroups,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubgroupId",
     SpvBuiltInSubgroupId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubgroupLocalInvocationId",
     SpvBuiltInSubgroupLocalInvocationId,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"VertexIndex",
     SpvBuiltInVertexIndex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"InstanceIndex",
     SpvBuiltInInstanceIndex,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t selectionControlEntries[] = {
    {"None", SpvSelectionControlMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Flatten", SpvSelectionControlFlattenMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"DontFlatten",
     SpvSelectionControlDontFlattenMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t loopControlEntries[] = {
    {"None", SpvLoopControlMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Unroll", SpvLoopControlUnrollMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"DontUnroll", SpvLoopControlDontUnrollMask, 0, {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t functionControlEntries[] = {
    {"None", SpvFunctionControlMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Inline", SpvFunctionControlInlineMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"DontInline",
     SpvFunctionControlDontInlineMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"Pure", SpvFunctionControlPureMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Const", SpvFunctionControlConstMask, 0, {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t memorySemanticsEntries[] = {
    // "Relaxed" should be a synonym for "None".
    // Put the Relaxed entry first so that the disassembler
    // will prefer to emit "Relaxed".
    {"Relaxed", SpvMemorySemanticsMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"None", SpvMemorySemanticsMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"SequentiallyConsistent",
     SpvMemorySemanticsSequentiallyConsistentMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"Acquire", SpvMemorySemanticsAcquireMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Release", SpvMemorySemanticsReleaseMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {"AcquireRelease",
     SpvMemorySemanticsAcquireReleaseMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"UniformMemory",
     SpvMemorySemanticsUniformMemoryMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityShader),
     {SPV_OPERAND_TYPE_NONE}},
    {"SubgroupMemory",
     SpvMemorySemanticsSubgroupMemoryMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"WorkgroupMemory",
     SpvMemorySemanticsWorkgroupMemoryMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"CrossWorkgroupMemory",
     SpvMemorySemanticsCrossWorkgroupMemoryMask,
     0,
     {SPV_OPERAND_TYPE_NONE}},
    {"AtomicCounterMemory",
     SpvMemorySemanticsAtomicCounterMemoryMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityAtomicStorage),
     {SPV_OPERAND_TYPE_NONE}},
    {
        "ImageMemory",
        SpvMemorySemanticsImageMemoryMask,
        0,
        {SPV_OPERAND_TYPE_NONE},
    },
};

static const spv_operand_desc_t memoryAccessEntries[] = {
    {"None", SpvMemoryAccessMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Volatile", SpvMemoryAccessVolatileMask, 0, {SPV_OPERAND_TYPE_NONE}},
    {
        "Aligned",
        SpvMemoryAccessAlignedMask,
        0,
        {SPV_OPERAND_TYPE_LITERAL_INTEGER, SPV_OPERAND_TYPE_NONE},
    },
    {"Nontemporal", SpvMemoryAccessNontemporalMask, 0, {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t scopeEntries[] = {
    {"CrossDevice", SpvScopeCrossDevice, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Device", SpvScopeDevice, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Workgroup", SpvScopeWorkgroup, 0, {SPV_OPERAND_TYPE_NONE}},
    {"Subgroup", SpvScopeSubgroup, 0, {SPV_OPERAND_TYPE_NONE}},
    {
        "Invocation", SpvScopeInvocation, 0, {SPV_OPERAND_TYPE_NONE},
    },
};

static const spv_operand_desc_t groupOperationEntries[] = {
    {"Reduce",
     SpvGroupOperationReduce,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"InclusiveScan",
     SpvGroupOperationInclusiveScan,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"ExclusiveScan",
     SpvGroupOperationExclusiveScan,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t kernelKernelEnqueueFlagssEntries[] = {
    {"NoWait",
     SpvKernelEnqueueFlagsNoWait,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"WaitKernel",
     SpvKernelEnqueueFlagsWaitKernel,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
    {"WaitWorkGroup",
     SpvKernelEnqueueFlagsWaitWorkGroup,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

static const spv_operand_desc_t kernelProfilingInfoEntries[] = {
    {"None", SpvKernelProfilingInfoMaskNone, 0, {SPV_OPERAND_TYPE_NONE}},
    {"CmdExecTime",
     SpvKernelProfilingInfoCmdExecTimeMask,
     SPV_CAPABILITY_AS_MASK(SpvCapabilityKernel),
     {SPV_OPERAND_TYPE_NONE}},
};

// A macro for defining a capability that doesn't depend on another capability.
#define CASE(NAME)                                           \
  {                                                          \
    #NAME, SpvCapability##NAME, 0, { SPV_OPERAND_TYPE_NONE } \
  }

// A macro for defining a capability that depends on another.
#define CASE_CAP(NAME, CAP)                                                   \
  {                                                                           \
    #NAME, SpvCapability##NAME, SPV_CAPABILITY_AS_MASK(SpvCapability##CAP), { \
      SPV_OPERAND_TYPE_NONE                                                   \
    }                                                                         \
  }

// clang-format off
static const spv_operand_desc_t capabilityInfoEntries[] = {
    CASE(Matrix),
    CASE_CAP(Shader, Matrix),
    CASE_CAP(Geometry, Shader),
    CASE_CAP(Tessellation, Shader),
    CASE(Addresses),
    CASE(Linkage),
    CASE(Kernel),
    CASE_CAP(Vector16, Kernel),
    CASE_CAP(Float16Buffer, Kernel),
    CASE(Float16),
    CASE(Float64),
    CASE(Int64),
    CASE_CAP(Int64Atomics, Int64),
    CASE_CAP(ImageBasic, Kernel),
    CASE_CAP(ImageReadWrite, ImageBasic),
    CASE_CAP(ImageMipmap, ImageBasic),
    // Value 16 intentionally missing.
    CASE_CAP(Pipes, Kernel),
    CASE(Groups),
    CASE_CAP(DeviceEnqueue, Kernel),
    CASE_CAP(LiteralSampler, Kernel),
    CASE_CAP(AtomicStorage, Shader),
    CASE(Int16),
    CASE_CAP(TessellationPointSize, Tessellation),
    CASE_CAP(GeometryPointSize, Geometry),
    CASE_CAP(ImageGatherExtended, Shader),
    // Value 26 intentionally missing.
    CASE_CAP(StorageImageMultisample, Shader),
    CASE_CAP(UniformBufferArrayDynamicIndexing, Shader),
    CASE_CAP(SampledImageArrayDynamicIndexing, Shader),
    CASE_CAP(StorageBufferArrayDynamicIndexing, Shader),
    CASE_CAP(StorageImageArrayDynamicIndexing, Shader),
    CASE_CAP(ClipDistance, Shader),
    CASE_CAP(CullDistance, Shader),
    CASE_CAP(ImageCubeArray, SampledCubeArray),
    CASE_CAP(SampleRateShading, Shader),
    CASE_CAP(ImageRect, SampledRect),
    CASE_CAP(SampledRect, Shader),
    CASE_CAP(GenericPointer, Addresses),
    CASE_CAP(Int8, Kernel),
    CASE_CAP(InputAttachment, Shader),
    CASE_CAP(SparseResidency, Shader),
    CASE_CAP(MinLod, Shader),
    CASE_CAP(Sampled1D, Shader),
    CASE_CAP(Image1D, Sampled1D),
    CASE_CAP(SampledCubeArray, Shader),
    CASE_CAP(SampledBuffer, Shader),
    CASE_CAP(ImageBuffer, SampledBuffer),
    CASE_CAP(ImageMSArray, Shader),
    CASE_CAP(StorageImageExtendedFormats, Shader),
    CASE_CAP(ImageQuery, Shader),
    CASE_CAP(DerivativeControl, Shader),
    CASE_CAP(InterpolationFunction, Shader),
    CASE_CAP(TransformFeedback, Shader),
    CASE_CAP(GeometryStreams, Geometry),
    CASE_CAP(StorageImageReadWithoutFormat, Shader),
    CASE_CAP(StorageImageWriteWithoutFormat, Shader),
    CASE_CAP(MultiViewport, Geometry),
};
// clang-format on
#undef CASE
#undef CASE_CAP

// Evaluates to the number of elements of array A.
// If we could use constexpr, then we could make this a template function.
// If the source arrays were std::array, then we could have used
// std::array::size.
#define ARRAY_SIZE(A) (static_cast<uint32_t>(sizeof(A) / sizeof(A[0])))

static const spv_operand_desc_group_t opcodeEntryTypes[] = {
    // TODO(dneto): Reformat this table.
    {SPV_OPERAND_TYPE_SOURCE_LANGUAGE, ARRAY_SIZE(sourceLanguageEntries),
     sourceLanguageEntries},
    {SPV_OPERAND_TYPE_EXECUTION_MODEL, ARRAY_SIZE(executionModelEntries),
     executionModelEntries},
    {SPV_OPERAND_TYPE_ADDRESSING_MODEL, ARRAY_SIZE(addressingModelEntries),
     addressingModelEntries},
    {SPV_OPERAND_TYPE_MEMORY_MODEL, ARRAY_SIZE(memoryModelEntries),
     memoryModelEntries},
    {SPV_OPERAND_TYPE_EXECUTION_MODE, ARRAY_SIZE(executionModeEntries),
     executionModeEntries},
    {SPV_OPERAND_TYPE_STORAGE_CLASS, ARRAY_SIZE(storageClassEntries),
     storageClassEntries},
    {SPV_OPERAND_TYPE_DIMENSIONALITY, ARRAY_SIZE(dimensionalityEntries),
     dimensionalityEntries},
    {SPV_OPERAND_TYPE_SAMPLER_ADDRESSING_MODE,
     ARRAY_SIZE(samplerAddressingModeEntries), samplerAddressingModeEntries},
    {SPV_OPERAND_TYPE_SAMPLER_FILTER_MODE, ARRAY_SIZE(samplerFilterModeEntries),
     samplerFilterModeEntries},
    {SPV_OPERAND_TYPE_SAMPLER_IMAGE_FORMAT,
     ARRAY_SIZE(samplerImageFormatEntries), samplerImageFormatEntries},
    {SPV_OPERAND_TYPE_IMAGE_CHANNEL_ORDER, ARRAY_SIZE(imageChannelOrderEntries),
     imageChannelOrderEntries},
    {SPV_OPERAND_TYPE_IMAGE_CHANNEL_DATA_TYPE,
     ARRAY_SIZE(imageChannelDataTypeEntries), imageChannelDataTypeEntries},
    {SPV_OPERAND_TYPE_IMAGE, ARRAY_SIZE(imageOperandEntries),
     imageOperandEntries},
    {SPV_OPERAND_TYPE_OPTIONAL_IMAGE,  // Same as *_IMAGE
     ARRAY_SIZE(imageOperandEntries), imageOperandEntries},
    {SPV_OPERAND_TYPE_FP_FAST_MATH_MODE, ARRAY_SIZE(fpFastMathModeEntries),
     fpFastMathModeEntries},
    {SPV_OPERAND_TYPE_FP_ROUNDING_MODE, ARRAY_SIZE(fpRoundingModeEntries),
     fpRoundingModeEntries},
    {SPV_OPERAND_TYPE_LINKAGE_TYPE, ARRAY_SIZE(linkageTypeEntries),
     linkageTypeEntries},
    {SPV_OPERAND_TYPE_ACCESS_QUALIFIER, ARRAY_SIZE(accessQualifierEntries),
     accessQualifierEntries},
    {SPV_OPERAND_TYPE_OPTIONAL_ACCESS_QUALIFIER,  // Same as *_ACCESS_QUALIFIER
     ARRAY_SIZE(accessQualifierEntries), accessQualifierEntries},
    {SPV_OPERAND_TYPE_FUNCTION_PARAMETER_ATTRIBUTE,
     ARRAY_SIZE(functionParameterAttributeEntries),
     functionParameterAttributeEntries},
    {SPV_OPERAND_TYPE_DECORATION, ARRAY_SIZE(decorationEntries),
     decorationEntries},
    {SPV_OPERAND_TYPE_BUILT_IN, ARRAY_SIZE(builtInEntries), builtInEntries},
    {SPV_OPERAND_TYPE_SELECTION_CONTROL, ARRAY_SIZE(selectionControlEntries),
     selectionControlEntries},
    {SPV_OPERAND_TYPE_LOOP_CONTROL, ARRAY_SIZE(loopControlEntries),
     loopControlEntries},
    {SPV_OPERAND_TYPE_FUNCTION_CONTROL, ARRAY_SIZE(functionControlEntries),
     functionControlEntries},
    {SPV_OPERAND_TYPE_MEMORY_SEMANTICS_ID, ARRAY_SIZE(memorySemanticsEntries),
     memorySemanticsEntries},
    {SPV_OPERAND_TYPE_MEMORY_ACCESS, ARRAY_SIZE(memoryAccessEntries),
     memoryAccessEntries},
    {SPV_OPERAND_TYPE_OPTIONAL_MEMORY_ACCESS,  // Same as *_MEMORY_ACCESS
     ARRAY_SIZE(memoryAccessEntries), memoryAccessEntries},
    {SPV_OPERAND_TYPE_SCOPE_ID, ARRAY_SIZE(scopeEntries), scopeEntries},
    {SPV_OPERAND_TYPE_GROUP_OPERATION, ARRAY_SIZE(groupOperationEntries),
     groupOperationEntries},
    {SPV_OPERAND_TYPE_KERNEL_ENQ_FLAGS,
     ARRAY_SIZE(kernelKernelEnqueueFlagssEntries),
     kernelKernelEnqueueFlagssEntries},
    {SPV_OPERAND_TYPE_KERNEL_PROFILING_INFO,
     ARRAY_SIZE(kernelProfilingInfoEntries), kernelProfilingInfoEntries},
    {SPV_OPERAND_TYPE_CAPABILITY, ARRAY_SIZE(capabilityInfoEntries),
     capabilityInfoEntries},
};

spv_result_t spvOperandTableGet(spv_operand_table* pOperandTable) {
  if (!pOperandTable) return SPV_ERROR_INVALID_POINTER;

  static const spv_operand_table_t table = {ARRAY_SIZE(opcodeEntryTypes),
                                            opcodeEntryTypes};

  *pOperandTable = &table;

  return SPV_SUCCESS;
}

#undef ARRAY_SIZE

spv_result_t spvOperandTableNameLookup(const spv_operand_table table,
                                       const spv_operand_type_t type,
                                       const char* name,
                                       const size_t nameLength,
                                       spv_operand_desc* pEntry) {
  if (!table) return SPV_ERROR_INVALID_TABLE;
  if (!name || !pEntry) return SPV_ERROR_INVALID_POINTER;

  for (uint64_t typeIndex = 0; typeIndex < table->count; ++typeIndex) {
    if (type == table->types[typeIndex].type) {
      for (uint64_t operandIndex = 0;
           operandIndex < table->types[typeIndex].count; ++operandIndex) {
        if (nameLength ==
                strlen(table->types[typeIndex].entries[operandIndex].name) &&
            !strncmp(table->types[typeIndex].entries[operandIndex].name, name,
                     nameLength)) {
          *pEntry = &table->types[typeIndex].entries[operandIndex];
          return SPV_SUCCESS;
        }
      }
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}

spv_result_t spvOperandTableValueLookup(const spv_operand_table table,
                                        const spv_operand_type_t type,
                                        const uint32_t value,
                                        spv_operand_desc* pEntry) {
  if (!table) return SPV_ERROR_INVALID_TABLE;
  if (!pEntry) return SPV_ERROR_INVALID_POINTER;

  for (uint64_t typeIndex = 0; typeIndex < table->count; ++typeIndex) {
    if (type == table->types[typeIndex].type) {
      for (uint64_t operandIndex = 0;
           operandIndex < table->types[typeIndex].count; ++operandIndex) {
        if (value == table->types[typeIndex].entries[operandIndex].value) {
          *pEntry = &table->types[typeIndex].entries[operandIndex];
          return SPV_SUCCESS;
        }
      }
    }
  }

  return SPV_ERROR_INVALID_LOOKUP;
}

const char* spvOperandTypeStr(spv_operand_type_t type) {
  switch (type) {
    case SPV_OPERAND_TYPE_ID:
    case SPV_OPERAND_TYPE_OPTIONAL_ID:
      return "ID";
    case SPV_OPERAND_TYPE_TYPE_ID:
      return "type ID";
    case SPV_OPERAND_TYPE_RESULT_ID:
      return "result ID";
    case SPV_OPERAND_TYPE_LITERAL_INTEGER:
    case SPV_OPERAND_TYPE_OPTIONAL_LITERAL_INTEGER:
    case SPV_OPERAND_TYPE_OPTIONAL_LITERAL_NUMBER:
      return "literal number";
    case SPV_OPERAND_TYPE_OPTIONAL_TYPED_LITERAL_INTEGER:
      return "possibly multi-word literal integer";
    case SPV_OPERAND_TYPE_TYPED_LITERAL_NUMBER:
      return "possibly multi-word literal number";
    case SPV_OPERAND_TYPE_EXTENSION_INSTRUCTION_NUMBER:
      return "extension instruction number";
    case SPV_OPERAND_TYPE_SPEC_CONSTANT_OP_NUMBER:
      return "OpSpecConstantOp opcode";
    case SPV_OPERAND_TYPE_LITERAL_STRING:
    case SPV_OPERAND_TYPE_OPTIONAL_LITERAL_STRING:
      return "literal string";
    case SPV_OPERAND_TYPE_SOURCE_LANGUAGE:
      return "source language";
    case SPV_OPERAND_TYPE_EXECUTION_MODEL:
      return "execution model";
    case SPV_OPERAND_TYPE_ADDRESSING_MODEL:
      return "addressing model";
    case SPV_OPERAND_TYPE_MEMORY_MODEL:
      return "memory model";
    case SPV_OPERAND_TYPE_EXECUTION_MODE:
      return "execution mode";
    case SPV_OPERAND_TYPE_STORAGE_CLASS:
      return "storage class";
    case SPV_OPERAND_TYPE_DIMENSIONALITY:
      return "dimensionality";
    case SPV_OPERAND_TYPE_SAMPLER_ADDRESSING_MODE:
      return "sampler addressing mode";
    case SPV_OPERAND_TYPE_SAMPLER_FILTER_MODE:
      return "sampler filter mode";
    case SPV_OPERAND_TYPE_SAMPLER_IMAGE_FORMAT:
      return "image format";
    case SPV_OPERAND_TYPE_FP_FAST_MATH_MODE:
      return "floating-point fast math mode";
    case SPV_OPERAND_TYPE_FP_ROUNDING_MODE:
      return "floating-point rounding mode";
    case SPV_OPERAND_TYPE_LINKAGE_TYPE:
      return "linkage type";
    case SPV_OPERAND_TYPE_ACCESS_QUALIFIER:
    case SPV_OPERAND_TYPE_OPTIONAL_ACCESS_QUALIFIER:
      return "access qualifier";
    case SPV_OPERAND_TYPE_FUNCTION_PARAMETER_ATTRIBUTE:
      return "function parameter attribute";
    case SPV_OPERAND_TYPE_DECORATION:
      return "decoration";
    case SPV_OPERAND_TYPE_BUILT_IN:
      return "built-in";
    case SPV_OPERAND_TYPE_SELECTION_CONTROL:
      return "selection control";
    case SPV_OPERAND_TYPE_LOOP_CONTROL:
      return "loop control";
    case SPV_OPERAND_TYPE_FUNCTION_CONTROL:
      return "function control";
    case SPV_OPERAND_TYPE_MEMORY_SEMANTICS_ID:
      return "memory semantics ID";
    case SPV_OPERAND_TYPE_MEMORY_ACCESS:
    case SPV_OPERAND_TYPE_OPTIONAL_MEMORY_ACCESS:
      return "memory access";
    case SPV_OPERAND_TYPE_SCOPE_ID:
      return "scope ID";
    case SPV_OPERAND_TYPE_GROUP_OPERATION:
      return "group operation";
    case SPV_OPERAND_TYPE_KERNEL_ENQ_FLAGS:
      return "kernel enqeue flags";
    case SPV_OPERAND_TYPE_KERNEL_PROFILING_INFO:
      return "kernel profiling info";
    case SPV_OPERAND_TYPE_CAPABILITY:
      return "capability";
    case SPV_OPERAND_TYPE_IMAGE:
    case SPV_OPERAND_TYPE_OPTIONAL_IMAGE:
      return "image";
    case SPV_OPERAND_TYPE_OPTIONAL_CIV:
      return "context-insensitive value";

    // The next values are for values returned from an instruction, not actually
    // an operand.  So the specific strings don't matter.  But let's add them
    // for completeness and ease of testing.
    case SPV_OPERAND_TYPE_IMAGE_CHANNEL_ORDER:
      return "image channel order";
    case SPV_OPERAND_TYPE_IMAGE_CHANNEL_DATA_TYPE:
      return "image channel data type";

    case SPV_OPERAND_TYPE_NONE:
      return "NONE";
    default:
      assert(0 && "Unhandled operand type!");
      break;
  }
  return "unknown";
}

void spvPrependOperandTypes(const spv_operand_type_t* types,
                            spv_operand_pattern_t* pattern) {
  const spv_operand_type_t* endTypes;
  for (endTypes = types; *endTypes != SPV_OPERAND_TYPE_NONE; ++endTypes)
    ;
  pattern->insert(pattern->begin(), types, endTypes);
}

void spvPrependOperandTypesForMask(const spv_operand_table operandTable,
                                   const spv_operand_type_t type,
                                   const uint32_t mask,
                                   spv_operand_pattern_t* pattern) {
  // Scan from highest bits to lowest bits because we will prepend in LIFO
  // fashion, and we need the operands for lower order bits to appear first.
  for (uint32_t candidate_bit = (1 << 31); candidate_bit; candidate_bit >>= 1) {
    if (candidate_bit & mask) {
      spv_operand_desc entry = nullptr;
      if (SPV_SUCCESS == spvOperandTableValueLookup(operandTable, type,
                                                    candidate_bit, &entry)) {
        spvPrependOperandTypes(entry->operandTypes, pattern);
      }
    }
  }
}

bool spvOperandIsConcreteMask(spv_operand_type_t type) {
  return SPV_OPERAND_TYPE_FIRST_CONCRETE_MASK_TYPE <= type &&
         type <= SPV_OPERAND_TYPE_LAST_CONCRETE_MASK_TYPE;
}

bool spvOperandIsOptional(spv_operand_type_t type) {
  return SPV_OPERAND_TYPE_FIRST_OPTIONAL_TYPE <= type &&
         type <= SPV_OPERAND_TYPE_LAST_OPTIONAL_TYPE;
}

bool spvOperandIsVariable(spv_operand_type_t type) {
  return SPV_OPERAND_TYPE_FIRST_VARIABLE_TYPE <= type &&
         type <= SPV_OPERAND_TYPE_LAST_VARIABLE_TYPE;
}

bool spvExpandOperandSequenceOnce(spv_operand_type_t type,
                                  spv_operand_pattern_t* pattern) {
  switch (type) {
    case SPV_OPERAND_TYPE_VARIABLE_ID:
      pattern->insert(pattern->begin(), {SPV_OPERAND_TYPE_OPTIONAL_ID, type});
      return true;
    case SPV_OPERAND_TYPE_VARIABLE_LITERAL_INTEGER:
      pattern->insert(pattern->begin(),
                      {SPV_OPERAND_TYPE_OPTIONAL_LITERAL_INTEGER, type});
      return true;
    case SPV_OPERAND_TYPE_VARIABLE_LITERAL_INTEGER_ID:
      // Represents Zero or more (Literal number, Id) pairs,
      // where the literal number must be a scalar integer.
      pattern->insert(pattern->begin(),
                      {SPV_OPERAND_TYPE_OPTIONAL_TYPED_LITERAL_INTEGER,
                       SPV_OPERAND_TYPE_ID, type});
      return true;
    case SPV_OPERAND_TYPE_VARIABLE_ID_LITERAL_INTEGER:
      // Represents Zero or more (Id, Literal number) pairs.
      pattern->insert(pattern->begin(),
                      {SPV_OPERAND_TYPE_OPTIONAL_ID,
                       SPV_OPERAND_TYPE_LITERAL_INTEGER, type});
      return true;
    default:
      break;
  }
  return false;
}

spv_operand_type_t spvTakeFirstMatchableOperand(
    spv_operand_pattern_t* pattern) {
  assert(!pattern->empty());
  spv_operand_type_t result;
  do {
    result = pattern->front();
    pattern->pop_front();
  } while (spvExpandOperandSequenceOnce(result, pattern));
  return result;
}

spv_operand_pattern_t spvAlternatePatternFollowingImmediate(
    const spv_operand_pattern_t& pattern) {
  spv_operand_pattern_t alternatePattern;
  for (const auto& operand : pattern) {
    if (operand == SPV_OPERAND_TYPE_RESULT_ID) {
      alternatePattern.push_back(operand);
      alternatePattern.push_back(SPV_OPERAND_TYPE_OPTIONAL_CIV);
      return alternatePattern;
    }
    alternatePattern.push_back(SPV_OPERAND_TYPE_OPTIONAL_CIV);
  }
  // No result-id found, so just expect CIVs.
  return {SPV_OPERAND_TYPE_OPTIONAL_CIV};
}

bool spvIsIdType(spv_operand_type_t type) {
  switch (type) {
    case SPV_OPERAND_TYPE_ID:
    case SPV_OPERAND_TYPE_TYPE_ID:
    case SPV_OPERAND_TYPE_RESULT_ID:
    case SPV_OPERAND_TYPE_MEMORY_SEMANTICS_ID:
    case SPV_OPERAND_TYPE_SCOPE_ID:
      return true;
    default:
      return false;
  }
  return false;
}
