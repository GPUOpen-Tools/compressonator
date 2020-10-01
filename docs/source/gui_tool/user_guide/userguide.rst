Mip Map Generation
------------------

You can generate different MIP map level for the original image by
selecting the image (Ruby.bmp in this example) on the project explorer
and then click on the |image51|\ button on the Application Toolbar.

Note: Only one image can be selected for MIP map generation at one time.

A Generate MIP Maps window will appear.

|image52|

Select the desired Mip-Level for MIP map generation from the drop down
list. This drop down list is generated based on the selected original
image. Select the lowest mip-level you want to generate, then press the
Generate button.

|image53|

When MIP map generation completes, |image54| will appear in Image View
toolbar. Click on the little triangle to expand the drop down list.

|image55|

You can select the view of each MIP level from the list.

For example, selecting the level 3 (72x53) will update the current image
view as shown below

|image56|


Image View of Ruby BMP file at Mip Level 3

You can always re-generate MIP map for the same image by repeating the
all the steps mentioned in this section.



Cube Maps
---------
Cube Maps can be compressed with or without MIP Levels. Only a limited set of texture types (RGBA_8888 and RGBA_F16) are currently supported in DDS and KTX file formats. Compressing, Generating MIP Levels and Viewing Cube Maps uses the same process as image textures. Just place the file onto the Project Explorer and process them as required.

A new notation is used for the cube faces labeled as “Frames” for each cube face. For cube mapped files maximum frames is set to 6. Support for volumetric texture files is been reviewed and the frames limit will be expanded as needed.

When textures are added to the Project Explorer, the properties view will now display the type of texture as either a 2D or Cube Map, The Depth field is used for the frames size, the Depth field will also be used to indicate the z component of a 3D image (These notations may change in future revisions).


|cubemaps_image1|


View Image Difference
---------------------

To view the difference between a processed image (Ruby\_bmp\_1) and
original image (Ruby\_bmp), right mouse button click on Ruby\_bmp\_1 and
select View Image Diff from the context menu or select the View Image
Diff Icon on the tool bar |image69|

|image70|

Mouse right mouse button click over Ruby\_bmp\_1 showing Context menu

You will now see a comparison of the original image with the compressed
image

|image71|

Image Difference view

Note: The windows in this view are not movable.

Analyzing Compressed Images
---------------------------

After clicking View Image diff, you can run analysis on the images that
show various statistics such as MSE, PSNR and Similarity Indices (SSIM)
by selecting |image72| on the top right corner of the image diff view.

When analysis process completed, the statistics result will be shown on
the Property View

|image73|


.. |image51| image:: media/image53.png
.. |image52| image:: media/image54.png
.. |image53| image:: media/image55.png
.. |image54| image:: media/image56.png
.. |image55| image:: media/image57.png
.. |image56| image:: media/image58.png
.. |image69| image:: media/image73.png
.. |image70| image:: media/image74.png
.. |image71| image:: media/image75.png
.. |image72| image:: media/image76.png
.. |image73| image:: media/image77.png
.. |image116| image:: media/image118.png
.. |image126| image:: media/image126.png
.. |image127| image:: media/image127.png
.. |image128| image:: media/image128.png
.. |image129| image:: media/image129.png
.. |image130| image:: media/image130.png
.. |cubemaps_image1| image:: media/cubemaps.png


