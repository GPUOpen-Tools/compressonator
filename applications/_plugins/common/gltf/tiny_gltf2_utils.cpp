// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
//
// Major Code based on Header-only tiny glTF 2.0 loader and serializer.
// The MIT License (MIT)
//
// Copyright (c) 2015 - 2017 Syoyo Fujita, Aurélien Chatelain and many
// contributors.
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
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "version.h"
#include "tiny_gltf2_utils.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef USE_MESH_DRACO_EXTENSION
#ifdef ERROR
#undef ERROR
#endif

#pragma warning( push )
#pragma warning( disable : 4244)
#pragma warning( disable : 4267)
#pragma warning( disable : 4018)
#pragma warning( disable : 4005)

#include <draco/compression/encode.h>
#include <draco/compression/decode.h>
#include <draco/io/mesh_io.h>
#include <draco/io/point_cloud_io.h>

#pragma warning( pop )
#endif

#ifdef __clang__
// Disable some warnings for external files.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#if __has_warning("-Wcomma")
#pragma clang diagnostic ignored "-Wcomma"
#endif
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if __has_warning("-Wcast-qual")
#pragma clang diagnostic ignored "-Wcast-qual"
#endif
#if __has_warning("-Wmissing-variable-declarations")
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif
#if __has_warning("-Wmissing-prototypes")
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif
#if __has_warning("-Wcast-align")
#pragma clang diagnostic ignored "-Wcast-align"
#endif
#if __has_warning("-Wnewline-eof")
#pragma clang diagnostic ignored "-Wnewline-eof"
#endif
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _WIN32
#include <windows.h>
#elif !defined(__ANDROID__)
#include <wordexp.h>
#endif

#if defined(__sparcv9)
// Big endian
#else
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || MINIZ_X86_OR_X64_CPU
#define TINYGLTF_LITTLE_ENDIAN 1
#endif
#endif

using nlohmann::json;
#define FILE_SPLIT_PATH "/\\"

#ifdef __APPLE__
#include "targetconditionals.h"
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

