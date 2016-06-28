/****************************************************************************
 **
 ** Copyright (C) 2013 Ivan Vizir <define-true-false@yandex.com>
 ** Contact: http://www.qt.io/licensing/
 **
 ** This file is part of the QtWinExtras module of the Qt Toolkit.
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

#ifndef WINDOWSGUIDSDEFS_P_H
#define WINDOWSGUIDSDEFS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QtWinExtras. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGlobal>
#include <qt_windows.h>

QT_BEGIN_NAMESPACE

extern const GUID qCLSID_DestinationList;
extern const GUID qCLSID_EnumerableObjectCollection;
extern const GUID qIID_ICustomDestinationList;
extern const GUID qIID_IApplicationDestinations;
extern const GUID qCLSID_ApplicationDestinations;
extern const GUID qIID_IApplicationDocumentLists;
extern const GUID qCLSID_ApplicationDocumentLists;
extern const GUID qIID_IObjectArray;
extern const GUID qIID_IObjectCollection;
extern const GUID qIID_IPropertyStore;
extern const GUID qIID_ITaskbarList3;
extern const GUID qIID_ITaskbarList4;
extern const GUID qIID_IShellItem2;
extern const GUID qIID_IShellLinkW;
extern const GUID qIID_ITaskbarList;
extern const GUID qIID_ITaskbarList2;
extern const GUID qIID_IUnknown;
extern const GUID qGUID_NULL;

QT_END_NAMESPACE

#endif // WINDOWSGUIDSDEFS_P_H
