//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtVector.h
///
//=====================================================================

//------------------------------ gtVector.h ------------------------------

#ifndef __GTVECTOR
#define __GTVECTOR

// STL:
#include <vector>

// Local:
#include <AMDTBaseTools/Include/gtIgnoreCompilerWarnings.h>

// Allow using types from the std namespace:
using namespace std;


// ----------------------------------------------------------------------------------
// Class Name:           gtVector
// General Description:
//   A class representing a vector of elements. It allows fast random access to its elements.
//   Use this container class for optimizing random access performance.
// Author:      AMD Developer Tools Team
// Creation Date:        11/5/2003
// ----------------------------------------------------------------------------------
template<class _Tp, class _Allocator = allocator<_Tp> >
class gtVector : public vector<_Tp, _Allocator>
{
public:
    typedef vector<_Tp, _Allocator> StdVector;

    gtVector() {}
    gtVector(std::initializer_list<_Tp> l) : StdVector(l) {}
    gtVector(const gtVector& other) : StdVector(other) {}
    explicit gtVector(size_t count) : StdVector(count) {}
    gtVector(size_t count, const _Tp& val) : StdVector(count, val) {}

    gtVector& operator=(const gtVector& other)
    {
        static_cast<StdVector*>(this)->operator=(other);
        return *this;
    }

#if AMDT_HAS_CPP0X
    gtVector(gtVector&& other) : StdVector(move(other)) {}

    gtVector& operator=(gtVector&& other)
    {
        static_cast<StdVector*>(this)->operator=(move(other));
        return *this;
    }
#endif

    const _Tp& rawData() const { return this->front(); }

    void removeItem(int index)
    {
        // Move the items above the removed item one place back:
        for (unsigned i = index; i < (*this).size() - 1; i++)
        {
            (*this)[i] = (*this)[i + 1];

        }

        // Pop the top item:
        (*this).pop_back();
    }
};


#endif  // __GTVECTOR
