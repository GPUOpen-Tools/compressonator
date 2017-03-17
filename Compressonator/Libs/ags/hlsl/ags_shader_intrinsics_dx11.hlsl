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
*************************************************************************************************************
* @File  ags_shader_intrinsics_dx11.hlsl
*
* @brief
*    AMD D3D Shader Intrinsics API hlsl file.
*    This include file contains the shader intrinsics definitions (structures, enums, constant)
*    and HLSL shader intrinsics functions.
*
*************************************************************************************************************
*/

#ifndef _AMDDXEXTSHADERINTRINSICS_HLSL_
#define _AMDDXEXTSHADERINTRINSICS_HLSL_

/**
*************************************************************************************************************
*   Definitions to construct the intrinsic instruction composed of an opcode and optional immediate data.
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsics_MagicCodeShift   28
#define AmdDxExtShaderIntrinsics_MagicCodeMask    0xf
#define AmdDxExtShaderIntrinsics_OpcodePhaseShift 24
#define AmdDxExtShaderIntrinsics_OpcodePhaseMask  0x3
#define AmdDxExtShaderIntrinsics_DataShift        8
#define AmdDxExtShaderIntrinsics_DataMask         0xffff
#define AmdDxExtShaderIntrinsics_OpcodeShift      0
#define AmdDxExtShaderIntrinsics_OpcodeMask       0xff

#define AmdDxExtShaderIntrinsics_MagicCode        0x5


/**
*************************************************************************************************************
*   Intrinsic opcodes.
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsicsOpcode_Readfirstlane  0x01
#define AmdDxExtShaderIntrinsicsOpcode_Readlane       0x02
#define AmdDxExtShaderIntrinsicsOpcode_LaneId         0x03
#define AmdDxExtShaderIntrinsicsOpcode_Swizzle        0x04
#define AmdDxExtShaderIntrinsicsOpcode_Ballot         0x05
#define AmdDxExtShaderIntrinsicsOpcode_MBCnt          0x06
#define AmdDxExtShaderIntrinsicsOpcode_Min3U          0x08
#define AmdDxExtShaderIntrinsicsOpcode_Min3F          0x09
#define AmdDxExtShaderIntrinsicsOpcode_Med3U          0x0a
#define AmdDxExtShaderIntrinsicsOpcode_Med3F          0x0b
#define AmdDxExtShaderIntrinsicsOpcode_Max3U          0x0c
#define AmdDxExtShaderIntrinsicsOpcode_Max3F          0x0d
#define AmdDxExtShaderIntrinsicsOpcode_BaryCoord      0x0e
#define AmdDxExtShaderIntrinsicsOpcode_VtxParam       0x0f
#define AmdDxExtShaderIntrinsicsOpCode_ViewportIndex  0x10
#define AmdDxExtShaderIntrinsicsOpCode_RtArraySlice   0x11


/**
*************************************************************************************************************
*   Intrinsic opcode phases.
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsicsOpcodePhase_0    0x0
#define AmdDxExtShaderIntrinsicsOpcodePhase_1    0x1
#define AmdDxExtShaderIntrinsicsOpcodePhase_2    0x2
#define AmdDxExtShaderIntrinsicsOpcodePhase_3    0x3

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsicsSwizzle defines for common swizzles.  Can be used as the operation parameter for
*   the AmdDxExtShaderIntrinsics_Swizzle intrinsic.
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsicsSwizzle_SwapX1      0x041f
#define AmdDxExtShaderIntrinsicsSwizzle_SwapX2      0x081f
#define AmdDxExtShaderIntrinsicsSwizzle_SwapX4      0x101f
#define AmdDxExtShaderIntrinsicsSwizzle_SwapX8      0x201f
#define AmdDxExtShaderIntrinsicsSwizzle_SwapX16     0x401f
#define AmdDxExtShaderIntrinsicsSwizzle_ReverseX2   0x041f
#define AmdDxExtShaderIntrinsicsSwizzle_ReverseX4   0x0c1f
#define AmdDxExtShaderIntrinsicsSwizzle_ReverseX8   0x1c1f
#define AmdDxExtShaderIntrinsicsSwizzle_ReverseX16  0x3c1f
#define AmdDxExtShaderIntrinsicsSwizzle_ReverseX32  0x7c1f
#define AmdDxExtShaderIntrinsicsSwizzle_BCastX2     0x003e
#define AmdDxExtShaderIntrinsicsSwizzle_BCastX4     0x003c
#define AmdDxExtShaderIntrinsicsSwizzle_BCastX8     0x0038
#define AmdDxExtShaderIntrinsicsSwizzle_BCastX16    0x0030
#define AmdDxExtShaderIntrinsicsSwizzle_BCastX32    0x0020


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsicsBarycentric defines for barycentric interpolation mode.  To be used with
*   AmdDxExtShaderIntrinsicsOpcode_IjBarycentricCoords to specify the interpolation mode.
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsicsBarycentric_LinearCenter    0x1
#define AmdDxExtShaderIntrinsicsBarycentric_LinearCentroid  0x2
#define AmdDxExtShaderIntrinsicsBarycentric_LinearSample    0x3
#define AmdDxExtShaderIntrinsicsBarycentric_PerspCenter     0x4
#define AmdDxExtShaderIntrinsicsBarycentric_PerspCentroid   0x5
#define AmdDxExtShaderIntrinsicsBarycentric_PerspSample     0x6
#define AmdDxExtShaderIntrinsicsBarycentric_PerspPullModel  0x7

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsicsBarycentric defines for specifying vertex and parameter indices.  To be used as
*   the inputs to the AmdDxExtShaderIntrinsicsOpcode_VertexParameter function
*************************************************************************************************************
*/
#define AmdDxExtShaderIntrinsicsBarycentric_Vertex0     0x0
#define AmdDxExtShaderIntrinsicsBarycentric_Vertex1     0x1
#define AmdDxExtShaderIntrinsicsBarycentric_Vertex2     0x2