namespace tinygltf2 {
#ifdef USE_MESH_DRACO_EXTENSION
std::vector<std::unique_ptr<draco::Mesh>> draco_de_meshes;     //decompressed draco geometry data
std::vector<draco::Mesh*> draco_en_meshes;                     //compressed draco gemetry data
#endif

static void swap4(unsigned int* val) {
#ifdef TINYGLTF_LITTLE_ENDIAN
    (void)val;
#else
    unsigned int   tmp = *val;
    unsigned char* dst = reinterpret_cast<unsigned char*>(val);
    unsigned char* src = reinterpret_cast<unsigned char*>(&tmp);

    dst[0]   = src[3];
    dst[1]   = src[2];
    dst[2]   = src[1];
    dst[3]   = src[0];
#endif
}

static bool FileExists(const std::string& abs_filename) {
    bool ret;
#ifdef _WIN32
    FILE*   fp;
    errno_t err = fopen_s(&fp, abs_filename.c_str(), "rb");
    if (err != 0) {
        return false;
    }
#else
    FILE* fp = fopen(abs_filename.c_str(), "rb");
#endif
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
}

static std::string ExpandFilePath(const std::string& filepath) {
#ifdef _WIN32
    DWORD len = ExpandEnvironmentStringsA(filepath.c_str(), NULL, 0);
    char* str = new char[len];
    ExpandEnvironmentStringsA(filepath.c_str(), str, len);

    std::string s(str);

    delete[] str;

    return s;
#else

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(__ANDROID__)
    // no expansion
    std::string s = filepath;
#else
    std::string s;
    wordexp_t   p;

    if (filepath.empty()) {
        return "";
    }

    // char** w;
    int ret = wordexp(filepath.c_str(), &p, 0);
    if (ret) {
        // err
        s = filepath;
        return s;
    }

    // Use first element only.
    if (p.we_wordv) {
        s = std::string(p.we_wordv[0]);
        wordfree(&p);
    } else {
        s = filepath;
    }

#endif

    return s;
#endif
}

static std::string JoinPath(const std::string& path0, const std::string& path1) {
    if (path0.empty()) {
        return path1;
    } else {
        // check '/'
        char lastChar = *path0.rbegin();
        if (lastChar != '/') {
            return path0 + std::string("/") + path1;
        } else {
            return path0 + path1;
        }
    }
}

static std::string FindFile(const std::vector<std::string>& paths, const std::string& filepath) {
    for (size_t i = 0; i < paths.size(); i++) {
        std::string absPath = ExpandFilePath(JoinPath(paths[i], filepath));
        if (FileExists(absPath)) {
            return absPath;
        }
    }

    return std::string();
}

static std::string GetFilePathExtension(const std::string& FileName) {
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}

static std::string GetBaseDir(const std::string& srcfileDirpath) {
    if (srcfileDirpath.find_last_of(FILE_SPLIT_PATH) != std::string::npos)
        return srcfileDirpath.substr(0, srcfileDirpath.find_last_of(FILE_SPLIT_PATH));
    return "";
}

static std::string GetFileName(const std::string& srcfileNamepath) {
    int pos = (int)srcfileNamepath.find_last_of(FILE_SPLIT_PATH);
    return srcfileNamepath.substr(pos + 1);
}

std::string base64_encode(unsigned char const*, unsigned int len);
std::string base64_decode(std::string const& s);

/*
base64.cpp and base64.h

Copyright (C) 2004-2008 René Nyffenegger

This source code is provided 'as-is', without any express or implied
warranty. In no event will the author be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this source code must not be misrepresented; you must not
claim that you wrote the original source code. If you use this source code
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original source code.

3. This notice may not be removed or altered from any source distribution.

René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wconversion"
#endif
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string   ret;
    int           i = 0;
    int           j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::string base64_decode(std::string const& encoded_string) {
    int           in_len = static_cast<int>(encoded_string.size());
    int           i      = 0;
    int           j      = 0;
    int           in_    = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string   ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

static bool LoadExternalFile(std::vector<unsigned char>* out, std::string* err, const std::string& filename, const std::string& basedir,
                             size_t reqBytes, bool checkSize) {
    out->clear();

    std::vector<std::string> paths;
    paths.push_back(basedir);
    paths.push_back(".");

    std::string filepath = FindFile(paths, filename);
    if (filepath.empty() || filename.empty()) {
        if (err) {
            (*err) += "File not found : " + filename + "\n";
        }
        return false;
    }

    std::ifstream f(filepath.c_str(), std::ifstream::binary);
    if (!f) {
        if (err) {
            (*err) += "File open error : " + filepath + "\n";
        }
        return false;
    }

    f.seekg(0, f.end);
    size_t sz = static_cast<size_t>(f.tellg());
    if (int(sz) < 0) {
        // Looks reading directory, not a file.
        return false;
    }

    if (sz == 0) {
        // Invalid file size.
        return false;
    }
    std::vector<unsigned char> buf(sz);

    f.seekg(0, f.beg);
    f.read(reinterpret_cast<char*>(&buf.at(0)), static_cast<std::streamsize>(sz));
    f.close();

    if (checkSize) {
        if (reqBytes == sz) {
            out->swap(buf);
            return true;
        } else {
            std::stringstream ss;
            ss << "File size mismatch : " << filepath << ", requestedBytes " << reqBytes << ", but got " << sz << std::endl;
            if (err) {
                (*err) += ss.str();
            }
            return false;
        }
    } else {
        if (reqBytes > sz) {
            std::stringstream ss;
            ss << "Reading file out of bound: " << filepath << ", requestedBytes " << reqBytes << ", but got " << sz << std::endl;
            if (err) {
                (*err) += ss.str();
            }
            return false;
        }
    }

    out->swap(buf);
    return true;
}

void TinyGLTF::SetImageLoader(LoadImageDataFunction func, void* user_data) {
    LoadImageData         = func;
    load_image_user_data_ = user_data;
}

#ifndef TINYGLTF_NO_STB_IMAGE
bool LoadImageData(Image* image, std::string* err, int req_width, int req_height, const unsigned char* bytes, int size, void*) {
    int w, h, comp;
    // if image cannot be decoded, ignore parsing and keep it by its path
    // don't break in this case
    // FIXME we should only enter this function if the image is embedded. If
    // image->uri references
    // an image file, it should be left as it is. Image loading should not be
    // mandatory (to support other formats)
    unsigned char* data = stbi_load_from_memory(bytes, size, &w, &h, &comp, 0);
    if (!data) {
        if (err) {
            (*err) += "Unknown image format.\n";
        }
        return false;
    }

    if (w < 1 || h < 1) {
        free(data);
        if (err) {
            (*err) += "Invalid image data.\n";
        }
        return false;
    }

    if (req_width > 0) {
        if (req_width != w) {
            free(data);
            if (err) {
                (*err) += "Image width mismatch.\n";
            }
            return false;
        }
    }

    if (req_height > 0) {
        if (req_height != h) {
            free(data);
            if (err) {
                (*err) += "Image height mismatch.\n";
            }
            return false;
        }
    }

    image->width     = w;
    image->height    = h;
    image->component = comp;
    image->image.resize(static_cast<size_t>(w * h * comp));
    std::copy(data, data + w * h * comp, image->image.begin());

    free(data);

    return true;
}
#endif

void TinyGLTF::SetImageWriter(WriteImageDataFunction func, void* user_data) {
    WriteImageData         = func;
    write_image_user_data_ = user_data;
}

#ifndef TINYGLTF_NO_STB_IMAGE_WRITE
static void WriteToMemory_stbi(void* context, void* data, int size) {
    std::vector<unsigned char>* buffer = reinterpret_cast<std::vector<unsigned char>*>(context);

    unsigned char* pData = reinterpret_cast<unsigned char*>(data);

    buffer->insert(buffer->end(), pData, pData + size);
}

bool WriteImageData(const std::string* basepath, const std::string* filename, Image* image, bool embedImages, void*) {
    const std::string ext = GetFilePathExtension(*filename);

    if (embedImages) {
        // Write image to memory and embed in output
        std::string                header;
        std::vector<unsigned char> data;

        if (ext == "png") {
            stbi_write_png_to_func(WriteToMemory_stbi, &data, image->width, image->height, image->component, &image->image[0], 0);
            header = "data:image/png;base64,";
        } else if (ext == "jpg") {
            stbi_write_jpg_to_func(WriteToMemory_stbi, &data, image->width, image->height, image->component, &image->image[0], 100);
            header = "data:image/jpeg;base64,";
        } else if (ext == "bmp") {
            stbi_write_bmp_to_func(WriteToMemory_stbi, &data, image->width, image->height, image->component, &image->image[0]);
            header = "data:image/bmp;base64,";
        }

        if (data.size()) {
            image->uri = header + base64_encode(&data[0], static_cast<unsigned int>(data.size()));
        } else {
            // Throw error?
        }
    } else {
        // Write image to disc

        const std::string imagefilepath = JoinPath(*basepath, *filename);
        if (ext == "png") {
            stbi_write_png(imagefilepath.c_str(), image->width, image->height, image->component, &image->image[0], 0);
        } else if (ext == "jpg") {
            // TODO (Bowald): Give user the option to set output quality?
            const int quality = 100;
            stbi_write_jpg(imagefilepath.c_str(), image->width, image->height, image->component, &image->image[0], quality);
        } else if (ext == "bmp") {
            stbi_write_bmp(imagefilepath.c_str(), image->width, image->height, image->component, &image->image[0]);
        } else {
            // Throw error? Cant output requested format.
        }
        image->uri = *filename;
    }

    return true;
}
#endif

static std::string MimeToExt(const std::string& mimeType) {
    if (mimeType == "image/jpeg") {
        return "jpg";
    } else if (mimeType == "image/png") {
        return "png";
    } else if (mimeType == "image/bmp") {
        return "bmp";
    } else if (mimeType == "image/gif") {
        return "gif";
    }

    return "";
}

static void UpdateImageObject(Image& image, std::string& baseDir, int index, bool embedImages, WriteImageDataFunction* WriteImageData = nullptr,
                              void* user_data = nullptr) {
    std::string filename;
    std::string ext;

    // If image have uri. Use it it as a filename
    if (image.uri.size()) {
        filename = GetFileName(image.uri);
        ext      = GetFilePathExtension(filename);
    } else if (image.name.size()) {
        ext = MimeToExt(image.mimeType);
        // Otherwise use name as filename
        filename = image.name + "." + ext;
    } else {
        ext = MimeToExt(image.mimeType);
        // Fallback to index of image as filename
        filename = std::to_string(index) + "." + ext;
    }

    // If callback is set, modify image data object
    if (*WriteImageData != nullptr) {
        std::string uri;
        (*WriteImageData)(&baseDir, &filename, &image, embedImages, user_data);
    }
}

static bool IsDataURI(const std::string& in) {
    std::string header = "data:application/octet-stream;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:image/jpeg;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:image/png;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:image/bmp;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:image/gif;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:text/plain;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    header = "data:application/gltf-buffer;base64,";
    if (in.find(header) == 0) {
        return true;
    }

    return false;
}

static bool DecodeDataURI(std::vector<unsigned char>* out, std::string& mime_type, const std::string& in, size_t reqBytes, bool checkSize) {
    std::string header = "data:application/octet-stream;base64,";
    std::string data;
    if (in.find(header) == 0) {
        data = base64_decode(in.substr(header.size()));  // cut mime string.
    }

    if (data.empty()) {
        header = "data:image/jpeg;base64,";
        if (in.find(header) == 0) {
            mime_type = "image/jpeg";
            data      = base64_decode(in.substr(header.size()));  // cut mime string.
        }
    }

    if (data.empty()) {
        header = "data:image/png;base64,";
        if (in.find(header) == 0) {
            mime_type = "image/png";
            data      = base64_decode(in.substr(header.size()));  // cut mime string.
        }
    }

    if (data.empty()) {
        header = "data:image/bmp;base64,";
        if (in.find(header) == 0) {
            mime_type = "image/bmp";
            data      = base64_decode(in.substr(header.size()));  // cut mime string.
        }
    }

    if (data.empty()) {
        header = "data:image/gif;base64,";
        if (in.find(header) == 0) {
            mime_type = "image/gif";
            data      = base64_decode(in.substr(header.size()));  // cut mime string.
        }
    }

    if (data.empty()) {
        header = "data:text/plain;base64,";
        if (in.find(header) == 0) {
            mime_type = "text/plain";
            data      = base64_decode(in.substr(header.size()));
        }
    }

    if (data.empty()) {
        header = "data:application/gltf-buffer;base64,";
        if (in.find(header) == 0) {
            data = base64_decode(in.substr(header.size()));
        }
    }

    if (data.empty()) {
        return false;
    }

    if (checkSize) {
        if (data.size() != reqBytes) {
            return false;
        }
        out->resize(reqBytes);
    } else {
        out->resize(data.size());
    }
    std::copy(data.begin(), data.end(), out->begin());
    return true;
}

static bool ParseJsonAsValue(Value* ret, const json& o) {
    Value val{};
    switch (o.type()) {
    case json::value_t::object: {
        Value::Object value_object;
        for (auto it = o.begin(); it != o.end(); it++) {
            Value entry;
            ParseJsonAsValue(&entry, it.value());
            if (entry.Type() != NULL_TYPE)
                value_object[it.key()] = entry;
        }
        if (value_object.size() > 0)
            val = Value(value_object);
    }
    break;
    case json::value_t::array: {
        Value::Array value_array;
        for (auto it = o.begin(); it != o.end(); it++) {
            Value entry;
            ParseJsonAsValue(&entry, it.value());
            if (entry.Type() != NULL_TYPE)
                value_array.push_back(entry);
        }
        if (value_array.size() > 0)
            val = Value(value_array);
    }
    break;
    case json::value_t::string:
        val = Value(o.get<std::string>());
        break;
    case json::value_t::boolean:
        val = Value(o.get<bool>());
        break;
    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
        val = Value(static_cast<int>(o.get<int64_t>()));
        break;
    case json::value_t::number_float:
        val = Value(o.get<double>());
        break;
    case json::value_t::null:
    case json::value_t::discarded:
        //default:
        break;
    }
    if (ret)
        *ret = val;

    return val.Type() != NULL_TYPE;
}

static bool ParseExtrasProperty(Value* ret, const json& o) {
    json::const_iterator it = o.find("extras");
    if (it == o.end()) {
        return false;
    }

    return ParseJsonAsValue(ret, it.value());
}

static bool ParseBooleanProperty(bool* ret, std::string* err, const json& o, const std::string& property, const bool required,
                                 const std::string& parent_node = "") {
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is missing";
                if (!parent_node.empty()) {
                    (*err) += " in " + parent_node;
                }
                (*err) += ".\n";
            }
        }
        return false;
    }

    if (!it.value().is_boolean()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not a bool type.\n";
            }
        }
        return false;
    }

    if (ret) {
        (*ret) = it.value().get<bool>();
    }

    return true;
}

static bool ParseNumberProperty(double* ret, std::string* err, const json& o, const std::string& property, const bool required,
                                const std::string& parent_node = "") {
#ifdef USE_MESH_DRACO_EXTENSION
    if (parent_node == "Accessor" && property == "bufferView" && !required) {
        return true;
    }
#endif
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is missing";
                if (!parent_node.empty()) {
                    (*err) += " in " + parent_node;
                }
                (*err) += ".\n";
            }
        }
        return false;
    }

    if (!it.value().is_number()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not a number type.\n";
            }
        }
        return false;
    }

    if (ret) {
        (*ret) = it.value().get<double>();
    }

    return true;
}

static bool ParseNumberArrayProperty(std::vector<double>* ret, std::string* err, const json& o, const std::string& property, bool required,
                                     const std::string& parent_node = "") {
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is missing";
                if (!parent_node.empty()) {
                    (*err) += " in " + parent_node;
                }
                (*err) += ".\n";
            }
        }
        return false;
    }

    if (!it.value().is_array()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not an array";
                if (!parent_node.empty()) {
                    (*err) += " in " + parent_node;
                }
                (*err) += ".\n";
            }
        }
        return false;
    }

    ret->clear();
    for (json::const_iterator i = it.value().begin(); i != it.value().end(); i++) {
        if (!i.value().is_number()) {
            if (required) {
                if (err) {
                    (*err) += "'" + property + "' property is not a number.\n";
                    if (!parent_node.empty()) {
                        (*err) += " in " + parent_node;
                    }
                    (*err) += ".\n";
                }
            }
            return false;
        }
        ret->push_back(i.value());
    }

    return true;
}

static bool ParseStringProperty(std::string* ret, std::string* err, const json& o, const std::string& property, bool required,
                                const std::string& parent_node = std::string()) {
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is missing";
                if (parent_node.empty()) {
                    (*err) += ".\n";
                } else {
                    (*err) += " in `" + parent_node + "'.\n";
                }
            }
        }
        return false;
    }

    if (!it.value().is_string()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not a string type.\n";
            }
        }
        return false;
    }

    if (ret) {
        (*ret) = it.value().get<std::string>();
    }

    return true;
}

static bool ParseStringIntProperty(std::map<std::string, int>* ret, std::string* err, const json& o, const std::string& property, bool required,
                                   const std::string& parent = "") {
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                if (!parent.empty()) {
                    (*err) += "'" + property + "' property is missing in " + parent + ".\n";
                } else {
                    (*err) += "'" + property + "' property is missing.\n";
                }
            }
        }
        return false;
    }

    // Make sure we are dealing with an object / dictionary.
    if (!it.value().is_object()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not an object.\n";
            }
        }
        return false;
    }

    ret->clear();
    const json& dict = it.value();

    json::const_iterator dictIt(dict.begin());
    json::const_iterator dictItEnd(dict.end());

    for (; dictIt != dictItEnd; ++dictIt) {
        if (!dictIt.value().is_number()) {
            if (required) {
                if (err) {
                    (*err) += "'" + property + "' value is not an int.\n";
                }
            }
            return false;
        }

        // Insert into the list.
        (*ret)[dictIt.key()] = static_cast<int>(dictIt.value());
    }
    return true;
}

static bool ParseJSONProperty(std::map<std::string, double>* ret, std::string* err, const json& o, const std::string& property, bool required) {
    json::const_iterator it = o.find(property);
    if (it == o.end()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is missing. \n'";
            }
        }
        return false;
    }

    if (!it.value().is_object()) {
        if (required) {
            if (err) {
                (*err) += "'" + property + "' property is not a JSON object.\n";
            }
        }
        return false;
    }

    ret->clear();
    const json&          obj = it.value();
    json::const_iterator it2(obj.begin());
    json::const_iterator itEnd(obj.end());
    for (; it2 != itEnd; it2++) {
        if (it2.value().is_number())
            ret->insert(std::pair<std::string, double>(it2.key(), it2.value()));
    }

    return true;
}

static bool ParseParameterProperty(Parameter* param, std::string* err, const json& o, const std::string& prop, bool required) {
    double num_val;

    // A parameter value can either be a string or an array of either a boolean or
    // a number. Booleans of any kind aren't supported here. Granted, it
    // complicates the Parameter structure and breaks it semantically in the sense
    // that the client probably works off the assumption that if the string is
    // empty the vector is used, etc. Would a tagged union work?
    if (ParseStringProperty(&param->string_value, err, o, prop, false)) {
        // Found string property.
        return true;
    } else if (ParseNumberArrayProperty(&param->number_array, err, o, prop, false)) {
        // Found a number array.
        return true;
    } else if (ParseNumberProperty(&num_val, err, o, prop, false)) {
        param->number_array.push_back(num_val);
        return true;
    } else if (ParseJSONProperty(&param->json_double_value, err, o, prop, false)) {
        return true;
    } else if (ParseBooleanProperty(&param->bool_value, err, o, prop, false)) {
        return true;
    } else {
        if (required) {
            if (err) {
                (*err) += "parameter must be a string or number / number array.\n";
            }
        }
        return false;
    }
}

static bool ParseExtensionsProperty(ExtensionMap* ret, std::string* err, const json& o) {
    (void)err;

    json::const_iterator it = o.find("extensions");
    if (it == o.end()) {
        return false;
    }
    if (!it.value().is_object()) {
        return false;
    }
    ExtensionMap         extensions;
    json::const_iterator extIt = it.value().begin();
    for (; extIt != it.value().end(); extIt++) {
        if (!extIt.value().is_object())
            continue;
        ParseJsonAsValue(&extensions[extIt.key()], extIt.value());
    }
    if (ret) {
        (*ret) = extensions;
    }
    return true;
}

static bool ParseAsset(Asset* asset, std::string* err, const json& o) {
    ParseStringProperty(&asset->version, err, o, "version", true, "Asset");
    ParseStringProperty(&asset->generator, err, o, "generator", false, "Asset");
    ParseStringProperty(&asset->minVersion, err, o, "minVersion", false, "Asset");

    ParseExtensionsProperty(&asset->extensions, err, o);

    // Unity exporter version is added as extra here
    ParseExtrasProperty(&(asset->extras), o);

    return true;
}

static bool ParseImage(Image* image, std::string* err, const json& o, const std::string& basedir, LoadImageDataFunction* LoadImageData = nullptr,
                       void* user_data = nullptr) {
    // A glTF image must either reference a bufferView or an image uri

    // schema says oneOf [`bufferView`, `uri`]
    // TODO(syoyo): Check the type of each parameters.
    bool hasBufferView = (o.find("bufferView") != o.end());
    bool hasURI        = (o.find("uri") != o.end());

    if (hasBufferView && hasURI) {
        // Should not both defined.
        if (err) {
            (*err) +=
                "Only one of `bufferView` or `uri` should be defined, but both are "
                "defined for Image.\n";
        }
        return false;
    }

    if (!hasBufferView && !hasURI) {
        if (err) {
            (*err) += "Neither required `bufferView` nor `uri` defined for Image.\n";
        }
        return false;
    }

    ParseStringProperty(&image->name, err, o, "name", false);

    if (hasBufferView) {
        double bufferView = -1;
        if (!ParseNumberProperty(&bufferView, err, o, "bufferView", true)) {
            if (err) {
                (*err) += "Failed to parse `bufferView` for Image.\n";
            }
            return false;
        }

        std::string mime_type;
        ParseStringProperty(&mime_type, err, o, "mimeType", false);

        double width = 0.0;
        ParseNumberProperty(&width, err, o, "width", false);

        double height = 0.0;
        ParseNumberProperty(&height, err, o, "height", false);

        // Just only save some information here. Loading actual image data from
        // bufferView is done after this `ParseImage` function.
        image->bufferView = static_cast<int>(bufferView);
        image->mimeType   = mime_type;
        image->width      = static_cast<int>(width);
        image->height     = static_cast<int>(height);

        return true;
    }

    // Parse URI & Load image data.

    std::string uri;
    std::string tmp_err;
    if (!ParseStringProperty(&uri, &tmp_err, o, "uri", true)) {
        if (err) {
            (*err) += "Failed to parse `uri` for Image.\n";
        }
        return false;
    }

    std::vector<unsigned char> img;

    if (IsDataURI(uri)) {
        if (!DecodeDataURI(&img, image->mimeType, uri, 0, false)) {
            if (err) {
                (*err) += "Failed to decode 'uri' for image parameter.\n";
            }
            return false;
        }
    } else {
        // Assume external file
        // Keep texture path (for textures that cannot be decoded)
        image->uri = uri;
#ifdef TINYGLTF_NO_EXTERNAL_IMAGE
        return true;
#endif
        if (!LoadExternalFile(&img, err, uri, basedir, 0, false)) {
            if (err) {
                (*err) += "Failed to load external 'uri' for image parameter\n";
            }
            // If the image cannot be loaded, keep uri as image->uri.
            return true;
        }

        if (img.empty()) {
            if (err) {
                (*err) += "Image is empty.\n";
            }
            return false;
        }
    }

    if (*LoadImageData != nullptr) {
        return (*LoadImageData)(image, err, 0, 0, &img.at(0), static_cast<int>(img.size()), user_data);
    }

    return true;
}

static bool ParseTexture(Texture* texture, std::string* err, const json& o, const std::string& basedir) {
    (void)basedir;
    double sampler = -1.0;
    double source  = -1.0;
    ParseNumberProperty(&sampler, err, o, "sampler", false);

    ParseNumberProperty(&source, err, o, "source", false);

    texture->sampler = static_cast<int>(sampler);
    texture->source  = static_cast<int>(source);

    ParseExtensionsProperty(&texture->extensions, err, o);
    ParseExtrasProperty(&texture->extras, o);

    return true;
}

static bool ParseBuffer(Buffer* buffer, std::string* err, const json& o, const std::string& basedir, bool is_binary = false,
                        const unsigned char* bin_data = nullptr, size_t bin_size = 0) {
    double byteLength;
    if (!ParseNumberProperty(&byteLength, err, o, "byteLength", true, "Buffer")) {
        return false;
    }

    // In glTF 2.0, uri is not mandatory anymore
    buffer->uri.clear();
    ParseStringProperty(&buffer->uri, err, o, "uri", false, "Buffer");

    // having an empty uri for a non embedded image should not be valid
    if (!is_binary && buffer->uri.empty()) {
        if (err) {
            (*err) += "'uri' is missing from non binary glTF file buffer.\n";
        }
    }

    json::const_iterator type = o.find("type");
    if (type != o.end()) {
        if (type.value().is_string()) {
            const std::string& ty = type.value();
            if (ty.compare("arraybuffer") == 0) {
                // buffer.type = "arraybuffer";
            }
        }
    }

    size_t bytes = static_cast<size_t>(byteLength);
    if (is_binary) {
        // Still binary glTF accepts external dataURI. First try external resources.

        if (!buffer->uri.empty()) {
            // External .bin file.
            LoadExternalFile(&buffer->data, err, buffer->uri, basedir, bytes, true);
        } else {
            // load data from (embedded) binary data

            if ((bin_size == 0) || (bin_data == nullptr)) {
                if (err) {
                    (*err) += "Invalid binary data in `Buffer'.\n";
                }
                return false;
            }

            if (byteLength > bin_size) {
                if (err) {
                    std::stringstream ss;
                    ss << "Invalid `byteLength'. Must be equal or less than binary size: "
                       "`byteLength' = "
                       << byteLength << ", binary size = " << bin_size << std::endl;
                    (*err) += ss.str();
                }
                return false;
            }

            // Read buffer data
            buffer->data.resize(static_cast<size_t>(byteLength));
            memcpy(&(buffer->data.at(0)), bin_data, static_cast<size_t>(byteLength));
        }
    } else {
        if (IsDataURI(buffer->uri)) {
            std::string mime_type;
            if (!DecodeDataURI(&buffer->data, mime_type, buffer->uri, bytes, true)) {
                if (err) {
                    (*err) += "Failed to decode 'uri' : " + buffer->uri + " in Buffer\n";
                }
                return false;
            }
        } else {
            // Assume external .bin file.
            if (!LoadExternalFile(&buffer->data, err, buffer->uri, basedir, bytes, true)) {
                return false;
            }
        }
    }

    ParseStringProperty(&buffer->name, err, o, "name", false);

    return true;
}

#ifdef USE_MESH_DRACO_EXTENSION
static bool ParseDracoBuffer(DracoData* dbuffer, std::string* err, const json& o, const std::string& basedir, bool is_binary = false,
                             const unsigned char* bin_data = nullptr, size_t bin_size = 0) {
    // if failed to get byteLength from bufferViews (for multiple meshes)
    // then get from buffers(only work for single mesh)
    if (dbuffer->byteLength < 0) {
        if (!ParseNumberProperty(&(dbuffer->byteLength), err, o, "byteLength", true, "buffers")) {
            return false;
        }
    }

    //only support external bin for gltf-draco
    dbuffer->uri.clear();
    ParseStringProperty(&dbuffer->uri, err, o, "uri", false, "buffers");

    // having an empty uri for a non embedded image should not be valid
    if (!is_binary && dbuffer->uri.empty()) {
        if (err) {
            (*err) += "'uri' is missing from non binary glTF file buffer.\n";
        }
    }

    json::const_iterator type = o.find("type");
    if (type != o.end()) {
        if (type.value().is_string()) {
            const std::string& ty = type.value();
            if (ty.compare("arraybuffer") == 0) {
                // buffer.type = "arraybuffer";
            }
        }
    }

    size_t bytes = static_cast<size_t>(dbuffer->byteLength);
    if (is_binary) {
        // Still binary glTF accepts external dataURI. First try external resources.

        if (!dbuffer->uri.empty()) {
            // External .bin file.
            LoadExternalFile(&dbuffer->data, err, dbuffer->uri, basedir, bytes, false);
        } else {
            // load data from (embedded) binary data

            if ((bin_size == 0) || (bin_data == nullptr)) {
                if (err) {
                    (*err) += "Invalid binary data in `Buffer'.\n";
                }
                return false;
            }

            if (dbuffer->byteLength > bin_size) {
                if (err) {
                    std::stringstream ss;
                    ss << "Invalid `byteLength'. Must be equal or less than binary size: "
                       "`byteLength' = "
                       << dbuffer->byteLength << ", binary size = " << bin_size << std::endl;
                    (*err) += ss.str();
                }
                return false;
            }

            // Read buffer data
            dbuffer->data.resize(static_cast<size_t>(dbuffer->byteLength));
            memcpy(&(dbuffer->data.at(0)), bin_data, static_cast<size_t>(dbuffer->byteLength));
        }
    } else {
        if (IsDataURI(dbuffer->uri)) {
            std::string mime_type;
            if (!DecodeDataURI(&dbuffer->data, mime_type, dbuffer->uri, bytes, false)) {
                if (err) {
                    (*err) += "Failed to decode 'uri' : " + dbuffer->uri + " in Buffer\n";
                }
                return false;
            }
        } else {
            // Assume external .bin file.
            if (!LoadExternalFile(&dbuffer->data, err, dbuffer->uri, basedir, bytes, false)) {
                return false;
            }
        }

        // Create a draco decoding buffer. Note that no data is copied in this step.
        draco::DecoderBuffer dracobuffer;
        dracobuffer.Init((const char*)dbuffer->data.data(), dbuffer->data.size());
        auto type_statusor = draco::Decoder::GetEncodedGeometryType(&dracobuffer);
        if (!type_statusor.ok()) {
            printf("Failed loading the input mesh: %s\n", type_statusor.status().error_msg());
            return false;
        }

        const draco::EncodedGeometryType geom_type = type_statusor.value();
        if (geom_type == draco::TRIANGULAR_MESH) {
            printf("Decode Mesh From Buffer\n");

            draco::Decoder decoder;

            auto statusor = decoder.DecodeMeshFromBuffer(&dracobuffer);
            if (!statusor.ok()) {
                printf("Draco decode mesh from buffer failed.: %s\n", statusor.status().error_msg());
                return false;
            }

            draco_de_meshes.push_back(std::move(statusor).value());
        } else if (geom_type == draco::POINT_CLOUD) {
            printf("Decode Point Cloud From Buffer\n");
            // Failed to decode it as mesh, so let's try to decode it as a point cloud.
            draco::Decoder decoder;
            auto           statusor = decoder.DecodePointCloudFromBuffer(&dracobuffer);
            if (!statusor.ok()) {
                printf("Failed loading the input mesh: %s\n", statusor.status().error_msg());
            } else {
                printf("Decode Point Cloud From Buffer is on-going. not supported yet.\n");
            }

            //return false for now as point cloud not supported yet.
            return false;
        }
    }
    return true;
}
#endif

static bool ParseBufferView(BufferView* bufferView, std::string* err, const json& o) {
    double buffer = -1.0;
    if (!ParseNumberProperty(&buffer, err, o, "buffer", true, "BufferView")) {
        return false;
    }

    double byteOffset = 0.0;
    ParseNumberProperty(&byteOffset, err, o, "byteOffset", false);

    double byteLength = 1.0;
    if (!ParseNumberProperty(&byteLength, err, o, "byteLength", true, "BufferView")) {
        return false;
    }

    size_t byteStride      = 0;
    double byteStrideValue = 0.0;
    if (!ParseNumberProperty(&byteStrideValue, err, o, "byteStride", false)) {
        // Spec says: When byteStride of referenced bufferView is not defined, it
        // means that accessor elements are tightly packed, i.e., effective stride
        // equals the size of the element.
        // We cannot determine the actual byteStride until Accessor are parsed, thus
        // set 0(= tightly packed) here(as done in OpenGL's VertexAttribPoiner)
        byteStride = 0;
    } else {
        byteStride = static_cast<size_t>(byteStrideValue);
    }

    if ((byteStride > 252) || ((byteStride % 4) != 0)) {
        if (err) {
            std::stringstream ss;
            ss << "Invalid `byteStride' value. `byteStride' must be the multiple of "
               "4 : "
               << byteStride << std::endl;

            (*err) += ss.str();
        }
        return false;
    }

    double target = 0.0;
    ParseNumberProperty(&target, err, o, "target", false);
    int targetValue = static_cast<int>(target);
    if ((targetValue == TINYGLTF_TARGET_ARRAY_BUFFER) || (targetValue == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER)) {
        // OK
    } else {
        targetValue = 0;
    }
    bufferView->target = targetValue;

    ParseStringProperty(&bufferView->name, err, o, "name", false);

    bufferView->buffer     = static_cast<int>(buffer);
    bufferView->byteOffset = static_cast<size_t>(byteOffset);
    bufferView->byteLength = static_cast<size_t>(byteLength);
    bufferView->byteStride = static_cast<size_t>(byteStride);

    return true;
}

static bool ParseAccessor(Accessor* accessor, std::string* err, const json& o, bool isrequired) {
    double bufferView = -1.0;
    if (!ParseNumberProperty(&bufferView, err, o, "bufferView", isrequired, "Accessor")) {
        return false;
    }

    double byteOffset = 0.0;
    ParseNumberProperty(&byteOffset, err, o, "byteOffset", false, "Accessor");

    bool normalized = false;
    ParseBooleanProperty(&normalized, err, o, "normalized", false, "Accessor");

    double componentType = 0.0;
    if (!ParseNumberProperty(&componentType, err, o, "componentType", true, "Accessor")) {
        return false;
    }

    double count = 0.0;
    if (!ParseNumberProperty(&count, err, o, "count", true, "Accessor")) {
        return false;
    }

    std::string type;
    if (!ParseStringProperty(&type, err, o, "type", true, "Accessor")) {
        return false;
    }

    if (type.compare("SCALAR") == 0) {
        accessor->type = TINYGLTF_TYPE_SCALAR;
    } else if (type.compare("VEC2") == 0) {
        accessor->type = TINYGLTF_TYPE_VEC2;
    } else if (type.compare("VEC3") == 0) {
        accessor->type = TINYGLTF_TYPE_VEC3;
    } else if (type.compare("VEC4") == 0) {
        accessor->type = TINYGLTF_TYPE_VEC4;
    } else if (type.compare("MAT2") == 0) {
        accessor->type = TINYGLTF_TYPE_MAT2;
    } else if (type.compare("MAT3") == 0) {
        accessor->type = TINYGLTF_TYPE_MAT3;
    } else if (type.compare("MAT4") == 0) {
        accessor->type = TINYGLTF_TYPE_MAT4;
    } else {
        std::stringstream ss;
        ss << "Unsupported `type` for accessor object. Got \"" << type << "\"\n";
        if (err) {
            (*err) += ss.str();
        }
        return false;
    }

    ParseStringProperty(&accessor->name, err, o, "name", false);

    accessor->minValues.clear();
    accessor->maxValues.clear();
    ParseNumberArrayProperty(&accessor->minValues, err, o, "min", false, "Accessor");

    ParseNumberArrayProperty(&accessor->maxValues, err, o, "max", false, "Accessor");

    accessor->count      = static_cast<size_t>(count);
    accessor->bufferView = static_cast<int>(bufferView);
    accessor->byteOffset = static_cast<size_t>(byteOffset);
    accessor->normalized = normalized;
    {
        int comp = static_cast<int>(componentType);
        if (comp >= TINYGLTF_COMPONENT_TYPE_BYTE && comp <= TINYGLTF_COMPONENT_TYPE_DOUBLE) {
            // OK
            accessor->componentType = comp;
        } else {
            std::stringstream ss;
            ss << "Invalid `componentType` in accessor. Got " << comp << "\n";
            if (err) {
                (*err) += ss.str();
            }
            return false;
        }
    }

    ParseExtrasProperty(&(accessor->extras), o);

    return true;
}

static bool ParsePrimitive(Primitive* primitive, std::string* err, const json& o) {
    double material = -1.0;
    ParseNumberProperty(&material, err, o, "material", false);
    primitive->material = static_cast<int>(material);

    double mode = static_cast<double>(TINYGLTF_MODE_TRIANGLES);
    ParseNumberProperty(&mode, err, o, "mode", false);

    int primMode    = static_cast<int>(mode);
    primitive->mode = primMode;  // Why only triangled were supported ?

    double indices = -1.0;
    ParseNumberProperty(&indices, err, o, "indices", false);
    primitive->indices = static_cast<int>(indices);
    if (!ParseStringIntProperty(&primitive->attributes, err, o, "attributes", true, "Primitive")) {
        return false;
    }

    // Look for morph targets
    json::const_iterator targetsObject = o.find("targets");
    if ((targetsObject != o.end()) && targetsObject.value().is_array()) {
        for (json::const_iterator i = targetsObject.value().begin(); i != targetsObject.value().end(); i++) {
            std::map<std::string, int> targetAttribues;

            const json&          dict = i.value();
            json::const_iterator dictIt(dict.begin());
            json::const_iterator dictItEnd(dict.end());

            for (; dictIt != dictItEnd; ++dictIt) {
                targetAttribues[dictIt.key()] = static_cast<int>(dictIt.value());
            }
            primitive->targets.push_back(targetAttribues);
        }
    }

    ParseExtrasProperty(&(primitive->extras), o);

    return true;
}

static bool ParseMesh(Mesh* mesh, std::string* err, const json& o) {
    ParseStringProperty(&mesh->name, err, o, "name", false);

    mesh->primitives.clear();
    json::const_iterator primObject = o.find("primitives");
    if ((primObject != o.end()) && primObject.value().is_array()) {
        for (json::const_iterator i = primObject.value().begin(); i != primObject.value().end(); i++) {
            Primitive primitive;
            if (ParsePrimitive(&primitive, err, i.value())) {
                // Only add the primitive if the parsing succeeds.
                mesh->primitives.push_back(primitive);
            }
        }
    }

    // Look for morph targets
    json::const_iterator targetsObject = o.find("targets");
    if ((targetsObject != o.end()) && targetsObject.value().is_array()) {
        for (json::const_iterator i = targetsObject.value().begin(); i != targetsObject.value().end(); i++) {
            std::map<std::string, int> targetAttribues;

            const json&          dict = i.value();
            json::const_iterator dictIt(dict.begin());
            json::const_iterator dictItEnd(dict.end());

            for (; dictIt != dictItEnd; ++dictIt) {
                targetAttribues[dictIt.key()] = static_cast<int>(dictIt.value());
            }
            mesh->targets.push_back(targetAttribues);
        }
    }

    // Should probably check if has targets and if dimensions fit
    ParseNumberArrayProperty(&mesh->weights, err, o, "weights", false);

    ParseExtensionsProperty(&mesh->extensions, err, o);
    ParseExtrasProperty(&(mesh->extras), o);

    return true;
}

static bool ParseLight(Light* light, std::string* err, const json& o) {
    ParseStringProperty(&light->name, err, o, "name", false);
    ParseNumberArrayProperty(&light->color, err, o, "color", false);
    ParseStringProperty(&light->type, err, o, "type", false);
    return true;
}

static bool ParseNode(Node* node, std::string* err, const json& o) {
    ParseStringProperty(&node->name, err, o, "name", false);

    double skin = -1.0;
    ParseNumberProperty(&skin, err, o, "skin", false);
    node->skin = static_cast<int>(skin);

    // Matrix and T/R/S are exclusive
    if (!ParseNumberArrayProperty(&node->matrix, err, o, "matrix", false)) {
        ParseNumberArrayProperty(&node->rotation, err, o, "rotation", false);
        ParseNumberArrayProperty(&node->scale, err, o, "scale", false);
        ParseNumberArrayProperty(&node->translation, err, o, "translation", false);
    }

    double camera = -1.0;
    ParseNumberProperty(&camera, err, o, "camera", false);
    node->camera = static_cast<int>(camera);

    double mesh = -1.0;
    ParseNumberProperty(&mesh, err, o, "mesh", false);
    node->mesh = int(mesh);

    node->children.clear();
    json::const_iterator childrenObject = o.find("children");
    if ((childrenObject != o.end()) && childrenObject.value().is_array()) {
        for (json::const_iterator i = childrenObject.value().begin(); i != childrenObject.value().end(); i++) {
            if (!i.value().is_number()) {
                if (err) {
                    (*err) += "Invalid `children` array.\n";
                }
                return false;
            }
            const int& childrenNode = static_cast<int>(i.value());
            node->children.push_back(childrenNode);
        }
    }

    ParseExtensionsProperty(&node->extensions, err, o);
    ParseExtrasProperty(&(node->extras), o);

    return true;
}

static bool ParseMaterial(Material* material, std::string* err, const json& o) {
    material->values.clear();
    material->extensions.clear();
    material->additionalValues.clear();

    json::const_iterator it(o.begin());
    json::const_iterator itEnd(o.end());

    for (; it != itEnd; it++) {
        if (it.key() == "pbrMetallicRoughness") {
            if (it.value().is_object()) {
                const json& values_object = it.value();

                json::const_iterator itVal(values_object.begin());
                json::const_iterator itValEnd(values_object.end());

                for (; itVal != itValEnd; itVal++) {
                    Parameter param;
                    if (ParseParameterProperty(&param, err, values_object, itVal.key(), false)) {
                        material->values[itVal.key()] = param;
                    }
                }
            }
        } else if (it.key() == "extensions" || it.key() == "extras") {
            // done later, skip, otherwise poorly parsed contents will be saved in the parametermap and serialized again later
        } else {
            Parameter param;
            if (ParseParameterProperty(&param, err, o, it.key(), false)) {
                material->additionalValues[it.key()] = param;
            }
        }
    }

    ParseExtensionsProperty(&material->extensions, err, o);
    ParseExtrasProperty(&(material->extras), o);

    return true;
}

static bool ParseAnimationChannel(AnimationChannel* channel, std::string* err, const json& o) {
    double samplerIndex = -1.0;
    double targetIndex  = -1.0;
    if (!ParseNumberProperty(&samplerIndex, err, o, "sampler", true, "AnimationChannel")) {
        if (err) {
            (*err) += "`sampler` field is missing in animation channels\n";
        }
        return false;
    }

    json::const_iterator targetIt = o.find("target");
    if ((targetIt != o.end()) && targetIt.value().is_object()) {
        const json& target_object = targetIt.value();

        if (!ParseNumberProperty(&targetIndex, err, target_object, "node", true)) {
            if (err) {
                (*err) += "`node` field is missing in animation.channels.target\n";
            }
            return false;
        }

        if (!ParseStringProperty(&channel->target_path, err, target_object, "path", true)) {
            if (err) {
                (*err) += "`path` field is missing in animation.channels.target\n";
            }
            return false;
        }
    }

    channel->sampler     = static_cast<int>(samplerIndex);
    channel->target_node = static_cast<int>(targetIndex);

    ParseExtrasProperty(&(channel->extras), o);

    return true;
}

static bool ParseAnimation(Animation* animation, std::string* err, const json& o) {
    {
        json::const_iterator channelsIt = o.find("channels");
        if ((channelsIt != o.end()) && channelsIt.value().is_array()) {
            for (json::const_iterator i = channelsIt.value().begin(); i != channelsIt.value().end(); i++) {
                AnimationChannel channel;
                if (ParseAnimationChannel(&channel, err, i.value())) {
                    // Only add the channel if the parsing succeeds.
                    animation->channels.push_back(channel);
                }
            }
        }
    }

    {
        json::const_iterator samplerIt = o.find("samplers");
        if ((samplerIt != o.end()) && samplerIt.value().is_array()) {
            const json& sampler_array = samplerIt.value();

            json::const_iterator it    = sampler_array.begin();
            json::const_iterator itEnd = sampler_array.end();

            for (; it != itEnd; it++) {
                const json& s = it->get<json>();

                AnimationSampler sampler;
                double           inputIndex  = -1.0;
                double           outputIndex = -1.0;
                if (!ParseNumberProperty(&inputIndex, err, s, "input", true)) {
                    if (err) {
                        (*err) += "`input` field is missing in animation.sampler\n";
                    }
                    return false;
                }
                if (!ParseStringProperty(&sampler.interpolation, err, s, "interpolation", true)) {
                    if (err) {
                        (*err) += "`interpolation` field is missing in animation.sampler\n";
                    }
                    return false;
                }
                if (!ParseNumberProperty(&outputIndex, err, s, "output", true)) {
                    if (err) {
                        (*err) += "`output` field is missing in animation.sampler\n";
                    }
                    return false;
                }
                sampler.input  = static_cast<int>(inputIndex);
                sampler.output = static_cast<int>(outputIndex);
                animation->samplers.push_back(sampler);
            }
        }
    }

    ParseStringProperty(&animation->name, err, o, "name", false);

    ParseExtrasProperty(&(animation->extras), o);

    return true;
}

static bool ParseSampler(Sampler* sampler, std::string* err, const json& o) {
    ParseStringProperty(&sampler->name, err, o, "name", false);

    double minFilter = static_cast<double>(TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR);
    double magFilter = static_cast<double>(TINYGLTF_TEXTURE_FILTER_LINEAR);
    double wrapS     = static_cast<double>(TINYGLTF_TEXTURE_WRAP_REPEAT);
    double wrapT     = static_cast<double>(TINYGLTF_TEXTURE_WRAP_REPEAT);
    ParseNumberProperty(&minFilter, err, o, "minFilter", false);
    ParseNumberProperty(&magFilter, err, o, "magFilter", false);
    ParseNumberProperty(&wrapS, err, o, "wrapS", false);
    ParseNumberProperty(&wrapT, err, o, "wrapT", false);

    sampler->minFilter = static_cast<int>(minFilter);
    sampler->magFilter = static_cast<int>(magFilter);
    sampler->wrapS     = static_cast<int>(wrapS);
    sampler->wrapT     = static_cast<int>(wrapT);

    ParseExtrasProperty(&(sampler->extras), o);

    return true;
}

static bool ParseSkin(Skin* skin, std::string* err, const json& o) {
    ParseStringProperty(&skin->name, err, o, "name", false, "Skin");

    std::vector<double> joints;
    if (!ParseNumberArrayProperty(&joints, err, o, "joints", false, "Skin")) {
        return false;
    }

    double skeleton = -1.0;
    ParseNumberProperty(&skeleton, err, o, "skeleton", false, "Skin");
    skin->skeleton = static_cast<int>(skeleton);

    skin->joints.resize(joints.size());
    for (size_t i = 0; i < joints.size(); i++) {
        skin->joints[i] = static_cast<int>(joints[i]);
    }

    double invBind = -1.0;
    ParseNumberProperty(&invBind, err, o, "inverseBindMatrices", true, "Skin");
    skin->inverseBindMatrices = static_cast<int>(invBind);

    return true;
}

static bool ParsePerspectiveCamera(PerspectiveCamera* camera, std::string* err, const json& o) {
    double yfov = 0.0;
    if (!ParseNumberProperty(&yfov, err, o, "yfov", true, "OrthographicCamera")) {
        return false;
    }

    double znear = 0.0;
    if (!ParseNumberProperty(&znear, err, o, "znear", true, "PerspectiveCamera")) {
        return false;
    }

    double aspectRatio = 0.0;  // = invalid
    ParseNumberProperty(&aspectRatio, err, o, "aspectRatio", false, "PerspectiveCamera");

    double zfar = 0.0;  // = invalid
    ParseNumberProperty(&zfar, err, o, "zfar", false, "PerspectiveCamera");

    camera->aspectRatio = float(aspectRatio);
    camera->zfar        = float(zfar);
    camera->yfov        = float(yfov);
    camera->znear       = float(znear);

    ParseExtensionsProperty(&camera->extensions, err, o);
    ParseExtrasProperty(&(camera->extras), o);

    // TODO(syoyo): Validate parameter values.

    return true;
}

static bool ParseOrthographicCamera(OrthographicCamera* camera, std::string* err, const json& o) {
    double xmag = 0.0;
    if (!ParseNumberProperty(&xmag, err, o, "xmag", true, "OrthographicCamera")) {
        return false;
    }

    double ymag = 0.0;
    if (!ParseNumberProperty(&ymag, err, o, "ymag", true, "OrthographicCamera")) {
        return false;
    }

    double zfar = 0.0;
    if (!ParseNumberProperty(&zfar, err, o, "zfar", true, "OrthographicCamera")) {
        return false;
    }

    double znear = 0.0;
    if (!ParseNumberProperty(&znear, err, o, "znear", true, "OrthographicCamera")) {
        return false;
    }

    ParseExtensionsProperty(&camera->extensions, err, o);
    ParseExtrasProperty(&(camera->extras), o);

    camera->xmag  = float(xmag);
    camera->ymag  = float(ymag);
    camera->zfar  = float(zfar);
    camera->znear = float(znear);

    // TODO(syoyo): Validate parameter values.

    return true;
}

static bool ParseCamera(Camera* camera, std::string* err, const json& o) {
    if (!ParseStringProperty(&camera->type, err, o, "type", true, "Camera")) {
        return false;
    }

    if (camera->type.compare("orthographic") == 0) {
        if (o.find("orthographic") == o.end()) {
            if (err) {
                std::stringstream ss;
                ss << "Orhographic camera description not found." << std::endl;
                (*err) += ss.str();
            }
            return false;
        }

        const json& v = o.find("orthographic").value();
        if (!v.is_object()) {
            if (err) {
                std::stringstream ss;
                ss << "\"orthographic\" is not a JSON object." << std::endl;
                (*err) += ss.str();
            }
            return false;
        }

        if (!ParseOrthographicCamera(&camera->orthographic, err, v.get<json>())) {
            return false;
        }
    } else if (camera->type.compare("perspective") == 0) {
        if (o.find("perspective") == o.end()) {
            if (err) {
                std::stringstream ss;
                ss << "Perspective camera description not found." << std::endl;
                (*err) += ss.str();
            }
            return false;
        }

        const json& v = o.find("perspective").value();
        if (!v.is_object()) {
            if (err) {
                std::stringstream ss;
                ss << "\"perspective\" is not a JSON object." << std::endl;
                (*err) += ss.str();
            }
            return false;
        }

        if (!ParsePerspectiveCamera(&camera->perspective, err, v.get<json>())) {
            return false;
        }
    } else {
        if (err) {
            std::stringstream ss;
            ss << "Invalid camera type: \"" << camera->type << "\". Must be \"perspective\" or \"orthographic\"" << std::endl;
            (*err) += ss.str();
        }
        return false;
    }

    ParseStringProperty(&camera->name, err, o, "name", false);

    ParseExtensionsProperty(&camera->extensions, err, o);
    ParseExtrasProperty(&(camera->extras), o);

    return true;
}

bool TinyGLTF::LoadFromString(Model* model, std::string* err, const char* str, unsigned int length, const std::string& base_dir,
                              bool use_draco_encode, unsigned int check_sections) {
    if (length < 4) {
        if (err) {
            (*err) = "JSON string too short.\n";
        }
        return false;
    }

    json v;

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && not defined(TINYGLTF_NOEXCEPTION)
    try {
        v = json::parse(str, str + length);
    } catch (const std::exception& e) {
        if (err) {
            (*err) = e.what();
        }
        return false;
    }
#else
    {
        v = json::parse(str, str + length, nullptr, /* exception */ false);

        if (!v.is_object()) {
            // Assume parsing was failed.
            if (err) {
                (*err) = "Failed to parse JSON object\n";
            }
            return false;
        }
    }
