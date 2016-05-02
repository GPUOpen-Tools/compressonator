//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtList.h 
/// 
//=====================================================================

//------------------------------ gtList.h ------------------------------

#ifndef __GTLIST
#define __GTLIST

// STL:
#include <list>

// Infra:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>

// Local:
#include <AMDTBaseTools/Include/gtIgnoreCompilerWarnings.h>

// Allow using types from the std namespace:
using namespace std;


// ----------------------------------------------------------------------------------
// Class Name:           gtList
// General Description:
//  A class representing a list of elements. It is implemented as a doubly linked list.
//  This enables:
//  a. Efficient forward and backward traversal on the list items.
//  b. Constant time insertion and removal of elements at the beginning or the end of the list.
//
//  Notice:
//  a. Insertion and splicing do not invalidate iterators to list elements.
//  b. Removal invalidates only the iterators that point to the elements that are removed.
//
// Author:      AMD Developer Tools Team
// Creation Date:        11/5/2003
// ----------------------------------------------------------------------------------
template<typename _Tp, typename _Alloc = allocator<_Tp> >
class gtList : public list<_Tp, _Alloc>
{
public:
    typedef list<_Tp, _Alloc> StdList;
    gtList() {};
    gtList(const gtList& other) : StdList(other) {};

    gtList& operator=(const gtList& other)
    {
        static_cast<StdList*>(this)->operator=(other);
        return *this;
    }

#if AMDT_HAS_CPP0X
    gtList(gtList&& other) : StdList(move(other)) {}

    gtList& operator=(gtList&& other)
    {
        static_cast<StdList*>(this)->operator=(move(other));
        return *this;
    }
#endif


    // ---------------------------------------------------------------------------
    // Name:        length
    // Description: Returns the list length.
    //              Notice that this function complexity is O(n).
    // Author:      AMD Developer Tools Team
    // Date:        8/5/2005
    // ---------------------------------------------------------------------------
    int length() const
    {
        int itemsAmount = 0;

        // Count the list items:
        typename list<_Tp, _Alloc>::const_iterator endIter = gtList::end();
        typename list<_Tp, _Alloc>::const_iterator iter = gtList::begin();

        while (iter != endIter)
        {
            itemsAmount++;
            iter++;
        }

        return itemsAmount;
    }
};


#endif  // __GTLIST
