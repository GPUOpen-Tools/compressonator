//
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

/**
***********************************************************************************************************************
* @file  ags_shader_intrinsics_dx12.hlsl
* @brief
*    AMD D3D Shader Intrinsics HLSL include file.
*    This include file contains the Shader Intrinsics definitions used in shader code by the application.
* @note
*    This does not work with immediate values or values that the compiler determines can produces denorms
*
***********************************************************************************************************************
*/

#ifndef _AMDEXTD3DSHADERINTRINICS_HLSL
#define _AMDEXTD3DSHADERINTRINICS_HLSL
// AMD shader intrinsics designated SpaceId.  Denotes Texture3D resource and static sampler used in conjuction with
// instrinsic instructions.
#define AmdExtD3DShaderIntrinsicsSpaceId space2147420894
// Texture3D and SamplerState used to access AMD shader instrinsics instruction set.
// Applications need to add descriptor table entries for these when creating root descriptor table.
///@note Requires SM 5.1 RWBuffer<uint>
RWByteAddressBuffer AmdExtD3DShaderIntrinsicsUAV : register(u0, AmdExtD3DShaderIntrinsicsSpaceId);

/**
***********************************************************************************************************************
*   Definitions to construct the intrinsic instruction composed of an opcode and optional immediate data.
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsics_MagicCodeShift   28
#define AmdExtD3DShaderIntrinsics_MagicCodeMask    0xf
#define AmdExtD3DShaderIntrinsics_OpcodePhaseShift 24
#define AmdExtD3DShaderIntrinsics_OpcodePhaseMask  0x3
#define AmdExtD3DShaderIntrinsics_DataShift        8
#define AmdExtD3DShaderIntrinsics_DataMask         0xffff
#define AmdExtD3DShaderIntrinsics_OpcodeShift      0
#define AmdExtD3DShaderIntrinsics_OpcodeMask       0xff

#define AmdExtD3DShaderIntrinsics_MagicCode        0x5


/**
***********************************************************************************************************************
*   Intrinsic opcodes.
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsicsOpcode_Readfirstlane  0x01
#define AmdExtD3DShaderIntrinsicsOpcode_Readlane       0x02
#define AmdExtD3DShaderIntrinsicsOpcode_LaneId         0x03
#define AmdExtD3DShaderIntrinsicsOpcode_Swizzle        0x04
#define AmdExtD3DShaderIntrinsicsOpcode_Ballot         0x05
#define AmdExtD3DShaderIntrinsicsOpcode_MBCnt          0x06
#define AmdExtD3DShaderIntrinsicsOpcode_Min3U          0x07
#define AmdExtD3DShaderIntrinsicsOpcode_Min3F          0x08
#define AmdExtD3DShaderIntrinsicsOpcode_Med3U          0x09
#define AmdExtD3DShaderIntrinsicsOpcode_Med3F          0x0a
#define AmdExtD3DShaderIntrinsicsOpcode_Max3U          0x0b
#define AmdExtD3DShaderIntrinsicsOpcode_Max3F          0x0c
#define AmdExtD3DShaderIntrinsicsOpcode_BaryCoord      0x0d
#define AmdExtD3DShaderIntrinsicsOpcode_VtxParam       0x0e


/**
***********************************************************************************************************************
*   Intrinsic opcode phases.
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsicsOpcodePhase_0    0x0
#define AmdExtD3DShaderIntrinsicsOpcodePhase_1    0x1
#define AmdExtD3DShaderIntrinsicsOpcodePhase_2    0x2
#define AmdExtD3DShaderIntrinsicsOpcodePhase_3    0x3

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsicsSwizzle defines for common swizzles.  Can be used as the operation parameter for the
*   AmdExtD3DShaderIntrinsics_Swizzle intrinsic.
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsicsSwizzle_SwapX1      0x041f
#define AmdExtD3DShaderIntrinsicsSwizzle_SwapX2      0x081f
#define AmdExtD3DShaderIntrinsicsSwizzle_SwapX4      0x101f
#define AmdExtD3DShaderIntrinsicsSwizzle_SwapX8      0x201f
#define AmdExtD3DShaderIntrinsicsSwizzle_SwapX16     0x401f
#define AmdExtD3DShaderIntrinsicsSwizzle_ReverseX2   0x041f
#define AmdExtD3DShaderIntrinsicsSwizzle_ReverseX4   0x0c1f
#define AmdExtD3DShaderIntrinsicsSwizzle_ReverseX8   0x1c1f
#define AmdExtD3DShaderIntrinsicsSwizzle_ReverseX16  0x3c1f
#define AmdExtD3DShaderIntrinsicsSwizzle_ReverseX32  0x7c1f
#define AmdExtD3DShaderIntrinsicsSwizzle_BCastX2     0x003e
#define AmdExtD3DShaderIntrinsicsSwizzle_BCastX4     0x003c
#define AmdExtD3DShaderIntrinsicsSwizzle_BCastX8     0x0038
#define AmdExtD3DShaderIntrinsicsSwizzle_BCastX16    0x0030
#define AmdExtD3DShaderIntrinsicsSwizzle_BCastX32    0x0020


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsicsBarycentric defines for barycentric interpolation mode.  To be used with
*   AmdExtD3DShaderIntrinsicsOpcode_IjBarycentricCoords to specify the interpolation mode.
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsicsBarycentric_LinearCenter    0x1
#define AmdExtD3DShaderIntrinsicsBarycentric_LinearCentroid  0x2
#define AmdExtD3DShaderIntrinsicsBarycentric_LinearSample    0x3
#define AmdExtD3DShaderIntrinsicsBarycentric_PerspCenter     0x4
#define AmdExtD3DShaderIntrinsicsBarycentric_PerspCentroid   0x5
#define AmdExtD3DShaderIntrinsicsBarycentric_PerspSample     0x6
#define AmdExtD3DShaderIntrinsicsBarycentric_PerspPullModel  0x7

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsicsBarycentric defines for specifying vertex and parameter indices.  To be used as inputs to
*   the AmdExtD3DShaderIntrinsicsOpcode_VertexParameter function
***********************************************************************************************************************
*/
#define AmdExtD3DShaderIntrinsicsBarycentric_Vertex0        0x0
#define AmdExtD3DShaderIntrinsicsBarycentric_Vertex1        0x1
#define AmdExtD3DShaderIntrinsicsBarycentric_Vertex2        0x2

