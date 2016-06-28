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

#ifndef QT3D_QSCENECHANGE_H
#define QT3D_QSCENECHANGE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QSharedPointer>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

enum ChangeFlag {
    NodeCreated             = 1 << 0,
    NodeAboutToBeDeleted    = 1 << 1,
    NodeDeleted             = 1 << 2,
    NodeUpdated             = 1 << 3,
    NodeAdded               = 1 << 4,
    NodeRemoved             = 1 << 5,
    ComponentAdded          = 1 << 6,
    ComponentRemoved        = 1 << 7,
    AllChanges              = 0xFFFFFFFF
};
Q_DECLARE_FLAGS(ChangeFlags, ChangeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(ChangeFlags)

class QNode;
class QObservableInterface;
class QSceneChangePrivate;

class QT3DCORESHARED_EXPORT QSceneChange
{
public:
    enum Priority {
        High,
        Standard,
        Low
    };

    enum ObservableType {
        Observable,
        Node
    };

    QSceneChange(ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, Priority priority = Standard);
    virtual ~QSceneChange();

    ChangeFlag type() const;
    qint64 timestamp() const;
    QSceneChange::Priority priority() const;
    QSceneChange::ObservableType observableType() const;
    QNodeId subjectId() const;

protected:
    Q_DECLARE_PRIVATE(QSceneChange)
    QSceneChangePrivate *d_ptr;
    QSceneChange(QSceneChangePrivate &dd);
    QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, ObservableType observableType, const QNodeId &subjectId, Priority priority = Standard);

    // TODO: add timestamp from central clock and priority level
    // These can be used to resolve any conflicts between events
    // posted from different aspects
};

typedef QSharedPointer<QSceneChange> QSceneChangePtr;

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QSCENECHANGE_H
