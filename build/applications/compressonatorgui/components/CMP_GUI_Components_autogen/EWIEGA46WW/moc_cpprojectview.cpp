/****************************************************************************
** Meta object code from reading C++ file 'cpprojectview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../applications/compressonatorgui/components/cpprojectview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cpprojectview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_signalProcessMsgHandler_t {
    QByteArrayData data[3];
    char stringdata0[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_signalProcessMsgHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_signalProcessMsgHandler_t qt_meta_stringdata_signalProcessMsgHandler = {
    {
QT_MOC_LITERAL(0, 0, 23), // "signalProcessMsgHandler"
QT_MOC_LITERAL(1, 24, 20), // "signalProcessMessage"
QT_MOC_LITERAL(2, 45, 0) // ""

    },
    "signalProcessMsgHandler\0signalProcessMessage\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_signalProcessMsgHandler[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void signalProcessMsgHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<signalProcessMsgHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signalProcessMessage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (signalProcessMsgHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&signalProcessMsgHandler::signalProcessMessage)) {
                *result = 0;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject signalProcessMsgHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_signalProcessMsgHandler.data,
    qt_meta_data_signalProcessMsgHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *signalProcessMsgHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *signalProcessMsgHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_signalProcessMsgHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int signalProcessMsgHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void signalProcessMsgHandler::signalProcessMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_ProjectView_t {
    QByteArrayData data[77];
    char stringdata0[1149];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProjectView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProjectView_t qt_meta_stringdata_ProjectView = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ProjectView"
QT_MOC_LITERAL(1, 12, 14), // "AddedImageFile"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 8), // "QString&"
QT_MOC_LITERAL(4, 37, 8), // "fileName"
QT_MOC_LITERAL(5, 46, 13), // "ViewImageFile"
QT_MOC_LITERAL(6, 60, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(7, 77, 4), // "item"
QT_MOC_LITERAL(8, 82, 15), // "DeleteImageView"
QT_MOC_LITERAL(9, 98, 17), // "ViewImageFileDiff"
QT_MOC_LITERAL(10, 116, 22), // "C_Destination_Options*"
QT_MOC_LITERAL(11, 139, 6), // "m_data"
QT_MOC_LITERAL(12, 146, 5), // "file1"
QT_MOC_LITERAL(13, 152, 5), // "file2"
QT_MOC_LITERAL(14, 158, 19), // "View3DModelFileDiff"
QT_MOC_LITERAL(15, 178, 18), // "C_3DSubModel_Info*"
QT_MOC_LITERAL(16, 197, 19), // "DeleteImageViewDiff"
QT_MOC_LITERAL(17, 217, 19), // "AddCompressSettings"
QT_MOC_LITERAL(18, 237, 20), // "EditCompressSettings"
QT_MOC_LITERAL(19, 258, 10), // "UpdateData"
QT_MOC_LITERAL(20, 269, 4), // "data"
QT_MOC_LITERAL(21, 274, 17), // "OnCompressionDone"
QT_MOC_LITERAL(22, 292, 12), // "OnProcessing"
QT_MOC_LITERAL(23, 305, 12), // "FilePathName"
QT_MOC_LITERAL(24, 318, 18), // "OnCompressionStart"
QT_MOC_LITERAL(25, 337, 13), // "OnSourceImage"
QT_MOC_LITERAL(26, 351, 10), // "ChildCount"
QT_MOC_LITERAL(27, 362, 17), // "OnDecompressImage"
QT_MOC_LITERAL(28, 380, 15), // "OnProjectLoaded"
QT_MOC_LITERAL(29, 396, 26), // "OnAddedCompressSettingNode"
QT_MOC_LITERAL(30, 423, 22), // "OnAddedImageSourceNode"
QT_MOC_LITERAL(31, 446, 19), // "OnSetToolBarActions"
QT_MOC_LITERAL(32, 466, 11), // "islevelType"
QT_MOC_LITERAL(33, 478, 24), // "onTree_ItemDoubleClicked"
QT_MOC_LITERAL(34, 503, 6), // "column"
QT_MOC_LITERAL(35, 510, 18), // "onTree_ItemClicked"
QT_MOC_LITERAL(36, 529, 18), // "onDroppedImageItem"
QT_MOC_LITERAL(37, 548, 12), // "filePathName"
QT_MOC_LITERAL(38, 561, 5), // "index"
QT_MOC_LITERAL(39, 567, 16), // "onSetCurrentItem"
QT_MOC_LITERAL(40, 584, 9), // "onEntered"
QT_MOC_LITERAL(41, 594, 11), // "QModelIndex"
QT_MOC_LITERAL(42, 606, 15), // "OnGlobalMessage"
QT_MOC_LITERAL(43, 622, 11), // "const char*"
QT_MOC_LITERAL(44, 634, 3), // "msg"
QT_MOC_LITERAL(45, 638, 15), // "onSetNewProject"
QT_MOC_LITERAL(46, 654, 24), // "onContextMenuCompression"
QT_MOC_LITERAL(47, 679, 20), // "onMenuBarCompression"
QT_MOC_LITERAL(48, 700, 18), // "OnCloseCompression"
QT_MOC_LITERAL(49, 719, 22), // "onSignalProcessMessage"
QT_MOC_LITERAL(50, 742, 23), // "onGlobalPropertyChanged"
QT_MOC_LITERAL(51, 766, 4), // "int&"
QT_MOC_LITERAL(52, 771, 15), // "saveProjectFile"
QT_MOC_LITERAL(53, 787, 17), // "saveAsProjectFile"
QT_MOC_LITERAL(54, 805, 15), // "saveToBatchFile"
QT_MOC_LITERAL(55, 821, 15), // "openProjectFile"
QT_MOC_LITERAL(56, 837, 18), // "openNewProjectFile"
QT_MOC_LITERAL(57, 856, 20), // "compressProjectFiles"
QT_MOC_LITERAL(58, 877, 6), // "QFile*"
QT_MOC_LITERAL(59, 884, 4), // "file"
QT_MOC_LITERAL(60, 889, 13), // "viewImageDiff"
QT_MOC_LITERAL(61, 903, 15), // "analyseMeshData"
QT_MOC_LITERAL(62, 919, 22), // "viewDiffImageFromChild"
QT_MOC_LITERAL(63, 942, 24), // "viewDiff3DModelFromChild"
QT_MOC_LITERAL(64, 967, 19), // "removeSelectedImage"
QT_MOC_LITERAL(65, 987, 20), // "openContainingFolder"
QT_MOC_LITERAL(66, 1008, 12), // "copyFullPath"
QT_MOC_LITERAL(67, 1021, 19), // "onCustomContextMenu"
QT_MOC_LITERAL(68, 1041, 5), // "point"
QT_MOC_LITERAL(69, 1047, 16), // "onTreeMousePress"
QT_MOC_LITERAL(70, 1064, 12), // "QMouseEvent*"
QT_MOC_LITERAL(71, 1077, 5), // "event"
QT_MOC_LITERAL(72, 1083, 6), // "onIcon"
QT_MOC_LITERAL(73, 1090, 14), // "onTreeKeyPress"
QT_MOC_LITERAL(74, 1105, 10), // "QKeyEvent*"
QT_MOC_LITERAL(75, 1116, 16), // "onImageLoadStart"
QT_MOC_LITERAL(76, 1133, 15) // "onImageLoadDone"

    },
    "ProjectView\0AddedImageFile\0\0QString&\0"
    "fileName\0ViewImageFile\0QTreeWidgetItem*\0"
    "item\0DeleteImageView\0ViewImageFileDiff\0"
    "C_Destination_Options*\0m_data\0file1\0"
    "file2\0View3DModelFileDiff\0C_3DSubModel_Info*\0"
    "DeleteImageViewDiff\0AddCompressSettings\0"
    "EditCompressSettings\0UpdateData\0data\0"
    "OnCompressionDone\0OnProcessing\0"
    "FilePathName\0OnCompressionStart\0"
    "OnSourceImage\0ChildCount\0OnDecompressImage\0"
    "OnProjectLoaded\0OnAddedCompressSettingNode\0"
    "OnAddedImageSourceNode\0OnSetToolBarActions\0"
    "islevelType\0onTree_ItemDoubleClicked\0"
    "column\0onTree_ItemClicked\0onDroppedImageItem\0"
    "filePathName\0index\0onSetCurrentItem\0"
    "onEntered\0QModelIndex\0OnGlobalMessage\0"
    "const char*\0msg\0onSetNewProject\0"
    "onContextMenuCompression\0onMenuBarCompression\0"
    "OnCloseCompression\0onSignalProcessMessage\0"
    "onGlobalPropertyChanged\0int&\0"
    "saveProjectFile\0saveAsProjectFile\0"
    "saveToBatchFile\0openProjectFile\0"
    "openNewProjectFile\0compressProjectFiles\0"
    "QFile*\0file\0viewImageDiff\0analyseMeshData\0"
    "viewDiffImageFromChild\0viewDiff3DModelFromChild\0"
    "removeSelectedImage\0openContainingFolder\0"
    "copyFullPath\0onCustomContextMenu\0point\0"
    "onTreeMousePress\0QMouseEvent*\0event\0"
    "onIcon\0onTreeKeyPress\0QKeyEvent*\0"
    "onImageLoadStart\0onImageLoadDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProjectView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      48,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  254,    2, 0x06 /* Public */,
       5,    2,  257,    2, 0x06 /* Public */,
       8,    1,  262,    2, 0x06 /* Public */,
       9,    3,  265,    2, 0x06 /* Public */,
      14,    3,  272,    2, 0x06 /* Public */,
      16,    1,  279,    2, 0x06 /* Public */,
      17,    1,  282,    2, 0x06 /* Public */,
      18,    1,  285,    2, 0x06 /* Public */,
      19,    1,  288,    2, 0x06 /* Public */,
      21,    0,  291,    2, 0x06 /* Public */,
      22,    1,  292,    2, 0x06 /* Public */,
      24,    0,  295,    2, 0x06 /* Public */,
      25,    1,  296,    2, 0x06 /* Public */,
      27,    0,  299,    2, 0x06 /* Public */,
      28,    1,  300,    2, 0x06 /* Public */,
      29,    0,  303,    2, 0x06 /* Public */,
      30,    0,  304,    2, 0x06 /* Public */,
      31,    1,  305,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      33,    2,  308,    2, 0x0a /* Public */,
      35,    2,  313,    2, 0x0a /* Public */,
      36,    2,  318,    2, 0x0a /* Public */,
      39,    1,  323,    2, 0x0a /* Public */,
      40,    1,  326,    2, 0x0a /* Public */,
      42,    1,  329,    2, 0x0a /* Public */,
      45,    1,  332,    2, 0x0a /* Public */,
      46,    0,  335,    2, 0x0a /* Public */,
      47,    0,  336,    2, 0x0a /* Public */,
      48,    0,  337,    2, 0x0a /* Public */,
      49,    0,  338,    2, 0x0a /* Public */,
      50,    1,  339,    2, 0x0a /* Public */,
      52,    0,  342,    2, 0x0a /* Public */,
      53,    0,  343,    2, 0x0a /* Public */,
      54,    0,  344,    2, 0x0a /* Public */,
      55,    0,  345,    2, 0x0a /* Public */,
      56,    0,  346,    2, 0x0a /* Public */,
      57,    1,  347,    2, 0x0a /* Public */,
      60,    0,  350,    2, 0x0a /* Public */,
      61,    0,  351,    2, 0x0a /* Public */,
      62,    0,  352,    2, 0x0a /* Public */,
      63,    0,  353,    2, 0x0a /* Public */,
      64,    0,  354,    2, 0x0a /* Public */,
      65,    0,  355,    2, 0x0a /* Public */,
      66,    0,  356,    2, 0x0a /* Public */,
      67,    1,  357,    2, 0x0a /* Public */,
      69,    2,  360,    2, 0x0a /* Public */,
      73,    1,  365,    2, 0x0a /* Public */,
      75,    0,  368,    2, 0x0a /* Public */,
      76,    0,  369,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 6,    4,    7,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 3, 0x80000000 | 3,   11,   12,   13,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 3, 0x80000000 | 3,   11,   12,   13,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QObjectStar,   20,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,   23,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   32,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int,    7,   34,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Int,    7,   34,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,   37,   38,
    QMetaType::Void, 0x80000000 | 3,   23,
    QMetaType::Void, 0x80000000 | 41,   38,
    QMetaType::Void, 0x80000000 | 43,   44,
    QMetaType::Void, 0x80000000 | 3,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 51,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 58,   59,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   68,
    QMetaType::Void, 0x80000000 | 70, QMetaType::Bool,   71,   72,
    QMetaType::Void, 0x80000000 | 74,   71,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ProjectView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProjectView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->AddedImageFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->ViewImageFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->DeleteImageView((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->ViewImageFileDiff((*reinterpret_cast< C_Destination_Options*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: _t->View3DModelFileDiff((*reinterpret_cast< C_3DSubModel_Info*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 5: _t->DeleteImageViewDiff((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->AddCompressSettings((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->EditCompressSettings((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->UpdateData((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 9: _t->OnCompressionDone(); break;
        case 10: _t->OnProcessing((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->OnCompressionStart(); break;
        case 12: _t->OnSourceImage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->OnDecompressImage(); break;
        case 14: _t->OnProjectLoaded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->OnAddedCompressSettingNode(); break;
        case 16: _t->OnAddedImageSourceNode(); break;
        case 17: _t->OnSetToolBarActions((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->onTree_ItemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 19: _t->onTree_ItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 20: _t->onDroppedImageItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->onSetCurrentItem((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 22: _t->onEntered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 23: _t->OnGlobalMessage((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 24: _t->onSetNewProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 25: _t->onContextMenuCompression(); break;
        case 26: _t->onMenuBarCompression(); break;
        case 27: _t->OnCloseCompression(); break;
        case 28: _t->onSignalProcessMessage(); break;
        case 29: _t->onGlobalPropertyChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->saveProjectFile(); break;
        case 31: _t->saveAsProjectFile(); break;
        case 32: _t->saveToBatchFile(); break;
        case 33: _t->openProjectFile(); break;
        case 34: _t->openNewProjectFile(); break;
        case 35: _t->compressProjectFiles((*reinterpret_cast< QFile*(*)>(_a[1]))); break;
        case 36: _t->viewImageDiff(); break;
        case 37: _t->analyseMeshData(); break;
        case 38: _t->viewDiffImageFromChild(); break;
        case 39: _t->viewDiff3DModelFromChild(); break;
        case 40: _t->removeSelectedImage(); break;
        case 41: _t->openContainingFolder(); break;
        case 42: _t->copyFullPath(); break;
        case 43: _t->onCustomContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 44: _t->onTreeMousePress((*reinterpret_cast< QMouseEvent*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 45: _t->onTreeKeyPress((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 46: _t->onImageLoadStart(); break;
        case 47: _t->onImageLoadDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< C_Destination_Options* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< C_3DSubModel_Info* >(); break;
            }
            break;
        case 35:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QFile* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProjectView::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::AddedImageFile)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QString & , QTreeWidgetItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::ViewImageFile)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::DeleteImageView)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(C_Destination_Options * , QString & , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::ViewImageFileDiff)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(C_3DSubModel_Info * , QString & , QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::View3DModelFileDiff)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::DeleteImageViewDiff)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QTreeWidgetItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::AddCompressSettings)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QTreeWidgetItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::EditCompressSettings)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QObject * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::UpdateData)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnCompressionDone)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnProcessing)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnCompressionStart)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnSourceImage)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnDecompressImage)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnProjectLoaded)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnAddedCompressSettingNode)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnAddedImageSourceNode)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (ProjectView::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectView::OnSetToolBarActions)) {
                *result = 17;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProjectView::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_ProjectView.data,
    qt_meta_data_ProjectView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProjectView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProjectView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectView.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int ProjectView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 48)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 48;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 48)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 48;
    }
    return _id;
}

// SIGNAL 0
void ProjectView::AddedImageFile(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProjectView::ViewImageFile(QString & _t1, QTreeWidgetItem * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProjectView::DeleteImageView(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProjectView::ViewImageFileDiff(C_Destination_Options * _t1, QString & _t2, QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProjectView::View3DModelFileDiff(C_3DSubModel_Info * _t1, QString & _t2, QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ProjectView::DeleteImageViewDiff(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ProjectView::AddCompressSettings(QTreeWidgetItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ProjectView::EditCompressSettings(QTreeWidgetItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ProjectView::UpdateData(QObject * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ProjectView::OnCompressionDone()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ProjectView::OnProcessing(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ProjectView::OnCompressionStart()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ProjectView::OnSourceImage(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void ProjectView::OnDecompressImage()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ProjectView::OnProjectLoaded(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void ProjectView::OnAddedCompressSettingNode()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void ProjectView::OnAddedImageSourceNode()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ProjectView::OnSetToolBarActions(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