#endif

    if (!v.is_object()) {
        // root is not an object.
        if (err) {
            (*err) = "Root element is not a JSON object\n";
        }
        return false;
    }

    // scene is not mandatory.
    // FIXME Maybe a better way to handle it than removing the code

    {
        json::const_iterator it = v.find("scenes");
        if ((it != v.end()) && it.value().is_array()) {
            // OK
        } else if (check_sections & REQUIRE_SCENES) {
            if (err) {
                (*err) += "\"scenes\" object not found in .gltf or not an array type\n";
            }
            return false;
        }
    }

    {
        json::const_iterator it = v.find("nodes");
        if ((it != v.end()) && it.value().is_array()) {
            // OK
        } else if (check_sections & REQUIRE_NODES) {
            if (err) {
                (*err) += "\"nodes\" object not found in .gltf\n";
            }
            return false;
        }
    }

    {
        json::const_iterator it = v.find("accessors");
        if ((it != v.end()) && it.value().is_array()) {
            // OK
        } else if (check_sections & REQUIRE_ACCESSORS) {
            if (err) {
                (*err) += "\"accessors\" object not found in .gltf\n";
            }
            return false;
        }
    }

    {
        json::const_iterator it = v.find("buffers");
        if ((it != v.end()) && it.value().is_array()) {
            // OK
        } else if (check_sections & REQUIRE_BUFFERS) {
            if (err) {
                (*err) += "\"buffers\" object not found in .gltf\n";
            }
            return false;
        }
    }

    {
        json::const_iterator it = v.find("bufferViews");
        if ((it != v.end()) && it.value().is_array()) {
            // OK
        } else if (check_sections & REQUIRE_BUFFER_VIEWS) {
            if (err) {
                (*err) += "\"bufferViews\" object not found in .gltf\n";
            }
            return false;
        }
    }

    model->buffers.clear();
    model->bufferViews.clear();
    model->accessors.clear();
    model->meshes.clear();
    model->cameras.clear();
    model->nodes.clear();
    model->extensionsUsed.clear();
    model->extensionsRequired.clear();
    model->extensions.clear();
    model->defaultScene = -1;
