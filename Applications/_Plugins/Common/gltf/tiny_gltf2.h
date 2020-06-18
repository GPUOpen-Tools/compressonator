// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
//
// Major Code based on Header-only tiny glTF 2.0 loader and serializer.
// The MIT License (MIT)
//
// Copyright (c) 2015 - 2018 Syoyo Fujita, Aurélien Chatelain and many
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

#ifndef TINY_GLTF_H_
#define TINY_GLTF_H_

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE

#ifndef TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#ifndef TINYGLTF_NO_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "Compressonator.h"
#include "Common.h"

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

namespace tinygltf2
{
#define TINYGLTF_MODE_POINTS (0)
#define TINYGLTF_MODE_LINE (1)
#define TINYGLTF_MODE_LINE_LOOP (2)
#define TINYGLTF_MODE_TRIANGLES (4)
#define TINYGLTF_MODE_TRIANGLE_STRIP (5)
#define TINYGLTF_MODE_TRIANGLE_FAN (6)

#define TINYGLTF_COMPONENT_TYPE_BYTE (5120)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE (5121)
#define TINYGLTF_COMPONENT_TYPE_SHORT (5122)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT (5123)
#define TINYGLTF_COMPONENT_TYPE_INT (5124)
#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT (5125)
#define TINYGLTF_COMPONENT_TYPE_FLOAT (5126)
#define TINYGLTF_COMPONENT_TYPE_DOUBLE (5130)

#define TINYGLTF_TEXTURE_FILTER_NEAREST (9728)
#define TINYGLTF_TEXTURE_FILTER_LINEAR (9729)
#define TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST (9984)
#define TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST (9985)
#define TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR (9986)
#define TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR (9987)

#define TINYGLTF_TEXTURE_WRAP_REPEAT (10497)
#define TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE (33071)
#define TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT (33648)

    // Redeclarations of the above for technique.parameters.
#define TINYGLTF_PARAMETER_TYPE_BYTE (5120)
#define TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE (5121)
#define TINYGLTF_PARAMETER_TYPE_SHORT (5122)
#define TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT (5123)
#define TINYGLTF_PARAMETER_TYPE_INT (5124)
#define TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT (5125)
#define TINYGLTF_PARAMETER_TYPE_FLOAT (5126)

#define TINYGLTF_PARAMETER_TYPE_FLOAT_VEC2 (35664)
#define TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 (35665)
#define TINYGLTF_PARAMETER_TYPE_FLOAT_VEC4 (35666)

#define TINYGLTF_PARAMETER_TYPE_INT_VEC2 (35667)
#define TINYGLTF_PARAMETER_TYPE_INT_VEC3 (35668)
#define TINYGLTF_PARAMETER_TYPE_INT_VEC4 (35669)

#define TINYGLTF_PARAMETER_TYPE_BOOL (35670)
#define TINYGLTF_PARAMETER_TYPE_BOOL_VEC2 (35671)
#define TINYGLTF_PARAMETER_TYPE_BOOL_VEC3 (35672)
#define TINYGLTF_PARAMETER_TYPE_BOOL_VEC4 (35673)

#define TINYGLTF_PARAMETER_TYPE_FLOAT_MAT2 (35674)
#define TINYGLTF_PARAMETER_TYPE_FLOAT_MAT3 (35675)
#define TINYGLTF_PARAMETER_TYPE_FLOAT_MAT4 (35676)

#define TINYGLTF_PARAMETER_TYPE_SAMPLER_2D (35678)