#define AmdExtD3DShaderIntrinsicsBarycentric_Param0         0x00
#define AmdExtD3DShaderIntrinsicsBarycentric_Param1         0x01
#define AmdExtD3DShaderIntrinsicsBarycentric_Param2         0x02
#define AmdExtD3DShaderIntrinsicsBarycentric_Param3         0x03
#define AmdExtD3DShaderIntrinsicsBarycentric_Param4         0x04
#define AmdExtD3DShaderIntrinsicsBarycentric_Param5         0x05
#define AmdExtD3DShaderIntrinsicsBarycentric_Param6         0x06
#define AmdExtD3DShaderIntrinsicsBarycentric_Param7         0x07
#define AmdExtD3DShaderIntrinsicsBarycentric_Param8         0x08
#define AmdExtD3DShaderIntrinsicsBarycentric_Param9         0x09
#define AmdExtD3DShaderIntrinsicsBarycentric_Param10        0x0a
#define AmdExtD3DShaderIntrinsicsBarycentric_Param11        0x0b
#define AmdExtD3DShaderIntrinsicsBarycentric_Param12        0x0c
#define AmdExtD3DShaderIntrinsicsBarycentric_Param13        0x0d
#define AmdExtD3DShaderIntrinsicsBarycentric_Param14        0x0e
#define AmdExtD3DShaderIntrinsicsBarycentric_Param15        0x0f
#define AmdExtD3DShaderIntrinsicsBarycentric_Param16        0x10
#define AmdExtD3DShaderIntrinsicsBarycentric_Param17        0x11
#define AmdExtD3DShaderIntrinsicsBarycentric_Param18        0x12
#define AmdExtD3DShaderIntrinsicsBarycentric_Param19        0x13
#define AmdExtD3DShaderIntrinsicsBarycentric_Param20        0x14
#define AmdExtD3DShaderIntrinsicsBarycentric_Param21        0x15
#define AmdExtD3DShaderIntrinsicsBarycentric_Param22        0x16
#define AmdExtD3DShaderIntrinsicsBarycentric_Param23        0x17
#define AmdExtD3DShaderIntrinsicsBarycentric_Param24        0x18
#define AmdExtD3DShaderIntrinsicsBarycentric_Param25        0x19
#define AmdExtD3DShaderIntrinsicsBarycentric_Param26        0x1a
#define AmdExtD3DShaderIntrinsicsBarycentric_Param27        0x1b
#define AmdExtD3DShaderIntrinsicsBarycentric_Param28        0x1c
#define AmdExtD3DShaderIntrinsicsBarycentric_Param29        0x1d
#define AmdExtD3DShaderIntrinsicsBarycentric_Param30        0x1e
#define AmdExtD3DShaderIntrinsicsBarycentric_Param31        0x1f

