//=====================================================================
// Copyright 2006-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
#ifndef _JML_VEC2_H_
#define _JML_VEC2_H_

namespace JML
{
/// \brief A template class for two-component vectors.
/**
    The template argument must be a numeric type
*/
template <class T>
class Vec2
{
public:
    T x;
    T y;

    // *****************************************
    //     Constructors
    // *****************************************

    /// Default constructor
    Vec2()
        : x((T)0)
        , y((T)0){};

    /// Value constructor
    Vec2(const T& vx, const T& vy)
        : x(vx)
        , y(vy){};

    /// Copy constructor
    Vec2(const Vec2<T>& val)
        : x(val.x)
        , y(val.y){};

    /// Single value constructor.  Sets all components to the given value
    Vec2(const T& v)
        : x(v)
        , y(v){};

    // *****************************************
    //     Conversions/Assignment/Indexing
    // *****************************************

    /// cast to T*
    operator const T*() const
    {
        return (const T*)this;
    };

    /// cast to T*
    operator T*()
    {
        return (T*)this;
    };

    /// Indexing
    const T& operator[](int i) const
    {
        return ((const T*)this)[i];
    };
    T& operator[](int i)
    {
        return ((T*)this)[i];
    };

    /// Assignment
    const Vec2<T>& operator=(const Vec2<T>& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    };

    // *****************************************
    //    Comparison
    // *****************************************

    /// Equality comparison
    bool operator==(const Vec2<T>& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    };

    /// Inequality comparision
    bool operator!=(const Vec2<T>& rhs) const
    {
        return (x != rhs.x || y != rhs.y);
    };

    // *****************************************
    //    Arithmetic
    // *****************************************

    /// Addition
    const Vec2<T> operator+(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x + rhs.x, y + rhs.y);
    };

    /// Subtraction
    const Vec2<T> operator-(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x - rhs.x, y - rhs.y);
    };

    /// Multiply by scalar
    const Vec2<T> operator*(const T& v) const
    {
        return Vec2<T>(x * v, y * v);
    };

    /// Divide by scalar
    const Vec2<T> operator/(const T& v) const
    {
        return Vec2<T>(x / v, y / v);
    };

    /// Addition in-place
    Vec2<T>& operator+=(const Vec2<T>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    };

    /// Subtract in-place
    Vec2<T>& operator-=(const Vec2<T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    };

    /// Scalar multiply in-place
    Vec2<T>& operator*=(const T& v)
    {
        x *= v;
        y *= v;
        return *this;
    };

    /// Scalar divide in-place
    Vec2<T>& operator/=(const T& v)
    {
        x /= v;
        y /= v;
        return *this;
    };
};

/// stream output
template <class T>
std::ostream& operator<<(std::ostream& sout, const Vec2<T>& vec)
{
    sout << "<" << vec.x << "," << vec.y << ">";
    return sout;
};

typedef Vec2<float>  Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int>    Vec2i;

};  // namespace JML

#endif