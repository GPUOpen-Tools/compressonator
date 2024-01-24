/****************************************************************************
** Meta object code from reading C++ file 'cpimageanalysis.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpimageanalysis.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpimageanalysis.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_C_AnalysisData_t {
    QByteArrayData data[10];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_AnalysisData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_AnalysisData_t qt_meta_stringdata_C_AnalysisData = {
    {
QT_MOC_LITERAL(0, 0, 14), // "C_AnalysisData"
QT_MOC_LITERAL(1, 15, 4), // "_MSE"
QT_MOC_LITERAL(2, 20, 5), // "_SSIM"
QT_MOC_LITERAL(3, 26, 10), // "_SSIM_Blue"
QT_MOC_LITERAL(4, 37, 11), // "_SSIM_Green"
QT_MOC_LITERAL(5, 49, 9), // "_SSIM_Red"
QT_MOC_LITERAL(6, 59, 5), // "_PSNR"
QT_MOC_LITERAL(7, 65, 10), // "_PSNR_Blue"
QT_MOC_LITERAL(8, 76, 11), // "_PSNR_Green"
QT_MOC_LITERAL(9, 88, 9) // "_PSNR_Red"

    },
    "C_AnalysisData\0_MSE\0_SSIM\0_SSIM_Blue\0"
    "_SSIM_Green\0_SSIM_Red\0_PSNR\0_PSNR_Blue\0"
    "_PSNR_Green\0_PSNR_Red"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_AnalysisData[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       9,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,
       6, QMetaType::Double, 0x00095003,
       7, QMetaType::Double, 0x00095003,
       8, QMetaType::Double, 0x00095003,
       9, QMetaType::Double, 0x00095003,

       0        // eod
};

void C_AnalysisData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_AnalysisData *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->m_MSE; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->m_SSIM; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Blue; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Green; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Red; break;
        case 5: *reinterpret_cast< double*>(_v) = _t->m_PSNR; break;
        case 6: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Blue; break;
        case 7: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Green; break;
        case 8: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Red; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_AnalysisData *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_MSE != *reinterpret_cast< double*>(_v)) {
                _t->m_MSE = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->m_SSIM != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->m_SSIM_Blue != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Blue = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->m_SSIM_Green != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Green = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->m_SSIM_Red != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Red = *reinterpret_cast< double*>(_v);
            }
            break;
        case 5:
            if (_t->m_PSNR != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR = *reinterpret_cast< double*>(_v);
            }
            break;
        case 6:
            if (_t->m_PSNR_Blue != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Blue = *reinterpret_cast< double*>(_v);
            }
            break;
        case 7:
            if (_t->m_PSNR_Green != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Green = *reinterpret_cast< double*>(_v);
            }
            break;
        case 8:
            if (_t->m_PSNR_Red != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Red = *reinterpret_cast< double*>(_v);
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

QT_INIT_METAOBJECT const QMetaObject C_AnalysisData::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_AnalysisData.data,
    qt_meta_data_C_AnalysisData,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_AnalysisData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_AnalysisData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_AnalysisData.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_AnalysisData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_C_SSIM_Analysis_t {
    QByteArrayData data[5];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_SSIM_Analysis_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_SSIM_Analysis_t qt_meta_stringdata_C_SSIM_Analysis = {
    {
QT_MOC_LITERAL(0, 0, 15), // "C_SSIM_Analysis"
QT_MOC_LITERAL(1, 16, 5), // "_SSIM"
QT_MOC_LITERAL(2, 22, 10), // "_SSIM_Blue"
QT_MOC_LITERAL(3, 33, 11), // "_SSIM_Green"
QT_MOC_LITERAL(4, 45, 9) // "_SSIM_Red"

    },
    "C_SSIM_Analysis\0_SSIM\0_SSIM_Blue\0"
    "_SSIM_Green\0_SSIM_Red"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_SSIM_Analysis[] = {

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
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,

       0        // eod
};

void C_SSIM_Analysis::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_SSIM_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->m_SSIM; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Blue; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Green; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->m_SSIM_Red; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_SSIM_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_SSIM != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->m_SSIM_Blue != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Blue = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->m_SSIM_Green != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Green = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->m_SSIM_Red != *reinterpret_cast< double*>(_v)) {
                _t->m_SSIM_Red = *reinterpret_cast< double*>(_v);
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

QT_INIT_METAOBJECT const QMetaObject C_SSIM_Analysis::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_SSIM_Analysis.data,
    qt_meta_data_C_SSIM_Analysis,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_SSIM_Analysis::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_SSIM_Analysis::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_SSIM_Analysis.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_SSIM_Analysis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_C_MSE_PSNR_Analysis_t {
    QByteArrayData data[6];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_MSE_PSNR_Analysis_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_MSE_PSNR_Analysis_t qt_meta_stringdata_C_MSE_PSNR_Analysis = {
    {
QT_MOC_LITERAL(0, 0, 19), // "C_MSE_PSNR_Analysis"
QT_MOC_LITERAL(1, 20, 4), // "_MSE"
QT_MOC_LITERAL(2, 25, 5), // "_PSNR"
QT_MOC_LITERAL(3, 31, 10), // "_PSNR_Blue"
QT_MOC_LITERAL(4, 42, 11), // "_PSNR_Green"
QT_MOC_LITERAL(5, 54, 9) // "_PSNR_Red"

    },
    "C_MSE_PSNR_Analysis\0_MSE\0_PSNR\0"
    "_PSNR_Blue\0_PSNR_Green\0_PSNR_Red"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_MSE_PSNR_Analysis[] = {

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
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,

       0        // eod
};

void C_MSE_PSNR_Analysis::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_MSE_PSNR_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->m_MSE; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->m_PSNR; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Blue; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Green; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Red; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_MSE_PSNR_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_MSE != *reinterpret_cast< double*>(_v)) {
                _t->m_MSE = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->m_PSNR != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->m_PSNR_Blue != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Blue = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->m_PSNR_Green != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Green = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->m_PSNR_Red != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Red = *reinterpret_cast< double*>(_v);
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

QT_INIT_METAOBJECT const QMetaObject C_MSE_PSNR_Analysis::staticMetaObject = { {
    QMetaObject::SuperData::link<C_SSIM_Analysis::staticMetaObject>(),
    qt_meta_stringdata_C_MSE_PSNR_Analysis.data,
    qt_meta_data_C_MSE_PSNR_Analysis,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_MSE_PSNR_Analysis::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_MSE_PSNR_Analysis::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_MSE_PSNR_Analysis.stringdata0))
        return static_cast<void*>(this);
    return C_SSIM_Analysis::qt_metacast(_clname);
}

int C_MSE_PSNR_Analysis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C_SSIM_Analysis::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_C_PSNR_MSE_Analysis_t {
    QByteArrayData data[6];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C_PSNR_MSE_Analysis_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C_PSNR_MSE_Analysis_t qt_meta_stringdata_C_PSNR_MSE_Analysis = {
    {
QT_MOC_LITERAL(0, 0, 19), // "C_PSNR_MSE_Analysis"
QT_MOC_LITERAL(1, 20, 4), // "_MSE"
QT_MOC_LITERAL(2, 25, 5), // "_PSNR"
QT_MOC_LITERAL(3, 31, 10), // "_PSNR_Blue"
QT_MOC_LITERAL(4, 42, 11), // "_PSNR_Green"
QT_MOC_LITERAL(5, 54, 9) // "_PSNR_Red"

    },
    "C_PSNR_MSE_Analysis\0_MSE\0_PSNR\0"
    "_PSNR_Blue\0_PSNR_Green\0_PSNR_Red"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C_PSNR_MSE_Analysis[] = {

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
       1, QMetaType::Double, 0x00095003,
       2, QMetaType::Double, 0x00095003,
       3, QMetaType::Double, 0x00095003,
       4, QMetaType::Double, 0x00095003,
       5, QMetaType::Double, 0x00095003,

       0        // eod
};

void C_PSNR_MSE_Analysis::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<C_PSNR_MSE_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->m_MSE; break;
        case 1: *reinterpret_cast< double*>(_v) = _t->m_PSNR; break;
        case 2: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Blue; break;
        case 3: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Green; break;
        case 4: *reinterpret_cast< double*>(_v) = _t->m_PSNR_Red; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<C_PSNR_MSE_Analysis *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_MSE != *reinterpret_cast< double*>(_v)) {
                _t->m_MSE = *reinterpret_cast< double*>(_v);
            }
            break;
        case 1:
            if (_t->m_PSNR != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR = *reinterpret_cast< double*>(_v);
            }
            break;
        case 2:
            if (_t->m_PSNR_Blue != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Blue = *reinterpret_cast< double*>(_v);
            }
            break;
        case 3:
            if (_t->m_PSNR_Green != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Green = *reinterpret_cast< double*>(_v);
            }
            break;
        case 4:
            if (_t->m_PSNR_Red != *reinterpret_cast< double*>(_v)) {
                _t->m_PSNR_Red = *reinterpret_cast< double*>(_v);
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

QT_INIT_METAOBJECT const QMetaObject C_PSNR_MSE_Analysis::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_C_PSNR_MSE_Analysis.data,
    qt_meta_data_C_PSNR_MSE_Analysis,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C_PSNR_MSE_Analysis::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C_PSNR_MSE_Analysis::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C_PSNR_MSE_Analysis.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int C_PSNR_MSE_Analysis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