#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentX     0x0
#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentY     0x1
#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentZ     0x2
#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentW     0x3

#define AmdExtD3DShaderIntrinsicsBarycentric_ParamShift     0
#define AmdExtD3DShaderIntrinsicsBarycentric_ParamMask      0x1f
#define AmdExtD3DShaderIntrinsicsBarycentric_VtxShift       0x5
#define AmdExtD3DShaderIntrinsicsBarycentric_VtxMask        0x3
#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift 0x7
#define AmdExtD3DShaderIntrinsicsBarycentric_ComponentMask  0x3


/**
***********************************************************************************************************************
*   MakeAmdShaderIntrinsicsInstruction
*
*   Creates instruction from supplied opcode and immediate data.
*   NOTE: This is an internal function and should not be called by the source HLSL shader directly.
*
***********************************************************************************************************************
*/
uint MakeAmdShaderIntrinsicsInstruction(uint opcode, uint opcodePhase, uint immediateData)
{
    return ((AmdExtD3DShaderIntrinsics_MagicCode << AmdExtD3DShaderIntrinsics_MagicCodeShift) |
            (immediateData << AmdExtD3DShaderIntrinsics_DataShift) |
            (opcodePhase << AmdExtD3DShaderIntrinsics_OpcodePhaseShift) |
            (opcode << AmdExtD3DShaderIntrinsics_OpcodeShift));
}


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_ReadfirstlaneF
*
*   Returns the value of float src for the first active lane of the wavefront.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Readfirstlane) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_ReadfirstlaneF(float src)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Readfirstlane, 0, 0);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, asuint(src), 0, retVal);
    return asfloat(retVal);
}


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_ReadfirstlaneU
*
*   Returns the value of unsigned integer src for the first active lane of the wavefront.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Readfirstlane) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_ReadfirstlaneU(uint src)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Readfirstlane, 0, 0);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, src, 0, retVal);
    return retVal;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Readlane
*
*   Returns the value of float src for the lane within the wavefront specified by laneId.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Readlane) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_ReadlaneF(float src, uint laneId)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Readlane, 0, laneId);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, asuint(src), 0, retVal);
    return asfloat(retVal);
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_ReadlaneU
*
*   Returns the value of unsigned integer src for the lane within the wavefront specified by laneId.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Readlane) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_ReadlaneU(uint src, uint laneId)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Readlane, 0, laneId);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, src, 0, retVal);
    return retVal;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_LaneId
*
*   Returns the current lane id for the thread within the wavefront.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_LaneId) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_LaneId()
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_LaneId, 0, 0);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, 0, 0, retVal);
    return retVal;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Swizzle
*
*   Generic instruction to shuffle the float src value among different lanes as specified by the operation.
*   Note that the operation parameter must be an immediately specified value not a value from a variable.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Swizzle) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_SwizzleF(float src, uint operation)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Swizzle, 0, operation);

    uint retVal;
    //InterlockedCompareExchange(AmdExtD3DShaderIntrinsicsUAV[instruction], asuint(src), 0, retVal);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, asuint(src), 0, retVal);
    return asfloat(retVal);
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_SwizzleU
*
*   Generic instruction to shuffle the unsigned integer src value among different lanes as specified by the operation.
*   Note that the operation parameter must be an immediately specified value not a value from a variable.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Swizzle) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_SwizzleU(uint src, uint operation)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Swizzle, 0, operation);

    uint retVal;
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, src, 0, retVal);
    return retVal;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Ballot
