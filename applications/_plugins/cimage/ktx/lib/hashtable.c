/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: c4b6d4c928f6a816d5bf1d8005ea2e95f663b9c0 $ */

/**
 * @file hashtable.c
 * @~English
 *
 * @brief Functions for creating and using a hash table of key-value
 *        pairs.
 *
 * @author Mark Callow, HI Corporation
 */

/*
Copyright (c) 2010 The Khronos Group Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and/or associated documentation files (the
"Materials"), to deal in the Materials without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Materials, and to
permit persons to whom the Materials are furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
unaltered in all copies or substantial portions of the Materials.
Any additions, deletions, or changes to the original source files
must be clearly indicated in accompanying documentation.

If only executable code is distributed, then the accompanying
documentation must state that "this software is based in part on the
work of the Khronos Group".

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// This is to avoid compile warnings. strlen is defined as returning
// size_t and is used by the uthash macros. This avoids having to
// make changes to uthash and a bunch of casts in this file. The
// casts would be required because the key and value lengths in KTX
// are specified as 4 byte quantities so we can't change _keyAndValue
// below to use size_t.
#define strlen(x) ((unsigned int)strlen(x))

#include "uthash.h"

#include "ktx.h"
#include "ktxint.h"


/**
 * @internal
 * @struct _keyAndValue
 * @brief Hash table entry structure
 */
typedef struct _keyAndValue {
    unsigned int keyLen;    /*!< Length of the key */
    char* key;                /*!< Pointer to key string */
    unsigned int valueLen;    /*!< Length of the value */
    void* value;            /*!< Pointer to the value */
    UT_hash_handle hh;        /*!< handle used by UT hash */
} key_and_value_t;
/**
 * @internal
 * @typedef key_and_value_t
 * @brief type of a hash table entry.
 */

/**
 * @~English
 * @brief Create an empty hash table for storying key-value pairs.
 *
 * @return pointer to the newly created hash table or NULL if there
 *         is not enough memory.
 *
 *
 */
KTX_hash_table
ktxHashTable_Create()
{
    key_and_value_t** kvt = (key_and_value_t**)malloc(sizeof (key_and_value_t**));
    *kvt = NULL;
    return (KTX_hash_table)kvt;
}


/**
 * @~English
 * @brief Destroy a hash table.
 *
 * All memory associated with the hash table and its keys and values
 * is freed.
 *
 * @param [in] This pointer to the hash table to be destroyed.
 */
 /*
 * @memberof ktxHashTable @public
 */
void
ktxHashTable_Destroy(KTX_hash_table This)
{
    key_and_value_t* kv;

    for(kv = *(key_and_value_t**)This; kv != NULL;) {
        key_and_value_t* tmp = (key_and_value_t*)kv->hh.next;
        HASH_DELETE(hh, /*head*/*(key_and_value_t**)This, kv);
        free(kv);
        kv = tmp;
    }
    free(This);
}


/**
 * @~English
 * @brief Adds a key value pair to a hash table
 *
 * @param [in] This        pointer to the target hash table.
 * @param [in] key        pointer to the UTF8 NUL-terminated string to be used as the key.
 * @param [in] valueLen    the number of bytes of data in @p value.
 * @param [in] value    pointer to the bytes of data constituting the value.
 *
 * @return KTX_SUCCESS or one of the following error codes.
 *
 * @exception KTX_INVALID_VALUE if @p This, @p key or @p value are NULL, @p key is an
 *            empty string or @p valueLen == 0.
 */
/*
 * @memberof ktxHashTable @public
 */
KTX_error_code
ktxHashTable_AddKVPair(KTX_hash_table This, const char* key, unsigned int valueLen, const void* value)
{
    if (This && key && value && valueLen != 0) {
        unsigned int keyLen = (unsigned int)strlen(key) + 1;
        /* key_and_value_t* head = *(key_and_value_t**)This; */
        key_and_value_t* kv;

        if (keyLen == 1)
            return KTX_INVALID_VALUE;    /* Empty string */

        /* Allocate all the memory as a block */
        kv = (key_and_value_t*)malloc(sizeof(key_and_value_t) + keyLen + valueLen);
        /* Put key first */
        kv->key = (char *)kv + sizeof(key_and_value_t);
        kv->keyLen = keyLen;
        /* then value */
        kv->value = kv->key + keyLen;
        kv->valueLen = valueLen;
        memcpy(kv->key, key, keyLen);
        memcpy(kv->value, value, valueLen);

        HASH_ADD_KEYPTR( hh, /*head*/*(key_and_value_t**)This, kv->key, kv->keyLen-1, kv);
        return KTX_SUCCESS;
    } else
        return KTX_INVALID_VALUE;
}


/**
 * @~English
 * @brief Looks up a key a hash table and returns the value.
 *
 * @param [in]     This            pointer to the target hash table.
 * @param [in]     key            pointer to a UTF8 NUL-terminated string to find.
 * @param [in,out] pValueLen    @p *pValueLen is set to the number of bytes of
 *                              data in the returned value.
 * @param [in,out] ppValue        @p *ppValue is set to the point to the value for
 *                                @p key.
 *
 * @return KTX_SUCCESS or one of the following error codes.
 *
 * @exception KTX_INVALID_VALUE if @p This, @p key or @p pValueLen or @p ppValue
 *                              is NULL.
 * @exception KTX_NOT_FOUND     an entry matching @p key was not found.
 */
