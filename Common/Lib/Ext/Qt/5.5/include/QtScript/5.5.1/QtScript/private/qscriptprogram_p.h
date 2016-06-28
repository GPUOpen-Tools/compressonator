/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTPROGRAM_P_H
#define QSCRIPTPROGRAM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

#include "RefPtr.h"

namespace JSC
{
    class EvalExecutable;
    class ExecState;
}

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;

class QScriptProgramPrivate : public QSharedData
{
public:
    QScriptProgramPrivate(const QString &sourceCode,
                          const QString &fileName,
                          int firstLineNumber);
    ~QScriptProgramPrivate();

    static QScriptProgramPrivate *get(const QScriptProgram &q);

    JSC::EvalExecutable *executable(JSC::ExecState *exec,
                                    QScriptEnginePrivate *engine);
    void detachFromEngine();

    QString sourceCode;
    QString fileName;
    int firstLineNumber;

    QScriptEnginePrivate *engine;
    WTF::RefPtr<JSC::EvalExecutable> _executable;
    intptr_t sourceId;
    bool isCompiled;
};

QT_END_NAMESPACE

#endif
