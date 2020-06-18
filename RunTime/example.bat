IF NOT EXIST .\results mkdir .\results

REM ---------------------------
REM * MIP MAP BC7 Compression *
REM ---------------------------

REM Example  2a: Convert PNG to MIP mapped with a minimum of 4 levels in DDS
CompressonatorCLI -miplevels 4 .\images\boat.png  .\results\2a_boat.dds

REM Example  2b: MIP MAPPED DDS (BC7) to compressed DDS (BC7)
CompressonatorCLI -fd BC7 -Quality 0.01 .\results\2a_boat.dds  .\results\2b_boat.dds

REM Example  2c: Compressed DDS (BC7) to uncompressed MIP Mapped DDS 
CompressonatorCLI .\results\2b_boat.dds  .\results\2c_boat.dds

REM --------------------
REM * BC6H Compression *
REM --------------------

REM Example  3a: EXR to compressed DDS (BC6H) 
CompressonatorCLI -fd BC6H .\images\Balls.exr  .\results\3a_Balls.dds

REM Example  3b: Compressed DDS (BC6H) to a decompressed EXR
CompressonatorCLI .\results\3a_Balls.dds .\results\3b_Balls.exr

REM Example  3c: Compress a EXR with BC6H and save to a uncompressed EXR
CompressonatorCLI -fd BC6H .\images\Balls.exr  .\results\3c_Balls.exr




