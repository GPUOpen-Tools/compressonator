//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtQueue.h 
/// 
//=====================================================================

//------------------------------ gtQueue.h ------------------------------

#ifndef __GTQUEUE
#define __GTQUEUE

// STL:
#include <deque>
#include <queue>

// Local:
#include <CXLBaseTools/Include/gtIgnoreCompilerWarnings.h>

// Allow using types from the std namespace:
using namespace std;


// ----------------------------------------------------------------------------------
// Class Name:           gtQueue
// General Description:
//   A class representing a queue of elements. Items insertion and extraction is done
//   in FIFO (First in first out) order.
//
// Author:      AMD Developer Tools Team
// Creation Date:        11/5/2003
// ----------------------------------------------------------------------------------
template<class Type, class Container = deque<Type> >
class gtQueue : public queue<Type, Container>
{
public:
    typedef queue<Type, Container> StdQueue;

    gtQueue() {};
    gtQueue(const gtQueue& other) : StdQueue(other) {};

    gtQueue& operator=(const gtQueue& other)
    {
        static_cast<StdQueue*>(this)->operator=(other);
        return *this;
    }

#if AMDT_HAS_CPP0X
    gtQueue(gtQueue&& other) : StdQueue(move(other)) {}

    gtQueue& operator=(gtQueue&& other)
    {
        static_cast<StdQueue*>(this)->operator=(move(other));
        return *this;
    }
#endif
};


#endif  // __GTQUEUE
