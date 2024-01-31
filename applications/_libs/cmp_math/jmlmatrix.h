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
#ifndef _JML_MATRIX_H_
#define _JML_MATRIX_H_

namespace JML
{

/**
    Simple templatized matrix class.
    The matrix is stored in column major order
*/
template <class T, int SIZE>
class Matrix
{
public:
    Matrix()
    {
        // initialize to identity
        for (int i = 0; i < SIZE * SIZE; i++)
        {
            if (i % (SIZE + 1) == 0)
            {
                m_values[i] = (T)1;
            }
            else
            {
                m_values[i] = (T)0;
            }
        }
    };

    Matrix(const T values[(SIZE * SIZE)])
    {
        memcpy(m_values, values, (SIZE * SIZE) * sizeof(T));
    };

    Matrix(const Matrix<T, SIZE>& rhs)
    {
        *this = rhs;
    };

    const Matrix<T, SIZE>& operator=(const Matrix<T, SIZE>& rhs)
    {
        memcpy(m_values, rhs.m_values, (SIZE * SIZE) * sizeof(T));
        return *this;
    };

    /// Returns pointer to matrix data in COLUMN-major order
    const T* GetValues() const
    {
        return m_values;
    };
    T* GetValues()
    {
        return m_values;
    };

    operator const T*() const
    {
        return (const T*)this;
    };

    operator T*()
    {
        return (T*)this;
    };

    Matrix<T, SIZE>& operator*=(const Matrix<T, SIZE>& rhs)
    {
        T   newdata[(SIZE * SIZE)];
        int i, j, x;

        for (j = 0; j < SIZE; j++)
        {
            for (i = 0; i < SIZE; i++)
            {
                /** loop across ith row and down jth column **/
                newdata[(SIZE * i) + j] = 0.0;

                for (x = 0; x < SIZE; x++)
                {
                    newdata[(SIZE * i) + j] += m_values[(SIZE * i) + x] * rhs.m_values[(SIZE * x) + j];
                }
            }
        }

        memcpy(m_values, newdata, 16 * sizeof(T));
    };

    Matrix<T, SIZE> operator*(const Matrix<T, SIZE>& rhs) const
    {
        T   newdata[(SIZE * SIZE)];
        int i, j, x;

        for (j = 0; j < SIZE; j++)
        {
            for (i = 0; i < SIZE; i++)
            {
                /** loop across ith row and down jth column **/
                newdata[(SIZE * i) + j] = 0.0;

                for (x = 0; x < SIZE; x++)
                {
                    newdata[(SIZE * i) + j] += m_values[(SIZE * i) + x] * rhs.m_values[(SIZE * x) + j];
                }
            }
        }

        return Matrix<T, SIZE>(newdata);
    }

    Matrix<T, SIZE> Transpose() const
    {
        T newdata[(SIZE * SIZE)];

        int i, j;

        for (i = 0; i < SIZE; i++)
        {
            for (j = 0; j < SIZE; j++)
            {
                /** loop across ith row and down jth column **/
                newdata[(SIZE * j) + i] = m_values[(SIZE * i) + j];
            }
        }

        return Matrix<T, SIZE>(newdata);
    };

    void Set(unsigned int row, unsigned int col, T val)
    {
        m_values[(col * SIZE) + row] = val;
    };

    T Get(unsigned int row, unsigned int col) const
    {
        return m_values[(col * SIZE) + row];
    };

private:
    T m_values[(SIZE * SIZE)];
};

typedef Matrix<float, 2> Matrix2f;
typedef Matrix<float, 3> Matrix3f;
typedef Matrix<float, 4> Matrix4f;

typedef Matrix<double, 2> Matrix2d;
typedef Matrix<double, 3> Matrix3d;
typedef Matrix<double, 4> Matrix4d;

};  // namespace JML

#endif