#ifdef USE_MESH_DRACO_EXTENSION
    model->dracomeshes.clear();
#endif

    // 1. Parse Asset
    {
        json::const_iterator it = v.find("asset");
        if ((it != v.end()) && it.value().is_object()) {
            const json& root = it.value();

            ParseAsset(&model->asset, err, root);
        }
    }

    bool isBufferViewrequired = true;

#ifdef USE_MESH_DRACO_EXTENSION
    bool use_draco_decode = false;  //flag to track potential use of draco extension
    int  draco_id         = -1;
#endif
    // 2. Parse extensionUsed
    {
        json::const_iterator it = v.find("extensionsUsed");
        if ((it != v.end()) && it.value().is_array()) {
            const json& root = it.value();
            for (unsigned int i = 0; i < root.size(); ++i) {
                model->extensionsUsed.push_back(root[i].get<std::string>());
            }
        }
#ifdef USE_MESH_DRACO_EXTENSION
        if (model->extensionsUsed.size() > 0)
            isBufferViewrequired = false;
        else
            isBufferViewrequired = true;
#endif
    }

    {
        json::const_iterator it = v.find("extensionsRequired");
        if ((it != v.end()) && it.value().is_array()) {
            const json& root = it.value();
            for (unsigned int i = 0; i < root.size(); ++i) {
                model->extensionsRequired.push_back(root[i].get<std::string>());
            }
        }
#ifdef USE_MESH_DRACO_EXTENSION
        if (model->extensionsRequired.size() > 0)
            isBufferViewrequired = false;
        else
            isBufferViewrequired = true;
#endif
    }