*
*   Given an input predicate returns a bit mask indicating for which lanes the predicate is true.
*   Inactive or non-existent lanes will always return 0.  The number of existent lanes is the wavefront size.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Ballot) returned S_OK.
*
***********************************************************************************************************************
*/
uint2 AmdExtD3DShaderIntrinsics_Ballot(bool predicate)
{
    uint instruction;

    uint retVal1;
    instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Ballot,
                                                     AmdExtD3DShaderIntrinsicsOpcodePhase_0, 0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, predicate, 0, retVal1);

    uint retVal2;
    instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Ballot,
                                                     AmdExtD3DShaderIntrinsicsOpcodePhase_1, 0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, predicate, 0, retVal2);

    return uint2(retVal1, retVal2);
}


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_BallotAny
*
*   Convenience routine that uses Ballot and returns true if for any of the active lanes the predicate is true.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Ballot) returned S_OK.
*
***********************************************************************************************************************
*/
bool AmdExtD3DShaderIntrinsics_BallotAny(bool predicate)
{
    uint2 retVal = AmdExtD3DShaderIntrinsics_Ballot(predicate);

    return ((retVal.x | retVal.y) != 0 ? true : false);
}


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_BallotAll
*
*   Convenience routine that uses Ballot and returns true if for all of the active lanes the predicate is true.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Ballot) returned S_OK.
*
***********************************************************************************************************************
*/
bool AmdExtD3DShaderIntrinsics_BallotAll(bool predicate)
{
    uint2 ballot = AmdExtD3DShaderIntrinsics_Ballot(predicate);

    uint2 execMask = AmdExtD3DShaderIntrinsics_Ballot(true);

    return ((ballot.x == execMask.x) && (ballot.y == execMask.y));
}


/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_MBCnt
*
*   Returns the masked bit count of the source register for this thread within all the active threads within a
*   wavefront.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_MBCnt) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_MBCnt(uint2 src)
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_MBCnt, 0, 0);

    uint retVal;

    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, src.x, src.y, retVal);

    return retVal;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Min3F
*
*   Returns the minimum value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_Min3F(float src0, float src1, float src2)
{
    uint minimum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Min3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, asuint(src0), asuint(src1), minimum);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Min3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, asuint(src2), minimum, minimum);

    return asfloat(minimum);
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Min3U
*
*   Returns the minimum value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_Min3U(uint src0, uint src1, uint src2)
{
    uint minimum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Min3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, src0, src1, minimum);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Min3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, src2, minimum, minimum);

    return minimum;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Med3F
*
*   Returns the median value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_Med3F(float src0, float src1, float src2)
{
    uint median;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Med3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, asuint(src0), asuint(src1), median);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Med3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, asuint(src2), median, median);

    return asfloat(median);
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Med3U
*
*   Returns the median value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_Med3U(uint src0, uint src1, uint src2)
{
    uint median;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Med3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, src0, src1, median);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Med3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, src2, median, median);

    return median;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Max3F
