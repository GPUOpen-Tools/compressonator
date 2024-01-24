/****************************************************************************
** Meta object code from reading C++ file 'accustomgraphics.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/accustomgraphics.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'accustomgraphics.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_acCustomGraphicsScene_t {
    QByteArrayData data[6];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_acCustomGraphicsScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_acCustomGraphicsScene_t qt_meta_stringdata_acCustomGraphicsScene = {
    {
QT_MOC_LITERAL(0, 0, 21), // "acCustomGraphicsScene"
QT_MOC_LITERAL(1, 22, 18), // "sceneMousePosition"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 8), // "QPointF*"
QT_MOC_LITERAL(4, 51, 3), // "pos"
QT_MOC_LITERAL(5, 55, 2) // "ID"

    },
    "acCustomGraphicsScene\0sceneMousePosition\0"
    "\0QPointF*\0pos\0ID"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_acCustomGraphicsScene[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,

       0        // eod
};

void acCustomGraphicsScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<acCustomGraphicsScene *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sceneMousePosition((*reinterpret_cast< QPointF*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (acCustomGraphicsScene::*)(QPointF * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsScene::sceneMousePosition)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject acCustomGraphicsScene::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_acCustomGraphicsScene.data,
    qt_meta_data_acCustomGraphicsScene,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *acCustomGraphicsScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *acCustomGraphicsScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_acCustomGraphicsScene.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsScene::qt_metacast(_clname);
}

int acCustomGraphicsScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void acCustomGraphicsScene::sceneMousePosition(QPointF * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_acVirtualMouseHub_t {
    QByteArrayData data[13];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_acVirtualMouseHub_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_acVirtualMouseHub_t qt_meta_stringdata_acVirtualMouseHub = {
    {
QT_MOC_LITERAL(0, 0, 17), // "acVirtualMouseHub"
QT_MOC_LITERAL(1, 18, 20), // "VirtialMousePosition"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 8), // "QPointF*"
QT_MOC_LITERAL(4, 49, 8), // "scenePos"
QT_MOC_LITERAL(5, 58, 8), // "localPos"
QT_MOC_LITERAL(6, 67, 2), // "ID"
QT_MOC_LITERAL(7, 70, 23), // "VirtualSignalWheelEvent"
QT_MOC_LITERAL(8, 94, 12), // "QWheelEvent*"
QT_MOC_LITERAL(9, 107, 8), // "theEvent"
QT_MOC_LITERAL(10, 116, 23), // "onVirtualMouseMoveEvent"
QT_MOC_LITERAL(11, 140, 4), // "onID"
QT_MOC_LITERAL(12, 145, 25) // "onVirtualSignalWheelEvent"

    },
    "acVirtualMouseHub\0VirtialMousePosition\0"
    "\0QPointF*\0scenePos\0localPos\0ID\0"
    "VirtualSignalWheelEvent\0QWheelEvent*\0"
    "theEvent\0onVirtualMouseMoveEvent\0onID\0"
    "onVirtualSignalWheelEvent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_acVirtualMouseHub[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06 /* Public */,
       7,    2,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    3,   46,    2, 0x0a /* Public */,
      12,    2,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, QMetaType::Int,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, QMetaType::Int,    4,    5,   11,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,    6,

       0        // eod
};

void acVirtualMouseHub::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<acVirtualMouseHub *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->VirtialMousePosition((*reinterpret_cast< QPointF*(*)>(_a[1])),(*reinterpret_cast< QPointF*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->VirtualSignalWheelEvent((*reinterpret_cast< QWheelEvent*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->onVirtualMouseMoveEvent((*reinterpret_cast< QPointF*(*)>(_a[1])),(*reinterpret_cast< QPointF*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->onVirtualSignalWheelEvent((*reinterpret_cast< QWheelEvent*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (acVirtualMouseHub::*)(QPointF * , QPointF * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acVirtualMouseHub::VirtialMousePosition)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (acVirtualMouseHub::*)(QWheelEvent * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acVirtualMouseHub::VirtualSignalWheelEvent)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject acVirtualMouseHub::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_acVirtualMouseHub.data,
    qt_meta_data_acVirtualMouseHub,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *acVirtualMouseHub::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *acVirtualMouseHub::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_acVirtualMouseHub.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int acVirtualMouseHub::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void acVirtualMouseHub::VirtialMousePosition(QPointF * _t1, QPointF * _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void acVirtualMouseHub::VirtualSignalWheelEvent(QWheelEvent * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_acCustomGraphicsView_t {
    QByteArrayData data[14];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_acCustomGraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_acCustomGraphicsView_t qt_meta_stringdata_acCustomGraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 20), // "acCustomGraphicsView"
QT_MOC_LITERAL(1, 21, 14), // "resetImageView"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11), // "ResizeEvent"
QT_MOC_LITERAL(4, 49, 13), // "QResizeEvent*"
QT_MOC_LITERAL(5, 63, 5), // "event"
QT_MOC_LITERAL(6, 69, 16), // "signalWheelEvent"
QT_MOC_LITERAL(7, 86, 12), // "QWheelEvent*"
QT_MOC_LITERAL(8, 99, 15), // "OnMouseHandDown"
QT_MOC_LITERAL(9, 115, 12), // "OnMouseHandD"
QT_MOC_LITERAL(10, 128, 14), // "OnWheelScaleUp"
QT_MOC_LITERAL(11, 143, 8), // "QPointF&"
QT_MOC_LITERAL(12, 152, 3), // "pos"
QT_MOC_LITERAL(13, 156, 16) // "OnWheelScaleDown"

    },
    "acCustomGraphicsView\0resetImageView\0"
    "\0ResizeEvent\0QResizeEvent*\0event\0"
    "signalWheelEvent\0QWheelEvent*\0"
    "OnMouseHandDown\0OnMouseHandD\0"
    "OnWheelScaleUp\0QPointF&\0pos\0"
    "OnWheelScaleDown"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_acCustomGraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    1,   50,    2, 0x06 /* Public */,
       6,    1,   53,    2, 0x06 /* Public */,
       8,    0,   56,    2, 0x06 /* Public */,
       9,    0,   57,    2, 0x06 /* Public */,
      10,    1,   58,    2, 0x06 /* Public */,
      13,    1,   61,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void acCustomGraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<acCustomGraphicsView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->resetImageView(); break;
        case 1: _t->ResizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 2: _t->signalWheelEvent((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 3: _t->OnMouseHandDown(); break;
        case 4: _t->OnMouseHandD(); break;
        case 5: _t->OnWheelScaleUp((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 6: _t->OnWheelScaleDown((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (acCustomGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::resetImageView)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)(QResizeEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::ResizeEvent)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)(QWheelEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::signalWheelEvent)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::OnMouseHandDown)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::OnMouseHandD)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)(QPointF & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::OnWheelScaleUp)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (acCustomGraphicsView::*)(QPointF & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acCustomGraphicsView::OnWheelScaleDown)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject acCustomGraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_acCustomGraphicsView.data,
    qt_meta_data_acCustomGraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *acCustomGraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *acCustomGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_acCustomGraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int acCustomGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void acCustomGraphicsView::resetImageView()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void acCustomGraphicsView::ResizeEvent(QResizeEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void acCustomGraphicsView::signalWheelEvent(QWheelEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void acCustomGraphicsView::OnMouseHandDown()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void acCustomGraphicsView::OnMouseHandD()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void acCustomGraphicsView::OnWheelScaleUp(QPointF & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void acCustomGraphicsView::OnWheelScaleDown(QPointF & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