    // End parameter types

#define TINYGLTF_TYPE_VEC2 (2)
#define TINYGLTF_TYPE_VEC3 (3)
#define TINYGLTF_TYPE_VEC4 (4)
#define TINYGLTF_TYPE_MAT2 (32 + 2)
#define TINYGLTF_TYPE_MAT3 (32 + 3)
#define TINYGLTF_TYPE_MAT4 (32 + 4)
#define TINYGLTF_TYPE_SCALAR (64 + 1)
#define TINYGLTF_TYPE_VECTOR (64 + 4)
#define TINYGLTF_TYPE_MATRIX (64 + 16)

#define TINYGLTF_IMAGE_FORMAT_JPEG (0)
#define TINYGLTF_IMAGE_FORMAT_PNG (1)
#define TINYGLTF_IMAGE_FORMAT_BMP (2)
#define TINYGLTF_IMAGE_FORMAT_GIF (3)

#define TINYGLTF_TEXTURE_FORMAT_ALPHA (6406)
#define TINYGLTF_TEXTURE_FORMAT_RGB (6407)
#define TINYGLTF_TEXTURE_FORMAT_RGBA (6408)
#define TINYGLTF_TEXTURE_FORMAT_LUMINANCE (6409)
#define TINYGLTF_TEXTURE_FORMAT_LUMINANCE_ALPHA (6410)

#define TINYGLTF_TEXTURE_TARGET_TEXTURE2D (3553)
#define TINYGLTF_TEXTURE_TYPE_UNSIGNED_BYTE (5121)

#define TINYGLTF_TARGET_ARRAY_BUFFER (34962)
#define TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER (34963)

#define TINYGLTF_SHADER_TYPE_VERTEX_SHADER (35633)
#define TINYGLTF_SHADER_TYPE_FRAGMENT_SHADER (35632)

    typedef enum
    {
        NULL_TYPE   = 0,
        NUMBER_TYPE = 1,
        INT_TYPE    = 2,
        BOOL_TYPE   = 3,
        STRING_TYPE = 4,
        ARRAY_TYPE  = 5,
        BINARY_TYPE = 6,
        OBJECT_TYPE = 7
    } Type;

