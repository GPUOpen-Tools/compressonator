/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ABSTRACTSCENEPARSER_H
#define ABSTRACTSCENEPARSER_H

#include <QObject>
#include <QStringList>
#include <QLoggingCategory>
#include <QUrl>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QEntity;

Q_DECLARE_LOGGING_CATEGORY(SceneParsers)

class QT3DRENDERERSHARED_EXPORT AbstractSceneParser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ParserStatus parserStatus READ parserStatus NOTIFY parserStatusChanged)
    Q_PROPERTY(QStringList errors READ errors NOTIFY errorsChanged)

public:
    enum ParserStatus {
        Empty,
        Loading,
        Loaded,
        Error
    };
    Q_ENUM(ParserStatus)

    AbstractSceneParser();
    virtual ~AbstractSceneParser();

    virtual void setSource(const QUrl &source) = 0;
    virtual bool isExtensionSupported(const QUrl &source) const = 0;
    virtual QEntity *scene(const QString &id = QString()) = 0;
    virtual QEntity *node(const QString &id) = 0;

    ParserStatus parserStatus() const;
    QStringList errors() const;

Q_SIGNALS:
    void parserStatusChanged();
    void errorsChanged();

protected:
    void setParserStatus(ParserStatus parserStatus);
    void logError(const QString &error);
    void logInfo(const QString &info);

private:
    ParserStatus m_parserStatus;
    QStringList m_errors;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // ABSTRACTSCENEPARSER_H
