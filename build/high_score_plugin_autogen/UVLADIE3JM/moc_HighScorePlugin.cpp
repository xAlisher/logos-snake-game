/****************************************************************************
** Meta object code from reading C++ file 'HighScorePlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/HighScorePlugin.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HighScorePlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15HighScorePluginE_t {};
} // unnamed namespace

template <> constexpr inline auto HighScorePlugin::qt_create_metaobjectdata<qt_meta_tag_ZN15HighScorePluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HighScorePlugin",
        "eventResponse",
        "",
        "eventName",
        "QVariantList",
        "data",
        "initLogos",
        "LogosAPI*",
        "api",
        "initialize",
        "submitScore",
        "playerName",
        "score",
        "getHighScores",
        "clearScores",
        "getPlayerBest"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'eventResponse'
        QtMocHelpers::SignalData<void(const QString &, const QVariantList &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Method 'initLogos'
        QtMocHelpers::MethodData<void(LogosAPI *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Method 'initialize'
        QtMocHelpers::MethodData<QString()>(9, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'submitScore'
        QtMocHelpers::MethodData<QString(const QString &, int)>(10, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 11 }, { QMetaType::Int, 12 },
        }}),
        // Method 'getHighScores'
        QtMocHelpers::MethodData<QString()>(13, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'clearScores'
        QtMocHelpers::MethodData<QString()>(14, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'getPlayerBest'
        QtMocHelpers::MethodData<QString(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HighScorePlugin, qt_meta_tag_ZN15HighScorePluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HighScorePlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HighScorePluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HighScorePluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15HighScorePluginE_t>.metaTypes,
    nullptr
} };

void HighScorePlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HighScorePlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->eventResponse((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantList>>(_a[2]))); break;
        case 1: _t->initLogos((*reinterpret_cast< std::add_pointer_t<LogosAPI*>>(_a[1]))); break;
        case 2: { QString _r = _t->initialize();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->submitScore((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->getHighScores();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->clearScores();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { QString _r = _t->getPlayerBest((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< LogosAPI* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HighScorePlugin::*)(const QString & , const QVariantList & )>(_a, &HighScorePlugin::eventResponse, 0))
            return;
    }
}

const QMetaObject *HighScorePlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HighScorePlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HighScorePluginE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "PluginInterface"))
        return static_cast< PluginInterface*>(this);
    if (!strcmp(_clname, "com.example.PluginInterface"))
        return static_cast< PluginInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int HighScorePlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void HighScorePlugin::eventResponse(const QString & _t1, const QVariantList & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_HighScorePlugin[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1c,  'o',  'r',  'g',  '.',  'l', 
    'o',  'g',  'o',  's',  '.',  'H',  'i',  'g', 
    'h',  'S',  'c',  'o',  'r',  'e',  'I',  'n', 
    't',  'e',  'r',  'f',  'a',  'c',  'e', 
    // "className"
    0x03,  0x6f,  'H',  'i',  'g',  'h',  'S',  'c', 
    'o',  'r',  'e',  'P',  'l',  'u',  'g',  'i', 
    'n', 
    // "MetaData"
    0x04,  0xa4,  0x6b,  'd',  'e',  's',  'c',  'r', 
    'i',  'p',  't',  'i',  'o',  'n',  0x78,  0x21, 
    'H',  'i',  'g',  'h',  ' ',  's',  'c',  'o', 
    'r',  'e',  ' ',  's',  't',  'o',  'r',  'a', 
    'g',  'e',  ' ',  'f',  'o',  'r',  ' ',  's', 
    'n',  'a',  'k',  'e',  ' ',  'g',  'a',  'm', 
    'e',  0x64,  'n',  'a',  'm',  'e',  0x6a,  'h', 
    'i',  'g',  'h',  '_',  's',  'c',  'o',  'r', 
    'e',  0x64,  't',  'y',  'p',  'e',  0x64,  'c', 
    'o',  'r',  'e',  0x67,  'v',  'e',  'r',  's', 
    'i',  'o',  'n',  0x65,  '1',  '.',  '0',  '.', 
    '0', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(HighScorePlugin, HighScorePlugin, qt_pluginMetaDataV2_HighScorePlugin)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_HighScorePlugin[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1c,  'o',  'r',  'g',  '.',  'l', 
    'o',  'g',  'o',  's',  '.',  'H',  'i',  'g', 
    'h',  'S',  'c',  'o',  'r',  'e',  'I',  'n', 
    't',  'e',  'r',  'f',  'a',  'c',  'e', 
    // "className"
    0x03,  0x6f,  'H',  'i',  'g',  'h',  'S',  'c', 
    'o',  'r',  'e',  'P',  'l',  'u',  'g',  'i', 
    'n', 
    // "MetaData"
    0x04,  0xa4,  0x6b,  'd',  'e',  's',  'c',  'r', 
    'i',  'p',  't',  'i',  'o',  'n',  0x78,  0x21, 
    'H',  'i',  'g',  'h',  ' ',  's',  'c',  'o', 
    'r',  'e',  ' ',  's',  't',  'o',  'r',  'a', 
    'g',  'e',  ' ',  'f',  'o',  'r',  ' ',  's', 
    'n',  'a',  'k',  'e',  ' ',  'g',  'a',  'm', 
    'e',  0x64,  'n',  'a',  'm',  'e',  0x6a,  'h', 
    'i',  'g',  'h',  '_',  's',  'c',  'o',  'r', 
    'e',  0x64,  't',  'y',  'p',  'e',  0x64,  'c', 
    'o',  'r',  'e',  0x67,  'v',  'e',  'r',  's', 
    'i',  'o',  'n',  0x65,  '1',  '.',  '0',  '.', 
    '0', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(HighScorePlugin, HighScorePlugin)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