#define AmdDxExtShaderIntrinsicsBarycentric_Param0      0x00
#define AmdDxExtShaderIntrinsicsBarycentric_Param1      0x01
#define AmdDxExtShaderIntrinsicsBarycentric_Param2      0x02
#define AmdDxExtShaderIntrinsicsBarycentric_Param3      0x03
#define AmdDxExtShaderIntrinsicsBarycentric_Param4      0x04
#define AmdDxExtShaderIntrinsicsBarycentric_Param5      0x05
#define AmdDxExtShaderIntrinsicsBarycentric_Param6      0x06
#define AmdDxExtShaderIntrinsicsBarycentric_Param7      0x07
#define AmdDxExtShaderIntrinsicsBarycentric_Param8      0x08
#define AmdDxExtShaderIntrinsicsBarycentric_Param9      0x09
#define AmdDxExtShaderIntrinsicsBarycentric_Param10     0x0a
#define AmdDxExtShaderIntrinsicsBarycentric_Param11     0x0b
#define AmdDxExtShaderIntrinsicsBarycentric_Param12     0x0c
#define AmdDxExtShaderIntrinsicsBarycentric_Param13     0x0d
#define AmdDxExtShaderIntrinsicsBarycentric_Param14     0x0e
#define AmdDxExtShaderIntrinsicsBarycentric_Param15     0x0f
#define AmdDxExtShaderIntrinsicsBarycentric_Param16     0x10
#define AmdDxExtShaderIntrinsicsBarycentric_Param17     0x11
#define AmdDxExtShaderIntrinsicsBarycentric_Param18     0x12
#define AmdDxExtShaderIntrinsicsBarycentric_Param19     0x13
#define AmdDxExtShaderIntrinsicsBarycentric_Param20     0x14
#define AmdDxExtShaderIntrinsicsBarycentric_Param21     0x15
#define AmdDxExtShaderIntrinsicsBarycentric_Param22     0x16
#define AmdDxExtShaderIntrinsicsBarycentric_Param23     0x17
#define AmdDxExtShaderIntrinsicsBarycentric_Param24     0x18
#define AmdDxExtShaderIntrinsicsBarycentric_Param25     0x19
#define AmdDxExtShaderIntrinsicsBarycentric_Param26     0x1a
#define AmdDxExtShaderIntrinsicsBarycentric_Param27     0x1b
#define AmdDxExtShaderIntrinsicsBarycentric_Param28     0x1c
#define AmdDxExtShaderIntrinsicsBarycentric_Param29     0x1d
#define AmdDxExtShaderIntrinsicsBarycentric_Param30     0x1e
#define AmdDxExtShaderIntrinsicsBarycentric_Param31     0x1f

