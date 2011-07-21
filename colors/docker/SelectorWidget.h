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
#ifndef SELECTORWIDGET_H
#define SELECTORWIDGET_H

#include <QWidget>
#include <QWeakPointer>
#include <ui_SelectorWidget.h>

#include <KShape.h>

class KCanvasBase;
class KImageCollection;
class KSelection;

class SelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SelectorWidget(QWidget *parent = 0);

    void setCanvas(KCanvasBase *canvas);
    void clearCanvas();

    enum FillStroke {
        Fill,
        Stroke
    };

private slots:
    void switchToFill();
    void switchToStroke();
    void setType(int);
    void shapeSelectionChanged();
    void repaintAllShapes();
    void setShapeTransparency(int percent);

protected:
    void showEvent(QShowEvent *event);

private:
    void updateUi();

    enum Type {
        TypeNoFill,
        TypeColors,
        TypeGradients,
        TypePatterns
    };
    Ui::SelectorWidget widget;

    Type m_type;
    FillStroke m_state;
    QWidget *m_current;
    KSelection *m_shapeSelection;
    QSet<KShape*> m_openShapes;
    KImageCollection *m_imageCollection;
    KShapeManager *m_shapeManager;
    bool m_showStrokeWidget;
};

#endif
