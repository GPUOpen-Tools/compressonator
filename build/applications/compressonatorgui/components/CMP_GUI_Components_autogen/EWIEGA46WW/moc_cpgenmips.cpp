/****************************************************************************
** Meta object code from reading C++ file 'cpgenmips.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpgenmips.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpgenmips.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CGenMips_t {
    QByteArrayData data[13];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CGenMips_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CGenMips_t qt_meta_stringdata_CGenMips = {
    {
QT_MOC_LITERAL(0, 0, 8), // "CGenMips"
QT_MOC_LITERAL(1, 9, 21), // "signalGenerateMipmaps"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 17), // "CMP_CFilterParams"
QT_MOC_LITERAL(4, 50, 15), // "m_CFilterParams"
QT_MOC_LITERAL(5, 66, 29), // "std::vector<QTreeWidgetItem*>"
QT_MOC_LITERAL(6, 96, 5), // "items"
QT_MOC_LITERAL(7, 102, 12), // "valueChanged"
QT_MOC_LITERAL(8, 115, 11), // "QtProperty*"
QT_MOC_LITERAL(9, 127, 8), // "property"
QT_MOC_LITERAL(10, 136, 5), // "value"
QT_MOC_LITERAL(11, 142, 8), // "onCancel"
QT_MOC_LITERAL(12, 151, 10) // "onGenerate"

    },
    "CGenMips\0signalGenerateMipmaps\0\0"
    "CMP_CFilterParams\0m_CFilterParams\0"
    "std::vector<QTreeWidgetItem*>\0items\0"
    "valueChanged\0QtProperty*\0property\0"
    "value\0onCancel\0onGenerate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CGenMips[] = {

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
       1,    2,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   39,    2, 0x0a /* Public */,
      11,    0,   44,    2, 0x0a /* Public */,
      12,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8, QMetaType::QVariant,    9,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CGenMips::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CGenMips *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signalGenerateMipmaps((*reinterpret_cast< CMP_CFilterParams(*)>(_a[1])),(*reinterpret_cast< const std::vector<QTreeWidgetItem*>(*)>(_a[2]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 2: _t->onCancel(); break;
        case 3: _t->onGenerate(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CGenMips::*)(CMP_CFilterParams , const std::vector<QTreeWidgetItem*> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CGenMips::signalGenerateMipmaps)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CGenMips::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CGenMips.data,
    qt_meta_data_CGenMips,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CGenMips::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CGenMips::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CGenMips.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CGenMips::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void CGenMips::signalGenerateMipmaps(CMP_CFilterParams _t1, const std::vector<QTreeWidgetItem*> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
