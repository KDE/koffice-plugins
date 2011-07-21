/* This file is part of the KDE project
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>
   Copyright (C) 2005-2011 Thomas Zander <zander@kde.org>
   Copyright (C) 2006-2007,2009 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>

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

#include "GradientStopsEditor.h"

#include <KLocale>
#include <KIconLoader>
#include <KColorDialog>

#include <QtGui/QLabel>
#include <QtGui/QBitmap>
#include <QtGui/QPaintEvent>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <limits>

static QColor colorAt(qreal position, const QGradientStops &stops)
{
    if (! stops.count())
        return QColor();

    if (stops.count() == 1)
        return stops.first().second;

    QGradientStop prevStop(-1.0, QColor());
    QGradientStop nextStop(2.0, QColor());
    // find framing gradient stops
    foreach(const QGradientStop & stop, stops) {
        if (stop.first > prevStop.first && stop.first < position)
            prevStop = stop;
        if (stop.first < nextStop.first && stop.first > position)
            nextStop = stop;
    }

    QColor theColor;

    if (prevStop.first < 0.0) {
        // new stop is before the first stop
        theColor = nextStop.second;
    } else if (nextStop.first > 1.0) {
        // new stop is after the last stop
        theColor = prevStop.second;
    } else {
        // linear interpolate colors between framing stops
        QColor prevColor = prevStop.second, nextColor = nextStop.second;
        qreal colorScale = (position - prevStop.first) / (nextStop.first - prevStop.first);
        theColor.setRedF(prevColor.redF() + colorScale *(nextColor.redF() - prevColor.redF()));
        theColor.setGreenF(prevColor.greenF() + colorScale *(nextColor.greenF() - prevColor.greenF()));
        theColor.setBlueF(prevColor.blueF() + colorScale *(nextColor.blueF() - prevColor.blueF()));
        theColor.setAlphaF(prevColor.alphaF() + colorScale *(nextColor.alphaF() - prevColor.alphaF()));
    }
    return theColor;
}

GradientStopsEditor::GradientStopsEditor(QWidget* parent)
        : QWidget(parent), m_currentStop(-1), m_checkerPainter(4)
{
    // initialize the gradient with some sane values
    m_stops.append(QGradientStop(0.0, QColor(6, 30, 60, 100)));
    m_stops.append(QGradientStop(1.0, QColor(15, 40, 80, 180)));

    setMinimumSize(105, 35);
}

GradientStopsEditor::~GradientStopsEditor()
{
}

void GradientStopsEditor::setStops(const QGradientStops &stops)
{
    m_stops = stops;
    update();
}

QGradientStops GradientStopsEditor::stops() const
{
    return m_stops;
}

void GradientStopsEditor::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    const int half = height() / 2;
    m_checkerPainter.paint(painter, QRectF(1, 1, width() - 2, half - 2));

    QLinearGradient gradient;
    gradient.setStart(QPointF(1, 1));
    gradient.setFinalStop(QPointF(width() - 2, half - 2));
    gradient.setStops(m_stops);
    painter.fillRect(1, 1, width() - 2, half - 2, gradient);
    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawRect(0, 0, width()-1, half - 1);

    m_paintArea.setRect(0, half + 1, width(), half);
    painter.fillRect(m_paintArea, palette().brush(QPalette::Window));

    QPolygon arrow;
    arrow.putPoints(/*index = */ 0, /* amount = */ 3,
            -7, half, 7, half, 0, 0);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(palette().brush(QPalette::Dark));
    painter.translate(m_paintArea.x(), m_paintArea.y());
    foreach (const QGradientStop &stop, m_stops) {
        const int x = (int)(stop.first * m_paintArea.width());
        painter.translate(x, 0);
        painter.setPen(stop.second);
        painter.drawPolygon(arrow);
        painter.translate(-x, 0);
    }
}

void GradientStopsEditor::mousePressEvent(QMouseEvent *e)
{
    m_currentStop = colorStopFromPosition(QPoint(e->x(), e->y()));
    setCursor(QCursor(m_currentStop == -1 ? Qt::ArrowCursor : Qt::SizeHorCursor));
}

