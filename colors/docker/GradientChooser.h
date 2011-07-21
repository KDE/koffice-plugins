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

#ifndef GRADIENT_CHOOSER_H
#define GRADIENT_CHOOSER_H

#include "SelectorWidget.h"

#include <KShape.h>

#include <QWidget>
#include <QSet>

class KShapeBorderBase;
class GradientEditor;
class KGradientBackground;

class GradientChooser : public QWidget
{
    Q_OBJECT
public:
    GradientChooser(QSet<KShape*> shapes, SelectorWidget::FillStroke type, QWidget *parent = 0);
    ~GradientChooser();

public slots:
    void shapeChanged(KShape *shape, KShape::ChangeType type);

signals:
    void changed();

private slots:
    void gradientChanged(const QGradient &gradient);

private:
    GradientEditor *m_gradientEditor;
    KGradientBackground *m_background;
    KShapeBorderBase *m_shapeBorder;
    QSet<KShape*> m_shapes;

    // the brush-gradient we are editing. Only used when editing a border!
    QBrush m_brush;
};

#endif
