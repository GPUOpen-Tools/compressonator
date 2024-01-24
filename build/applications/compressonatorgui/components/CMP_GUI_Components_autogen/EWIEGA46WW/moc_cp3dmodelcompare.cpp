/****************************************************************************
** Meta object code from reading C++ file 'cp3dmodelcompare.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cp3dmodelcompare.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cp3dmodelcompare.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_C3DModelCompare_t {
    QByteArrayData data[8];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_C3DModelCompare_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_C3DModelCompare_t qt_meta_stringdata_C3DModelCompare = {
    {
QT_MOC_LITERAL(0, 0, 15), // "C3DModelCompare"
QT_MOC_LITERAL(1, 16, 10), // "UpdateData"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "data"
QT_MOC_LITERAL(4, 33, 12), // "changeLayout"
QT_MOC_LITERAL(5, 46, 11), // "resetLayout"
QT_MOC_LITERAL(6, 58, 19), // "onManual_renderView"
QT_MOC_LITERAL(7, 78, 4) // "mode"

    },
    "C3DModelCompare\0UpdateData\0\0data\0"
    "changeLayout\0resetLayout\0onManual_renderView\0"
    "mode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_C3DModelCompare[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    0,   38,    2, 0x0a /* Public */,
       6,    1,   39,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QObjectStar,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void C3DModelCompare::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<C3DModelCompare *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->UpdateData((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: _t->changeLayout(); break;
        case 2: _t->resetLayout(); break;
        case 3: _t->onManual_renderView((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (C3DModelCompare::*)(QObject * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&C3DModelCompare::UpdateData)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject C3DModelCompare::staticMetaObject = { {
    QMetaObject::SuperData::link<acCustomDockWidget::staticMetaObject>(),
    qt_meta_stringdata_C3DModelCompare.data,
    qt_meta_data_C3DModelCompare,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *C3DModelCompare::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *C3DModelCompare::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_C3DModelCompare.stringdata0))
        return static_cast<void*>(this);
    return acCustomDockWidget::qt_metacast(_clname);
}

int C3DModelCompare::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = acCustomDockWidget::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void C3DModelCompare::UpdateData(QObject * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
