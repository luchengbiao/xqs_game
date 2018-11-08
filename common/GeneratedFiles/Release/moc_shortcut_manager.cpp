/****************************************************************************
** Meta object code from reading C++ file 'shortcut_manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Shortcut/shortcut_manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shortcut_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ShortCutManager_t {
    QByteArrayData data[13];
    char stringdata0[236];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ShortCutManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ShortCutManager_t qt_meta_stringdata_ShortCutManager = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ShortCutManager"
QT_MOC_LITERAL(1, 16, 20), // "SignalMockHeartStart"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19), // "SignalMockHeartStop"
QT_MOC_LITERAL(4, 58, 21), // "SignalMockAgoraLogout"
QT_MOC_LITERAL(5, 80, 25), // "SignalMockServerHeartStop"
QT_MOC_LITERAL(6, 106, 26), // "SignalMockServerHeartStart"
QT_MOC_LITERAL(7, 133, 23), // "SignalMockOpenClassroom"
QT_MOC_LITERAL(8, 157, 11), // "std::string"
QT_MOC_LITERAL(9, 169, 17), // "SignalO2OPlayback"
QT_MOC_LITERAL(10, 187, 9), // "course_id"
QT_MOC_LITERAL(11, 197, 17), // "SignalO2MPlayback"
QT_MOC_LITERAL(12, 215, 20) // "SignalReadingSetting"

    },
    "ShortCutManager\0SignalMockHeartStart\0"
    "\0SignalMockHeartStop\0SignalMockAgoraLogout\0"
    "SignalMockServerHeartStop\0"
    "SignalMockServerHeartStart\0"
    "SignalMockOpenClassroom\0std::string\0"
    "SignalO2OPlayback\0course_id\0"
    "SignalO2MPlayback\0SignalReadingSetting"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShortCutManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,
       5,    0,   62,    2, 0x06 /* Public */,
       6,    0,   63,    2, 0x06 /* Public */,
       7,    1,   64,    2, 0x06 /* Public */,
       9,    1,   67,    2, 0x06 /* Public */,
      11,    1,   70,    2, 0x06 /* Public */,
      12,    1,   73,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, QMetaType::ULongLong,   10,
    QMetaType::Void, QMetaType::ULongLong,   10,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void ShortCutManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ShortCutManager *_t = static_cast<ShortCutManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalMockHeartStart(); break;
        case 1: _t->SignalMockHeartStop(); break;
        case 2: _t->SignalMockAgoraLogout(); break;
        case 3: _t->SignalMockServerHeartStop(); break;
        case 4: _t->SignalMockServerHeartStart(); break;
        case 5: _t->SignalMockOpenClassroom((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 6: _t->SignalO2OPlayback((*reinterpret_cast< unsigned long long(*)>(_a[1]))); break;
        case 7: _t->SignalO2MPlayback((*reinterpret_cast< unsigned long long(*)>(_a[1]))); break;
        case 8: _t->SignalReadingSetting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ShortCutManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockHeartStart)) {
                *result = 0;
            }
        }
        {
            typedef void (ShortCutManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockHeartStop)) {
                *result = 1;
            }
        }
        {
            typedef void (ShortCutManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockAgoraLogout)) {
                *result = 2;
            }
        }
        {
            typedef void (ShortCutManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockServerHeartStop)) {
                *result = 3;
            }
        }
        {
            typedef void (ShortCutManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockServerHeartStart)) {
                *result = 4;
            }
        }
        {
            typedef void (ShortCutManager::*_t)(std::string );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalMockOpenClassroom)) {
                *result = 5;
            }
        }
        {
            typedef void (ShortCutManager::*_t)(unsigned long long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalO2OPlayback)) {
                *result = 6;
            }
        }
        {
            typedef void (ShortCutManager::*_t)(unsigned long long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalO2MPlayback)) {
                *result = 7;
            }
        }
        {
            typedef void (ShortCutManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ShortCutManager::SignalReadingSetting)) {
                *result = 8;
            }
        }
    }
}

const QMetaObject ShortCutManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ShortCutManager.data,
      qt_meta_data_ShortCutManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ShortCutManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShortCutManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ShortCutManager.stringdata0))
        return static_cast<void*>(const_cast< ShortCutManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ShortCutManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ShortCutManager::SignalMockHeartStart()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void ShortCutManager::SignalMockHeartStop()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void ShortCutManager::SignalMockAgoraLogout()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void ShortCutManager::SignalMockServerHeartStop()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void ShortCutManager::SignalMockServerHeartStart()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void ShortCutManager::SignalMockOpenClassroom(std::string _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ShortCutManager::SignalO2OPlayback(unsigned long long _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ShortCutManager::SignalO2MPlayback(unsigned long long _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ShortCutManager::SignalReadingSetting(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