#ifdef USE_MESH_DRACO_EXTENSION
    if ((model->extensionsRequired.size() > 0) || (model->extensionsUsed.size() > 0))
        use_draco_decode = true;
    else
        use_draco_decode = false;

    // if use draco is true, parse draco buffer view first
    if (use_draco_decode) {
        json::const_iterator rootIt = v.find("meshes");
        DracoData            dracodata;

        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root                = rootIt.value();
            int         mesh_primitive_size = (int)root.size();

#ifndef USE_MULTIPLE_MESH_DECODE
            if (mesh_primitive_size > 1) {
                if (err) {
                    (*err) = "Note: Mesh decode under development. Only support one \"meshes\" for now.";
                }
                return false;
            }
#endif
            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            int m = 0;
            int n = 0;
            for (; it != itEnd; ++it) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`meshes' does not contain an JSON object.";
                    }
                    return false;
                }
                const json           jsonMesh   = it->get<json>();
                json::const_iterator primObject = jsonMesh.find("primitives");

                dracodata.mesh_index = m;
                m++;

                if ((primObject != jsonMesh.end()) && primObject.value().is_array()) {
                    for (json::const_iterator i = primObject.value().begin(); i != primObject.value().end(); i++) {
                        dracodata.prim_index = n;
                        n++;

                        const json           jsonPrim = i.value();
                        json::const_iterator itExt    = jsonPrim.find("extensions");
                        if (itExt != jsonPrim.end()) {
                            if (itExt.value().is_object()) {
                                const json& values_object = itExt.value();

                                json::const_iterator itVal(values_object.begin());
                                json::const_iterator itValEnd(values_object.end());
                                for (; itVal != itValEnd; itVal++) {
                                    if (itVal.key() == "KHR_draco_mesh_compression") {
                                        if (use_draco_encode) {
                                            (*err) = "Error: Source File " + std::string(str) + " 's buffer is already draco-compressed.";
                                            return false;
                                        }
                                        if (itVal.value().is_object()) {
                                            const json& khrvalues_object = itVal.value();

                                            json::const_iterator itkhrVal(khrvalues_object.begin());
                                            json::const_iterator itkhrValEnd(khrvalues_object.end());
                                            for (; itkhrVal != itkhrValEnd; itkhrVal++) {
                                                if (!ParseNumberProperty(&(dracodata.draco_buffer_view), err, khrvalues_object, "bufferView",
                                                                         true, "KHR_draco_mesh_compression")) {
                                                    return false;
                                                }
                                            }
                                            if (!ParseStringIntProperty(&dracodata.draco_attributes, err, khrvalues_object, "attributes", true,
                                                                        "KHR_draco_mesh_compression")) {
                                                return false;
                                            }

                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        //if there is draco bufferview
                        if (dracodata.draco_buffer_view > -1) {
                            json::const_iterator bufferView = v.find("bufferViews");
                            if ((bufferView != v.end()) && bufferView.value().is_array()) {
                                int id = 0;
                                for (json::const_iterator itbufv = bufferView.value().begin(); itbufv != bufferView.value().end(); itbufv++) {
                                    const json jsonBufV = itbufv.value();
                                    //parse on the bufferview id 's buffer id
                                    if (id == dracodata.draco_buffer_view) {
                                        // reset byteLength stored earlier
                                        dracodata.byteLength = -1;

                                        // parse draco buffer ID
                                        if (!ParseNumberProperty(&(dracodata.buffer_id), err, jsonBufV, "buffer", true, "bufferViews")) {
                                            return false;
                                        }

                                        // parse draco buffer byteLength
                                        if (!ParseNumberProperty(&(dracodata.byteLength), err, jsonBufV, "byteLength", true, "bufferViews")) {
                                            return false;
                                        }
                                    }
                                    id++;
                                }
                            }
                        }

                        //if there is draco buffer id, then parse and decode the buffer
                        if (dracodata.buffer_id > -1) {
                            draco_id = (int)dracodata.buffer_id;
                            int                  id = 0;  //id
                            json::const_iterator rootIt2 = v.find("buffers");
                            if ((rootIt2 != v.end()) && rootIt2.value().is_array()) {
                                const json& root = rootIt2.value();

                                json::const_iterator it(root.begin());
                                json::const_iterator itEnd(root.end());
                                for (; it != itEnd; it++) {
                                    if (!it.value().is_object()) {
                                        if (err) {
                                            (*err) += "`buffers' does not contain an JSON object.";
                                        }
                                        return false;
                                    }

                                    //parse only draco buffer
                                    if (id == draco_id) {
                                        if (!ParseDracoBuffer(&dracodata, err, it->get<json>(), base_dir, is_binary_, bin_data_, bin_size_)) {
                                            return false;
                                        }
                                    }
                                    id++;
                                }
                                model->dracomeshes.push_back(dracodata);
                            }
                        }


                    }
                }
                n = 0;
            }
        }
    }
#endif

    // 3. Parse Buffer
    {
        json::const_iterator rootIt = v.find("buffers");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            int         i    = 0;
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`buffers' does not contain an JSON object.";
                    }
                    return false;
                }

#ifdef USE_MESH_DRACO_EXTENSION
                //skip parse draco buffer
                if (i == draco_id) {
                    continue;
                }
#endif

                Buffer buffer;
                if (!ParseBuffer(&buffer, err, it->get<json>(), base_dir, is_binary_, bin_data_, bin_size_)) {
                    return false;
                }

                model->buffers.push_back(buffer);
                i++;
            }
        }
    }

    // 4. Parse BufferView
    {
        json::const_iterator rootIt = v.find("bufferViews");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            int                  i = 0;
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`bufferViews' does not contain an JSON object.";
                    }
                    return false;
                }

#ifdef USE_MESH_DRACO_EXTENSION
                //skip parse draco bufferview
                if (i == draco_id) {
                    continue;
                }
#endif

                BufferView bufferView;
                if (!ParseBufferView(&bufferView, err, it->get<json>())) {
                    return false;
                }

                model->bufferViews.push_back(bufferView);
                i++;
            }
        }
    }

    // 5. Parse Accessor
    {
        json::const_iterator rootIt = v.find("accessors");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`accessors' does not contain an JSON object.";
                    }
                    return false;
                }
                Accessor accessor;
                if (!ParseAccessor(&accessor, err, it->get<json>(), isBufferViewrequired)) {
                    return false;
                }

                model->accessors.push_back(accessor);
            }
        }
    }

    // 6. Parse Mesh
    {
        json::const_iterator rootIt = v.find("meshes");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`meshes' does not contain an JSON object.";
                    }
                    return false;
                }
                Mesh mesh;
                if (!ParseMesh(&mesh, err, it->get<json>())) {
                    return false;
                }

                model->meshes.push_back(mesh);
            }
        }
    }

    // 7. Parse Node
    {
        json::const_iterator rootIt = v.find("nodes");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`nodes' does not contain an JSON object.";
                    }
                    return false;
                }
                Node node;
                if (!ParseNode(&node, err, it->get<json>())) {
                    return false;
                }

                model->nodes.push_back(node);
            }
        }
    }

    // 8. Parse scenes.
    {
        json::const_iterator rootIt = v.find("scenes");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!(it.value().is_object())) {
                    if (err) {
                        (*err) += "`scenes' does not contain an JSON object.";
                    }
                    return false;
                }
                const json&         o = it->get<json>();
                std::vector<double> nodes;
                if (!ParseNumberArrayProperty(&nodes, err, o, "nodes", false)) {
                    return false;
                }

                Scene scene;
                ParseStringProperty(&scene.name, err, o, "name", false);
                std::vector<int> nodesIds;
                for (size_t i = 0; i < nodes.size(); i++) {
                    nodesIds.push_back(static_cast<int>(nodes[i]));
                }
                scene.nodes = nodesIds;

                ParseExtensionsProperty(&scene.extensions, err, o);
                ParseExtrasProperty(&scene.extras, o);

                model->scenes.push_back(scene);
            }
        }
    }

    // 9. Parse default scenes.
    {
        json::const_iterator rootIt = v.find("scene");
        if ((rootIt != v.end()) && rootIt.value().is_number()) {
            const int defaultScene = rootIt.value();

            model->defaultScene = static_cast<int>(defaultScene);
        }
    }

    // 10. Parse Material
    {
        json::const_iterator rootIt = v.find("materials");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`materials' does not contain an JSON object.";
                    }
                    return false;
                }
                json jsonMaterial = it->get<json>();

                Material material;
                ParseStringProperty(&material.name, err, jsonMaterial, "name", false);

                if (!ParseMaterial(&material, err, jsonMaterial)) {
                    return false;
                }

                model->materials.push_back(material);
            }
        }
    }

    // 11. Parse Image
    {
        json::const_iterator rootIt = v.find("images");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`images' does not contain an JSON object.";
                    }
                    return false;
                }
                Image image;
                if (!ParseImage(&image, err, it.value(), base_dir, &this->LoadImageData, load_image_user_data_)) {
                    return false;
                }

                if (image.bufferView != -1) {
                    // Load image from the buffer view.
                    if (size_t(image.bufferView) >= model->bufferViews.size()) {
                        if (err) {
                            std::stringstream ss;
                            ss << "bufferView \"" << image.bufferView << "\" not found in the scene." << std::endl;
                            (*err) += ss.str();
                        }
                        return false;
                    }

                    const BufferView& bufferView = model->bufferViews[size_t(image.bufferView)];
                    const Buffer&     buffer     = model->buffers[size_t(bufferView.buffer)];

                    if (*LoadImageData != nullptr) {
                        bool ret = LoadImageData(&image, err, image.width, image.height, &buffer.data[bufferView.byteOffset],
                                                 static_cast<int>(bufferView.byteLength), load_image_user_data_);
                        if (!ret) {
                            return false;
                        }
                    } else
                        return true;
                }

                model->images.push_back(image);
            }
        }
    }

    // 12. Parse Texture
    {
        json::const_iterator rootIt = v.find("textures");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; it++) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`textures' does not contain an JSON object.";
                    }
                    return false;
                }
                Texture texture;
                if (!ParseTexture(&texture, err, it->get<json>(), base_dir)) {
                    return false;
                }

                model->textures.push_back(texture);
            }
        }
    }

    // 13. Parse Animation
    {
        json::const_iterator rootIt = v.find("animations");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; ++it) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`animations' does not contain an JSON object.";
                    }
                    return false;
                }
                Animation animation;
                if (!ParseAnimation(&animation, err, it->get<json>())) {
                    return false;
                }

                model->animations.push_back(animation);
            }
        }
    }

    // 14. Parse Skin
    {
        json::const_iterator rootIt = v.find("skins");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; ++it) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`skins' does not contain an JSON object.";
                    }
                    return false;
                }
                Skin skin;
                if (!ParseSkin(&skin, err, it->get<json>())) {
                    return false;
                }

                model->skins.push_back(skin);
            }
        }
    }

    // 15. Parse Sampler
    {
        json::const_iterator rootIt = v.find("samplers");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; ++it) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`samplers' does not contain an JSON object.";
                    }
                    return false;
                }
                Sampler sampler;
                if (!ParseSampler(&sampler, err, it->get<json>())) {
                    return false;
                }

                model->samplers.push_back(sampler);
            }
        }
    }

    // 16. Parse Camera
    {
        json::const_iterator rootIt = v.find("cameras");
        if ((rootIt != v.end()) && rootIt.value().is_array()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; ++it) {
                if (!it.value().is_object()) {
                    if (err) {
                        (*err) += "`cameras' does not contain an JSON object.";
                    }
                    return false;
                }
                Camera camera;
                if (!ParseCamera(&camera, err, it->get<json>())) {
                    return false;
                }

                model->cameras.push_back(camera);
            }
        }
    }

    // 17. Parse Extensions
    ParseExtensionsProperty(&model->extensions, err, v);

    // 18. Specific extension implementations
    {
        json::const_iterator rootIt = v.find("extensions");
        if ((rootIt != v.end()) && rootIt.value().is_object()) {
            const json& root = rootIt.value();

            json::const_iterator it(root.begin());
            json::const_iterator itEnd(root.end());
            for (; it != itEnd; ++it) {
                // parse KHR_lights_cmn extension
                if ((it.key().compare("KHR_lights_cmn") == 0) && it.value().is_object()) {
                    const json&          object = it.value();
                    json::const_iterator itLight(object.find("lights"));
                    json::const_iterator itLightEnd(object.end());
                    if (itLight == itLightEnd) {
                        continue;
                    }

                    if (!itLight.value().is_array()) {
                        continue;
                    }

                    const json&          lights = itLight.value();
                    json::const_iterator arrayIt(lights.begin());
                    json::const_iterator arrayItEnd(lights.end());
                    for (; arrayIt != arrayItEnd; ++arrayIt) {
                        Light light;
                        if (!ParseLight(&light, err, arrayIt.value())) {
                            return false;
                        }
                        model->lights.push_back(light);
                    }
                }
            }
        }
    }

#ifdef USE_MESH_DRACO_EXTENSION
    // Assign buffer to draco::mesh for draco encode case, expect only one buffer for now
    if (use_draco_encode) {
        std::vector<unsigned char> buf_data    = model->buffers[0].data;
        unsigned char*             buf_dataptr = nullptr;
        int                        data_stride = 0;
        draco::Mesh* draco_en_mesh;   //temp holder when indexing draco mesh vector

        for (int m = 0; m < model->meshes.size(); m++) {
            for (int n = 0; n < model->meshes[m].primitives.size(); n++) {
                draco_en_mesh = new draco::Mesh();
                for (std::map<std::string, int>::iterator attrIt = model->meshes[m].primitives[n].attributes.begin();
                        attrIt != model->meshes[m].primitives[n].attributes.end(); ++attrIt) {
                    draco::PointAttribute att;
                    std::string           attr_str = attrIt->first;
                    int                   attr_id = attrIt->second;
                    int8_t                comp_count = 3;
                    int                   data_count = (int)model->accessors[attr_id].count;
                    int                   attr_comp_type = model->accessors[attr_id].componentType;
                    int                   comp_type = model->accessors[attr_id].type;

                    switch (comp_type) {
                    case TINYGLTF_TYPE_VEC2:
                        comp_count = 2;
                        break;
                    case TINYGLTF_TYPE_VEC3:
                        comp_count = 3;
                        break;
                    case TINYGLTF_TYPE_VEC4:
                        comp_count = 4;
                        break;
                    case TINYGLTF_TYPE_SCALAR:
                        comp_count = 1;
                        break;
                    }

                    int bufferV_ind = model->accessors[attr_id].bufferView;
                    int byte_length = (int)model->bufferViews[bufferV_ind].byteLength;
                    int byte_offset = 0;
                    int byte_offset_accessor = (int)model->accessors[attr_id].byteOffset;
                    int byte_offset_bufview = (int)model->bufferViews[bufferV_ind].byteOffset;

                    byte_offset = byte_offset_accessor + byte_offset_bufview;

                    buf_dataptr = reinterpret_cast<unsigned char*>(&buf_data[0]);
                    buf_dataptr += byte_offset;

                    draco::GeometryAttribute::Type type;
                    if (attr_str == "NORMAL") {
                        type = draco::GeometryAttribute::NORMAL;
                    } else if (attr_str == "POSITION") {
                        type = draco::GeometryAttribute::POSITION;
                    } else if (attr_str.find("TEXCOORD") != std::string::npos) {
                        type = draco::GeometryAttribute::TEX_COORD;
                    } else if (attr_str == "TANGENT") {
                        type = draco::GeometryAttribute::GENERIC;
                    } else {
                        if (err) {
                            (*err) += "Attribute [" + attr_str + "] not supported yet.";
                        }
                        return false;
                    }

                    draco::DataType data_type = draco::DataType::DT_INT8;
                    switch (attr_comp_type) {
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                        data_type = draco::DataType::DT_INT8;
                        data_stride = 1;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        data_type = draco::DataType::DT_UINT8;
                        data_stride = 1;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                        data_type = draco::DataType::DT_INT16;
                        data_stride = 2;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        data_type = draco::DataType::DT_UINT16;
                        data_stride = 2;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_INT:
                        data_type = draco::DataType::DT_INT32;
                        data_stride = 4;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        data_type = draco::DataType::DT_UINT32;
                        data_stride = 4;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        data_type = draco::DataType::DT_FLOAT32;
                        data_stride = 4;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        data_type = draco::DataType::DT_FLOAT32;
                        data_stride = 4;
                        break;
                    }

                    att.Init(type, nullptr, comp_count, data_type, false, comp_count * draco::DataTypeLength(data_type), 0);

                    const int              dracoAttId = draco_en_mesh->AddAttribute(att, true, data_count);
                    draco::PointAttribute* attPtr = draco_en_mesh->attribute(dracoAttId);

                    std::vector<uint8_t> buf;
                    buf.clear();
                    buf.resize(byte_length);
                    for (int ii = 0; ii < data_count; ii++) {
                        //uint8_t*     ptr = &buf[0];
                        std::vector<char> temp;
                        temp.insert(temp.end(), buf_dataptr, buf_dataptr + (data_stride * comp_count));
                        attPtr->SetAttributeValue(attPtr->mapped_index(draco::PointIndex(ii)), &temp[0]);
                        temp.clear();
                        buf_dataptr += (data_stride * comp_count);
                    }
                }

                if (draco_en_mesh->num_attributes() > 0) {
                    // Add faces with identity mapping between vertex and corner indices.
                    // Duplicate vertices will get removed later.
                    int accessor_ind = model->meshes[m].primitives[n].indices;
                    int num_idx = (int)model->accessors[accessor_ind].count;
                    int num_face = num_idx / 3;
                    int bufferV_ind = model->accessors[accessor_ind].bufferView;

                    //retrieve index data
                    unsigned  byte_offset = 0;
                    unsigned byte_offset_accessor = (int)model->accessors[accessor_ind].byteOffset;
                    unsigned byte_offset_bufview = (int)model->bufferViews[bufferV_ind].byteOffset;

                    byte_offset = byte_offset_accessor + byte_offset_bufview;

                    buf_data = model->buffers[0].data;
                    if (buf_data.size() <= byte_offset) {
                        (*err) = "Error loading: byte offset is bigger than byte length of data buffer.";
                        return false;
                    }
                    buf_dataptr = reinterpret_cast<unsigned char*>(&buf_data[0]);
                    buf_dataptr += byte_offset;

                    int comp_type = model->accessors[accessor_ind].componentType;
                    int data_stride = 0;
                    switch (comp_type) {
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        data_stride = 1;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        data_stride = 2;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_INT:
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        data_stride = 4;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        data_stride = 8;
                        break;
                    }

                    draco::Mesh::Face     face;
                    std::vector<unsigned char> temp;
                    for (int ii = 0; ii < num_face; ii++) {
                        temp.insert(temp.end(), buf_dataptr, buf_dataptr + data_stride);
                        memcpy(&face[0], temp.data(), data_stride);
                        temp.clear();
                        buf_dataptr += data_stride;
                        temp.insert(temp.end(), buf_dataptr, buf_dataptr + data_stride);
                        memcpy(&face[1], temp.data(), data_stride);
                        temp.clear();
                        buf_dataptr += data_stride;
                        temp.insert(temp.end(), buf_dataptr, buf_dataptr + data_stride);
                        memcpy(&face[2], temp.data(), data_stride);
                        temp.clear();
                        buf_dataptr += data_stride;
                        draco_en_mesh->SetFace(draco::FaceIndex(ii), face);
                    }

                    draco_en_meshes.push_back(draco_en_mesh);
                }

            }
        }

    }
