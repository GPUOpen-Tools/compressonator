// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#pragma once

#include "cmp_gltfstructures.h"
#include <json/json.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#ifdef _WIN32
#include <DirectXMath.h>
#endif

using json = nlohmann::json;

int GetFormatSize(int id);
int GetDimensions(std::string str);

#ifdef _WIN32
DirectX::XMVECTOR GetXVector(json::array_t accessor);
DirectX::XMMATRIX GetXMatrix(json::array_t accessor);
#endif
glm::vec4 GetVector(json::array_t accessor);
glm::mat4x4 GetMatrix(json::array_t accessor);
std::string GetElementString(json::object_t root, char *path, std::string pDefault);
float GetElementFloat(json::object_t root, char *path, float pDefault);
int GetElementInt(json::object_t root, char *path, int pDefault);
bool GetElementBoolean(json::object_t &root, char *path, bool pDefault);
json::array_t GetElementJsonArray(json::object_t root, char *path, json::array_t pDefault);
void SplitGltfAttribute(std::string attribute, std::string *semanticName, unsigned int*semanticIndex);

