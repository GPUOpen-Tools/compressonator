//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtAutoPtr.h 
/// 
//=====================================================================

//------------------------------ gtAutoPtr.h ------------------------------

#ifndef __GTAUTOPTR
#define __GTAUTOPTR

// Local:
#include <CXLBaseTools/Include/AMDTDefinitions.h>

// ----------------------------------------------------------------------------------
// Class Name:           gtAutoPtr
// General Description:
//   Automatic pointer manager. gtAutoPtr is a class that manages the memory of
//   its pointed object. Only one gtAutoPtr can hold the pointer to the pointed
//   object at any given time. When a gtAutoPtr that holds a pointer dies, it deletes
//   the pointed object.
//   gtAutoPtr are a good solution in cases where an object is created by one function
//   and deleted by another function.
//
//   For more details see "More Effective C++" / Scott Meyers.
//
//   Example:
//   void foo()
//   {
//      gtAutoPtr<int> ap1 = new int;  // ap1 holds the pointer.
//      gtAutoPtr<int> ap2 = ap1;      // ap2 now holds the pointer, ap1 holds null.
//      ...
//   }                                 // ap2 destructor is called - the int object is deleted.
//
//
//   Warning: Do not pass a gtAutoPtr to a function by value !!!
//            When passing an argument by value, the compiler generates a temporary object
//            that will hold the copied value. Upon entry to the function, this object's
//            copy constructor gets the pointed object ownership. When the function exits,
//            this temporary object's destructor is called, causing the pointed object
//            to be deleted !
//
// Author:      AMD Developer Tools Team
// Creation Date:        12/4/2004
// ----------------------------------------------------------------------------------
template<class T> class gtAutoPtr
{
public:
    // Constructor:
    // Inputs a pointer to the object who's memory will be managed by this class.
    gtAutoPtr(T* pPointedObject = NULL) : _pPointedObject(pPointedObject) {};

    // Copy constructor:
    // Gets the pointed object ownership from the other gtAutoPtr.
    gtAutoPtr(const gtAutoPtr<T>& other)
    {
        _pPointedObject = ((gtAutoPtr<T>&)(other)).releasePointedObjectOwnership();
    };

    // Destructor:
    // If I have a pointed object - delete it.
    ~gtAutoPtr()
    {
        if (_pPointedObject != NULL)
        {
            delete _pPointedObject;
        }
    }

    // Assignment operators:
    // Gets the pointed object ownership from the other gtAutoPtr.
    gtAutoPtr<T>& operator=(const gtAutoPtr<T>& other)
    {
        delete _pPointedObject;
        _pPointedObject = ((gtAutoPtr<T>&)(other)).releasePointedObjectOwnership();
        return *this;
    }

    // Assignment operators:
    // This class becomes the owner of the pointed object:
    gtAutoPtr<T>& operator=(T* pPointedObject)
    {
        delete _pPointedObject;
        _pPointedObject = pPointedObject;
        return *this;
    }

    // Dereferencing operator:
    // Returns a reference to my pointed object.
    T& operator*() const
    {
        return *_pPointedObject;
    }

    // Pointer access operator:
    // Returns a pointer to my pointed object.
    T* operator->() const
    {
        return _pPointedObject;
    }

    // Returns my pointed object:
    T* pointedObject() const
    {
        return _pPointedObject;
    };

    // Release the pointed object ownership:
    // (Return a pointer to the pointed object).
    T* releasePointedObjectOwnership()
    {
        T* pPointedObject = _pPointedObject;
        _pPointedObject = NULL;
        return pPointedObject;
    };

private:
    // The object who's memory will be managed by this class:
    T* _pPointedObject;
};


#endif  // __GTAUTOPTR
