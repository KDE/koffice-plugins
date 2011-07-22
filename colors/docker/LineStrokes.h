/* This file is part of the KDE project
 * Copyright (C) 2011 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef LINESTROKES_H
#define LINESTROKES_H

#include <QWidget>

#include <ui_LineStrokes.h>

class QPen;
class KUnit;

class LineStrokes : public QWidget
{
    Q_OBJECT
public:
    LineStrokes(QWidget *parent = 0);

    void setUnit(const KUnit &unit);

    void setPen(const QPen &pen);

signals:
    void strokeChanged(const QPen &pen);

private slots:
    void setLineStyle(int);
    void setCaps(int);
    void setJoins(int);

private:
    Ui::LineStrokes widget;
    QButtonGroup *m_caps;
    QButtonGroup *m_joins;
};

#endif