#define AmdDxExtShaderIntrinsicsBarycentric_ComponentX  0x0
#define AmdDxExtShaderIntrinsicsBarycentric_ComponentY  0x1
#define AmdDxExtShaderIntrinsicsBarycentric_ComponentZ  0x2
#define AmdDxExtShaderIntrinsicsBarycentric_ComponentW  0x3

#define AmdDxExtShaderIntrinsicsBarycentric_ParamShift     0
#define AmdDxExtShaderIntrinsicsBarycentric_ParamMask      0x1f
#define AmdDxExtShaderIntrinsicsBarycentric_VtxShift       0x5
#define AmdDxExtShaderIntrinsicsBarycentric_VtxMask        0x3
#define AmdDxExtShaderIntrinsicsBarycentric_ComponentShift 0x7
#define AmdDxExtShaderIntrinsicsBarycentric_ComponentMask  0x3

/**
*************************************************************************************************************
*   Resource slots for intrinsics using imm_atomic_cmp_exch.
*************************************************************************************************************
*/
#ifndef AmdDxExtShaderIntrinsicsUAVSlot
#define AmdDxExtShaderIntrinsicsUAVSlot       u7
#endif

RWByteAddressBuffer AmdDxExtShaderIntrinsicsUAV : register(AmdDxExtShaderIntrinsicsUAVSlot);

/**
*************************************************************************************************************
*   Resource and sampler slots for intrinsics using sample_l.
*************************************************************************************************************
*/
#ifndef AmdDxExtShaderIntrinsicsResSlot
#define AmdDxExtShaderIntrinsicsResSlot       t127
#endif

#ifndef AmdDxExtShaderIntrinsicsSamplerSlot
#define AmdDxExtShaderIntrinsicsSamplerSlot   s15
#endif

SamplerState AmdDxExtShaderIntrinsicsSamplerState : register (AmdDxExtShaderIntrinsicsSamplerSlot);
Texture3D<float4> AmdDxExtShaderIntrinsicsResource : register (AmdDxExtShaderIntrinsicsResSlot);

/**
*************************************************************************************************************
*   MakeAmdShaderIntrinsicsInstruction
*
*   Creates instruction from supplied opcode and immediate data.
*   NOTE: This is an internal function and should not be called by the source HLSL shader directly.
*
*************************************************************************************************************
*/
uint MakeAmdShaderIntrinsicsInstruction( uint opcode, uint opcodePhase, uint immediateData )
{
    return ((AmdDxExtShaderIntrinsics_MagicCode << AmdDxExtShaderIntrinsics_MagicCodeShift) |
        (immediateData << AmdDxExtShaderIntrinsics_DataShift) |
        (opcodePhase << AmdDxExtShaderIntrinsics_OpcodePhaseShift) |
        (opcode << AmdDxExtShaderIntrinsics_OpcodeShift));
}


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_ReadfirstlaneF
*
*   Returns the value of float src for the first active lane of the wavefront.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Readfirstlane) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_ReadfirstlaneF( float src )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Readfirstlane,
        0, 0 );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, asuint( src ), 0, retVal );
    return asfloat( retVal );
}


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_ReadfirstlaneU
*
*   Returns the value of unsigned integer src for the first active lane of the wavefront.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Readfirstlane) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_ReadfirstlaneU( uint src )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Readfirstlane,
        0, 0 );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, src, 0, retVal );
    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Readlane
