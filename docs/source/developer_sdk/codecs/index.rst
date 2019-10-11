Texture Compression and Decompression
=====================================
Compressonator SDK's supported codecs includes BC1-BC7/DXTC, ETC1, ETC2, ASTC, ATC, ATI1N, ATI2N.

The main API call for both compression and decompression as well as texture conversion:

.. code-block:: c

    /// Converts the source texture to the destination texture
    /// This can be compression, decompression or converting between two uncompressed formats.
    /// \param[in] pSourceTexture A pointer to the source texture.
    /// \param[in] pDestTexture A pointer to the destination texture.
    /// \param[in] pOptions A pointer to the compression options - can be NULL.
    /// \param[in] pFeedbackProc A pointer to the feedback function - can be NULL.
    /// \param[in] pUser1 User data to pass to the feedback function.
    /// \param[in] pUser2 User data to pass to the feedback function.
    /// \return    CMP_OK if successful, otherwise the error code.
    CMP_ERROR CMP_API CMP_ConvertTexture(CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions,
                                         CMP_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

Example Usage of Compressonator SDK
-----------------------------------

You will need to include a header file and a lib file: **Compressonator.h** and **Compressonator_MD.lib**

and a simple usage is shown as below:

.. code-block:: c

    //==========================
    // Load Source Texture
    //==========================
    CMP_Texture srcTexture;
    // note that LoadDDSFile function is a utils function to initialize the source CMP_Texture
    // you can also initialize the source CMP_Texture the same way as initialize destination CMP_Texture 
    if (!LoadDDSFile(pszSourceFile, srcTexture))
    {
        std::printf("Error loading source file!\n");
        return 0;
    }

    //===================================
    // Initialize Compressed Destination 
    //===================================
    CMP_Texture destTexture;
    destTexture.dwSize     = sizeof(destTexture);
    destTexture.dwWidth    = srcTexture.dwWidth;
    destTexture.dwHeight   = srcTexture.dwHeight;
    destTexture.dwPitch    = 0;
    destTexture.format     = destFormat;
    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
    destTexture.pData = (CMP_BYTE*)malloc(destTexture.dwDataSize);

    //==========================
    // Set Compression Options
    //==========================
    CMP_CompressOptions options = {0};
    options.dwSize       = sizeof(options);
    options.fquality     = fQuality;
    options.dwnumThreads = 8;

    //==========================
    // Compress Texture
    //==========================
    CMP_ERROR   cmp_status;
    cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback, NULL, NULL);
    if (cmp_status != CMP_OK)
    {
        free(srcTexture.pData);
        free(destTexture.pData);
        std::printf("Compression returned an error %d\n", cmp_status);
        return cmp_status;
    }

    //==========================
    // Save Compressed Testure
    //==========================
    if (cmp_status == CMP_OK)
        SaveDDSFile(pszDestFile, destTexture);

    free(srcTexture.pData);
    free(destTexture.pData);

Example projects have been provided `here <https://github.com/GPUOpen-Tools/Compressonator/tree/master/Compressonator/Examples/>`_ with:

* example1 demonstrates simple SDK API usage as shown above. 
* example2 demonstrates how to use the SDK API in multihreaded environment.
* example3 demonstrates how to use the block level SDK API.

These examples are also distributed through Compressonator SDK installer in the `release <https://github.com/GPUOpen-Tools/Compressonator/releases>`_ page.

.. toctree::
   :maxdepth: 4
   :name: toc-developer_sdk-codecs
   
