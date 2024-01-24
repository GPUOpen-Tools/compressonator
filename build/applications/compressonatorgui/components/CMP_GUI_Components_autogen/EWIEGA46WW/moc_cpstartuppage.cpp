/****************************************************************************
** Meta object code from reading C++ file 'cpstartuppage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpstartuppage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpstartuppage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_cpStartupPage_t {
    QByteArrayData data[21];
    char stringdata0[341];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_cpStartupPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_cpStartupPage_t qt_meta_stringdata_cpStartupPage = {
    {
QT_MOC_LITERAL(0, 0, 13), // "cpStartupPage"
QT_MOC_LITERAL(1, 14, 15), // "PageButtonClick"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "QString&"
QT_MOC_LITERAL(4, 40, 7), // "Request"
QT_MOC_LITERAL(5, 48, 3), // "Msg"
QT_MOC_LITERAL(6, 52, 9), // "setSource"
QT_MOC_LITERAL(7, 62, 4), // "name"
QT_MOC_LITERAL(8, 67, 17), // "onPageButtonClick"
QT_MOC_LITERAL(9, 85, 18), // "onPButtonUserGuide"
QT_MOC_LITERAL(10, 104, 23), // "onPButtonGettingStarted"
QT_MOC_LITERAL(11, 128, 26), // "onPButtonCompressonatorWeb"
QT_MOC_LITERAL(12, 155, 24), // "onProjectlistviewClicked"
QT_MOC_LITERAL(13, 180, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(14, 197, 4), // "item"
QT_MOC_LITERAL(15, 202, 27), // "onRecentProjectsviewClicked"
QT_MOC_LITERAL(16, 230, 24), // "onNewFeaturesviewClicked"
QT_MOC_LITERAL(17, 255, 32), // "BuildRecentlyOpenedProjectsTable"
QT_MOC_LITERAL(18, 288, 8), // "htmlText"
QT_MOC_LITERAL(19, 297, 17), // "QVector<QString>&"
QT_MOC_LITERAL(20, 315, 25) // "recentlyUsedProjectsNames"

    },
    "cpStartupPage\0PageButtonClick\0\0QString&\0"
    "Request\0Msg\0setSource\0name\0onPageButtonClick\0"
    "onPButtonUserGuide\0onPButtonGettingStarted\0"
    "onPButtonCompressonatorWeb\0"
    "onProjectlistviewClicked\0QListWidgetItem*\0"
    "item\0onRecentProjectsviewClicked\0"
    "onNewFeaturesviewClicked\0"
    "BuildRecentlyOpenedProjectsTable\0"
    "htmlText\0QVector<QString>&\0"
    "recentlyUsedProjectsNames"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cpStartupPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   69,    2, 0x0a /* Public */,
       8,    2,   72,    2, 0x0a /* Public */,
       9,    0,   77,    2, 0x0a /* Public */,
      10,    0,   78,    2, 0x0a /* Public */,
      11,    0,   79,    2, 0x0a /* Public */,
      12,    1,   80,    2, 0x0a /* Public */,
      15,    1,   83,    2, 0x0a /* Public */,
      16,    1,   86,    2, 0x0a /* Public */,
      17,    2,   89,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    7,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Bool, 0x80000000 | 3, 0x80000000 | 19,   18,   20,

       0        // eod
};

void cpStartupPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<cpStartupPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->PageButtonClick((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->setSource((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 2: _t->onPageButtonClick((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->onPButtonUserGuide(); break;
        case 4: _t->onPButtonGettingStarted(); break;
        case 5: _t->onPButtonCompressonatorWeb(); break;
        case 6: _t->onProjectlistviewClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->onRecentProjectsviewClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->onNewFeaturesviewClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 9: { bool _r = _t->BuildRecentlyOpenedProjectsTable((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVector<QString>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (cpStartupPage::*)(QString & , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cpStartupPage::PageButtonClick)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject cpStartupPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_cpStartupPage.data,
    qt_meta_data_cpStartupPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *cpStartupPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cpStartupPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_cpStartupPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int cpStartupPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void cpStartupPage::PageButtonClick(QString & _t1, QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
