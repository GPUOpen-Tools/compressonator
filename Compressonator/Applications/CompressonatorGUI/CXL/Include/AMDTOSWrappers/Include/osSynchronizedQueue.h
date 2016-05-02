//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osSynchronizedQueue.h
///
//=====================================================================

//------------------------------ osSynchronizedQueue.h ------------------------------

#ifndef __OSSYNCHRONIZEDQUEUE
#define __OSSYNCHRONIZEDQUEUE

// Infra:
#include <AMDTBaseTools/Include/gtQueue.h>

// Local:
#include <AMDTOSWrappers/Include/osCriticalSectionLocker.h>


// ----------------------------------------------------------------------------------
// Class Name:           osSynchronizedQueue
// General Description:
//   A Queue that enables synchronized access by many threads.
//   This queue is a useful synchronization tool between two or more threads.
//
// Author:      AMD Developer Tools Team
// Creation Date:        29/3/2004
// ----------------------------------------------------------------------------------
template <class QueueItemType> class osSynchronizedQueue
{
public:
    osSynchronizedQueue() {};

    // Insert an item copy into the back of the queue:
    void push(const QueueItemType& item)
    {
        osCriticalSectionLocker criticalSectionLocker(_criticalSection);
        _queue.push(item);
    };


    // Returns true iff the queue is empty:
    bool isEmpty() const
    {
        osCriticalSectionLocker criticalSectionLocker(((osSynchronizedQueue*)(this))->_criticalSection);
        return _queue.empty();
    };


    // Returns the amount of items currently in the queue:
    unsigned long size() const
    {
        osCriticalSectionLocker criticalSectionLocker(((osSynchronizedQueue*)(this))->_criticalSection);
        return (unsigned long)_queue.size();
    }


    // Returns a copy of the queue front item:
    QueueItemType front() const
    {
        osCriticalSectionLocker criticalSectionLocker(((osSynchronizedQueue*)(this))->_criticalSection);
        return _queue.front();
    };


    // Removes the queue front item:
    QueueItemType pop()
    {
        osCriticalSectionLocker criticalSectionLocker(_criticalSection);
        QueueItemType retVal = _queue.front();
        _queue.pop();
        return retVal;
    };

    // Remove all items from the queue in a single call
    void popAll(gtQueue<QueueItemType>& queueOfAllItems)
    {
        osCriticalSectionLocker criticalSectionLocker(_criticalSection);

        while (!_queue.empty())
        {
            queueOfAllItems.push(_queue.front());
            _queue.pop();
        }
    }


private:
    // The queue implementation:
    gtQueue<QueueItemType> _queue;

    // A critical section object that will be used to synchronize access to this
    // synchronized queue:
    osCriticalSection _criticalSection;
};

#endif  // __OSSYNCHRONIZEDQUEUE
