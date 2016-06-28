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

#ifndef BLENDSTATE_H
#define BLENDSTATE_H

#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/genericstate_p.h>

#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

class Q_AUTOTEST_EXPORT BlendState : public GenericState2<BlendState, GLenum, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return BlendStateMask; }

    static BlendState *getOrCreate(GLenum src, GLenum dst);
private:
    BlendState(GLenum src, GLenum dst);
};

class Q_AUTOTEST_EXPORT BlendStateSeparate : public GenericState4<BlendStateSeparate, GLenum, GLenum, GLenum, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return BlendStateMask; }

    static BlendStateSeparate *getOrCreate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
private:
    BlendStateSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
};

class Q_AUTOTEST_EXPORT BlendEquation : public GenericState1<BlendEquation, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return BlendStateMask; }

    static BlendEquation *getOrCreate(GLenum func);

private:
    BlendEquation(GLenum func);
};


class Q_AUTOTEST_EXPORT AlphaFunc : public GenericState2<AlphaFunc, GLenum, GLclampf>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return AlphaTestMask; }

    static AlphaFunc *getOrCreate(GLenum func, GLclampf value);
private:
    AlphaFunc(GLenum func, GLclampf value);
};

class Q_AUTOTEST_EXPORT DepthTest : public GenericState1<DepthTest, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return DepthTestStateMask; }

    static DepthTest *getOrCreate(GLenum func);

private:
    DepthTest(GLenum func);
};

class Q_AUTOTEST_EXPORT DepthMask : public GenericState1<DepthMask, GLboolean>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const  Q_DECL_OVERRIDE
    { return DepthWriteStateMask; }

    static DepthMask *getOrCreate(GLboolean func);

private:
    DepthMask(GLboolean func);
};

class Q_AUTOTEST_EXPORT CullFace : public GenericState1<CullFace, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return CullFaceStateMask; }

    static CullFace *getOrCreate(GLenum func);

private:
    CullFace(GLenum func);
};

class Q_AUTOTEST_EXPORT FrontFace : public GenericState1<FrontFace, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return FrontFaceStateMask; }
    static FrontFace *getOrCreate(GLenum func);

private:
    FrontFace(GLenum func);
};

class Q_AUTOTEST_EXPORT Dithering : public Qt3D::Render::RenderState
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return DitheringStateMask; }

    bool isEqual(const Dithering &) { return true; }

    static Dithering *getOrCreate();
private:
    Dithering();
};

class Q_AUTOTEST_EXPORT ScissorTest : public GenericState4<ScissorTest, int, int, int, int>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return ScissorStateMask; }

    static ScissorTest *getOrCreate(int left, int bottom, int width, int height);

private:
    ScissorTest(int left, int bottom, int width, int height);
};

class Q_AUTOTEST_EXPORT StencilTest : public GenericState6<StencilTest, GLenum, int, uint, GLenum, int, uint>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return StencilTestStateMask; }
    static StencilTest *getOrCreate(GLenum frontFunc, int frontRef, uint frontMask, GLenum backFunc, int backRef, uint backMask);

private:
    StencilTest(GLenum frontFunc, int frontRef, uint frontMask, GLenum backFunc, int backRef, uint backMask);
};

class Q_AUTOTEST_EXPORT AlphaCoverage : public Qt3D::Render::RenderState
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    StateMaskSet mask() const Q_DECL_OVERRIDE
    { return AlphaCoverageStateMask; }

    bool isEqual(const AlphaCoverage &) { return true; }

    static AlphaCoverage *getOrCreate();

private:
    AlphaCoverage();
};

class Q_AUTOTEST_EXPORT PolygonOffset : public GenericState2<PolygonOffset, GLfloat, GLfloat>
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    StateMaskSet mask() const Q_DECL_OVERRIDE
    { return PolygonOffsetStateMask; }

    static PolygonOffset *getOrCreate(GLfloat factor, GLfloat units);

private:
    PolygonOffset(GLfloat factor, GLfloat units);
};

class Q_AUTOTEST_EXPORT ColorMask : public GenericState4<ColorMask, GLboolean, GLboolean, GLboolean, GLboolean>
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_FINAL;
    StateMaskSet mask() const Q_DECL_FINAL { return ColorStateMask; }

    static ColorMask *getOrCreate(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

private:
    ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
};

class Q_AUTOTEST_EXPORT ClipPlane : public GenericState1<ClipPlane, int>
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_FINAL;

    StateMaskSet mask() const Q_DECL_FINAL
    { return ClipPlaneMask; }
    static ClipPlane *getOrCreate(int plane);

private:
    ClipPlane(int plane);
};

class Q_AUTOTEST_EXPORT StencilOp : public GenericState6<StencilOp, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum>
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_FINAL;

    StateMaskSet mask() const Q_DECL_FINAL
    { return StencilOpMask; }
    static StencilOp *getOrCreate(GLenum fsfail, GLenum fdfail, GLenum fdspass,
                                  GLenum bsfail, GLenum bdfail, GLenum bdspass);

private:
    StencilOp(GLenum fsfail, GLenum fdfail, GLenum fdspass,
              GLenum bsfail, GLenum bdfail, GLenum bdspass);
};

class Q_AUTOTEST_EXPORT StencilMask : public GenericState2<StencilMask, uint, uint>
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_FINAL;

    StateMaskSet mask() const Q_DECL_FINAL
    { return StencilWriteStateMask; }
    static StencilMask *getOrCreate(uint frontMask, uint backMask);

private:
    StencilMask(uint frontMask, uint backMask);
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // BLENDSTATE_H
