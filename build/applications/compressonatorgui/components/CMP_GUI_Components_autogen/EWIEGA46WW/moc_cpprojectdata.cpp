/****************************************************************************
** Meta object code from reading C++ file 'cpprojectdata.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpprojectdata.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpprojectdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_x_Options_Controller_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_x_Options_Controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_x_Options_Controller_t qt_meta_stringdata_x_Options_Controller = {
    {
QT_MOC_LITERAL(0, 0, 20) // "x_Options_Controller"

    },
    "x_Options_Controller"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_x_Options_Controller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void x_Options_Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject x_Options_Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_x_Options_Controller.data,
    qt_meta_data_x_Options_Controller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *x_Options_Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *x_Options_Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_x_Options_Controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int x_Options_Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_C_Input_HDR_Image_Properties_t {
    QByteArrayData data[14];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Input_HDR_Image_Properties_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Input_HDR_Image_Properties_t qt_meta_stringdata_C_Input_HDR_Image_Properties = {
    {
QT_MOC_LITERAL(0, 0, 28), // "C_Input_HDR_Image_Properties"
QT_MOC_LITERAL(1, 29, 12), // "defogChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 7), // "double&"
QT_MOC_LITERAL(4, 51, 15), // "exposureChanged"
QT_MOC_LITERAL(5, 67, 14), // "kneeLowChanged"
QT_MOC_LITERAL(6, 82, 15), // "kneeHighChanged"
QT_MOC_LITERAL(7, 98, 12), // "gammaChanged"
QT_MOC_LITERAL(8, 111, 11), // "dataChanged"
QT_MOC_LITERAL(9, 123, 5), // "Defog"
QT_MOC_LITERAL(10, 129, 8), // "Exposure"
QT_MOC_LITERAL(11, 138, 7), // "KneeLow"
QT_MOC_LITERAL(12, 146, 8), // "KneeHigh"
QT_MOC_LITERAL(13, 155, 5) // "Gamma"

    },
    "C_Input_HDR_Image_Properties\0defogChanged\0"
    "\0double&\0exposureChanged\0kneeLowChanged\0"
    "kneeHighChanged\0gammaChanged\0dataChanged\0"
    "Defog\0Exposure\0KneeLow\0KneeHigh\0Gamma"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Input_HDR_Image_Properties[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       5,   60, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       5,    1,   50,    2, 0x06 /* Public */,
       6,    1,   53,    2, 0x06 /* Public */,
       7,    1,   56,    2, 0x06 /* Public */,
       8,    0,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::Double, 0x00495103,
      10, QMetaType::Double, 0x00495103,
      11, QMetaType::Double, 0x00495103,
      12, QMetaType::Double, 0x00495103,
      13, QMetaType::Double, 0x00495103,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,

       0        // eod
};

