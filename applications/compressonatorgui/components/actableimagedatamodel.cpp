//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file acTableImageDataModel.cpp
/// \version 2.21
//
//=====================================================================

// mymodel.cpp
#include "actableimagedatamodel.h"

acTableImageDataModel::acTableImageDataModel(int rowsize, int colsize, QObject *parent)
    :QAbstractTableModel(parent) {
    m_maxrow = rowsize;
    m_maxcol = colsize;
}

int acTableImageDataModel::rowCount(const QModelIndex & /*parent*/) const {
    return m_maxrow;
}

int acTableImageDataModel::columnCount(const QModelIndex & /*parent*/) const {
    return m_maxcol;
}

QVariant acTableImageDataModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return QString("%1,%2")
               .arg(index.row() + 1)
               .arg(index.column() + 1);
    }
    return QVariant();
}