/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEXMLLISTMODEL_H
#define QDECLARATIVEXMLLISTMODEL_H

#include <qdeclarative.h>
#include <qdeclarativeinfo.h>

#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>
#include <QtScript/qscriptvalue.h>

#include <private/qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeContext;
class QDeclarativeXmlListModelRole;
class QDeclarativeXmlListModelPrivate;

struct QDeclarativeXmlQueryResult {
    int queryId;
    int size;
    QList<QList<QVariant> > data;
    QList<QPair<int, int> > inserted;
    QList<QPair<int, int> > removed;
    QStringList keyRoleResultsCache;
};

class Q_AUTOTEST_EXPORT QDeclarativeXmlListModel : public QListModelInterface, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString xml READ xml WRITE setXml NOTIFY xmlChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QString namespaceDeclarations READ namespaceDeclarations WRITE setNamespaceDeclarations NOTIFY namespaceDeclarationsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeXmlListModelRole> roles READ roleObjects)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_CLASSINFO("DefaultProperty", "roles")

public:
    QDeclarativeXmlListModel(QObject *parent = 0);
    ~QDeclarativeXmlListModel();

    virtual QHash<int,QVariant> data(int index, const QList<int> &roles = (QList<int>())) const;
    virtual QVariant data(int index, int role) const;
    virtual int count() const;
    virtual QList<int> roles() const;
    virtual QString toString(int role) const;

    QDeclarativeListProperty<QDeclarativeXmlListModelRole> roleObjects();

    QUrl source() const;
    void setSource(const QUrl&);

    QString xml() const;
    void setXml(const QString&);

    QString query() const;
    void setQuery(const QString&);

    QString namespaceDeclarations() const;
    void setNamespaceDeclarations(const QString&);

    Q_INVOKABLE QScriptValue get(int index) const;

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    Q_INVOKABLE QString errorString() const;

    virtual void classBegin();
    virtual void componentComplete();

Q_SIGNALS:
    void statusChanged(QDeclarativeXmlListModel::Status);
    void progressChanged(qreal progress);
    void countChanged();
    void sourceChanged();
    void xmlChanged();
    void queryChanged();
    void namespaceDeclarationsChanged();

public Q_SLOTS:
    // ### need to use/expose Expiry to guess when to call this?
    // ### property to auto-call this on reasonable Expiry?
    // ### LastModified/Age also useful to guess.
    // ### Probably also applies to other network-requesting types.
    void reload();

private Q_SLOTS:
    void requestFinished();
    void requestProgress(qint64,qint64);
    void dataCleared();
    void queryCompleted(const QDeclarativeXmlQueryResult &);
    void queryError(void* object, const QString& error);

private:
    Q_DECLARE_PRIVATE(QDeclarativeXmlListModel)
    Q_DISABLE_COPY(QDeclarativeXmlListModel)
};

class Q_AUTOTEST_EXPORT QDeclarativeXmlListModelRole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(bool isKey READ isKey WRITE setIsKey NOTIFY isKeyChanged)
public:
    QDeclarativeXmlListModelRole() : m_isKey(false) {}
    ~QDeclarativeXmlListModelRole() {}

    QString name() const { return m_name; }
    void setName(const QString &name) {
        if (name == m_name)
            return;
        m_name = name;
        emit nameChanged();
    }

    QString query() const { return m_query; }
    void setQuery(const QString &query)
    {
        if (query.startsWith(QLatin1Char('/'))) {
            qmlInfo(this) << tr("An XmlRole query must not start with '/'");
            return;
        }
        if (m_query == query)
            return;
        m_query = query;
        emit queryChanged();
    }

    bool isKey() const { return m_isKey; }
    void setIsKey(bool b) {
        if (m_isKey == b)
            return;
        m_isKey = b;
        emit isKeyChanged();
    }

    bool isValid() {
        return !m_name.isEmpty() && !m_query.isEmpty();
    }

Q_SIGNALS:
    void nameChanged();
    void queryChanged();
    void isKeyChanged();

private:
    QString m_name;
    QString m_query;
    bool m_isKey;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeXmlListModel)
QML_DECLARE_TYPE(QDeclarativeXmlListModelRole)

#endif // QDECLARATIVEXMLLISTMODEL_H
