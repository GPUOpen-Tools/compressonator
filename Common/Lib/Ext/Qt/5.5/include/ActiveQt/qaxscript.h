/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the ActiveQt framework of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAXSCRIPT_H
#define QAXSCRIPT_H

#include <ActiveQt/qaxobject.h>

struct IActiveScript;

QT_BEGIN_NAMESPACE

class QAxBase;
class QAxScript;
class QAxScriptSite;
class QAxScriptEngine;
class QAxScriptManager;
class QAxScriptManagerPrivate;

class QAxScriptEngine : public QAxObject
{
public:
    enum State {
        Uninitialized = 0,
        Initialized = 5,
        Started = 1,
        Connected = 2,
        Disconnected = 3,
        Closed = 4
    };

    QAxScriptEngine(const QString &language, QAxScript *script);
    ~QAxScriptEngine();

    bool isValid() const;
    bool hasIntrospection() const;

    QString scriptLanguage() const;

    State state() const;
    void setState(State st);

    void addItem(const QString &name);

    long queryInterface(const QUuid &, void**) const;

protected:
    bool initialize(IUnknown** ptr);

private:
    QAxScript *script_code;
    IActiveScript *engine;

    QString script_language;
};

class QAxScript : public QObject
{
    Q_OBJECT

public:
    enum FunctionFlags {
        FunctionNames = 0,
        FunctionSignatures
    };

    QAxScript(const QString &name, QAxScriptManager *manager);
    ~QAxScript();

    bool load(const QString &code, const QString &language = QString());

    QStringList functions(FunctionFlags = FunctionNames) const;

    QString scriptCode() const;
    QString scriptName() const;
    QAxScriptEngine *scriptEngine() const;

    QVariant call(const QString &function, const QVariant &v1 = QVariant(),
                                           const QVariant &v2 = QVariant(),
                                           const QVariant &v3 = QVariant(),
                                           const QVariant &v4 = QVariant(),
                                           const QVariant &v5 = QVariant(),
                                           const QVariant &v6 = QVariant(),
                                           const QVariant &v7 = QVariant(),
                                           const QVariant &v8 = QVariant());
    QVariant call(const QString &function, QList<QVariant> &arguments);

Q_SIGNALS:
    void entered();
    void finished();
    void finished(const QVariant &result);
    void finished(int code, const QString &source,const QString &description, const QString &help);
    void stateChanged(int state);
    void error(int code, const QString &description, int sourcePosition, const QString &sourceText);

private:
    friend class QAxScriptSite;
    friend class QAxScriptEngine;

    void updateObjects();
    QAxBase *findObject(const QString &name);

    QString script_name;
    QString script_code;
    QAxScriptManager *script_manager;
    QAxScriptEngine *script_engine;
    QAxScriptSite *script_site;
};

class QAxScriptManager : public QObject
{
    Q_OBJECT

public:
    QAxScriptManager(QObject *parent = 0);
    ~QAxScriptManager();

    void addObject(QAxBase *object);
    void addObject(QObject *object);

    QStringList functions(QAxScript::FunctionFlags = QAxScript::FunctionNames) const;
    QStringList scriptNames() const;
    QAxScript *script(const QString &name) const;

    QAxScript* load(const QString &code, const QString &name, const QString &language);
    QAxScript* load(const QString &file, const QString &name);

    QVariant call(const QString &function, const QVariant &v1 = QVariant(),
                                           const QVariant &v2 = QVariant(),
                                           const QVariant &v3 = QVariant(),
                                           const QVariant &v4 = QVariant(),
                                           const QVariant &v5 = QVariant(),
                                           const QVariant &v6 = QVariant(),
                                           const QVariant &v7 = QVariant(),
                                           const QVariant &v8 = QVariant());
    QVariant call(const QString &function, QList<QVariant> &arguments);

    static bool registerEngine(const QString &name, const QString &extension, const QString &code = QString());
    static QString scriptFileFilter();

Q_SIGNALS:
    void error(QAxScript *script, int code, const QString &description, int sourcePosition, const QString &sourceText);

private Q_SLOTS:
    void objectDestroyed(QObject *o);
    void scriptError(int code, const QString &description, int sourcePosition, const QString &sourceText);

private:
    friend class QAxScript;
    QAxScriptManagerPrivate *d;

    void updateScript(QAxScript*);
    QAxScript *scriptForFunction(const QString &function) const;
};


// QAxScript inlines

inline QString QAxScript::scriptCode() const
{
    return script_code;
}

inline QString QAxScript::scriptName() const
{
    return script_name;
}

inline QAxScriptEngine *QAxScript::scriptEngine() const
{
    return script_engine;
}

// QAxScriptEngine inlines

inline bool QAxScriptEngine::isValid() const
{
    return engine != 0;
}

inline QString QAxScriptEngine::scriptLanguage() const
{
    return script_language;
}

// QAxScriptManager inlines

extern QAxBase *qax_create_object_wrapper(QObject*);

inline void QAxScriptManager::addObject(QObject *object)
{
    QAxBase *wrapper = qax_create_object_wrapper(object);
    if (!wrapper) {
        qWarning("QAxScriptMananger::addObject: Class %s not exposed through the QAxFactory",
            object->metaObject()->className());
        Q_ASSERT(wrapper);
    }
    addObject(wrapper);
}

QT_END_NAMESPACE

#endif // QAXSCRIPT_H
