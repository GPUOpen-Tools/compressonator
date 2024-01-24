/****************************************************************************
** Meta object code from reading C++ file 'acprogressanimationwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/acprogressanimationwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'acprogressanimationwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_acProgressAnimationWidget_t {
    QByteArrayData data[8];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_acProgressAnimationWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_acProgressAnimationWidget_t qt_meta_stringdata_acProgressAnimationWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "acProgressAnimationWidget"
QT_MOC_LITERAL(1, 26, 14), // "StartAnimation"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 13), // "StopAnimation"
QT_MOC_LITERAL(4, 56, 18), // "IsAnimationStarted"
QT_MOC_LITERAL(5, 75, 9), // "Increment"
QT_MOC_LITERAL(6, 85, 5), // "value"
QT_MOC_LITERAL(7, 91, 16) // "SetProgressValue"

    },
    "acProgressAnimationWidget\0StartAnimation\0"
    "\0StopAnimation\0IsAnimationStarted\0"
    "Increment\0value\0SetProgressValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_acProgressAnimationWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x0a /* Public */,
       5,    1,   47,    2, 0x0a /* Public */,
       5,    0,   50,    2, 0x2a /* Public | MethodCloned */,
       7,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::UInt,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,    6,

       0        // eod
};

void acProgressAnimationWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<acProgressAnimationWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->StartAnimation(); break;
        case 1: _t->StopAnimation(); break;
        case 2: { bool _r = _t->IsAnimationStarted();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->Increment((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 4: _t->Increment(); break;
        case 5: _t->SetProgressValue((*reinterpret_cast< uint(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject acProgressAnimationWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_acProgressAnimationWidget.data,
    qt_meta_data_acProgressAnimationWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *acProgressAnimationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *acProgressAnimationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_acProgressAnimationWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int acProgressAnimationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
