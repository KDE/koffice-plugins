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

#ifndef GRADIENTEDITOR_H
#define GRADIENTEDITOR_H

#include <QtGui/QWidget>
#include <ui_GradientEditor.h>

class KoResource;

/**
 * A widget for managing gradients.
 */
class GradientEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a new gradient editor widget
     * @param parent the widgets parent
     */
    explicit GradientEditor(QWidget *parent = 0);
    virtual ~GradientEditor();

    void setGradientEdit(QGradient *gradient);

signals:
    /// Is emitted a soon as the gradient changes
    void changed();

    /// emitted when we can no longer continue with an existing instance of QGradient
    void changedGradientType(const QGradient &gradient);

private slots:
    void setEditMode(bool on);
    void setGradientType(int);
    void setGradientRepeat(int);
    void updateStops(const QGradientStops &stops);
    void setGradient(KoResource*);

private:
    QGradient::Type m_type;
    QGradient::Spread m_spread;

    QGradient *m_gradientEdit;

    Ui::GradientEditor widget;
};

#endif
