/* This file is part of the KDE project
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005-2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2007 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef GRADIENTSTOPSEDITOR_H
#define GRADIENTSTOPSEDITOR_H

#include <KoCheckerBoardPainter.h>
#include <QWidget>

class QGradient;
class QPainter;
class QPaintEvent;
class QMouseEvent;
class QColor;

/**
 * A widget for editing a gradients color stops.
 */
class GradientStopsEditor : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a new gradient widget with the given parent
     * @param parent the gradient widgets parent
     */
    explicit GradientStopsEditor(QWidget* parent = 0);

    /// Destroys the gradient widget
    ~GradientStopsEditor();

    /**
     * Sets the gradient color stops to be edited.
     * @param stops the gradient color stops to edit
     */
    void setStops(const QGradientStops &stops);

    /**
     * Returns the edited gradient color stops.
     * @return the edited gradient color stops
     */
    QGradientStops stops() const;

signals:
    /// Is emitted as soon as a color stop was changed/added/removed
    void changed(const QGradientStops &stops);

protected:
    virtual void paintEvent(QPaintEvent*);
    /** mouse events... For color stops manipulation */
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

private:
    /// Returns index of color stop at given mouse position, -1 if non found
    int colorStopFromPosition(const QPoint &mousePos);

    /// Sorts color stops by position, retaining the index of the current stop
    void sortStops();

    /// moves color stop with given index setting to new position, returning the new stop index
    int moveColorStop(int index, qreal newPosition);

    QGradientStops m_stops; ///< the gradient color stops
    int m_currentStop; ///< the stop to modify.
    QRect m_paintArea; ///< the area where the gradient is painted

    KoCheckerBoardPainter m_checkerPainter;
};

#endif