*
*   Returns the value of float src for the lane within the wavefront specified by laneId.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Readlane) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_ReadlaneF( float src, uint laneId )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Readlane, 0,
        laneId );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, asuint( src ), 0, retVal );
    return asfloat( retVal );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_ReadlaneU
*
*   Returns the value of unsigned integer src for the lane within the wavefront specified by laneId.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Readlane) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_ReadlaneU( uint src, uint laneId )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Readlane, 0,
        laneId );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, src, 0, retVal );
    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_LaneId
*
*   Returns the current lane id for the thread within the wavefront.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_LaneId) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_LaneId()
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_LaneId, 0, 0 );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, 0, 0, retVal );
    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Swizzle
*
*   Generic instruction to shuffle the float src value among different lanes as specified by the
*   operation.
*   Note that the operation parameter must be an immediately specified value not a value from a variable.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Swizzle) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_SwizzleF( float src, uint operation )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Swizzle, 0,
        operation );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, asuint( src ), 0, retVal );
    return asfloat( retVal );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_SwizzleU
*
*   Generic instruction to shuffle the unsigned integer src value among different lanes as specified by the
*   operation.
*   Note that the operation parameter must be an immediately specified value not a value from a variable.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Swizzle) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_SwizzleU( uint src, uint operation )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Swizzle, 0,
        operation );

    uint retVal;
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, src, 0, retVal );
    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Ballot
*
*   Given an input predicate returns a bit mask indicating for which lanes the predicate is true.
*   Inactive or non-existent lanes will always return 0.  The number of existent lanes is the
*   wavefront size.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Ballot) returned S_OK.
*
*************************************************************************************************************
*/
uint2 AmdDxExtShaderIntrinsics_Ballot( bool predicate )
{
    uint instruction;

    uint retVal1;
    instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Ballot,
        AmdDxExtShaderIntrinsicsOpcodePhase_0, 0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, predicate, 0, retVal1 );

    uint retVal2;
    instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Ballot,
        AmdDxExtShaderIntrinsicsOpcodePhase_1, 0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, predicate, 0, retVal2 );

    return uint2(retVal1, retVal2);
}


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_BallotAny
*
*   Convenience routine that uses Ballot and returns true if for any of the active lanes the predicate
*   is true.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Ballot) returned S_OK.
*
*************************************************************************************************************
*/
bool AmdDxExtShaderIntrinsics_BallotAny( bool predicate )
{
    uint2 retVal = AmdDxExtShaderIntrinsics_Ballot( predicate );

    return ((retVal.x | retVal.y) != 0 ? true : false);
}


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_BallotAll
*
*   Convenience routine that uses Ballot and returns true if for all of the active lanes the predicate
*   is true.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Ballot) returned S_OK.
*
*************************************************************************************************************
*/
bool AmdDxExtShaderIntrinsics_BallotAll( bool predicate )
{
    uint2 ballot = AmdDxExtShaderIntrinsics_Ballot( predicate );

    uint2 execMask = AmdDxExtShaderIntrinsics_Ballot( true );

    return ((ballot.x == execMask.x) && (ballot.y == execMask.y));
}


/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_MBCnt
*
*   Returns the masked bit count of the source register for this thread within all the active threads
*   within a wavefront.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_MBCnt) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_MBCnt( uint2 src )
{
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_MBCnt, 0, 0 );

    uint retVal;

    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, src.x, src.y, retVal );

    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Min3F
*
*   Returns the minimum value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_Min3F( float src0, float src1, float src2 )
{
    uint minimum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Min3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, asuint( src0 ), asuint( src1 ), minimum );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Min3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, asuint( src2 ), minimum, minimum );

    return asfloat( minimum );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Min3U
*
*   Returns the minimum value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_Min3U( uint src0, uint src1, uint src2 )
{
    uint minimum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Min3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, src0, src1, minimum );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Min3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, src2, minimum, minimum );

    return minimum;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Med3F