#endif
    return true;
}

bool TinyGLTF::LoadASCIIFromString(Model* model, std::string* err, const char* str, unsigned int length, const std::string& base_dir,
                                   bool use_draco_encode, unsigned int check_sections) {
    is_binary_ = false;
    bin_data_  = nullptr;
    bin_size_  = 0;

    return LoadFromString(model, err, str, length, base_dir, use_draco_encode, check_sections);
}

bool TinyGLTF::LoadASCIIFromFile(Model* model, std::string* err, const std::string& filename, bool use_draco_encode, unsigned int check_sections) {
    std::stringstream ss;

    std::ifstream f(filename.c_str());
    if (!f) {
        ss << "Failed to open file: " << filename << std::endl;
        if (err) {
            (*err) = ss.str();
        }
        return false;
    }

#ifdef USE_MESH_DRACO_EXTENSION
    // clean up the draco meshes for each load
    draco_en_meshes.clear();
    draco_de_meshes.clear();
#endif
    f.seekg(0, f.end);
    size_t            sz = static_cast<size_t>(f.tellg());
    std::vector<char> buf(sz);

    if (sz == 0) {
        if (err) {
            (*err) = "Empty file.";
        }
        return false;
    }

    f.seekg(0, f.beg);
    f.read(&buf.at(0), static_cast<std::streamsize>(sz));
    f.close();

    std::string basedir = GetBaseDir(filename);

    bool ret = LoadASCIIFromString(model, err, &buf.at(0), static_cast<unsigned int>(buf.size()), basedir, use_draco_encode, check_sections);

    return ret;
}

bool TinyGLTF::LoadBinaryFromMemory(Model* model, std::string* err, const unsigned char* bytes, unsigned int size, const std::string& base_dir,
                                    unsigned int check_sections) {
    if (size < 20) {
        if (err) {
            (*err) = "Too short data size for glTF Binary.";
        }
        return false;
    }

    if (bytes[0] == 'g' && bytes[1] == 'l' && bytes[2] == 'T' && bytes[3] == 'F') {
        // ok
    } else {
        if (err) {
            (*err) = "Invalid magic.";
        }
        return false;
    }

    unsigned int version;       // 4 bytes
    unsigned int length;        // 4 bytes
    unsigned int model_length;  // 4 bytes
    unsigned int model_format;  // 4 bytes;

    // @todo { Endian swap for big endian machine. }
    memcpy(&version, bytes + 4, 4);
    swap4(&version);
    memcpy(&length, bytes + 8, 4);
    swap4(&length);
    memcpy(&model_length, bytes + 12, 4);
    swap4(&model_length);
    memcpy(&model_format, bytes + 16, 4);
    swap4(&model_format);

    // In case the Bin buffer is not present, the size is exactly 20 + size of
    // JSON contents,
    // so use "greater than" operator.
    if ((20 + model_length > size) || (model_length < 1) || (model_format != 0x4E4F534A)) {
        // 0x4E4F534A = JSON format.
        if (err) {
            (*err) = "Invalid glTF binary.";
        }
        return false;
    }

    // Extract JSON string.
    std::string jsonString(reinterpret_cast<const char*>(&bytes[20]), model_length);

    is_binary_ = true;
    bin_data_  = bytes + 20 + model_length + 8;  // 4 bytes (buffer_length) + 4 bytes(buffer_format)
    bin_size_  = length - (20 + model_length);   // extract header + JSON scene data.

    bool ret = LoadFromString(model, err, reinterpret_cast<const char*>(&bytes[20]), model_length, base_dir, false, check_sections);
    if (!ret) {
        return ret;
    }

    return true;
}

bool TinyGLTF::LoadBinaryFromFile(Model* model, std::string* err, const std::string& filename, unsigned int check_sections) {
    std::stringstream ss;

    std::ifstream f(filename.c_str(), std::ios::binary);
    if (!f) {
        ss << "Failed to open file: " << filename << std::endl;
        if (err) {
            (*err) = ss.str();
        }
        return false;
    }

    f.seekg(0, f.end);
    size_t            sz = static_cast<size_t>(f.tellg());
    std::vector<char> buf(sz);

    f.seekg(0, f.beg);
    f.read(&buf.at(0), static_cast<std::streamsize>(sz));
    f.close();

    std::string basedir = GetBaseDir(filename);

    bool ret = LoadBinaryFromMemory(model, err, reinterpret_cast<unsigned char*>(&buf.at(0)), static_cast<unsigned int>(buf.size()), basedir,
                                    check_sections);

    return ret;
}

///////////////////////
// GLTF Serialization
///////////////////////

// typedef std::pair<std::string, json> json_object_pair;

template <typename T>
static void SerializeNumberProperty(const std::string& key, T number, json& obj) {
    // obj.insert(
    //    json_object_pair(key, json(static_cast<double>(number))));
    // obj[key] = static_cast<double>(number);
    obj[key] = number;
}

template <typename T>
static void SerializeNumberArrayProperty(const std::string& key, const std::vector<T>& value, json& obj) {
    json o;
    json vals;

    for (unsigned int i = 0; i < value.size(); ++i) {
        vals.push_back(static_cast<double>(value[i]));
    }
    if (!vals.is_null()) {
        obj[key] = vals;
    }
}

static void SerializeStringProperty(const std::string& key, const std::string& value, json& obj) {
    obj[key] = value;
}

static void SerializeStringArrayProperty(const std::string& key, const std::vector<std::string>& value, json& obj) {
    json o;
    json vals;

    for (unsigned int i = 0; i < value.size(); ++i) {
        vals.push_back(value[i]);
    }

    obj[key] = vals;
}

static bool ValueToJson(const Value& value, json* ret) {
    json obj;
    switch (value.Type()) {
    case NUMBER_TYPE:
        obj = json(value.Get<double>());
        break;
    case INT_TYPE:
        obj = json(value.Get<int>());
        break;
    case BOOL_TYPE:
        obj = json(value.Get<bool>());
        break;
    case STRING_TYPE:
        obj = json(value.Get<std::string>());
        break;
    case ARRAY_TYPE: {
        for (unsigned int i = 0; i < value.ArrayLen(); ++i) {
            Value elementValue = value.Get(int(i));
            json  elementJson;
            if (ValueToJson(value.Get(int(i)), &elementJson))
                obj.push_back(elementJson);
        }
        break;
    }
    case BINARY_TYPE:
        //TODO
        //obj = json(value.Get<std::vector<unsigned char>>());
        return false;
        break;
    case OBJECT_TYPE: {
        Value::Object objMap = value.Get<Value::Object>();
        for (auto& it : objMap) {
            json elementJson;
            if (ValueToJson(it.second, &elementJson))
                obj[it.first] = elementJson;
        }
        break;
    }
    case NULL_TYPE:
    default:
        return false;
    }
    if (ret)
        *ret = obj;
    return true;
}

static void SerializeValue(const std::string& key, const Value& value, json& obj) {
    json ret;
    if (ValueToJson(value, &ret))
        obj[key] = ret;
}

static void SerializeGltfBufferData(const std::vector<unsigned char>& data, json& o) {
    std::string header      = "data:application/octet-stream;base64,";
    std::string encodedData = base64_encode(&data[0], static_cast<unsigned int>(data.size()));
    SerializeStringProperty("uri", header + encodedData, o);
}

static void SerializeGltfBufferData(const std::vector<unsigned char>& data, const std::string& binFilename) {
    std::ofstream output(binFilename.c_str(), std::ofstream::binary);
    output.write(reinterpret_cast<const char*>(&data[0]), std::streamsize(data.size()));
    output.close();
}

static void SerializeParameterMap(ParameterMap& param, json& o) {
    for (ParameterMap::iterator paramIt = param.begin(); paramIt != param.end(); ++paramIt) {
        if (paramIt->second.number_array.size()) {
            SerializeNumberArrayProperty<double>(paramIt->first, paramIt->second.number_array, o);
        } else if (paramIt->second.json_double_value.size()) {
            json json_double_value;

            for (std::map<std::string, double>::iterator it = paramIt->second.json_double_value.begin();
                    it != paramIt->second.json_double_value.end(); ++it) {
                json_double_value[it->first] = it->second;
            }

            o[paramIt->first] = json_double_value;
        } else if (!paramIt->second.string_value.empty()) {
            SerializeStringProperty(paramIt->first, paramIt->second.string_value, o);
        } else {
            o[paramIt->first] = paramIt->second.bool_value;
        }
    }
}

static void SerializeExtensionMap(ExtensionMap& extensions, json& o) {
    if (!extensions.size())
        return;

    json extMap;
    for (ExtensionMap::iterator extIt = extensions.begin(); extIt != extensions.end(); ++extIt) {
        json extension_values;
        SerializeValue(extIt->first, extIt->second, extMap);
    }
    o["extensions"] = extMap;
}

static void SerializeGltfAccessor(Accessor& accessor, json& o, bool is_draco_encode = false) {
    if (!is_draco_encode)
        SerializeNumberProperty<int>("bufferView", accessor.bufferView, o);

    if (accessor.byteOffset != 0.0)
        SerializeNumberProperty<int>("byteOffset", int(accessor.byteOffset), o);

    SerializeNumberProperty<int>("componentType", accessor.componentType, o);
    SerializeNumberProperty<size_t>("count", accessor.count, o);
    SerializeNumberArrayProperty<double>("min", accessor.minValues, o);
    SerializeNumberArrayProperty<double>("max", accessor.maxValues, o);
    std::string type;
    switch (accessor.type) {
    case TINYGLTF_TYPE_SCALAR:
        type = "SCALAR";
        break;
    case TINYGLTF_TYPE_VEC2:
        type = "VEC2";
        break;
    case TINYGLTF_TYPE_VEC3:
        type = "VEC3";
        break;
    case TINYGLTF_TYPE_VEC4:
        type = "VEC4";
        break;
    case TINYGLTF_TYPE_MAT2:
        type = "MAT2";
        break;
    case TINYGLTF_TYPE_MAT3:
        type = "MAT3";
        break;
    case TINYGLTF_TYPE_MAT4:
        type = "MAT4";
        break;
    }

    SerializeStringProperty("type", type, o);
}

static void SerializeGltfAnimationChannel(AnimationChannel& channel, json& o) {
    SerializeNumberProperty("sampler", channel.sampler, o);
    json target;
    SerializeNumberProperty("node", channel.target_node, target);
    SerializeStringProperty("path", channel.target_path, target);

    o["target"] = target;
}

static void SerializeGltfAnimationSampler(AnimationSampler& sampler, json& o) {
    SerializeNumberProperty("input", sampler.input, o);
    SerializeNumberProperty("output", sampler.output, o);
    SerializeStringProperty("interpolation", sampler.interpolation, o);
}

static void SerializeGltfAnimation(Animation& animation, json& o) {
    SerializeStringProperty("name", animation.name, o);
    json channels;
    for (unsigned int i = 0; i < animation.channels.size(); ++i) {
        json             channel;
        AnimationChannel gltfChannel = animation.channels[i];
        SerializeGltfAnimationChannel(gltfChannel, channel);
        channels.push_back(channel);
    }
    o["channels"] = channels;

    json samplers;
    for (unsigned int i = 0; i < animation.samplers.size(); ++i) {
        json             sampler;
        AnimationSampler gltfSampler = animation.samplers[i];
        SerializeGltfAnimationSampler(gltfSampler, sampler);
        samplers.push_back(sampler);
    }

    o["samplers"] = samplers;
}

static void SerializeGltfAsset(Asset& asset, json& o) {
    if (!asset.generator.empty()) {
        SerializeStringProperty("generator", asset.generator, o);
    }

    if (!asset.version.empty()) {
        SerializeStringProperty("version", asset.version, o);
    }

    if (asset.extras.Keys().size()) {
        SerializeValue("extras", asset.extras, o);
    }

    SerializeExtensionMap(asset.extensions, o);
}

static void SerializeGltfBuffer(Buffer& buffer, json& o) {
    SerializeNumberProperty("byteLength", buffer.data.size(), o);
    SerializeGltfBufferData(buffer.data, o);

    if (buffer.name.size())
        SerializeStringProperty("name", buffer.name, o);
}

static void SerializeGltfBuffer(Buffer& buffer, json& o, const std::string& binFilename, const std::string& binBaseFilename) {
    SerializeGltfBufferData(buffer.data, binFilename);
    SerializeNumberProperty("byteLength", buffer.data.size(), o);
    SerializeStringProperty("uri", binBaseFilename, o);

    if (buffer.name.size())
        SerializeStringProperty("name", buffer.name, o);
}

