/****************************************************************************
** Meta object code from reading C++ file 'acdockwidgettitlebar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/acdockwidgettitlebar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'acdockwidgettitlebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_acDockWidgetTitlebar_t {
    QByteArrayData data[11];
    char stringdata0[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_acDockWidgetTitlebar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_acDockWidgetTitlebar_t qt_meta_stringdata_acDockWidgetTitlebar = {
    {
QT_MOC_LITERAL(0, 0, 20), // "acDockWidgetTitlebar"
QT_MOC_LITERAL(1, 21, 13), // "ToolBarCliked"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 14), // "OnAboutToClose"
QT_MOC_LITERAL(4, 51, 8), // "QString&"
QT_MOC_LITERAL(5, 60, 5), // "Title"
QT_MOC_LITERAL(6, 66, 16), // "OnToolBarClicked"
QT_MOC_LITERAL(7, 83, 10), // "onMaximize"
QT_MOC_LITERAL(8, 94, 8), // "onNormal"
QT_MOC_LITERAL(9, 103, 10), // "onMinimize"
QT_MOC_LITERAL(10, 114, 7) // "onClose"

    },
    "acDockWidgetTitlebar\0ToolBarCliked\0\0"
    "OnAboutToClose\0QString&\0Title\0"
    "OnToolBarClicked\0onMaximize\0onNormal\0"
    "onMinimize\0onClose"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_acDockWidgetTitlebar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    1,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   53,    2, 0x0a /* Public */,
       7,    0,   54,    2, 0x0a /* Public */,
       8,    0,   55,    2, 0x0a /* Public */,
       9,    0,   56,    2, 0x0a /* Public */,
      10,    0,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void acDockWidgetTitlebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<acDockWidgetTitlebar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ToolBarCliked(); break;
        case 1: _t->OnAboutToClose((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->OnToolBarClicked(); break;
        case 3: _t->onMaximize(); break;
        case 4: _t->onNormal(); break;
        case 5: _t->onMinimize(); break;
        case 6: _t->onClose(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (acDockWidgetTitlebar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acDockWidgetTitlebar::ToolBarCliked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (acDockWidgetTitlebar::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&acDockWidgetTitlebar::OnAboutToClose)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject acDockWidgetTitlebar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_acDockWidgetTitlebar.data,
    qt_meta_data_acDockWidgetTitlebar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *acDockWidgetTitlebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *acDockWidgetTitlebar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_acDockWidgetTitlebar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int acDockWidgetTitlebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void acDockWidgetTitlebar::ToolBarCliked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void acDockWidgetTitlebar::OnAboutToClose(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