*
*   Returns the median value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_Med3F( float src0, float src1, float src2 )
{
    uint median;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Med3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, asuint( src0 ), asuint( src1 ), median );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Med3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, asuint( src2 ), median, median );

    return asfloat( median );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Med3U
*
*   Returns the median value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_Med3U( uint src0, uint src1, uint src2 )
{
    uint median;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Med3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, src0, src1, median );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Med3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, src2, median, median );

    return median;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Max3F
*
*   Returns the maximum value of the three floating point source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_Max3F( float src0, float src1, float src2 )
{
    uint maximum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Max3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, asuint( src0 ), asuint( src1 ), maximum );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Max3F,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, asuint( src2 ), maximum, maximum );

    return asfloat( maximum );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_Max3U
*
*   Returns the maximum value of the three unsigned integer source arguments.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_Compare3) returned S_OK.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_Max3U( uint src0, uint src1, uint src2 )
{
    uint maximum;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Max3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, src0, src1, maximum );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_Max3U,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        0 );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, src2, maximum, maximum );

    return maximum;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_IjBarycentricCoords
*
*   Returns the (i, j) barycentric coordinate pair for this shader invocation with the specified
*   interpolation mode at the specified pixel location.  Should not be used for "pull-model" interpolation,
*   PullModelBarycentricCoords should be used instead
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_BaryCoord) returned S_OK.
*
*   Can only be used in pixel shader stages.
*
*************************************************************************************************************
*/
float2 AmdDxExtShaderIntrinsics_IjBarycentricCoords( uint interpMode )
{
    uint2 retVal;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_BaryCoord,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        interpMode );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, 0, 0, retVal.x );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_BaryCoord,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        interpMode );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, retVal.x, 0, retVal.y );

    return float2(asfloat( retVal.x ), asfloat( retVal.y ));
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_PullModelBarycentricCoords
*
*   Returns the (1/W,1/I,1/J) coordinates at the pixel center which can be used for custom interpolation at
*   any location in the pixel.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_BaryCoord) returned S_OK.
*
*   Can only be used in pixel shader stages.
*
*************************************************************************************************************
*/
float3 AmdDxExtShaderIntrinsics_PullModelBarycentricCoords()
{
    uint3 retVal;

    uint instruction1 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_BaryCoord,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        AmdDxExtShaderIntrinsicsBarycentric_PerspPullModel );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction1, 0, 0, retVal.x );

    uint instruction2 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_BaryCoord,
        AmdDxExtShaderIntrinsicsOpcodePhase_1,
        AmdDxExtShaderIntrinsicsBarycentric_PerspPullModel );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction2, retVal.x, 0, retVal.y );

    uint instruction3 = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_BaryCoord,
        AmdDxExtShaderIntrinsicsOpcodePhase_2,
        AmdDxExtShaderIntrinsicsBarycentric_PerspPullModel );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction3, retVal.y, 0, retVal.z );

    return float3(asfloat( retVal.x ), asfloat( retVal.y ), asfloat( retVal.z ));
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_VertexParameter
*
*   Returns the triangle's parameter information at the specified triangle vertex.
*   The vertex and parameter indices must specified as immediate values.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_VtxParam) returned S_OK.
*
*   Only available in pixel shader stages.
*
*************************************************************************************************************
*/
float4 AmdDxExtShaderIntrinsics_VertexParameter( uint vertexIdx, uint parameterIdx )
{
    uint4 retVal;
    uint4 instruction;

    instruction.x = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_VtxParam,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        ((vertexIdx << AmdDxExtShaderIntrinsicsBarycentric_VtxShift) |
        (parameterIdx << AmdDxExtShaderIntrinsicsBarycentric_ParamShift) |
            (AmdDxExtShaderIntrinsicsBarycentric_ComponentX << AmdDxExtShaderIntrinsicsBarycentric_ComponentShift)) );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction.x, 0, 0, retVal.x );

    instruction.y = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_VtxParam,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        ((vertexIdx << AmdDxExtShaderIntrinsicsBarycentric_VtxShift) |
        (parameterIdx << AmdDxExtShaderIntrinsicsBarycentric_ParamShift) |
            (AmdDxExtShaderIntrinsicsBarycentric_ComponentY << AmdDxExtShaderIntrinsicsBarycentric_ComponentShift)) );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction.y, 0, 0, retVal.y );

    instruction.z = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_VtxParam,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        ((vertexIdx << AmdDxExtShaderIntrinsicsBarycentric_VtxShift) |
        (parameterIdx << AmdDxExtShaderIntrinsicsBarycentric_ParamShift) |
            (AmdDxExtShaderIntrinsicsBarycentric_ComponentZ << AmdDxExtShaderIntrinsicsBarycentric_ComponentShift)) );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction.z, 0, 0, retVal.z );

    instruction.w = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_VtxParam,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        ((vertexIdx << AmdDxExtShaderIntrinsicsBarycentric_VtxShift) |
        (parameterIdx << AmdDxExtShaderIntrinsicsBarycentric_ParamShift) |
            (AmdDxExtShaderIntrinsicsBarycentric_ComponentW << AmdDxExtShaderIntrinsicsBarycentric_ComponentShift)) );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction.w, 0, 0, retVal.w );

    return float4(asfloat( retVal.x ), asfloat( retVal.y ), asfloat( retVal.z ), asfloat( retVal.w ));
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_VertexParameterComponent
*
*   Returns the triangle's parameter information at the specified triangle vertex and component.
*   The vertex, parameter and component indices must be specified as immediate values.
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_VtxParam) returned S_OK.
*
*   Only available in pixel shader stages.
*
*************************************************************************************************************
*/
float AmdDxExtShaderIntrinsics_VertexParameterComponent( uint vertexIdx, uint parameterIdx, uint componentIdx )
{
    uint retVal;
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpcode_VtxParam,
        AmdDxExtShaderIntrinsicsOpcodePhase_0,
        ((vertexIdx << AmdDxExtShaderIntrinsicsBarycentric_VtxShift) |
        (parameterIdx << AmdDxExtShaderIntrinsicsBarycentric_ParamShift) |
            (componentIdx << AmdDxExtShaderIntrinsicsBarycentric_ComponentShift)) );
    AmdDxExtShaderIntrinsicsUAV.InterlockedCompareExchange( instruction, 0, 0, retVal );

    return asfloat( retVal );
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_GetViewportIndex
*
*   Returns current viewport index for replicated draws when MultiView extension is enabled (broadcast masks
*   are set).
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_MultiViewIndices) returned S_OK.
*
*   Only available in vertex/geometry/domain shader stages.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_GetViewportIndex()
{
    uint retVal;
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpCode_ViewportIndex, 0, 0 );

    retVal = asuint( AmdDxExtShaderIntrinsicsResource.SampleLevel( AmdDxExtShaderIntrinsicsSamplerState,
        float3(0, 0, 0),
        asfloat( instruction ) ).x );
    return retVal;
}

/**
*************************************************************************************************************
*   AmdDxExtShaderIntrinsics_GetRTArraySlice
*
*   Returns current RT array slice for replicated draws when MultiView extension is enabled (broadcast masks
*   are set).
*
*   Available if CheckSupport(AmdDxExtShaderIntrinsicsSupport_MultiViewIndices) returned S_OK.
*
*   Only available in vertex/geometry/domain shader stages.
*
*************************************************************************************************************
*/
uint AmdDxExtShaderIntrinsics_GetRTArraySlice()
{
    uint retVal;
    uint instruction = MakeAmdShaderIntrinsicsInstruction( AmdDxExtShaderIntrinsicsOpCode_RtArraySlice, 0, 0 );

    retVal = asuint( AmdDxExtShaderIntrinsicsResource.SampleLevel( AmdDxExtShaderIntrinsicsSamplerState,
        float3(0, 0, 0),
        asfloat( instruction ) ).x );
    return retVal;
}

#endif // AMD_HLSL_EXTENSION