static void SerializeGltfBufferView(BufferView& bufferView, json& o) {
    SerializeNumberProperty("buffer", bufferView.buffer, o);
    SerializeNumberProperty<size_t>("byteLength", bufferView.byteLength, o);

    // byteStride is optional, minimum allowed is 4
    if (bufferView.byteStride >= 4) {
        SerializeNumberProperty<size_t>("byteStride", bufferView.byteStride, o);
    }
    // byteOffset is optional, default is 0
    if (bufferView.byteOffset > 0) {
        SerializeNumberProperty<size_t>("byteOffset", bufferView.byteOffset, o);
    }
    // Target is optional, check if it contains a valid value
    if (bufferView.target == TINYGLTF_TARGET_ARRAY_BUFFER || bufferView.target == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER) {
        SerializeNumberProperty("target", bufferView.target, o);
    }
    if (bufferView.name.size()) {
        SerializeStringProperty("name", bufferView.name, o);
    }
}

static void SerializeGltfImage(Image& image, json& o) {
    SerializeStringProperty("uri", image.uri, o);

    if (image.name.size()) {
        SerializeStringProperty("name", image.name, o);
    }
}

static void SerializeGltfMaterial(Material& material, json& o) {
    if (material.extras.Size())
        SerializeValue("extras", material.extras, o);
    SerializeExtensionMap(material.extensions, o);

    if (material.values.size()) {
        json pbrMetallicRoughness;
        SerializeParameterMap(material.values, pbrMetallicRoughness);
        o["pbrMetallicRoughness"] = pbrMetallicRoughness;
    }

    SerializeParameterMap(material.additionalValues, o);

    if (material.name.size()) {
        SerializeStringProperty("name", material.name, o);
    }
}

static bool SerializeGltfMesh(Mesh& mesh, json& o, int* mesh_index, bool use_draco_encode = false) {
    json primitives;
    for (unsigned int i = 0; i < mesh.primitives.size(); ++i) {
        json      primitive;
        json      attributes;
        Primitive gltfPrimitive = mesh.primitives[i];
        for (std::map<std::string, int>::iterator attrIt = gltfPrimitive.attributes.begin(); attrIt != gltfPrimitive.attributes.end(); ++attrIt) {
            SerializeNumberProperty<int>(attrIt->first, attrIt->second, attributes);
        }

        primitive["attributes"] = attributes;

        // Indicies is optional
        if (gltfPrimitive.indices > -1) {
            SerializeNumberProperty<int>("indices", gltfPrimitive.indices, primitive);
        }
        // Material is optional
        if (gltfPrimitive.material > -1) {
            SerializeNumberProperty<int>("material", gltfPrimitive.material, primitive);
        }
#ifdef USE_MESH_DRACO_EXTENSION
        // write extension for draco encode (for now we expect only one primitive for draco encode)
        if (use_draco_encode) {
            json khrcomp;
            json khrext;
            json      khrattributes;
            Primitive gltfkhrPrimitive = mesh.primitives[i];
            draco::Mesh* draco_en_mesh = nullptr;

            for (std::map<std::string, int>::iterator khrattrIt = gltfkhrPrimitive.attributes.begin();
                    khrattrIt != gltfkhrPrimitive.attributes.end(); ++khrattrIt) {
                draco::GeometryAttribute::Type type;
                if (khrattrIt->first == "NORMAL") {
                    type = draco::GeometryAttribute::NORMAL;
                } else if (khrattrIt->first == "TANGENT") {
                    type = draco::GeometryAttribute::GENERIC;
                } else if (khrattrIt->first == "POSITION") {
                    type = draco::GeometryAttribute::POSITION;
                } else if (khrattrIt->first.find("TEXCOORD") != std::string::npos) {
                    type = draco::GeometryAttribute::TEX_COORD;
                } else {
                    printf("Draco encode failed: Attribute [%s] not supported yet.\n", khrattrIt->first.c_str());
                    return false;
                }

                //GetNamedAttributeId
                if (mesh_index) {
                    if ((*mesh_index) < draco_en_meshes.size()) {
                        draco_en_mesh = draco_en_meshes[*mesh_index];
                    } else {
                        printf("Writting draco encode failed: draco meshes numbers is not match with primitives numbers.\n");
                        return false;
                    }
                } else {
                    printf("Writting draco encode failed: draco meshes index is null.\n");
                    return false;
                }
                khrattributes[khrattrIt->first] = draco_en_mesh->GetNamedAttributeId(type);
            }
            if (mesh_index) {
                //todo: check with bufferviews, maybe this one is wrong
                khrext["bufferView"] = (*mesh_index);
                (*mesh_index)++;
            } else {
                printf("Writting draco encode failed: draco meshes index is null.\n");
                return false;
            }
            khrext["attributes"]                  = khrattributes;
            khrcomp["KHR_draco_mesh_compression"] = khrext;
            primitive["extensions"]               = khrcomp;
        }
#endif
        SerializeNumberProperty<int>("mode", gltfPrimitive.mode, primitive);

        // Morph targets
        if (gltfPrimitive.targets.size()) {
            json targets;
            for (unsigned int k = 0; k < gltfPrimitive.targets.size(); ++k) {
                json                       targetAttributes;
                std::map<std::string, int> targetData = gltfPrimitive.targets[k];
                for (std::map<std::string, int>::iterator attrIt = targetData.begin(); attrIt != targetData.end(); ++attrIt) {
                    SerializeNumberProperty<int>(attrIt->first, attrIt->second, targetAttributes);
                }

                targets.push_back(targetAttributes);
            }
            primitive["targets"] = targets;
        }

        primitives.push_back(primitive);
    }

    o["primitives"] = primitives;
    if (mesh.weights.size()) {
        SerializeNumberArrayProperty<double>("weights", mesh.weights, o);
    }

    if (mesh.name.size()) {
        SerializeStringProperty("name", mesh.name, o);
    }

    return true;
}

static void SerializeGltfLight(Light& light, json& o) {
    SerializeStringProperty("name", light.name, o);
    SerializeNumberArrayProperty("color", light.color, o);
    SerializeStringProperty("type", light.type, o);
}

static void SerializeGltfNode(Node& node, json& o) {
    if (node.translation.size() > 0) {
        SerializeNumberArrayProperty<double>("translation", node.translation, o);
    }
    if (node.rotation.size() > 0) {
        SerializeNumberArrayProperty<double>("rotation", node.rotation, o);
    }
    if (node.scale.size() > 0) {
        SerializeNumberArrayProperty<double>("scale", node.scale, o);
    }
    if (node.matrix.size() > 0) {
        SerializeNumberArrayProperty<double>("matrix", node.matrix, o);
    }
    if (node.mesh != -1) {
        SerializeNumberProperty<int>("mesh", node.mesh, o);
    }

    if (node.skin != -1) {
        SerializeNumberProperty<int>("skin", node.skin, o);
    }

    if (node.camera != -1) {
        SerializeNumberProperty<int>("camera", node.camera, o);
    }

    SerializeExtensionMap(node.extensions, o);
    SerializeStringProperty("name", node.name, o);
    SerializeNumberArrayProperty<int>("children", node.children, o);
}

static void SerializeGltfSampler(Sampler& sampler, json& o) {
    SerializeNumberProperty("magFilter", sampler.magFilter, o);
    SerializeNumberProperty("minFilter", sampler.minFilter, o);
    SerializeNumberProperty("wrapS", sampler.wrapS, o);
    SerializeNumberProperty("wrapT", sampler.wrapT, o);
}

static void SerializeGltfOrthographicCamera(const OrthographicCamera& camera, json& o) {
    SerializeNumberProperty("zfar", camera.zfar, o);
    SerializeNumberProperty("znear", camera.znear, o);
    SerializeNumberProperty("xmag", camera.xmag, o);
    SerializeNumberProperty("ymag", camera.ymag, o);
}

static void SerializeGltfPerspectiveCamera(const PerspectiveCamera& camera, json& o) {
    SerializeNumberProperty("zfar", camera.zfar, o);
    SerializeNumberProperty("znear", camera.znear, o);
    if (camera.aspectRatio > 0) {
        SerializeNumberProperty("aspectRatio", camera.aspectRatio, o);
    }

    if (camera.yfov > 0) {
        SerializeNumberProperty("yfov", camera.yfov, o);
    }
}

static void SerializeGltfCamera(const Camera& camera, json& o) {
    SerializeStringProperty("type", camera.type, o);
    if (!camera.name.empty()) {
        SerializeStringProperty("name", camera.type, o);
    }

    if (camera.type.compare("orthographic") == 0) {
        json orthographic;
        SerializeGltfOrthographicCamera(camera.orthographic, orthographic);
        o["orthographic"] = orthographic;
    } else if (camera.type.compare("perspective") == 0) {
        json perspective;
        SerializeGltfPerspectiveCamera(camera.perspective, perspective);
        o["perspective"] = perspective;
    } else {
        // ???
    }
}

static void SerializeGltfScene(Scene& scene, json& o) {
    SerializeNumberArrayProperty<int>("nodes", scene.nodes, o);

    if (scene.name.size()) {
        SerializeStringProperty("name", scene.name, o);
    }
    if (scene.extras.Keys().size()) {
        SerializeValue("extras", scene.extras, o);
    }
    SerializeExtensionMap(scene.extensions, o);
}

static void SerializeGltfSkin(Skin& skin, json& o) {
    if (skin.inverseBindMatrices != -1)
        SerializeNumberProperty("inverseBindMatrices", skin.inverseBindMatrices, o);

    SerializeNumberArrayProperty<int>("joints", skin.joints, o);
    SerializeNumberProperty("skeleton", skin.skeleton, o);
    if (skin.name.size()) {
        SerializeStringProperty("name", skin.name, o);
    }
}

static void SerializeGltfTexture(Texture& texture, json& o) {
    if (texture.sampler > -1) {
        SerializeNumberProperty("sampler", texture.sampler, o);
    }
    SerializeNumberProperty("source", texture.source, o);

    if (texture.extras.Size()) {
        json extras;
        SerializeValue("extras", texture.extras, o);
    }
    SerializeExtensionMap(texture.extensions, o);
}

static void WriteGltfFile(const std::string& output, const std::string& content) {
    std::ofstream gltfFile(output.c_str());
    gltfFile << content << std::endl;
}

