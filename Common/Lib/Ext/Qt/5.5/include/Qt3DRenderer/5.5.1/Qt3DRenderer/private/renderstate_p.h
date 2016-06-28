/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3D_RENDER_RENDERSTATE_H
#define QT3D_RENDER_RENDERSTATE_H

#include <QVector>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QRenderState;

namespace Render {

class QGraphicsContext;

enum StateMask
{
    BlendStateMask          = 1 << 0,
    StencilWriteStateMask   = 1 << 1,
    StencilTestStateMask    = 1 << 2,
    ScissorStateMask        = 1 << 3,
    DepthTestStateMask      = 1 << 4,
    DepthWriteStateMask     = 1 << 5,
    CullFaceStateMask       = 1 << 6,
    AlphaTestMask           = 1 << 7,
    FrontFaceStateMask      = 1 << 8,
    DitheringStateMask      = 1 << 9,
    AlphaCoverageStateMask  = 1 << 10,
    PolygonOffsetStateMask  = 1 << 11,
    ColorStateMask          = 1 << 12,
    ClipPlaneMask           = 1 << 13,
    StencilOpMask           = 1 << 14
};

typedef quint64 StateMaskSet;

class Q_AUTOTEST_EXPORT RenderState
{
public:
    virtual ~RenderState() {}

    virtual void apply(QGraphicsContext* gc) const = 0;

    virtual StateMaskSet mask() const = 0;

    static RenderState *getOrCreateBackendState(QRenderState *renderState);
protected:
};

class RenderStateSet
{
public:
    RenderStateSet();
    ~RenderStateSet();

    void addState(RenderState* ds);

    /**
     * @brief changeCost - metric of cost to change to this state-set from
     * a candidate previous state-set. This is used to find an optimal
     * ordering of state-sets when sending draw commands.
     * @param previousState
     * @return
     */
    int changeCost(RenderStateSet* previousState);

    void apply(QGraphicsContext* gc);

    StateMaskSet stateMask() const;
    void merge(RenderStateSet *other);
    void resetMasked(StateMaskSet maskOfStatesToReset, QGraphicsContext* gc);
private:
    /**
     * @brief contains - check if this set contains a matching piece of state
     * @param ds
     * @return
     */
    bool contains(RenderState* ds) const;

    QVector<RenderState*> m_states;

    StateMaskSet m_stateMask;

    RenderStateSet* m_cachedPrevious;
    QVector<RenderState*> m_cachedDeltaStates;

};

} // Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSTATE_H