*
*   Returns the maximum value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_Max3F(float src0, float src1, float src2)
{
    uint maximum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Max3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, asuint(src0), asuint(src1), maximum);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Max3F,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, asuint(src2), maximum, maximum);

    return asfloat(maximum);
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_Max3U
*
*   Returns the maximum value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_Compare3) returned S_OK.
*
***********************************************************************************************************************
*/
uint AmdExtD3DShaderIntrinsics_Max3U(uint src0, uint src1, uint src2)
{
    uint maximum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Max3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, src0, src1, maximum);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_Max3U,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           0);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, src2, maximum, maximum);

    return maximum;
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_IjBarycentricCoords
*
*   Returns the (i, j) barycentric coordinate pair for this shader invocation with the specified interpolation mode at
*   the specified pixel location.  Should not be used for "pull-model" interpolation, PullModelBarycentricCoords should
*   be used instead
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_BaryCoord) returned S_OK.
*
*   Can only be used in pixel shader stages.
*
***********************************************************************************************************************
*/
float2 AmdExtD3DShaderIntrinsics_IjBarycentricCoords(uint interpMode)
{
    uint2 retVal;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_BaryCoord,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           interpMode);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, 0, 0, retVal.x);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_BaryCoord,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           interpMode);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, retVal.x, 0, retVal.y);

    return float2(asfloat(retVal.x), asfloat(retVal.y));
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_PullModelBarycentricCoords
*
*   Returns the (1/W,1/I,1/J) coordinates at the pixel center which can be used for custom interpolation at any
*   location in the pixel.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_BaryCoord) returned S_OK.
*
*   Can only be used in pixel shader stages.
*
***********************************************************************************************************************
*/
float3 AmdExtD3DShaderIntrinsics_PullModelBarycentricCoords()
{
    uint3 retVal;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_BaryCoord,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                                           AmdExtD3DShaderIntrinsicsBarycentric_PerspPullModel);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction1, 0, 0, retVal.x);

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_BaryCoord,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_1,
                                                           AmdExtD3DShaderIntrinsicsBarycentric_PerspPullModel);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction2, retVal.x, 0, retVal.y);

    uint instruction3 = MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_BaryCoord,
                                                           AmdExtD3DShaderIntrinsicsOpcodePhase_2,
                                                           AmdExtD3DShaderIntrinsicsBarycentric_PerspPullModel);
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction3, retVal.y, 0, retVal.z);

    return float3(asfloat(retVal.x), asfloat(retVal.y), asfloat(retVal.z));
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_VertexParameter
*
*   Returns the triangle's parameter information at the specified triangle vertex.
*   The vertex and parameter indices must specified as immediate values.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_VtxParam) returned S_OK.
*
*   Only available in pixel shader stages.
*
***********************************************************************************************************************
*/
float4 AmdExtD3DShaderIntrinsics_VertexParameter(uint vertexIdx, uint parameterIdx)
{
    uint4 retVal;
    uint4 instruction;

    instruction.x = MakeAmdShaderIntrinsicsInstruction(
		     AmdExtD3DShaderIntrinsicsOpcode_VtxParam,
             AmdExtD3DShaderIntrinsicsOpcodePhase_0,
           ((vertexIdx << AmdExtD3DShaderIntrinsicsBarycentric_VtxShift) |
            (parameterIdx << AmdExtD3DShaderIntrinsicsBarycentric_ParamShift) |
            (AmdExtD3DShaderIntrinsicsBarycentric_ComponentX << AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift)));
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction.x, 0, 0, retVal.x);

    instruction.y = MakeAmdShaderIntrinsicsInstruction(
		     AmdExtD3DShaderIntrinsicsOpcode_VtxParam,
             AmdExtD3DShaderIntrinsicsOpcodePhase_0,
           ((vertexIdx << AmdExtD3DShaderIntrinsicsBarycentric_VtxShift) |
            (parameterIdx << AmdExtD3DShaderIntrinsicsBarycentric_ParamShift) |
            (AmdExtD3DShaderIntrinsicsBarycentric_ComponentY << AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift)));
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction.y, 0, 0, retVal.y);

    instruction.z = MakeAmdShaderIntrinsicsInstruction(
		     AmdExtD3DShaderIntrinsicsOpcode_VtxParam,
             AmdExtD3DShaderIntrinsicsOpcodePhase_0,
           ((vertexIdx << AmdExtD3DShaderIntrinsicsBarycentric_VtxShift) |
            (parameterIdx << AmdExtD3DShaderIntrinsicsBarycentric_ParamShift) |
            (AmdExtD3DShaderIntrinsicsBarycentric_ComponentZ << AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift)));
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction.z, 0, 0, retVal.z);

    instruction.w = MakeAmdShaderIntrinsicsInstruction(
		     AmdExtD3DShaderIntrinsicsOpcode_VtxParam,
             AmdExtD3DShaderIntrinsicsOpcodePhase_0,
           ((vertexIdx << AmdExtD3DShaderIntrinsicsBarycentric_VtxShift) |
            (parameterIdx << AmdExtD3DShaderIntrinsicsBarycentric_ParamShift) |
            (AmdExtD3DShaderIntrinsicsBarycentric_ComponentW << AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift)));
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction.w, 0, 0, retVal.w);

    return float4(asfloat(retVal.x), asfloat(retVal.y), asfloat(retVal.z), asfloat(retVal.w));
}

/**
***********************************************************************************************************************
*   AmdExtD3DShaderIntrinsics_VertexParameterComponent
*
*   Returns the triangle's parameter information at the specified triangle vertex and component.
*   The vertex, parameter and component indices must be specified as immediate values.
*
*   Available if CheckSupport(AmdExtD3DShaderIntrinsicsSupport_VtxParam) returned S_OK.
*
*   Only available in pixel shader stages.
*
***********************************************************************************************************************
*/
float AmdExtD3DShaderIntrinsics_VertexParameterComponent(uint vertexIdx, uint parameterIdx, uint componentIdx)
{
    uint retVal;
    uint instruction =
		MakeAmdShaderIntrinsicsInstruction(AmdExtD3DShaderIntrinsicsOpcode_VtxParam,
                                           AmdExtD3DShaderIntrinsicsOpcodePhase_0,
                                          ((vertexIdx << AmdExtD3DShaderIntrinsicsBarycentric_VtxShift) |
                                           (parameterIdx << AmdExtD3DShaderIntrinsicsBarycentric_ParamShift) |
                                           (componentIdx << AmdExtD3DShaderIntrinsicsBarycentric_ComponentShift)));
    AmdExtD3DShaderIntrinsicsUAV.InterlockedCompareExchange(instruction, 0, 0, retVal);

    return asfloat(retVal);
}

#endif // AMDEXTD3DSHADERINTRINICS_HLSL