void GradientStopsEditor::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::RightButton && m_currentStop >= 0) {
        if (m_paintArea.contains(e->x(), e->y())) {
            int x = e->x() - m_paintArea.left();
            // check if we are still above the actual ramp point
            int r = int(m_stops[ m_currentStop ].first * m_paintArea.width());
            if ((x > r - 5) && (x < r + 5)) {
                m_stops.remove(m_currentStop);
                update();
                emit changed(m_stops);
            }
        }
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void GradientStopsEditor::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (! m_paintArea.contains(e->x(), e->y()))
        return;

    if (e->button() != Qt::LeftButton)
        return;

    if (m_currentStop >= 0) {
        // color stop hit -> change color
        int result = KColorDialog::getColor(m_stops[m_currentStop].second, this);
        if (result == KColorDialog::Accepted) {
            update();
            emit changed(m_stops);
        }
    } else if (m_currentStop == -1) {
        // no color stop hit -> new color stop
        qreal newStopPosition = static_cast<qreal>(e->x() - m_paintArea.left()) / m_paintArea.width();
        QColor newStopColor = colorAt(newStopPosition, m_stops);
        int result = KColorDialog::getColor(newStopColor, this);
        if (result == KColorDialog::Accepted) {
            m_stops.append(QGradientStop(newStopPosition, newStopColor));
            update();
            emit changed(m_stops);
        }
    }
}

void GradientStopsEditor::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::RightButton)
        return;

    // do we have a current color stop?
    if (m_currentStop >= 0 && m_currentStop < m_stops.count()) {
        int newPixelPos = e->x() - m_paintArea.left();
        int oldPixelPos = static_cast<int>(m_stops[m_currentStop].first * m_paintArea.width());
        // check if we really have changed the pixel based position
        if (newPixelPos == oldPixelPos)
            return;

        qreal newPosition = static_cast<qreal>(newPixelPos) / static_cast<qreal>(m_paintArea.width());
        m_currentStop = moveColorStop(m_currentStop, newPosition);

        update();
        emit changed(m_stops);
    } else {
        int colorStop = colorStopFromPosition(QPoint(e->x(), e->y()));
        setCursor(QCursor(colorStop == -1 ? Qt::ArrowCursor : Qt::SizeHorCursor));
    }
}

int GradientStopsEditor::moveColorStop(int stopIndex, qreal newPosition)
{
    // get old position of the color stop we want to move
    const qreal oldPosition = m_stops[stopIndex].first;

    // clip new position to [0,1] interval
    newPosition = qBound(qreal(0.0), newPosition, qreal(1.0));

    // initialize new stop index to the current index
    int newStopIndex = stopIndex;

    // we move to the left, so we swap color stops as long as our new position
    // is smaller or equal to the previous color stop position
    if (newPosition < oldPosition && m_currentStop > 0) {
        for (int i = m_currentStop - 1; i >= 0; i--) {
            const qreal prevStopPos = m_stops[i].first;
            if (newPosition <= prevStopPos && prevStopPos > 0.0) {
                qSwap(m_stops[i], m_stops[newStopIndex]);
                newStopIndex = i;
            } else
                break;
        }
    }
    // we move to the right, so we swap color stops as long as our new position
    // is greater or equal to the next color stop position
    else if (newPosition > oldPosition && m_currentStop < m_stops.count() - 1) {
        for (int i = m_currentStop + 1; i < m_stops.count(); ++i) {
            const qreal nextStopPos = m_stops[i].first;
            if (newPosition >= nextStopPos && nextStopPos < 1.0) {
                qSwap(m_stops[i], m_stops[newStopIndex]);
                newStopIndex = i;
            } else
                break;
        }
    }

    // work around qt bug: make sure we do not have color stops with same position
    if (newStopIndex < stopIndex && newPosition == m_stops[newStopIndex+1].first) {
        // if we have decreased the stop index and the new position is equal to the
        // position of the next stop we move the new position a tiny bit to the left
        newPosition -= std::numeric_limits<qreal>::epsilon();
    } else if (newStopIndex > stopIndex && newPosition == m_stops[newStopIndex-1].first) {
        // if we have increased the stop index and the new position is equal to the
        // position of the previous stop we move the new position a tiny bit to the right
        newPosition += std::numeric_limits<qreal>::epsilon();
    } else {
        // we have not changed the stop index, but we have to check if we our new
        // stop position is not equal to the stop at 0.0 or 1.0
        if (newStopIndex == 1 && newPosition == 0.0)
            newPosition += std::numeric_limits<qreal>::epsilon();
        if (newStopIndex == m_stops.count() - 2 && newPosition == 1.0)
            newPosition -= std::numeric_limits<qreal>::epsilon();
    }

    // finally set the new stop position
    m_stops[newStopIndex].first = newPosition;

    return newStopIndex;
}

int GradientStopsEditor::colorStopFromPosition(const QPoint &mousePos)
{
    if (! m_paintArea.contains(mousePos))
        return -1;

    int x = mousePos.x() - m_paintArea.left();

    for (int i = m_stops.count() - 1; i >= 0; i--) {
        int r = int(m_stops[i].first * m_paintArea.width());
        if ((x > r - 5) && (x < r + 5)) {
            // found color stop at position
            return i;
        }
    }

    return -1;
}

#include "GradientStopsEditor.moc"
