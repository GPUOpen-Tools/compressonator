IF NOT EXIST .\results mkdir .\results

REM -------------------
REM * BC7 Compression *
REM -------------------

REM Example  1a: PNG to Compressed DDS (BC7)
AMDCompressCLI -fd BC7 -Quality 0.01 .\images\boat.png  .\results\1a_boat.dds

REM Example  1b: Compressed DDS (BC7) to uncompressed PNG  	
AMDCompressCLI .\results\1a_boat.dds   .\results\1b_boat.png 

REM Example  1c: Compress a PNG with BC7 and save to a uncompressed BMP
AMDCompressCLI -fd BC7 -Quality 0.01 .\images\boat.png  .\results\1c_boat.bmp

REM Example  1d: Change PNG to a BMP format
AMDCompressCLI -fd ARGB_8888  .\images\boat.png  .\results\1d_boat.bmp

REM ---------------------------
REM * MIP MAP BC7 Compression *
REM ---------------------------

REM Example  2a: Convert PNG to MIP mapped with a minimum of 4 levels in DDS
AMDCompressCLI -miplevels 4 .\images\boat.png  .\results\2a_boat.dds

REM Example  2b: MIP MAPPED DDS (BC7) to compressed DDS (BC7)
AMDCompressCLI -fd BC7 -Quality 0.01 .\results\2a_boat.dds  .\results\2b_boat.dds

REM Example  2c: Compressed DDS (BC7) to uncompressed MIP Mapped DDS 
AMDCompressCLI .\results\2b_boat.dds  .\results\2c_boat.dds

REM --------------------
REM * BC6H Compression *
REM --------------------

REM Example  3a: EXR to compressed DDS (BC6H) 
AMDCompressCLI -fd BC6H .\images\Balls.exr  .\results\3a_Balls.dds

REM Example  3b: Compressed DDS (BC6H) to a decompressed EXR
AMDCompressCLI .\results\3a_Balls.dds .\results\3b_Balls.exr

REM Example  3c: Compress a EXR with BC6H and save to a uncompressed EXR
AMDCompressCLI -fd BC6H .\images\Balls.exr  .\results\3c_Balls.exr




