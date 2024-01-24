/****************************************************************************
** Meta object code from reading C++ file 'cpimagecompare.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpimagecompare.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpimagecompare.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CImageCompare_t {
    QByteArrayData data[8];
    char stringdata0[75];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CImageCompare_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CImageCompare_t qt_meta_stringdata_CImageCompare = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CImageCompare"
QT_MOC_LITERAL(1, 14, 10), // "UpdateData"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "data"
QT_MOC_LITERAL(4, 31, 12), // "changeLayout"
QT_MOC_LITERAL(5, 44, 11), // "resetLayout"
QT_MOC_LITERAL(6, 56, 7), // "runSSIM"
QT_MOC_LITERAL(7, 64, 10) // "runPsnrMse"

    },
    "CImageCompare\0UpdateData\0\0data\0"
    "changeLayout\0resetLayout\0runSSIM\0"
    "runPsnrMse"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CImageCompare[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x0a /* Public */,
       5,    0,   43,    2, 0x0a /* Public */,
       6,    0,   44,    2, 0x0a /* Public */,
       7,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QObjectStar,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CImageCompare::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CImageCompare *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->UpdateData((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: _t->changeLayout(); break;
        case 2: _t->resetLayout(); break;
        case 3: _t->runSSIM(); break;
        case 4: _t->runPsnrMse(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CImageCompare::*)(QObject * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CImageCompare::UpdateData)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CImageCompare::staticMetaObject = { {
    QMetaObject::SuperData::link<acCustomDockWidget::staticMetaObject>(),
    qt_meta_stringdata_CImageCompare.data,
    qt_meta_data_CImageCompare,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CImageCompare::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CImageCompare::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CImageCompare.stringdata0))
        return static_cast<void*>(this);
    return acCustomDockWidget::qt_metacast(_clname);
}

int CImageCompare::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = acCustomDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CImageCompare::UpdateData(QObject * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