/*
 * @memberof ktxHashTable @public
 */
KTX_error_code
ktxHashTable_FindValue(KTX_hash_table This, const char* key, unsigned int* pValueLen, void** ppValue)
{
    if (This && key && pValueLen && ppValue) {
        key_and_value_t* kv;
        /* key_and_value_t* head = *(key_and_value_t**)This; */

        HASH_FIND_STR( /*head*/*(key_and_value_t**)This, key, kv );  /* kv: output pointer */

        if (kv) {
            *pValueLen = kv->valueLen;
            *ppValue = kv->value;
            return KTX_SUCCESS;
        } else
            return KTX_NOT_FOUND;
    } else
        return KTX_INVALID_VALUE;
}


/**
 * @~English
 * @brief Serialize a hash table to a block of data suitable for writing
 *        to a file.
 *
 * The caller is responsible for freeing the data block returned by this
 * function.
 *
 * @param [in]     This            pointer to the target hash table.
 * @param [in,out] pKvdLen        @p *pKvdLen is set to the number of bytes of
 *                              data in the returned data block.
 * @param [in,out] ppKvd        @p *ppKvd is set to the point to the block of
 *                              memory containing the serialized data.
 *
 * @return KTX_SUCCESS or one of the following error codes.
 *
 * @exception KTX_INVALID_VALUE if @p This, @p pKvdLen or @p ppKvd is NULL.
 * @exception KTX_OUT_OF_MEMORY there was not enough memory to serialize the data.
 */
 /* @memberof ktxHashTable @public
 */
KTX_error_code
ktxHashTable_Serialize(KTX_hash_table This, unsigned int* pKvdLen, unsigned char** ppKvd)
{

    if (This && pKvdLen && ppKvd) {
        key_and_value_t* kv;
        unsigned int bytesOfKeyValueData = 0;
        unsigned int keyValueLen;
        unsigned char* sd;
        char padding[4] = {0, 0, 0, 0};

        for (kv = *(key_and_value_t**)This; kv != NULL; kv = kv->hh.next) {
            /* sizeof(*sd) is to make space to write keyAndValueByteSize */
            keyValueLen = kv->keyLen + kv->valueLen + sizeof(khronos_uint32_t);
            /* Add valuePadding */
            keyValueLen += 3 - ((keyValueLen + 3) % 4);
            bytesOfKeyValueData += keyValueLen;
        }
        sd = malloc(bytesOfKeyValueData);
        if (!sd)
            return KTX_OUT_OF_MEMORY;

        *pKvdLen = bytesOfKeyValueData;
        *ppKvd = sd;

        for (kv = *(key_and_value_t **)This; kv != NULL; kv = kv->hh.next) {
            int padLen;

            keyValueLen = kv->keyLen + kv->valueLen;
            *(khronos_uint32_t*)sd = keyValueLen;
            sd += sizeof(khronos_uint32_t);
            memcpy(sd, kv->key, kv->keyLen);
            sd += kv->keyLen;
            memcpy(sd, kv->value, kv->valueLen);
            sd += kv->valueLen;
            padLen = 3 - ((keyValueLen + 3) % 4);
            memcpy(sd, padding, padLen);
            sd += padLen;
        }
        return KTX_SUCCESS;
    } else
        return KTX_INVALID_VALUE;
}


/**
 * @~English
 * @brief Create a hash table from a block of serialized key-value
 *        data read from a file.
 *
 * The caller is responsible for freeing the returned hash table.
 *
 * @param [in]        kvdLen        the length of the serialized key-value data.
 * @param [in]        pKvd        pointer to the serialized key-value data.
 * @param [in,out]    pHt            @p *pHt is set to point to the created hash
 *                              table.
 *
 * @return KTX_SUCCESS or one of the following error codes.
 *
 * @exception KTX_INVALID_VALUE if @p pKvd or @p pHt is NULL or kvdLen == 0.
 * @exception KTX_OUT_OF_MEMORY there was not enough memory to create the hash
 *                              table.
 */
 /* @memberof ktxHashTable @public
 */
KTX_error_code
ktxHashTable_Deserialize(unsigned int kvdLen, void* pKvd, KTX_hash_table* pHt)
{
    KTX_hash_table kvt;
    char* src = pKvd;

    if (kvdLen == 0 || pKvd == NULL || pHt == NULL)
        return KTX_INVALID_VALUE;

    kvt = ktxHashTable_Create();
    if (kvt == NULL)
        return KTX_OUT_OF_MEMORY;

    while (src < (char *)pKvd + kvdLen) {
        char* key;
        unsigned int keyLen;
        void* value;
        khronos_uint32_t keyAndValueByteSize = *((khronos_uint32_t*)src);

        src += sizeof(keyAndValueByteSize);
        key = src;
        keyLen = (unsigned int)strlen(key) + 1;
        value = key + keyLen;

        ktxHashTable_AddKVPair(kvt, key, keyAndValueByteSize - keyLen, value);
        /* Round keyAndValueByteSize */
        keyAndValueByteSize = (keyAndValueByteSize + 3) & ~(khronos_uint32_t)3;
        src += keyAndValueByteSize;
    }

    *pHt = kvt;
    return KTX_SUCCESS;
}


