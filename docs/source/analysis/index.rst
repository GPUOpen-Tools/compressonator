Analysis
========

CompressonatorCLI application provides options for analysis and comparison between original and processed images.

Test Analysis Log Features And File Filters
-------------------------------------------

Ref: `CLI Analysis Log File: <../command_line_tool/commandline.html#test-analysis-logging-features-and-file-filters>`__

Captures details of the source and destination files along with statistical data on performance and quality into a text file “process_results.txt” 

Example: **CompressonatorCLI.exe -log <source directory>  <destination directory>**


CompressonatorCLI Analysis
--------------------------

Compressonator CLI has a command line option to run Image Analysis on two sample images of the same dimension and format using the option -analysis

Example:

Step 1: Do compression on a sample source image, say we used BC7 on a source file Ruby.dds and the destination file Ruby_BC7.dds

CompressonatorCLI.exe -fd BC7 ruby.dds ruby_bc7.dds

Step 2: Check the result of the compressed image with the original 

CompressonatorCLI.exe -analysis ruby.dds ruby_bc7.dds

The analysis results is saved to file Analysis_Result.xml

.. code-block:: c

    <?xml version="1.0" encoding="UTF-8"?>
    <ANALYSIS>
        <ruby_ruby_bc7>
            <MSE>0.81335721644469494</MSE>
            <SSIM>0.99769089871409922</SSIM>
            <SSIM_BLUE>0.99735912437459595</SSIM_BLUE>
            <SSIM_GREEN>0.9974541763291831</SSIM_GREEN>
            <SSIM_RED>0.99825939543851883</SSIM_RED>
            <PSNR>49.027990366039383</PSNR>
            <PSNR_BLUE>48.733856839394129</PSNR_BLUE>
            <PSNR_GREEN>49.702877166369824</PSNR_GREEN>
            <PSNR_RED>48.71854158766061</PSNR_RED>
        </ruby_ruby_bc7>
    </ANALYSIS>


Notice the markup label below <ANALYSIS> contains the source file name "ruby" and destination file name "ruby_bc7" appended by an underscore "ruby_ruby_bc7"

SSIM is the calculated average of RGB Channels. A simple file parser can then be used to run SSIM and other Matrix analysis for test automation.

if you wish to generate diff image file, run 

"CompressonatorCLI.exe -analysis -diff_image ruby.dds ruby_bc7.dds" in Step 2. 

A image diff (i.e. ruby_diff.bmp) will be generated. Please use the Compressonator.exe (UI app) to view the diff bmp file with adjusted brightness levels.


Analysis For Images In Folders
------------------------------

1. Process a folder of images and performa analysis between images inside the source folder and destination folder

**CompressonatorCLI.exe -fd <Codec format> <source directory>  <destination directory>**

Example: ``CompressonatorCLI.exe -fd BC1 -log ./images  ./results``

2. Generate analysis report between a source image and a processed image

**CompressonatorCLI.exe -analysis <source image file> <resulted image file>**

For example: ``CompressonatorCLI.exe -analysis Boat.png result.dds``

This option will generate an Analysis_Result.xml report file which contain the SSIM, PSNR and MSE values between the original and processed textures.

3. Generate a difference image between a source image and a processed image

**CompressonatorCLI.exe -diff_image <source image file> <resulted image file>**

For example: ``CompressonatorCLI.exe -diff_image Boat.png result.dds``

This option will generate difference between 2 images with same size. A result_diff.bmp file will be generated. Please use compressonator GUI to increase the contrast to view the diff pixels.

4. Print image properties of an image file.

**CompressonatorCLI.exe -imageprops <image file>**

For example: ``CompressonatorCLI.exe -imageprops Boat.png``

This option will print file name, path, size, image width, height, mip levels and format.

