//=====================================================================
// Copyright 2022-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================

#ifndef BRLG_DATA_H_
#define BRLG_DATA_H_

// TODO: in the future we could use a mixture of different types of data, bitmap, text, compressed data, etc.

static const char BRLG_UNCOMPRESSED_DATA[] = "This is some test text for BRLG encoding, hopefully this works alright.";

static const char BRLG_COMPRESSED_DATA[] = {0x05, 0xFA, 0x01, 0x00, 0x21, 0x01, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x54, 0x68, 0x69, 0x73, 0x20,
                                            0x69, 0x73, 0x20, 0x73, 0x6F, 0x6D, 0x65, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x74, 0x65, 0x78, 0x74,
                                            0x20, 0x66, 0x6F, 0x72, 0x20, 0x42, 0x52, 0x4C, 0x47, 0x20, 0x65, 0x6E, 0x63, 0x6F, 0x64, 0x69, 0x6E,
                                            0x67, 0x2C, 0x20, 0x68, 0x6F, 0x70, 0x65, 0x66, 0x75, 0x6C, 0x6C, 0x79, 0x20, 0x74, 0x68, 0x69, 0x73,
                                            0x20, 0x77, 0x6F, 0x72, 0x6B, 0x73, 0x20, 0x61, 0x6C, 0x72, 0x69, 0x67, 0x68, 0x74, 0x2E, 0X00};

#endif