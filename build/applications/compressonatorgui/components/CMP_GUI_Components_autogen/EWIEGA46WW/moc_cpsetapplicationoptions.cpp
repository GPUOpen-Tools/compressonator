/****************************************************************************
** Meta object code from reading C++ file 'cpsetapplicationoptions.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpsetapplicationoptions.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpsetapplicationoptions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CSetApplicationOptions_t {
    QByteArrayData data[12];
    char stringdata0[172];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSetApplicationOptions_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSetApplicationOptions_t qt_meta_stringdata_CSetApplicationOptions = {
    {
QT_MOC_LITERAL(0, 0, 22), // "CSetApplicationOptions"
QT_MOC_LITERAL(1, 23, 16), // "OnAppSettingHide"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 7), // "onClose"
QT_MOC_LITERAL(4, 49, 20), // "oncurrentItemChanged"
QT_MOC_LITERAL(5, 70, 14), // "QtBrowserItem*"
QT_MOC_LITERAL(6, 85, 4), // "item"
QT_MOC_LITERAL(7, 90, 20), // "onImageEncodeChanged"
QT_MOC_LITERAL(8, 111, 9), // "QVariant&"
QT_MOC_LITERAL(9, 121, 5), // "value"
QT_MOC_LITERAL(10, 127, 24), // "onImageViewDecodeChanged"
QT_MOC_LITERAL(11, 152, 19) // "onLogResultsChanged"

    },
    "CSetApplicationOptions\0OnAppSettingHide\0"
    "\0onClose\0oncurrentItemChanged\0"
    "QtBrowserItem*\0item\0onImageEncodeChanged\0"
    "QVariant&\0value\0onImageViewDecodeChanged\0"
    "onLogResultsChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSetApplicationOptions[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,
      10,    1,   52,    2, 0x0a /* Public */,
      11,    1,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void CSetApplicationOptions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CSetApplicationOptions *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->OnAppSettingHide(); break;
        case 1: _t->onClose(); break;
        case 2: _t->oncurrentItemChanged((*reinterpret_cast< QtBrowserItem*(*)>(_a[1]))); break;
        case 3: _t->onImageEncodeChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 4: _t->onImageViewDecodeChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 5: _t->onLogResultsChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CSetApplicationOptions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CSetApplicationOptions::OnAppSettingHide)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CSetApplicationOptions::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CSetApplicationOptions.data,
    qt_meta_data_CSetApplicationOptions,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CSetApplicationOptions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSetApplicationOptions::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CSetApplicationOptions.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CSetApplicationOptions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void CSetApplicationOptions::OnAppSettingHide()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