    static inline int32_t GetComponentSizeInBytes(uint32_t componentType)
    {
        if (componentType == TINYGLTF_COMPONENT_TYPE_BYTE)
        {
            return 1;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
        {
            return 1;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_SHORT)
        {
            return 2;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            return 2;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_INT)
        {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
        {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
        {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
        {
            return 8;
        }
        else
        {
            // Unknown componenty type
            return -1;
        }
    }

    static inline int32_t GetTypeSizeInBytes(uint32_t ty)
    {
        if (ty == TINYGLTF_TYPE_SCALAR)
        {
            return 1;
        }
        else if (ty == TINYGLTF_TYPE_VEC2)
        {
            return 2;
        }
        else if (ty == TINYGLTF_TYPE_VEC3)
        {
            return 3;
        }
        else if (ty == TINYGLTF_TYPE_VEC4)
        {
            return 4;
        }
        else if (ty == TINYGLTF_TYPE_MAT2)
        {
            return 4;
        }
        else if (ty == TINYGLTF_TYPE_MAT3)
        {
            return 9;
        }
        else if (ty == TINYGLTF_TYPE_MAT4)
        {
            return 16;
        }
        else
        {
            // Unknown componenty type
            return -1;
        }
    }

#ifdef __clang__
#pragma clang diagnostic push
    // Suppress warning for : static Value null_value
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wpadded"
#endif

    // Simple class to represent JSON object
    class Value
    {
       public:
        typedef std::vector<Value>           Array;
        typedef std::map<std::string, Value> Object;

        Value() : type_(NULL_TYPE)
        {
        }

        explicit Value(bool b) : type_(BOOL_TYPE)
        {
            boolean_value_ = b;
        }
        explicit Value(int i) : type_(INT_TYPE)
        {
            int_value_ = i;
        }
        explicit Value(double n) : type_(NUMBER_TYPE)
        {
            number_value_ = n;
        }
        explicit Value(const std::string& s) : type_(STRING_TYPE)
        {
            string_value_ = s;
        }
        explicit Value(const unsigned char* p, size_t n) : type_(BINARY_TYPE)
        {
            binary_value_.resize(n);
            memcpy(binary_value_.data(), p, n);
        }
        explicit Value(const Array& a) : type_(ARRAY_TYPE)
        {
            array_value_ = Array(a);
        }
        explicit Value(const Object& o) : type_(OBJECT_TYPE)
        {
            object_value_ = Object(o);
        }

        char Type() const
        {
            return static_cast<const char>(type_);
        }

        bool IsBool() const
        {
            return (type_ == BOOL_TYPE);
        }

        bool IsInt() const
        {
            return (type_ == INT_TYPE);
        }

        bool IsNumber() const
        {
            return (type_ == NUMBER_TYPE);
        }

        bool IsString() const
        {
            return (type_ == STRING_TYPE);
        }

        bool IsBinary() const
        {
            return (type_ == BINARY_TYPE);
        }

        bool IsArray() const
        {
            return (type_ == ARRAY_TYPE);
        }

        bool IsObject() const
        {
            return (type_ == OBJECT_TYPE);
        }

        // Accessor
        template <typename T>
        const T& Get() const;
        template <typename T>
        T& Get();

        // Lookup value from an array
        const Value& Get(int idx) const
        {
            static Value null_value;
            assert(IsArray());
            assert(idx >= 0);
            return (static_cast<size_t>(idx) < array_value_.size()) ? array_value_[static_cast<size_t>(idx)] : null_value;
        }

        // Lookup value from a key-value pair
        const Value& Get(const std::string& key) const
        {
            static Value null_value;
            assert(IsObject());
            Object::const_iterator it = object_value_.find(key);
            return (it != object_value_.end()) ? it->second : null_value;
        }

        size_t ArrayLen() const
        {
            if (!IsArray())
                return 0;
            return array_value_.size();
        }

        // Valid only for object type.
        bool Has(const std::string& key) const
        {
            if (!IsObject())
                return false;
            Object::const_iterator it = object_value_.find(key);
            return (it != object_value_.end()) ? true : false;
        }

        // List keys
        std::vector<std::string> Keys() const
        {
            std::vector<std::string> keys;
            if (!IsObject())
                return keys;  // empty

            for (Object::const_iterator it = object_value_.begin(); it != object_value_.end(); ++it)
            {
                keys.push_back(it->first);
            }

            return keys;
        }

        size_t Size() const
        {
            return (IsArray() ? ArrayLen() : Keys().size());
        }

       protected:
        int type_;

        int                        int_value_;
        double                     number_value_;
        std::string                string_value_;
        std::vector<unsigned char> binary_value_;
        Array                      array_value_;
        Object                     object_value_;
        bool                       boolean_value_;
    };

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define TINYGLTF_VALUE_GET(ctype, var)            \
    template <>                                   \
    inline const ctype& Value::Get<ctype>() const \
    {                                             \
        return var;                               \
    }                                             \
    template <>                                   \
    inline ctype& Value::Get<ctype>()             \
    {                                             \
        return var;                               \
    }
    TINYGLTF_VALUE_GET(bool, boolean_value_)
    TINYGLTF_VALUE_GET(double, number_value_)
    TINYGLTF_VALUE_GET(int, int_value_)
    TINYGLTF_VALUE_GET(std::string, string_value_)
    TINYGLTF_VALUE_GET(std::vector<unsigned char>, binary_value_)
    TINYGLTF_VALUE_GET(Value::Array, array_value_)
    TINYGLTF_VALUE_GET(Value::Object, object_value_)
#undef TINYGLTF_VALUE_GET

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wpadded"
#endif

    /// Agregate object for representing a color
    using ColorValue = std::array<double, 4>;

    struct Parameter
    {
        bool                          bool_value;
        std::string                   string_value;
        std::vector<double>           number_array;
        std::map<std::string, double> json_double_value;

        // context sensitive methods. depending the type of the Parameter you are
        // accessing, these are either valid or not
        // If this parameter represent a texture map in a material, will return the
        // texture index

        /// Return the index of a texture if this Parameter is a texture map.
        /// Returned value is only valid if the parameter represent a texture from a
        /// material
        int TextureIndex() const
        {
            const auto it = json_double_value.find("index");
            if (it != std::end(json_double_value))
            {
                return int(it->second);
            }
            return -1;
        }

        /// Material factor, like the roughness or metalness of a material
        /// Returned value is only valid if the parameter represent a texture from a
        /// material
        double Factor() const
        {
            return number_array[0];
        }

        /// Return the color of a material
        /// Returned value is only valid if the parameter represent a texture from a
        /// material
        ColorValue ColorFactor() const
        {
            return {{// this agregate intialize the std::array object, and uses C++11 RVO.
                     number_array[0], number_array[1], number_array[2], (number_array.size() > 3 ? number_array[3] : 1.0)}};
        }
    };

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

    typedef std::map<std::string, Parameter> ParameterMap;
    typedef std::map<std::string, Value>     ExtensionMap;

    struct AnimationChannel
    {
        int         sampler;      // required
        int         target_node;  // required (index of the node to target)
        std::string target_path;  // required in ["translation", "rotation", "scale",
                                  // "weights"]
        Value extras;

        AnimationChannel() : sampler(-1), target_node(-1)
        {
        }
    };

    struct AnimationSampler
    {
        int         input;          // required
        int         output;         // required
        std::string interpolation;  // in ["LINEAR", "STEP", "CATMULLROMSPLINE",
                                    // "CUBICSPLINE"], default "LINEAR"

        AnimationSampler() : input(-1), output(-1), interpolation("LINEAR")
        {
        }
    };

    struct Animation
    {
        std::string                   name;
        std::vector<AnimationChannel> channels;
        std::vector<AnimationSampler> samplers;
        Value                         extras;
    };

    struct Skin
    {
        std::string      name;
        int              inverseBindMatrices;  // required here but not in the spec
        int              skeleton;             // The index of the node used as a skeleton root
        std::vector<int> joints;               // Indices of skeleton nodes

        Skin()
        {
            inverseBindMatrices = -1;
            skeleton            = -1;
        }
    };

    struct Sampler
    {
        std::string name;
        int         minFilter;  // ["NEAREST", "LINEAR", "NEAREST_MIPMAP_LINEAR",
                                // "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR",
                                // "LINEAR_MIPMAP_LINEAR"]
        int magFilter;          // ["NEAREST", "LINEAR"]
        int wrapS;              // ["CLAMP_TO_EDGE", "MIRRORED_REPEAT", "REPEAT"], default
                                // "REPEAT"
        int wrapT;              // ["CLAMP_TO_EDGE", "MIRRORED_REPEAT", "REPEAT"], default
                                // "REPEAT"
        int   wrapR;            // TinyGLTF extension
        Value extras;

        Sampler() : wrapS(TINYGLTF_TEXTURE_WRAP_REPEAT), wrapT(TINYGLTF_TEXTURE_WRAP_REPEAT)
        {
        }
    };

    struct Image
    {
        std::string                name;
        int                        width;
        int                        height;
        int                        component;
        std::vector<unsigned char> image;
        int                        bufferView;  // (required if no uri)
        std::string                mimeType;    // (required if no uri) ["image/jpeg", "image/png",
                                                // "image/bmp", "image/gif"]
        std::string uri;                        // (required if no mimeType)
        Value       extras;

        Image()
        {
            bufferView = -1;
        }
    };

    struct KHRExtension
    {
        std::string name;
        Value       extras;
    };

    struct Texture
    {
        int          sampler;
        int          source;  // Required (not specified in the spec ?)
        Value        extras;
        ExtensionMap extensions;

        Texture() : sampler(-1), source(-1)
        {
        }
    };

    // Each extension should be stored in a ParameterMap.
    // members not in the values could be included in the ParameterMap
    // to keep a single material model
    struct Material
    {
        std::string name;

        ParameterMap values;            // PBR metal/roughness workflow
        ParameterMap additionalValues;  // normal/occlusion/emissive values

        ExtensionMap extensions;
        Value        extras;
    };

    struct BufferView
    {
        std::string name;
        int         buffer;      // Required
        size_t      byteOffset;  // minimum 0, default 0
        size_t      byteLength;  // required, minimum 1
        size_t      byteStride;  // minimum 4, maximum 252 (multiple of 4), default 0 =
                                 // understood to be tightly packed
        int   target;            // ["ARRAY_BUFFER", "ELEMENT_ARRAY_BUFFER"]
        Value extras;

        BufferView() : byteOffset(0), byteStride(0)
        {
        }
    };

    struct Accessor
    {
        int bufferView;  // optional in spec but required here since sparse accessor
                         // are not supported
        std::string name;
        size_t      byteOffset;
        bool        normalized;     // optinal.
        int         componentType;  // (required) One of TINYGLTF_COMPONENT_TYPE_***
        size_t      count;          // required
        int         type;           // (required) One of TINYGLTF_TYPE_***   ..
        Value       extras;

        std::vector<double> minValues;  // optional
        std::vector<double> maxValues;  // optional

        // TODO(syoyo): "sparse"

        ///
        /// Utility function to compute byteStride for a given bufferView object.
        /// Returns -1 upon invalid glTF value or parameter configuration.
        ///
        int ByteStride(const BufferView& bufferViewObject) const
        {
            if (bufferViewObject.byteStride == 0)
            {
                // Assume data is tightly packed.
                int componentSizeInBytes = GetComponentSizeInBytes(static_cast<uint32_t>(componentType));
                if (componentSizeInBytes <= 0)
                {
                    return -1;
                }

                int typeSizeInBytes = GetTypeSizeInBytes(static_cast<uint32_t>(type));
                if (typeSizeInBytes <= 0)
                {
                    return -1;
                }

                return componentSizeInBytes * typeSizeInBytes;
            }
            else
            {
                // Check if byteStride is a mulple of the size of the accessor's component
                // type.
                int componentSizeInBytes = GetComponentSizeInBytes(static_cast<uint32_t>(componentType));
                if (componentSizeInBytes <= 0)
                {
                    return -1;
                }

                if ((bufferViewObject.byteStride % uint32_t(componentSizeInBytes)) != 0)
                {
                    return -1;
                }
                return static_cast<int>(bufferViewObject.byteStride);
            }

            return 0;
        }

        Accessor()
        {
            bufferView = -1;
        }
    };

    struct PerspectiveCamera
    {
        float aspectRatio;  // min > 0
        float yfov;         // required. min > 0
        float zfar;         // min > 0
        float znear;        // required. min > 0

        PerspectiveCamera() :
            aspectRatio(0.0f),
            yfov(0.0f),
            zfar(0.0f)  // 0 = use infinite projecton matrix
            ,
            znear(0.0f)
        {
        }

        ExtensionMap extensions;
        Value        extras;
    };

    struct OrthographicCamera
    {
        float xmag;   // required. must not be zero.
        float ymag;   // required. must not be zero.
        float zfar;   // required. `zfar` must be greater than `znear`.
        float znear;  // required

        OrthographicCamera() : xmag(0.0f), ymag(0.0f), zfar(0.0f), znear(0.0f)
        {
        }

        ExtensionMap extensions;
        Value        extras;
    };

    struct Camera
    {
        std::string type;  // required. "perspective" or "orthographic"
        std::string name;

        PerspectiveCamera  perspective;
        OrthographicCamera orthographic;

        Camera()
        {
        }

        ExtensionMap extensions;
        Value        extras;
    };

    struct Primitive
    {
        std::map<std::string, int> attributes;             // (required) A dictionary object of
                                                           // integer, where each integer
                                                           // is the index of the accessor
                                                           // containing an attribute.
        int material;                                      // The index of the material to apply to this primitive
                                                           // when rendering.
        int                                      indices;  // The index of the accessor that contains the indices.
        int                                      mode;     // one of TINYGLTF_MODE_***
        std::vector<std::map<std::string, int> > targets;  // array of morph targets,
                                                           // where each target is a dict with attribues in ["POSITION, "NORMAL",
                                                           // "TANGENT"] pointing
                                                           // to their corresponding accessors
        Value extras;

        Primitive()
        {
            material = -1;
            indices  = -1;
        }
    };

    struct Mesh
    {
        std::string                              name;
        std::vector<Primitive>                   primitives;
        std::vector<double>                      weights;  // weights to be applied to the Morph Targets
        std::vector<std::map<std::string, int> > targets;
        ExtensionMap                             extensions;
        Value                                    extras;
    };

    class Node
    {
       public:
        Node() : camera(-1), skin(-1), mesh(-1)
        {
        }

        Node(const Node& rhs)
        {
            camera = rhs.camera;

            name        = rhs.name;
            skin        = rhs.skin;
            mesh        = rhs.mesh;
            children    = rhs.children;
            rotation    = rhs.rotation;
            scale       = rhs.scale;
            translation = rhs.translation;
            matrix      = rhs.matrix;
            weights     = rhs.weights;

            extensions = rhs.extensions;
            extras     = rhs.extras;
        }

        ~Node()
        {
        }

        int camera;  // the index of the camera referenced by this node

        std::string         name;
        int                 skin;
        int                 mesh;
        std::vector<int>    children;
        std::vector<double> rotation;     // length must be 0 or 4
        std::vector<double> scale;        // length must be 0 or 3
        std::vector<double> translation;  // length must be 0 or 3
        std::vector<double> matrix;       // length must be 0 or 16
        std::vector<double> weights;      // The weights of the instantiated Morph Target

        ExtensionMap extensions;
        Value        extras;
    };

    struct Buffer
    {
        std::string                name;
        std::vector<unsigned char> data;
        std::string                uri;  // considered as required here but not in the spec (need to clarify)
        Value                      extras;
    };

#ifdef USE_MESH_DRACO_EXTENSION
    struct DracoData
    {
        std::vector<unsigned char> data;  //compressed data
        std::string                uri;
        double                     byteLength        = -1;
        double                     draco_buffer_view = -1;
        double                     buffer_id         = -1;
        std::map<std::string, int> draco_attributes;
		int mesh_index = 0;
		int prim_index = 0;
    };
#endif

    struct Asset
    {
        std::string  version;  // required
        std::string  generator;
        std::string  minVersion;
        std::string  copyright;
        ExtensionMap extensions;
        Value        extras;
    };

    struct Scene
    {
        std::string      name;
        std::vector<int> nodes;

        ExtensionMap extensions;
        Value        extras;
    };

    struct Light
    {
        std::string         name;
        std::vector<double> color;
        std::string         type;
    };

    class Model
    {
       public:
        Model()
        {
        }
        ~Model()
        {
        }

        std::vector<Accessor>   accessors;
        std::vector<Animation>  animations;
        std::vector<Buffer>     buffers;
        std::vector<BufferView> bufferViews;
        std::vector<Material>   materials;
        std::vector<Mesh>       meshes;
        std::vector<Node>       nodes;
        std::vector<Texture>    textures;
        std::vector<Image>      images;
        std::vector<Skin>       skins;
        std::vector<Sampler>    samplers;
        std::vector<Camera>     cameras;
        std::vector<Scene>      scenes;
        std::vector<Light>      lights;

#ifdef USE_MESH_DRACO_EXTENSION
        std::vector<DracoData> dracomeshes;
#endif
        ExtensionMap extensions;

        int                      defaultScene;
        std::vector<std::string> extensionsUsed;
        std::vector<std::string> extensionsRequired;

        Asset asset;

        Value extras;
    };

    enum SectionCheck
    {
        NO_REQUIRE           = 0x00,
        REQUIRE_SCENE        = 0x01,
        REQUIRE_SCENES       = 0x02,
        REQUIRE_NODES        = 0x04,
        REQUIRE_ACCESSORS    = 0x08,
        REQUIRE_BUFFERS      = 0x10,
        REQUIRE_BUFFER_VIEWS = 0x20,
        REQUIRE_ALL          = 0x3f
    };

    ///
    /// LoadImageDataFunction type. Signature for custom image loading callbacks.
    ///
    typedef bool (*LoadImageDataFunction)(Image*, std::string*, int, int, const unsigned char*, int, void*);

    ///
    /// WriteImageDataFunction type. Signature for custom image writing callbacks.
    ///
    typedef bool (*WriteImageDataFunction)(const std::string*, const std::string*, Image*, bool, void*);

#ifndef TINYGLTF_NO_STB_IMAGE
    // Declaration of default image loader callback
    bool LoadImageData(Image* image, std::string* err, int req_width, int req_height, const unsigned char* bytes, int size, void*);
#endif

#ifndef TINYGLTF_NO_STB_IMAGE_WRITE
    // Declaration of default image writer callback
    bool WriteImageData(const std::string* basepath, const std::string* filename, Image* image, bool embedImages, void*);
#endif

    class TinyGLTF
    {
       public:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

        TinyGLTF() : bin_data_(nullptr), bin_size_(0), is_binary_(false)
        {
        }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

        ~TinyGLTF()
        {
        }

        ///
        /// Loads glTF ASCII asset from a file.
        /// Returns false and set error string to `err` if there's an error.
        ///
        bool LoadASCIIFromFile(Model* model, std::string* err, const std::string& filename, bool use_draco_encode,
                               unsigned int check_sections = REQUIRE_ALL);

        ///
        /// Loads glTF ASCII asset from string(memory).
        /// `length` = strlen(str);
        /// Returns false and set error string to `err` if there's an error.
        ///
        bool LoadASCIIFromString(Model* model, std::string* err, const char* str, const unsigned int length, const std::string& base_dir,
                                 bool use_draco_encode, unsigned int check_sections = REQUIRE_ALL);

        ///
        /// Loads glTF binary asset from a file.
        /// Returns false and set error string to `err` if there's an error.
        ///
        bool LoadBinaryFromFile(Model* model, std::string* err, const std::string& filename, unsigned int check_sections = REQUIRE_ALL);

        ///
        /// Loads glTF binary asset from memory.
        /// `length` = strlen(str);
        /// Returns false and set error string to `err` if there's an error.
        ///
        bool LoadBinaryFromMemory(Model* model, std::string* err, const unsigned char* bytes, const unsigned int length,
                                  const std::string& base_dir = "", unsigned int check_sections = REQUIRE_ALL);

        ///
        /// Write glTF to file.
        ///
        bool WriteGltfSceneToFile(Model* model, std::string* err, const std::string& filename, CMP_CompressOptions& option, bool decodedDraco = false,
                                  bool use_draco_encode = false, bool embedImages = false, bool embedBuffers = false /*, bool writeBinary*/);

        ///
        /// Set callback to use for loading image data
        ///
        void SetImageLoader(LoadImageDataFunction LoadImageData, void* user_data);

        ///
        /// Set callback to use for writing image data
        ///
        void SetImageWriter(WriteImageDataFunction WriteImageData, void* user_data);

       private:
        ///
        /// Loads glTF asset from string(memory).
        /// `length` = strlen(str);
        /// Returns false and set error string to `err` if there's an error.
        ///
        bool LoadFromString(Model* model, std::string* err, const char* str, const unsigned int length, const std::string& base_dir,
                            bool use_draco_encode, unsigned int check_sections);

        const unsigned char* bin_data_;
        size_t               bin_size_;
        bool                 is_binary_;

        LoadImageDataFunction LoadImageData =
#ifndef TINYGLTF_NO_STB_IMAGE
            &tinygltf2::LoadImageData;
#else
            nullptr;
#endif
        void* load_image_user_data_ = nullptr;

        WriteImageDataFunction WriteImageData =
#ifndef TINYGLTF_NO_STB_IMAGE_WRITE
            &tinygltf2::WriteImageData;
#else
            nullptr;
#endif
        void* write_image_user_data_ = nullptr;
    };

#ifdef __clang__
#pragma clang diagnostic pop  // -Wpadded
#endif

}  // namespace tinygltf2

#endif  // TINY_GLTF_H_
