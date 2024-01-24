/****************************************************************************
** Meta object code from reading C++ file 'cptreewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/common/cptreewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cptreewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_cpTreeWidget_t {
    QByteArrayData data[13];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_cpTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_cpTreeWidget_t qt_meta_stringdata_cpTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "cpTreeWidget"
QT_MOC_LITERAL(1, 13, 16), // "DroppedImageItem"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "QString&"
QT_MOC_LITERAL(4, 40, 12), // "filePathName"
QT_MOC_LITERAL(5, 53, 5), // "index"
QT_MOC_LITERAL(6, 59, 16), // "event_mousePress"
QT_MOC_LITERAL(7, 76, 12), // "QMouseEvent*"
QT_MOC_LITERAL(8, 89, 5), // "event"
QT_MOC_LITERAL(9, 95, 6), // "onIcon"
QT_MOC_LITERAL(10, 102, 14), // "event_keyPress"
QT_MOC_LITERAL(11, 117, 10), // "QKeyEvent*"
QT_MOC_LITERAL(12, 128, 10) // "selChanged"

    },
    "cpTreeWidget\0DroppedImageItem\0\0QString&\0"
    "filePathName\0index\0event_mousePress\0"
    "QMouseEvent*\0event\0onIcon\0event_keyPress\0"
    "QKeyEvent*\0selChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cpTreeWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       6,    2,   39,    2, 0x06 /* Public */,
      10,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Bool,    8,    9,
    QMetaType::Void, 0x80000000 | 11,    8,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void cpTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<cpTreeWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DroppedImageItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->event_mousePress((*reinterpret_cast< QMouseEvent*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->event_keyPress((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 3: _t->selChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (cpTreeWidget::*)(QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cpTreeWidget::DroppedImageItem)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (cpTreeWidget::*)(QMouseEvent * , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cpTreeWidget::event_mousePress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (cpTreeWidget::*)(QKeyEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cpTreeWidget::event_keyPress)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject cpTreeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTreeWidget::staticMetaObject>(),
    qt_meta_stringdata_cpTreeWidget.data,
    qt_meta_data_cpTreeWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *cpTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cpTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_cpTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int cpTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
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
void cpTreeWidget::DroppedImageItem(QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void cpTreeWidget::event_mousePress(QMouseEvent * _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void cpTreeWidget::event_keyPress(QKeyEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
