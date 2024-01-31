//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \file acTableImageDataModel.h
/// \version 2.21
//
//=====================================================================

#ifndef __ACTABLEIMAGEDATAYMODEL_H
#define __ACTABLEIMAGEDATAYMODEL_H

// TableImageDataModel.h
#include <QAbstractTableModel>

// ----------------------------------------------------------------------------------
// Class Name:          acTableImageDataModel:
// General Description: This class is used as data model for the image manager
// Creation Date:       10/20/2015
// ----------------------------------------------------------------------------------

class acTableImageDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    acTableImageDataModel(int rowsize, int colsize, QObject* parent);
    int      rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int      columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    int m_maxrow;
    int m_maxcol;
};

#endif