bool TinyGLTF::WriteGltfSceneToFile(tinygltf2::Model* model, std::string* err, const std::string& filename, CMP_CompressOptions& option, bool decodedDraco,
                                    bool use_draco_encode, bool embedImages, bool embedBuffers

                                    /*, bool writeBinary*/) {
    json output;
#ifdef USE_MESH_DRACO_EXTENSION
    //write the decoded draco buffer to gltf file
    if (decodedDraco) {
        if (model->dracomeshes.size() == 0) {
            *err = "Error: Source File is not compressed. No decoded buffer found.";
            return false;
        }

        if (model->dracomeshes.size() != draco_de_meshes.size()) {
            *err = "Error: decoded draco meshes number and model draco buffer numbers are not match.";
            return false;
        }

        // ACCESSORS
        json        accessors;
        std::string type;
        int         count = 0;
        int         i     = 0;

        for (int m = 0; m < model->dracomeshes.size(); m++) {
            int temp_mesh_idx = model->dracomeshes[m].mesh_index;
            int temp_prim_idx = model->dracomeshes[m].prim_index;
            for (auto& x : model->dracomeshes[m].draco_attributes) {
                json accessor;
                accessor["bufferView"] = i;
                accessor["componentType"] = 5126;
                int id = x.second;
                count = (int)draco_de_meshes[m]->GetAttributeByUniqueId(id)->indices_map_size();
                accessor["count"] = count;

                //type is hardcoded to support as below
                if (x.first.find("TEXCOORD") != std::string::npos) {
                    type = "VEC2";
                } else if (x.first == "NORMAL") {
                    type = "VEC3";
                } else if (x.first == "TANGENT") {
                    type = "VEC4";
                } else if (x.first == "POSITION") {
                    type = "VEC3";
                    int oriattri_id = model->meshes[temp_mesh_idx].primitives[temp_prim_idx].attributes[x.first];  //original attribute id

                    SerializeNumberArrayProperty<double>("min", model->accessors[oriattri_id].minValues, accessor);
                    SerializeNumberArrayProperty<double>("max", model->accessors[oriattri_id].maxValues, accessor);
                }
                accessor["type"] = type;
                accessors.push_back(accessor);

                //to do: find primitive with draco extension then update the attribute id respectively
                //now just assume single mesh and single primitive

                model->meshes[temp_mesh_idx].primitives[temp_prim_idx].attributes[x.first] = i;
                i++;
            }

            //index
            json accessor;
            accessor["bufferView"] = i;
            accessor["componentType"] = 5125;

            count = draco_de_meshes[m]->num_faces() * 3;
            accessor["count"] = count;
            type = "SCALAR";
            accessor["type"] = type;
            accessors.push_back(accessor);
            //update existing mesh array
            model->meshes[temp_mesh_idx].primitives[temp_prim_idx].indices = i;
            i++;
            output["accessors"] = accessors;
        }
    } else
#endif
    {
        // ACCESSORS
        json accessors;
        for (unsigned int i = 0; i < model->accessors.size(); ++i) {
            json accessor;
            SerializeGltfAccessor(model->accessors[i], accessor, use_draco_encode);
            accessors.push_back(accessor);
        }
        output["accessors"] = accessors;
    }

    // ANIMATIONS
    if (model->animations.size()) {
        json animations;
        for (unsigned int i = 0; i < model->animations.size(); ++i) {
            if (model->animations[i].channels.size()) {
                json animation;
                SerializeGltfAnimation(model->animations[i], animation);
                animations.push_back(animation);
            }
        }
        output["animations"] = animations;
    }

    // ASSET
    json asset;

    //Update model asset generator
    model->asset.generator  = COMPRESSONATOR_TEXT;
    model->asset.version    = VERSION_TEXT_SHORT;
    model->asset.minVersion = "";
    model->asset.copyright  = "";

    SerializeGltfAsset(model->asset, asset);
    output["asset"] = asset;

    std::string            binFilename = GetFileName(filename);
    std::string            ext         = ".bin";
    std::string::size_type pos         = binFilename.rfind('.', binFilename.length());

    if (pos != std::string::npos) {
        binFilename = binFilename.substr(0, pos) + ext;
    } else {
        binFilename = binFilename + ".bin";
    }
    std::string baseDir = GetBaseDir(filename);
    if (baseDir.empty()) {
        baseDir = "./";
    }

    std::string binSaveFilePath = JoinPath(baseDir, binFilename);
#ifdef USE_MESH_DRACO_EXTENSION
    // BUFFERS and BUFFERVIEWS (Note: We expect only one buffer here)
    if (decodedDraco) {
        // BUFFERVIEWS
        json              bufferViews;
        int               byteLength = 0;
        int               byteOffset = 0;
        std::vector<char> tempdata;
        std::vector<char> data;

        for(int i =0; i < model->dracomeshes.size(); i++) {
            for (auto& x : model->dracomeshes[i].draco_attributes) {
                json                               bufferView;
                int                                id = x.second;
                const draco::PointAttribute* const att = draco_de_meshes[i]->GetAttributeByUniqueId(id);
                int                                dimension = 0;  //SCALAR =1, VEC2=2, VEC3=3, VEC4 =4
                int                                formatsz = 0;  //format refers to int, float etc.

                if (x.first.find("TEXCOORD") != std::string::npos) {
                    dimension = 2;
                    formatsz = 4;
                    std::array<float, 2> value;

                    //loop through draco mesh indices map
                    for (draco::AttributeValueIndex i(0); i < (int)att->indices_map_size(); ++i) {
                        //retrieve the index from mapped_index
                        draco::AttributeValueIndex temp = att->mapped_index((draco::PointIndex)i.value());

                        //retrieve data from draco mesh buffer with the index and convert the buffer data to "value"
                        if (!att->ConvertValue<float, 2>(temp, &value[0]))
                            return false;
                        char* pFloat = reinterpret_cast<char*>(&value);
                        data.insert(data.end(), pFloat, pFloat + 8);
                    }
                } else if (x.first == "NORMAL") {
                    dimension = 3;
                    formatsz = 4;
                    std::array<float, 3> value;

                    for (draco::AttributeValueIndex i(0); i < (int)att->indices_map_size(); ++i) {
                        draco::AttributeValueIndex temp = att->mapped_index((draco::PointIndex)i.value());
                        if (!att->ConvertValue<float, 3>(temp, &value[0]))
                            return false;
                        char* pFloat = reinterpret_cast<char*>(&value);
                        data.insert(data.end(), pFloat, pFloat + 12);
                    }
                } else if (x.first == "TANGENT") {
                    dimension = 4;
                    formatsz = 4;
                    std::array<float, 4> value;
                    for (draco::AttributeValueIndex i(0); i < (int)att->indices_map_size(); ++i) {
                        draco::AttributeValueIndex temp = att->mapped_index((draco::PointIndex)i.value());
                        if (!att->ConvertValue<float, 4>(temp, &value[0]))
                            return false;
                        char* pFloat = reinterpret_cast<char*>(&value);
                        data.insert(data.end(), pFloat, pFloat + 16);
                    }
                } else if (x.first == "POSITION") {
                    dimension = 3;
                    formatsz = 4;
                    std::array<float, 3> value;
                    for (draco::AttributeValueIndex i(0); i < (int)att->indices_map_size(); ++i) {
                        draco::AttributeValueIndex temp = att->mapped_index((draco::PointIndex)i.value());
                        if (!att->ConvertValue<float, 3>(temp, &value[0]))
                            return false;
                        char* pFloat = reinterpret_cast<char*>(&value);
                        data.insert(data.end(), pFloat, pFloat + 12);
                    }
                }

                byteLength = (int)att->indices_map_size() * dimension * formatsz;

                bufferView["buffer"] = 0;
                bufferView["byteLength"] = byteLength;
                bufferView["byteOffset"] = byteOffset;
                bufferView["target"] = 34962;
                byteOffset += byteLength;
                bufferViews.push_back(bufferView);
            }

            //index bufferviews and index buffer
            json bufferView;
            bufferView["buffer"] = 0;
            //            int vertid           = model->dracomeshes[0].draco_attributes["POSITION"];
            byteLength = draco_de_meshes[i]->num_faces() * 3 * 4;
            std::vector<char> tempc;
            for (draco::FaceIndex k(0); k < draco_de_meshes[i]->num_faces(); ++k) {
                for (int j = 0; j < 3; ++j) {
                    uint32_t temp = draco_de_meshes[i]->face(k)[j].value();
                    tempc.clear();
                    tempc.resize(4);
                    memcpy(&tempc[0], &temp, 4);
                    data.insert(data.end(), tempc.begin(), tempc.end());
                }
            }

            bufferView["byteOffset"] = byteOffset;
            bufferView["byteLength"] = byteLength;
            bufferView["target"] = 34963;
            byteOffset += (byteLength);
            bufferViews.push_back(bufferView);

        }

        output["bufferViews"] = bufferViews;

        //BUFFERS (only works for one buffer)
        json buffers;

        json buffer;

        std::ofstream outputbin(binSaveFilePath.c_str(), std::ofstream::binary);
        outputbin.write(reinterpret_cast<const char*>(&data[0]), std::streamsize(data.size()));
        outputbin.close();

        buffer["uri"]        = binFilename;
        buffer["byteLength"] = byteOffset;
        buffers.push_back(buffer);

        output["buffers"] = buffers;
    } else if (use_draco_encode) {
        json                 buffers;
        json bufferViews;
        //only work for one buffer only
        for (unsigned int i = 0; i < model->buffers.size(); i++) {
            json buffer;
            if (embedBuffers) {
                // embedded draco encode not supported yet
                *err = "Error: Draco Encode failed. Embedded draco is not supported yet.";
                return false;
            } else {
                model->buffers[i].data.clear();
                int size = 0;
                int offset = 0;
                draco::EncoderBuffer dracoBuffer;
                draco::Encoder encode;

                // Set up the encoder.
                for (int j =0; j< draco_en_meshes.size(); j++) {
                    json bufferView;

                    encode.SetSpeedOptions(option.iCmpLevel, option.iCmpLevel);
                    encode.SetAttributeQuantization(draco::GeometryAttribute::POSITION, option.iPosBits);
                    encode.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, option.iTexCBits);
                    encode.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, option.iNormalBits);
                    encode.SetAttributeQuantization(draco::GeometryAttribute::COLOR, option.iGenericBits);
                    encode.SetAttributeQuantization(draco::GeometryAttribute::GENERIC, option.iGenericBits);

                    draco::Status status = encode.EncodeMeshToBuffer(*(draco_en_meshes[j]), &dracoBuffer);
                    if (status.code() != draco::Status::OK) {
                        *err = "Error: Draco Encode failed.";
                        return false;
                    }
                    offset = size;
                    size += (int)dracoBuffer.size();
                    model->buffers[i].data.insert(model->buffers[i].data.end(), dracoBuffer.data(), dracoBuffer.data() + dracoBuffer.size());
                    bufferView["byteOffset"] = offset;
                    bufferView["buffer"] = 0;
                    bufferView["byteLength"] = dracoBuffer.size();
                    bufferViews.push_back(bufferView);

                    dracoBuffer.Clear();
                    encode.Reset();
                }
                SerializeGltfBuffer(model->buffers[i], buffer, binSaveFilePath, binFilename);
            }
            buffers.push_back(buffer);
        }
        output["buffers"] = buffers;

        output["bufferViews"] = bufferViews;
    }

    else
#endif
    {
        json buffers;
        for (unsigned int i = 0; i < model->buffers.size(); ++i) {
            json buffer;
            if (embedBuffers) {
                SerializeGltfBuffer(model->buffers[i], buffer);
            } else {
                SerializeGltfBuffer(model->buffers[i], buffer, binSaveFilePath, binFilename);
            }
            buffers.push_back(buffer);
        }
        output["buffers"] = buffers;

        // BUFFERVIEWS
        json bufferViews;
        for (unsigned int i = 0; i < model->bufferViews.size(); ++i) {
            json bufferView;
            SerializeGltfBufferView(model->bufferViews[i], bufferView);
            bufferViews.push_back(bufferView);
        }
        output["bufferViews"] = bufferViews;
    }

    if (!decodedDraco) {
        // Extensions used
        if (model->extensionsUsed.size()) {
            SerializeStringArrayProperty("extensionsUsed", model->extensionsUsed, output);
        }

        // Extensions required
        if (model->extensionsRequired.size()) {
            SerializeStringArrayProperty("extensionsRequired", model->extensionsRequired, output);
        }
    }

    if (use_draco_encode) {
        json ext_used;
        ext_used.push_back("KHR_draco_mesh_compression");
        output["extensionsUsed"]     = ext_used;
        output["extensionsRequired"] = ext_used;
    }

    // IMAGES
    if (model->images.size()) {
        json images;
        for (unsigned int i = 0; i < model->images.size(); ++i) {
            json image;

            UpdateImageObject(model->images[i], baseDir, int(i), embedImages, &this->WriteImageData, &this->write_image_user_data_);
            SerializeGltfImage(model->images[i], image);
            images.push_back(image);
        }
        output["images"] = images;
    }

    // MATERIALS

    if (model->materials.size()) {
        json materials;
        for (unsigned int i = 0; i < model->materials.size(); ++i) {
            json material;
            SerializeGltfMaterial(model->materials[i], material);
            materials.push_back(material);
        }
        output["materials"] = materials;
    }

    // MESHES
    if (model->meshes.size()) {
#ifdef USE_MESH_DRACO_EXTENSION
        if (use_draco_encode) {
            int mesh_num = 0;
            for (int m = 0; m < model->meshes.size(); m++)
                for (int n = 0; n < model->meshes[m].primitives.size(); n++)
                    mesh_num++;

            if (mesh_num != draco_en_meshes.size()) {
                *err = "Draco encode failed: \"meshes\" number and draco mesh number not match.";
                return false;
            }
        }
#endif
        json meshes;
        int mesh_index = 0;
        for (unsigned int i = 0; i < model->meshes.size(); ++i) {
            json mesh;
            if (SerializeGltfMesh(model->meshes[i], mesh, &mesh_index, use_draco_encode))
                meshes.push_back(mesh);
            else
                return false;
        }
        output["meshes"] = meshes;
    }

    // NODES
    if (model->nodes.size()) {
        json nodes;
        for (unsigned int i = 0; i < model->nodes.size(); ++i) {
            json node;
            SerializeGltfNode(model->nodes[i], node);
            nodes.push_back(node);
        }
        output["nodes"] = nodes;
    }

    // SCENE
    if (model->defaultScene > -1) {
        SerializeNumberProperty<int>("scene", model->defaultScene, output);
    }

    // SCENES
    if (model->scenes.size()) {
        json scenes;
        for (unsigned int i = 0; i < model->scenes.size(); ++i) {
            json currentScene;
            SerializeGltfScene(model->scenes[i], currentScene);
            scenes.push_back(currentScene);
        }
        output["scenes"] = scenes;
    }

    // SKINS
    if (model->skins.size()) {
        json skins;
        for (unsigned int i = 0; i < model->skins.size(); ++i) {
            json skin;
            SerializeGltfSkin(model->skins[i], skin);
            skins.push_back(skin);
        }
        output["skins"] = skins;
    }

    // TEXTURES
    if (model->textures.size()) {
        unsigned int dds_offset = model->images.size();
        if(option.DestFormat != CMP_FORMAT_Unknown) {
            output["extensionsUsed"].push_back("MSFT_texture_dds");
        }
        json textures;
        for (unsigned int i = 0; i < model->textures.size(); ++i) {
            Texture texture = model->textures[i];

            if(option.DestFormat != CMP_FORMAT_Unknown) {
                Image image;

                std::string short_output_path = model->images[i].uri;
                short_output_path.replace(short_output_path.rfind('.'), 1, "_");
                short_output_path += ".dds";
                image.uri = short_output_path;

                json imageJson;
                SerializeGltfImage(image, imageJson);
                output["images"].push_back(imageJson);

                std::map<std::string, Value> map;
                map.emplace("source", Value(int(dds_offset+texture.source)));

                texture.extensions["MSFT_texture_dds"] = Value(map);
            }

            json textureJson;
            SerializeGltfTexture(texture, textureJson);
            std::string contents = textureJson.dump();
            textures.push_back(textureJson);
        }
        output["textures"] = textures;
    }

    // SAMPLERS
    if (model->samplers.size()) {
        json samplers;
        for (unsigned int i = 0; i < model->samplers.size(); ++i) {
            json sampler;
            SerializeGltfSampler(model->samplers[i], sampler);
            samplers.push_back(sampler);
        }
        output["samplers"] = samplers;
    }

    // CAMERAS
    if (model->cameras.size()) {
        json cameras;
        for (unsigned int i = 0; i < model->cameras.size(); ++i) {
            json camera;
            SerializeGltfCamera(model->cameras[i], camera);
            cameras.push_back(camera);
        }
        output["cameras"] = cameras;
    }

    // EXTENSIONS
    SerializeExtensionMap(model->extensions, output);

    // LIGHTS as KHR_lights_cmn
    if (model->lights.size()) {
        json lights;
        for (unsigned int i = 0; i < model->lights.size(); ++i) {
            json light;
            SerializeGltfLight(model->lights[i], light);
            lights.push_back(light);
        }
        json khr_lights_cmn;
        khr_lights_cmn["lights"] = lights;
        json ext_j;

        if (output.find("extensions") != output.end()) {
            ext_j = output["extensions"];
        }

        ext_j["KHR_lights_cmn"] = khr_lights_cmn;

        output["extensions"] = ext_j;
    }

    WriteGltfFile(filename, output.dump());
    return true;
}

}  // namespace tinygltf2


bool hasGLTFTextures(std::string filename) {
    nlohmann::json j3;
    std::ifstream  f(filename);
    if (!f) {
        return false;
    }

    f >> j3;

    auto textures = j3["textures"];

    return textures.size() > 0;
}



bool isGLTFDracoFile(std::string filename) {
    nlohmann::json j3;
    std::ifstream  f(filename);
    if (!f) {
        return false;
    }

    f >> j3;

    auto extrequired = j3["extensionsRequired"];

    for (unsigned int i = 0; i < extrequired.size(); i++) {
        std::string extname = extrequired[i].get<std::string>();
        if (extname.find("KHR_draco_mesh_compression") != std::string::npos) {
            return true;
        }
    }

    auto extused = j3["extensionsUsed"];

    for (unsigned int j = 0; j < extused.size(); j++) {
        std::string extnameused = extused[j].get<std::string>();
        if (extnameused.find("KHR_draco_mesh_compression") != std::string::npos) {
            return true;
        }
    }

    return false;
}


bool decompressglTFfile(std::string srcFile, std::string tempdstFile, bool useDracoEncode, CMP_CompressOptions& option) {
    std::string         err;
    tinygltf2::Model    model;
    tinygltf2::TinyGLTF loader;
    tinygltf2::TinyGLTF saver;
    bool                ret = loader.LoadASCIIFromFile(&model, &err, srcFile, useDracoEncode);
    if (!err.empty()) {
        printf("Error processing gltf source:[%s] file [%s]\n", srcFile.c_str(), err.c_str());
        return false;
    }
    if (!ret) {
        printf("Failed in decoding glTF file : [%s].\n", srcFile.c_str());
        return false;
    }

    bool is_draco_src = false;
#ifdef USE_MESH_DRACO_EXTENSION
    if (model.dracomeshes.size() > 0) {
        is_draco_src = true;
    }
#endif
    err.clear();

    ret = saver.WriteGltfSceneToFile(&model, &err, tempdstFile, option, is_draco_src, useDracoEncode);

    if (!err.empty()) {
        printf("Error processing gltf destination:[%s] file [%s]\n", tempdstFile.c_str(), err.c_str());
        return false;
    }
    if (!ret) {
        printf("Failed to save temporary decoded glTF file %s\n", tempdstFile.c_str());
        return false;
    }

    return true;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