void C_Input_HDR_Image_Properties::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Input_HDR_Image_Properties *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->defogChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->exposureChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->kneeLowChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->kneeHighChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->gammaChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Input_HDR_Image_Properties::*)(double & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::defogChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (C_Input_HDR_Image_Properties::*)(double & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::exposureChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (C_Input_HDR_Image_Properties::*)(double & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::kneeLowChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (C_Input_HDR_Image_Properties::*)(double & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::kneeHighChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (C_Input_HDR_Image_Properties::*)(double & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::gammaChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (C_Input_HDR_Image_Properties::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Input_HDR_Image_Properties::dataChanged)) {
                *result = 5;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Input_HDR_Image_Properties *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->getDefog(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getExposure(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->getKneeLow(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->getKneeHigh(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->getGamma(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Input_HDR_Image_Properties *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDefog(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setExposure(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setKneeLow(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setKneeHigh(*reinterpret_cast< double*>(_v)); break;
        case 4: _t->setGamma(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Input_HDR_Image_Properties::staticMetaObject = { {
    QMetaObject::SuperData::link<x_Options_Controller::staticMetaObject>(),
    qt_meta_stringdata_C_Input_HDR_Image_Properties.data,
    qt_meta_data_C_Input_HDR_Image_Properties,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Input_HDR_Image_Properties::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Input_HDR_Image_Properties::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Input_HDR_Image_Properties.stringdata0))
        return static_cast<void*>(this);
    return x_Options_Controller::qt_metacast(_clname);
}

int C_Input_HDR_Image_Properties::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = x_Options_Controller::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Input_HDR_Image_Properties::defogChanged(double & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void C_Input_HDR_Image_Properties::exposureChanged(double & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void C_Input_HDR_Image_Properties::kneeLowChanged(double & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void C_Input_HDR_Image_Properties::kneeHighChanged(double & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void C_Input_HDR_Image_Properties::gammaChanged(double & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void C_Input_HDR_Image_Properties::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
struct qt_meta_stringdata_C_Codec_Block_t {
    QByteArrayData data[7];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Codec_Block_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Codec_Block_t qt_meta_stringdata_C_Codec_Block = {
    {
QT_MOC_LITERAL(0, 0, 13), // "C_Codec_Block"
QT_MOC_LITERAL(1, 14, 14), // "bitrateChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8), // "QString&"
QT_MOC_LITERAL(4, 39, 4), // "int&"
QT_MOC_LITERAL(5, 44, 11), // "dataChanged"
QT_MOC_LITERAL(6, 56, 7) // "Bitrate"

    },
    "C_Codec_Block\0bitrateChanged\0\0QString&\0"
    "int&\0dataChanged\0Bitrate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Codec_Block[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,
       5,    0,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 4, 0x80000000 | 4,    2,    2,    2,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::QString, 0x00495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

void C_Codec_Block::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Codec_Block *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bitrateChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Codec_Block::*)(QString & , int & , int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Codec_Block::bitrateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (C_Codec_Block::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Codec_Block::dataChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Codec_Block *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getBitrate(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Codec_Block *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBitrate(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Codec_Block::staticMetaObject = { {
    QMetaObject::SuperData::link<C_Input_HDR_Image_Properties::staticMetaObject>(),
    qt_meta_stringdata_C_Codec_Block.data,
    qt_meta_data_C_Codec_Block,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Codec_Block::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Codec_Block::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Codec_Block.stringdata0))
        return static_cast<void*>(this);
    return C_Input_HDR_Image_Properties::qt_metacast(_clname);
}

int C_Codec_Block::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C_Input_HDR_Image_Properties::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Codec_Block::bitrateChanged(QString & _t1, int & _t2, int & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void C_Codec_Block::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_C_Refine_t {
    QByteArrayData data[6];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Refine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Refine_t qt_meta_stringdata_C_Refine = {
    {
QT_MOC_LITERAL(0, 0, 8), // "C_Refine"
QT_MOC_LITERAL(1, 9, 19), // "Refine_StepsChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 9), // "QVariant&"
QT_MOC_LITERAL(4, 40, 11), // "dataChanged"
QT_MOC_LITERAL(5, 52, 12) // "Refine_Steps"

    },
    "C_Refine\0Refine_StepsChanged\0\0QVariant&\0"
    "dataChanged\0Refine_Steps"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Refine[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   28, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    0,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

void C_Refine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Refine *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->Refine_StepsChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 1: _t->dataChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Refine::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Refine::Refine_StepsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (C_Refine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Refine::dataChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Refine *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getRefine_Steps(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Refine *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRefine_Steps(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Refine::staticMetaObject = { {
    QMetaObject::SuperData::link<C_Codec_Block::staticMetaObject>(),
    qt_meta_stringdata_C_Refine.data,
    qt_meta_data_C_Refine,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Refine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Refine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Refine.stringdata0))
        return static_cast<void*>(this);
    return C_Codec_Block::qt_metacast(_clname);
}

int C_Refine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C_Codec_Block::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Refine::Refine_StepsChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void C_Refine::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_DXT1_Alpha_t {
    QByteArrayData data[5];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DXT1_Alpha_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DXT1_Alpha_t qt_meta_stringdata_DXT1_Alpha = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DXT1_Alpha"
QT_MOC_LITERAL(1, 11, 16), // "thresholdChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 9), // "QVariant&"
QT_MOC_LITERAL(4, 39, 9) // "Threshold"

    },
    "DXT1_Alpha\0thresholdChanged\0\0QVariant&\0"
    "Threshold"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DXT1_Alpha[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // properties: name, type, flags
       4, QMetaType::Int, 0x00495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

void DXT1_Alpha::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DXT1_Alpha *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->thresholdChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DXT1_Alpha::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DXT1_Alpha::thresholdChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<DXT1_Alpha *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getThreshold(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<DXT1_Alpha *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setThreshold(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject DXT1_Alpha::staticMetaObject = { {
    QMetaObject::SuperData::link<C_Refine::staticMetaObject>(),
    qt_meta_stringdata_DXT1_Alpha.data,
    qt_meta_data_DXT1_Alpha,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DXT1_Alpha::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DXT1_Alpha::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DXT1_Alpha.stringdata0))
        return static_cast<void*>(this);
    return C_Refine::qt_metacast(_clname);
}

int DXT1_Alpha::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C_Refine::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void DXT1_Alpha::thresholdChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Channel_Weighting_t {
    QByteArrayData data[10];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Channel_Weighting_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Channel_Weighting_t qt_meta_stringdata_Channel_Weighting = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Channel_Weighting"
QT_MOC_LITERAL(1, 18, 11), // "redwChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "QVariant&"
QT_MOC_LITERAL(4, 41, 13), // "greenwChanged"
QT_MOC_LITERAL(5, 55, 12), // "bluewChanged"
QT_MOC_LITERAL(6, 68, 8), // "Adaptive"
QT_MOC_LITERAL(7, 77, 5), // "X_RED"
QT_MOC_LITERAL(8, 83, 7), // "Y_GREEN"
QT_MOC_LITERAL(9, 91, 6) // "Z_BLUE"

    },
    "Channel_Weighting\0redwChanged\0\0QVariant&\0"
    "greenwChanged\0bluewChanged\0Adaptive\0"
    "X_RED\0Y_GREEN\0Z_BLUE"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Channel_Weighting[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       4,   38, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       5,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // properties: name, type, flags
       6, QMetaType::Bool, 0x00095003,
       7, QMetaType::Double, 0x00495003,
       8, QMetaType::Double, 0x00495003,
       9, QMetaType::Double, 0x00495003,

 // properties: notify_signal_id
       0,
       0,
       1,
       2,

       0        // eod
};

void Channel_Weighting::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Channel_Weighting *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->redwChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 1: _t->greenwChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->bluewChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Channel_Weighting::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Channel_Weighting::redwChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Channel_Weighting::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Channel_Weighting::greenwChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Channel_Weighting::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Channel_Weighting::bluewChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Channel_Weighting *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->Adaptive; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getX_Red(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->getY_Green(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->getZ_Blue(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Channel_Weighting *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->Adaptive != *reinterpret_cast< bool*>(_v)) {
                _t->Adaptive = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 1: _t->setX_Red(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setY_Green(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setZ_Blue(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Channel_Weighting::staticMetaObject = { {
    QMetaObject::SuperData::link<DXT1_Alpha::staticMetaObject>(),
    qt_meta_stringdata_Channel_Weighting.data,
    qt_meta_data_Channel_Weighting,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Channel_Weighting::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Channel_Weighting::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Channel_Weighting.stringdata0))
        return static_cast<void*>(this);
    return DXT1_Alpha::qt_metacast(_clname);
}

int Channel_Weighting::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DXT1_Alpha::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Channel_Weighting::redwChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Channel_Weighting::greenwChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Channel_Weighting::bluewChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_Mesh_Compression_Settings_t {
    QByteArrayData data[8];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Mesh_Compression_Settings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Mesh_Compression_Settings_t qt_meta_stringdata_Mesh_Compression_Settings = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Mesh_Compression_Settings"
QT_MOC_LITERAL(1, 26, 18), // "onMesh_Compression"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 9), // "QVariant&"
QT_MOC_LITERAL(4, 56, 5), // "value"
QT_MOC_LITERAL(5, 62, 18), // "Compression_Format"
QT_MOC_LITERAL(6, 81, 16), // "eMeshCompression"
QT_MOC_LITERAL(7, 98, 6) // "NoComp"

    },
    "Mesh_Compression_Settings\0onMesh_Compression\0"
    "\0QVariant&\0value\0Compression_Format\0"
    "eMeshCompression\0NoComp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Mesh_Compression_Settings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   22, // properties
       1,   25, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // properties: name, type, flags
       5, 0x80000000 | 6, 0x0009500b,

 // enums: name, alias, flags, count, data
       6,    6, 0x0,    1,   30,

 // enum data: key, value
       7, uint(Mesh_Compression_Settings::NoComp),

       0        // eod
};

void Mesh_Compression_Settings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Mesh_Compression_Settings *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMesh_Compression((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Mesh_Compression_Settings::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Mesh_Compression_Settings::onMesh_Compression)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Mesh_Compression_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< eMeshCompression*>(_v) = _t->getDo_Mesh_Compression(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Mesh_Compression_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDo_Mesh_Compression(*reinterpret_cast< eMeshCompression*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Mesh_Compression_Settings::staticMetaObject = { {
    QMetaObject::SuperData::link<Channel_Weighting::staticMetaObject>(),
    qt_meta_stringdata_Mesh_Compression_Settings.data,
    qt_meta_data_Mesh_Compression_Settings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Mesh_Compression_Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Mesh_Compression_Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Mesh_Compression_Settings.stringdata0))
        return static_cast<void*>(this);
    return Channel_Weighting::qt_metacast(_clname);
}

int Mesh_Compression_Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Channel_Weighting::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Mesh_Compression_Settings::onMesh_Compression(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Mesh_Optimizer_Settings_t {
    QByteArrayData data[19];
    char stringdata0[337];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Mesh_Optimizer_Settings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Mesh_Optimizer_Settings_t qt_meta_stringdata_Mesh_Optimizer_Settings = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Mesh_Optimizer_Settings"
QT_MOC_LITERAL(1, 24, 19), // "onMesh_Optimization"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 9), // "QVariant&"
QT_MOC_LITERAL(4, 55, 5), // "value"
QT_MOC_LITERAL(5, 61, 19), // "Optimization_Format"
QT_MOC_LITERAL(6, 81, 17), // "eMeshOptimization"
QT_MOC_LITERAL(7, 99, 26), // "x____Optimize_Vertex_Cache"
QT_MOC_LITERAL(8, 126, 19), // "x________Cache_Size"
QT_MOC_LITERAL(9, 146, 25), // "x____Optimize_Vertex_FIFO"
QT_MOC_LITERAL(10, 172, 24), // "x________FIFO_Cache_Size"
QT_MOC_LITERAL(11, 197, 22), // "x____Optimize_Overdraw"
QT_MOC_LITERAL(12, 220, 23), // "x________ACMR_Threshold"
QT_MOC_LITERAL(13, 244, 26), // "x____Optimize_Vertex_Fetch"
QT_MOC_LITERAL(14, 271, 18), // "x____Simplify_Mesh"
QT_MOC_LITERAL(15, 290, 24), // "x________Level_of_Detail"
QT_MOC_LITERAL(16, 315, 5), // "NoOpt"
QT_MOC_LITERAL(17, 321, 7), // "AutoOpt"
QT_MOC_LITERAL(18, 329, 7) // "UserOpt"

    },
    "Mesh_Optimizer_Settings\0onMesh_Optimization\0"
    "\0QVariant&\0value\0Optimization_Format\0"
    "eMeshOptimization\0x____Optimize_Vertex_Cache\0"
    "x________Cache_Size\0x____Optimize_Vertex_FIFO\0"
    "x________FIFO_Cache_Size\0"
    "x____Optimize_Overdraw\0x________ACMR_Threshold\0"
    "x____Optimize_Vertex_Fetch\0"
    "x____Simplify_Mesh\0x________Level_of_Detail\0"
    "NoOpt\0AutoOpt\0UserOpt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Mesh_Optimizer_Settings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      10,   22, // properties
       1,   52, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // properties: name, type, flags
       5, 0x80000000 | 6, 0x0009500b,
       7, QMetaType::Bool, 0x00095003,
       8, QMetaType::Int, 0x00095003,
       9, QMetaType::Bool, 0x00095003,
      10, QMetaType::Int, 0x00095003,
      11, QMetaType::Bool, 0x00095003,
      12, QMetaType::Double, 0x00095003,
      13, QMetaType::Bool, 0x00095003,
      14, QMetaType::Bool, 0x00095003,
      15, QMetaType::Int, 0x00095003,

 // enums: name, alias, flags, count, data
       6,    6, 0x0,    3,   57,

 // enum data: key, value
      16, uint(Mesh_Optimizer_Settings::NoOpt),
      17, uint(Mesh_Optimizer_Settings::AutoOpt),
      18, uint(Mesh_Optimizer_Settings::UserOpt),

       0        // eod
};

void Mesh_Optimizer_Settings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Mesh_Optimizer_Settings *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMesh_Optimization((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Mesh_Optimizer_Settings::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Mesh_Optimizer_Settings::onMesh_Optimization)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Mesh_Optimizer_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< eMeshOptimization*>(_v) = _t->getDo_Mesh_Optimization(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getOptimizeVCacheChecked(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getCacheSize(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->getOptimizeVCacheFifoChecked(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->getCacheSizeFifo(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->getOptimizeOverdrawChecked(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->getACMRThreshold(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->getOptimizeVFetchChecked(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->getMeshSimplifyChecked(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->getLODValue(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Mesh_Optimizer_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDo_Mesh_Optimization(*reinterpret_cast< eMeshOptimization*>(_v)); break;
        case 1: _t->setOptimizeVCacheChecked(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setCacheSize(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setOptimizeVCacheFifoChecked(*reinterpret_cast< bool*>(_v)); break;
        case 4: _t->setCacheSizeFifo(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setOptimizeOverdrawChecked(*reinterpret_cast< bool*>(_v)); break;
        case 6: _t->setACMRThreshold(*reinterpret_cast< double*>(_v)); break;
        case 7: _t->setOptimizeVFetchChecked(*reinterpret_cast< bool*>(_v)); break;
        case 8: _t->setMeshSimplifyChecked(*reinterpret_cast< bool*>(_v)); break;
        case 9: _t->setLODValue(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Mesh_Optimizer_Settings::staticMetaObject = { {
    QMetaObject::SuperData::link<Mesh_Compression_Settings::staticMetaObject>(),
    qt_meta_stringdata_Mesh_Optimizer_Settings.data,
    qt_meta_data_Mesh_Optimizer_Settings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Mesh_Optimizer_Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Mesh_Optimizer_Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Mesh_Optimizer_Settings.stringdata0))
        return static_cast<void*>(this);
    return Mesh_Compression_Settings::qt_metacast(_clname);
}

int Mesh_Optimizer_Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Mesh_Compression_Settings::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Mesh_Optimizer_Settings::onMesh_Optimization(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_C_Destination_Image_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Destination_Image_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Destination_Image_t qt_meta_stringdata_C_Destination_Image = {
    {
QT_MOC_LITERAL(0, 0, 19), // "C_Destination_Image"
QT_MOC_LITERAL(1, 20, 5), // "_Name"
QT_MOC_LITERAL(2, 26, 10), // "_File_Size"
QT_MOC_LITERAL(3, 37, 6), // "_Width"
QT_MOC_LITERAL(4, 44, 7), // "_Height"
QT_MOC_LITERAL(5, 52, 18), // "_Compression_Ratio"
QT_MOC_LITERAL(6, 71, 17) // "_Compression_Time"

    },
    "C_Destination_Image\0_Name\0_File_Size\0"
    "_Width\0_Height\0_Compression_Ratio\0"
    "_Compression_Time"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Destination_Image[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       6,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,
       4, QMetaType::QString, 0x00095003,
       5, QMetaType::QString, 0x00095003,
       6, QMetaType::QString, 0x00095003,

       0        // eod
};

void C_Destination_Image::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Destination_Image *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_FileInfoDestinationName; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_FileSizeStr; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_WidthStr; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->m_HeightStr; break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->m_CompressionRatio; break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->m_CompressionTimeStr; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Destination_Image *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_FileInfoDestinationName != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileInfoDestinationName = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_FileSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_WidthStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_WidthStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3:
            if (_t->m_HeightStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_HeightStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 4:
            if (_t->m_CompressionRatio != *reinterpret_cast< QString*>(_v)) {
                _t->m_CompressionRatio = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 5:
            if (_t->m_CompressionTimeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_CompressionTimeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_Destination_Image::staticMetaObject = { {
    QMetaObject::SuperData::link<Mesh_Optimizer_Settings::staticMetaObject>(),
    qt_meta_stringdata_C_Destination_Image.data,
    qt_meta_data_C_Destination_Image,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Destination_Image::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Destination_Image::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Destination_Image.stringdata0))
        return static_cast<void*>(this);
    return Mesh_Optimizer_Settings::qt_metacast(_clname);
}

int C_Destination_Image::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Mesh_Optimizer_Settings::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_Destination_Options_t {
    QByteArrayData data[44];
    char stringdata0[422];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Destination_Options_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Destination_Options_t qt_meta_stringdata_C_Destination_Options = {
    {
QT_MOC_LITERAL(0, 0, 21), // "C_Destination_Options"
QT_MOC_LITERAL(1, 22, 18), // "compressionChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 9), // "QVariant&"
QT_MOC_LITERAL(4, 52, 22), // "meshCompressionChanged"
QT_MOC_LITERAL(5, 75, 14), // "qualityChanged"
QT_MOC_LITERAL(6, 90, 15), // "enableHQChanged"
QT_MOC_LITERAL(7, 106, 6), // "Format"
QT_MOC_LITERAL(8, 113, 20), // "eCompression_options"
QT_MOC_LITERAL(9, 134, 7), // "Quality"
QT_MOC_LITERAL(10, 142, 3), // "BC1"
QT_MOC_LITERAL(11, 146, 3), // "BC2"
QT_MOC_LITERAL(12, 150, 3), // "BC3"
QT_MOC_LITERAL(13, 154, 3), // "BC4"
QT_MOC_LITERAL(14, 158, 5), // "BC4_S"
QT_MOC_LITERAL(15, 164, 3), // "BC5"
QT_MOC_LITERAL(16, 168, 5), // "BC5_S"
QT_MOC_LITERAL(17, 174, 4), // "BC6H"
QT_MOC_LITERAL(18, 179, 7), // "BC6H_SF"
QT_MOC_LITERAL(19, 187, 3), // "BC7"
QT_MOC_LITERAL(20, 191, 7), // "ETC_RGB"
QT_MOC_LITERAL(21, 199, 8), // "ETC2_RGB"
QT_MOC_LITERAL(22, 208, 9), // "ETC2_RGBA"
QT_MOC_LITERAL(23, 218, 10), // "ETC2_RGBA1"
QT_MOC_LITERAL(24, 229, 7), // "ATC_RGB"
QT_MOC_LITERAL(25, 237, 17), // "ATC_RGBA_Explicit"
QT_MOC_LITERAL(26, 255, 21), // "ATC_RGBA_Interpolated"
QT_MOC_LITERAL(27, 277, 5), // "ATI1N"
QT_MOC_LITERAL(28, 283, 5), // "ATI2N"
QT_MOC_LITERAL(29, 289, 8), // "ATI2N_XY"
QT_MOC_LITERAL(30, 298, 10), // "ATI2N_DXT5"
QT_MOC_LITERAL(31, 309, 4), // "DXT1"
QT_MOC_LITERAL(32, 314, 4), // "DXT3"
QT_MOC_LITERAL(33, 319, 4), // "DXT5"
QT_MOC_LITERAL(34, 324, 9), // "DXT5_xGBR"
QT_MOC_LITERAL(35, 334, 9), // "DXT5_RxBG"
QT_MOC_LITERAL(36, 344, 9), // "DXT5_RBxG"
QT_MOC_LITERAL(37, 354, 9), // "DXT5_xRBG"
QT_MOC_LITERAL(38, 364, 9), // "DXT5_RGxB"
QT_MOC_LITERAL(39, 374, 9), // "DXT5_xGxR"
QT_MOC_LITERAL(40, 384, 9), // "ARGB_8888"
QT_MOC_LITERAL(41, 394, 8), // "ARGB_16F"
QT_MOC_LITERAL(42, 403, 8), // "ARGB_32F"
QT_MOC_LITERAL(43, 412, 9) // "MESH_DATA"

    },
    "C_Destination_Options\0compressionChanged\0"
    "\0QVariant&\0meshCompressionChanged\0"
    "qualityChanged\0enableHQChanged\0Format\0"
    "eCompression_options\0Quality\0BC1\0BC2\0"
    "BC3\0BC4\0BC4_S\0BC5\0BC5_S\0BC6H\0BC6H_SF\0"
    "BC7\0ETC_RGB\0ETC2_RGB\0ETC2_RGBA\0"
    "ETC2_RGBA1\0ATC_RGB\0ATC_RGBA_Explicit\0"
    "ATC_RGBA_Interpolated\0ATI1N\0ATI2N\0"
    "ATI2N_XY\0ATI2N_DXT5\0DXT1\0DXT3\0DXT5\0"
    "DXT5_xGBR\0DXT5_RxBG\0DXT5_RBxG\0DXT5_xRBG\0"
    "DXT5_RGxB\0DXT5_xGxR\0ARGB_8888\0ARGB_16F\0"
    "ARGB_32F\0MESH_DATA"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Destination_Options[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       2,   46, // properties
       1,   54, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       5,    1,   40,    2, 0x06 /* Public */,
       6,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // properties: name, type, flags
       7, 0x80000000 | 8, 0x0049500b,
       9, QMetaType::Double, 0x00495103,

 // properties: notify_signal_id
       0,
       2,

 // enums: name, alias, flags, count, data
       8,    8, 0x0,   34,   59,

 // enum data: key, value
      10, uint(C_Destination_Options::BC1),
      11, uint(C_Destination_Options::BC2),
      12, uint(C_Destination_Options::BC3),
      13, uint(C_Destination_Options::BC4),
      14, uint(C_Destination_Options::BC4_S),
      15, uint(C_Destination_Options::BC5),
      16, uint(C_Destination_Options::BC5_S),
      17, uint(C_Destination_Options::BC6H),
      18, uint(C_Destination_Options::BC6H_SF),
      19, uint(C_Destination_Options::BC7),
      20, uint(C_Destination_Options::ETC_RGB),
      21, uint(C_Destination_Options::ETC2_RGB),
      22, uint(C_Destination_Options::ETC2_RGBA),
      23, uint(C_Destination_Options::ETC2_RGBA1),
      24, uint(C_Destination_Options::ATC_RGB),
      25, uint(C_Destination_Options::ATC_RGBA_Explicit),
      26, uint(C_Destination_Options::ATC_RGBA_Interpolated),
      27, uint(C_Destination_Options::ATI1N),
      28, uint(C_Destination_Options::ATI2N),
      29, uint(C_Destination_Options::ATI2N_XY),
      30, uint(C_Destination_Options::ATI2N_DXT5),
      31, uint(C_Destination_Options::DXT1),
      32, uint(C_Destination_Options::DXT3),
      33, uint(C_Destination_Options::DXT5),
      34, uint(C_Destination_Options::DXT5_xGBR),
      35, uint(C_Destination_Options::DXT5_RxBG),
      36, uint(C_Destination_Options::DXT5_RBxG),
      37, uint(C_Destination_Options::DXT5_xRBG),
      38, uint(C_Destination_Options::DXT5_RGxB),
      39, uint(C_Destination_Options::DXT5_xGxR),
      40, uint(C_Destination_Options::ARGB_8888),
      41, uint(C_Destination_Options::ARGB_16F),
      42, uint(C_Destination_Options::ARGB_32F),
      43, uint(C_Destination_Options::MESH_DATA),

       0        // eod
};

void C_Destination_Options::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Destination_Options *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->compressionChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 1: _t->meshCompressionChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->qualityChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 3: _t->enableHQChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Destination_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Destination_Options::compressionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (C_Destination_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Destination_Options::meshCompressionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (C_Destination_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Destination_Options::qualityChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (C_Destination_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Destination_Options::enableHQChanged)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Destination_Options *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< eCompression_options*>(_v) = _t->getCompression(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getQuality(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Destination_Options *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setCompression(*reinterpret_cast< eCompression_options*>(_v)); break;
        case 1: _t->setQuality(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Destination_Options::staticMetaObject = { {
    QMetaObject::SuperData::link<C_Destination_Image::staticMetaObject>(),
    qt_meta_stringdata_C_Destination_Options.data,
    qt_meta_data_C_Destination_Options,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Destination_Options::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Destination_Options::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Destination_Options.stringdata0))
        return static_cast<void*>(this);
    return C_Destination_Image::qt_metacast(_clname);
}

int C_Destination_Options::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C_Destination_Image::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Destination_Options::compressionChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void C_Destination_Options::meshCompressionChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void C_Destination_Options::qualityChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void C_Destination_Options::enableHQChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_C_Source_Info_t {
    QByteArrayData data[11];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Source_Info_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Source_Info_t qt_meta_stringdata_C_Source_Info = {
    {
QT_MOC_LITERAL(0, 0, 13), // "C_Source_Info"
QT_MOC_LITERAL(1, 14, 5), // "_Name"
QT_MOC_LITERAL(2, 20, 10), // "_Full_Path"
QT_MOC_LITERAL(3, 31, 10), // "_File_Size"
QT_MOC_LITERAL(4, 42, 11), // "_Image_Size"
QT_MOC_LITERAL(5, 54, 6), // "_Width"
QT_MOC_LITERAL(6, 61, 7), // "_Height"
QT_MOC_LITERAL(7, 69, 6), // "_Depth"
QT_MOC_LITERAL(8, 76, 11), // "_Mip_Levels"
QT_MOC_LITERAL(9, 88, 7), // "_Format"
QT_MOC_LITERAL(10, 96, 13) // "_Texture_Type"

    },
    "C_Source_Info\0_Name\0_Full_Path\0"
    "_File_Size\0_Image_Size\0_Width\0_Height\0"
    "_Depth\0_Mip_Levels\0_Format\0_Texture_Type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Source_Info[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,
       4, QMetaType::QString, 0x00095003,
       5, QMetaType::QString, 0x00095003,
       6, QMetaType::QString, 0x00095003,
       7, QMetaType::QString, 0x00095003,
       8, QMetaType::Int, 0x00095003,
       9, QMetaType::QString, 0x00095003,
      10, QMetaType::QString, 0x00095003,

 // enums: name, alias, flags, count, data

 // enum data: key, value

       0        // eod
};

void C_Source_Info::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Source_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_Name; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_Full_Path; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_FileSizeStr; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->m_ImageSizeStr; break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->m_WidthStr; break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->m_HeightStr; break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->m_DepthStr; break;
        case 7: *reinterpret_cast< int*>(_v) = _t->m_Mip_Levels; break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->m_FormatStr; break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->m_TextureTypeStr; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Source_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_Name != *reinterpret_cast< QString*>(_v)) {
                _t->m_Name = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_Full_Path != *reinterpret_cast< QString*>(_v)) {
                _t->m_Full_Path = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_FileSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3:
            if (_t->m_ImageSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_ImageSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 4:
            if (_t->m_WidthStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_WidthStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 5:
            if (_t->m_HeightStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_HeightStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 6:
            if (_t->m_DepthStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_DepthStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 7:
            if (_t->m_Mip_Levels != *reinterpret_cast< int*>(_v)) {
                _t->m_Mip_Levels = *reinterpret_cast< int*>(_v);
            }
            break;
        case 8:
            if (_t->m_FormatStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FormatStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 9:
            if (_t->m_TextureTypeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_TextureTypeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_Source_Info::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_Source_Info.data,
    qt_meta_data_C_Source_Info,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Source_Info::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Source_Info::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Source_Info.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_Source_Info::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_Global_Process_Settings_t {
    QByteArrayData data[7];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Global_Process_Settings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Global_Process_Settings_t qt_meta_stringdata_C_Global_Process_Settings = {
    {
QT_MOC_LITERAL(0, 0, 25), // "C_Global_Process_Settings"
QT_MOC_LITERAL(1, 26, 21), // "globalPropertyChanged"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 4), // "int&"
QT_MOC_LITERAL(4, 54, 7), // "Enabled"
QT_MOC_LITERAL(5, 62, 11), // "Set_Quality"
QT_MOC_LITERAL(6, 74, 16) // "Set_Refine_Steps"

    },
    "C_Global_Process_Settings\0"
    "globalPropertyChanged\0\0int&\0Enabled\0"
    "Set_Quality\0Set_Refine_Steps"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Global_Process_Settings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // properties: name, type, flags
       4, QMetaType::Bool, 0x00095103,
       5, QMetaType::Double, 0x00095003,
       6, QMetaType::Int, 0x00095003,

       0        // eod
};

void C_Global_Process_Settings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Global_Process_Settings *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->globalPropertyChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Global_Process_Settings::*)(int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Global_Process_Settings::globalPropertyChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Global_Process_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->getEnabled(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getQuality(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getRefineSteps(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Global_Process_Settings *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setQuality(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setRefineSteps(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Global_Process_Settings::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_Global_Process_Settings.data,
    qt_meta_data_C_Global_Process_Settings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Global_Process_Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Global_Process_Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Global_Process_Settings.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_Global_Process_Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Global_Process_Settings::globalPropertyChanged(int & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_C_Mesh_Buffer_Info_t {
    QByteArrayData data[4];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Mesh_Buffer_Info_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Mesh_Buffer_Info_t qt_meta_stringdata_C_Mesh_Buffer_Info = {
    {
QT_MOC_LITERAL(0, 0, 18), // "C_Mesh_Buffer_Info"
QT_MOC_LITERAL(1, 19, 5), // "_Name"
QT_MOC_LITERAL(2, 25, 10), // "_Full_Path"
QT_MOC_LITERAL(3, 36, 10) // "_File_Size"

    },
    "C_Mesh_Buffer_Info\0_Name\0_Full_Path\0"
    "_File_Size"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Mesh_Buffer_Info[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,

       0        // eod
};

void C_Mesh_Buffer_Info::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Mesh_Buffer_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_Name; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_Full_Path; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_FileSizeStr; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Mesh_Buffer_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_Name != *reinterpret_cast< QString*>(_v)) {
                _t->m_Name = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_Full_Path != *reinterpret_cast< QString*>(_v)) {
                _t->m_Full_Path = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_FileSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_Mesh_Buffer_Info::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_Mesh_Buffer_Info.data,
    qt_meta_data_C_Mesh_Buffer_Info,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Mesh_Buffer_Info::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Mesh_Buffer_Info::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Mesh_Buffer_Info.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_Mesh_Buffer_Info::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_3DMesh_Statistic_t {
    QByteArrayData data[5];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_3DMesh_Statistic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_3DMesh_Statistic_t qt_meta_stringdata_C_3DMesh_Statistic = {
    {
QT_MOC_LITERAL(0, 0, 18), // "C_3DMesh_Statistic"
QT_MOC_LITERAL(1, 19, 9), // "_Clusters"
QT_MOC_LITERAL(2, 29, 18), // "_CacheIn_Out_Ratio"
QT_MOC_LITERAL(3, 48, 21), // "_OverdrawIn_Out_Ratio"
QT_MOC_LITERAL(4, 70, 24) // "_OverdrawMaxIn_Out_Ratio"

    },
    "C_3DMesh_Statistic\0_Clusters\0"
    "_CacheIn_Out_Ratio\0_OverdrawIn_Out_Ratio\0"
    "_OverdrawMaxIn_Out_Ratio"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_3DMesh_Statistic[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,
       4, QMetaType::QString, 0x00095003,

       0        // eod
};

void C_3DMesh_Statistic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_3DMesh_Statistic *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_Clusters; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_CacheInOutRatio; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_OverdrawInOutRatio; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->m_OverdrawMaxInOutRatio; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_3DMesh_Statistic *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_Clusters != *reinterpret_cast< QString*>(_v)) {
                _t->m_Clusters = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_CacheInOutRatio != *reinterpret_cast< QString*>(_v)) {
                _t->m_CacheInOutRatio = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_OverdrawInOutRatio != *reinterpret_cast< QString*>(_v)) {
                _t->m_OverdrawInOutRatio = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3:
            if (_t->m_OverdrawMaxInOutRatio != *reinterpret_cast< QString*>(_v)) {
                _t->m_OverdrawMaxInOutRatio = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_3DMesh_Statistic::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_3DMesh_Statistic.data,
    qt_meta_data_C_3DMesh_Statistic,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_3DMesh_Statistic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_3DMesh_Statistic::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_3DMesh_Statistic.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_3DMesh_Statistic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_3DSubModel_Info_t {
    QByteArrayData data[6];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_3DSubModel_Info_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_3DSubModel_Info_t qt_meta_stringdata_C_3DSubModel_Info = {
    {
QT_MOC_LITERAL(0, 0, 17), // "C_3DSubModel_Info"
QT_MOC_LITERAL(1, 18, 5), // "_Name"
QT_MOC_LITERAL(2, 24, 10), // "_Full_Path"
QT_MOC_LITERAL(3, 35, 10), // "_File_Size"
QT_MOC_LITERAL(4, 46, 10), // "_Generator"
QT_MOC_LITERAL(5, 57, 8) // "_Version"

    },
    "C_3DSubModel_Info\0_Name\0_Full_Path\0"
    "_File_Size\0_Generator\0_Version"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_3DSubModel_Info[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,
       4, QMetaType::QString, 0x00095003,
       5, QMetaType::QString, 0x00095003,

       0        // eod
};

void C_3DSubModel_Info::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_3DSubModel_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_Name; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_Full_Path; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_FileSizeStr; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->m_GeneratorStr; break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->m_VersionStr; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_3DSubModel_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_Name != *reinterpret_cast< QString*>(_v)) {
                _t->m_Name = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_Full_Path != *reinterpret_cast< QString*>(_v)) {
                _t->m_Full_Path = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_FileSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3:
            if (_t->m_GeneratorStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_GeneratorStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 4:
            if (_t->m_VersionStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_VersionStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_3DSubModel_Info::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_3DSubModel_Info.data,
    qt_meta_data_C_3DSubModel_Info,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_3DSubModel_Info::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_3DSubModel_Info::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_3DSubModel_Info.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_3DSubModel_Info::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_3DModel_Info_t {
    QByteArrayData data[6];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_3DModel_Info_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_3DModel_Info_t qt_meta_stringdata_C_3DModel_Info = {
    {
QT_MOC_LITERAL(0, 0, 14), // "C_3DModel_Info"
QT_MOC_LITERAL(1, 15, 5), // "_Name"
QT_MOC_LITERAL(2, 21, 10), // "_Full_Path"
QT_MOC_LITERAL(3, 32, 10), // "_File_Size"
QT_MOC_LITERAL(4, 43, 10), // "_Generator"
QT_MOC_LITERAL(5, 54, 8) // "_Version"

    },
    "C_3DModel_Info\0_Name\0_Full_Path\0"
    "_File_Size\0_Generator\0_Version"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_3DModel_Info[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095003,
       2, QMetaType::QString, 0x00095003,
       3, QMetaType::QString, 0x00095003,
       4, QMetaType::QString, 0x00095003,
       5, QMetaType::QString, 0x00095003,

       0        // eod
};

void C_3DModel_Info::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_3DModel_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->m_Name; break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->m_Full_Path; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->m_FileSizeStr; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->m_GeneratorStr; break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->m_VersionStr; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_3DModel_Info *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_Name != *reinterpret_cast< QString*>(_v)) {
                _t->m_Name = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->m_Full_Path != *reinterpret_cast< QString*>(_v)) {
                _t->m_Full_Path = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 2:
            if (_t->m_FileSizeStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_FileSizeStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3:
            if (_t->m_GeneratorStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_GeneratorStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 4:
            if (_t->m_VersionStr != *reinterpret_cast< QString*>(_v)) {
                _t->m_VersionStr = *reinterpret_cast< QString*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject C_3DModel_Info::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_3DModel_Info.data,
    qt_meta_data_C_3DModel_Info,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_3DModel_Info::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_3DModel_Info::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_3DModel_Info.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_3DModel_Info::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_Application_Options_t {
    QByteArrayData data[33];
    char stringdata0[602];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_Application_Options_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_Application_Options_t qt_meta_stringdata_C_Application_Options = {
    {
QT_MOC_LITERAL(0, 0, 21), // "C_Application_Options"
QT_MOC_LITERAL(1, 22, 22), // "ImageViewDecodeChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 9), // "QVariant&"
QT_MOC_LITERAL(4, 56, 17), // "LogResultsChanged"
QT_MOC_LITERAL(5, 74, 18), // "ImageEncodeChanged"
QT_MOC_LITERAL(6, 93, 11), // "Encode_with"
QT_MOC_LITERAL(7, 105, 15), // "ImageEncodeWith"
QT_MOC_LITERAL(8, 121, 27), // "Use_GPU_To_Generate_MipMaps"
QT_MOC_LITERAL(9, 149, 30), // "Use_SRGB_Frames_While_Encoding"
QT_MOC_LITERAL(10, 180, 11), // "Decode_with"
QT_MOC_LITERAL(11, 192, 15), // "ImageDecodeWith"
QT_MOC_LITERAL(12, 208, 31), // "Reload_image_views_on_selection"
QT_MOC_LITERAL(13, 240, 38), // "Close_all_image_views_prior_t..."
QT_MOC_LITERAL(14, 279, 33), // "Mouse_click_on_icon_to_view_i..."
QT_MOC_LITERAL(15, 313, 30), // "Load_recent_project_on_startup"
QT_MOC_LITERAL(16, 344, 23), // "Set_Image_Diff_Contrast"
QT_MOC_LITERAL(17, 368, 21), // "Set_Number_of_Threads"
QT_MOC_LITERAL(18, 390, 18), // "Render_Models_with"
QT_MOC_LITERAL(19, 409, 16), // "RenderModelsWith"
QT_MOC_LITERAL(20, 426, 26), // "Show_MSE_PSNR_SSIM_Results"
QT_MOC_LITERAL(21, 453, 27), // "Show_Analysis_Results_Table"
QT_MOC_LITERAL(22, 481, 23), // "Use_Original_File_Names"
QT_MOC_LITERAL(23, 505, 3), // "CPU"
QT_MOC_LITERAL(24, 509, 3), // "HPC"
QT_MOC_LITERAL(25, 513, 11), // "GPU_DirectX"
QT_MOC_LITERAL(26, 525, 10), // "GPU_OpenCL"
QT_MOC_LITERAL(27, 536, 6), // "GPU_HW"
QT_MOC_LITERAL(28, 543, 10), // "GPU_OpenGL"
QT_MOC_LITERAL(29, 554, 10), // "GPU_Vulkan"
QT_MOC_LITERAL(30, 565, 12), // "glTF_DX12_EX"
QT_MOC_LITERAL(31, 578, 11), // "glTF_Vulkan"
QT_MOC_LITERAL(32, 590, 11) // "glTF_OpenGL"

    },
    "C_Application_Options\0ImageViewDecodeChanged\0"
    "\0QVariant&\0LogResultsChanged\0"
    "ImageEncodeChanged\0Encode_with\0"
    "ImageEncodeWith\0Use_GPU_To_Generate_MipMaps\0"
    "Use_SRGB_Frames_While_Encoding\0"
    "Decode_with\0ImageDecodeWith\0"
    "Reload_image_views_on_selection\0"
    "Close_all_image_views_prior_to_process\0"
    "Mouse_click_on_icon_to_view_image\0"
    "Load_recent_project_on_startup\0"
    "Set_Image_Diff_Contrast\0Set_Number_of_Threads\0"
    "Render_Models_with\0RenderModelsWith\0"
    "Show_MSE_PSNR_SSIM_Results\0"
    "Show_Analysis_Results_Table\0"
    "Use_Original_File_Names\0CPU\0HPC\0"
    "GPU_DirectX\0GPU_OpenCL\0GPU_HW\0GPU_OpenGL\0"
    "GPU_Vulkan\0glTF_DX12_EX\0glTF_Vulkan\0"
    "glTF_OpenGL"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_Application_Options[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
      14,   38, // properties
       3,   94, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       5,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // properties: name, type, flags
       6, 0x80000000 | 7, 0x0049500b,
       8, QMetaType::Bool, 0x00095003,
       9, QMetaType::Bool, 0x00095003,
      10, 0x80000000 | 11, 0x0049500b,
      12, QMetaType::Bool, 0x00095003,
      13, QMetaType::Bool, 0x00095003,
      14, QMetaType::Bool, 0x00095003,
      15, QMetaType::Bool, 0x00095003,
      16, QMetaType::Double, 0x00095003,
      17, QMetaType::Int, 0x00095003,
      18, 0x80000000 | 19, 0x0009500b,
      20, QMetaType::Bool, 0x00495003,
      21, QMetaType::Bool, 0x00095003,
      22, QMetaType::Bool, 0x00095003,

 // properties: notify_signal_id
       2,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       1,
       0,
       0,

 // enums: name, alias, flags, count, data
       7,    7, 0x2,    5,  109,
      11,   11, 0x2,    4,  119,
      19,   19, 0x2,    3,  127,

 // enum data: key, value
      23, uint(C_Application_Options::ImageEncodeWith::CPU),
      24, uint(C_Application_Options::ImageEncodeWith::HPC),
      25, uint(C_Application_Options::ImageEncodeWith::GPU_DirectX),
      26, uint(C_Application_Options::ImageEncodeWith::GPU_OpenCL),
      27, uint(C_Application_Options::ImageEncodeWith::GPU_HW),
      23, uint(C_Application_Options::ImageDecodeWith::CPU),
      28, uint(C_Application_Options::ImageDecodeWith::GPU_OpenGL),
      25, uint(C_Application_Options::ImageDecodeWith::GPU_DirectX),
      29, uint(C_Application_Options::ImageDecodeWith::GPU_Vulkan),
      30, uint(C_Application_Options::RenderModelsWith::glTF_DX12_EX),
      31, uint(C_Application_Options::RenderModelsWith::glTF_Vulkan),
      32, uint(C_Application_Options::RenderModelsWith::glTF_OpenGL),

       0        // eod
};

void C_Application_Options::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C_Application_Options *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ImageViewDecodeChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 1: _t->LogResultsChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->ImageEncodeChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C_Application_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Application_Options::ImageViewDecodeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (C_Application_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Application_Options::LogResultsChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (C_Application_Options::*)(QVariant & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C_Application_Options::ImageEncodeChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_Application_Options *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ImageEncodeWith*>(_v) = _t->getImageEncode(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getUseGPUMipMaps(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->getUseSRGBFrames(); break;
        case 3: *reinterpret_cast< ImageDecodeWith*>(_v) = _t->getImageViewDecode(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->getUseNewImageViews(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->getCloseAllImageViews(); break;
        case 6: *reinterpret_cast< bool*>(_v) = _t->getclickIconToViewImage(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->getLoadRecentFile(); break;
        case 8: *reinterpret_cast< double*>(_v) = _t->getImagediffContrast(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->getThreads(); break;
        case 10: *reinterpret_cast< RenderModelsWith*>(_v) = _t->getGLTFRender(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->getLogResults(); break;
        case 12: *reinterpret_cast< bool*>(_v) = _t->getAnalysisResultTable(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->getUseOriginalFileNames(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_Application_Options *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setImageEncode(*reinterpret_cast< ImageEncodeWith*>(_v)); break;
        case 1: _t->setUseGPUMipMaps(*reinterpret_cast< bool*>(_v)); break;
        case 2: _t->setUseSRGBFrames(*reinterpret_cast< bool*>(_v)); break;
        case 3: _t->setImageViewDecode(*reinterpret_cast< ImageDecodeWith*>(_v)); break;
        case 4: _t->setUseNewImageViews(*reinterpret_cast< bool*>(_v)); break;
        case 5: _t->setCloseAllImageViews(*reinterpret_cast< bool*>(_v)); break;
        case 6: _t->setclickIconToViewImage(*reinterpret_cast< bool*>(_v)); break;
        case 7: _t->setLoadRecentFile(*reinterpret_cast< bool*>(_v)); break;
        case 8: _t->setImagediffContrast(*reinterpret_cast< double*>(_v)); break;
        case 9: _t->setThreads(*reinterpret_cast< int*>(_v)); break;
        case 10: _t->setGLTFRender(*reinterpret_cast< RenderModelsWith*>(_v)); break;
        case 11: _t->setLogResults(*reinterpret_cast< bool*>(_v)); break;
        case 12: _t->setAnalysisResultTable(*reinterpret_cast< bool*>(_v)); break;
        case 13: _t->setUseOriginalFileNames(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject C_Application_Options::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_Application_Options.data,
    qt_meta_data_C_Application_Options,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_Application_Options::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_Application_Options::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_Application_Options.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_Application_Options::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 14;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void C_Application_Options::ImageViewDecodeChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void C_Application_Options::LogResultsChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void C_Application_Options::ImageEncodeChanged(QVariant & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
