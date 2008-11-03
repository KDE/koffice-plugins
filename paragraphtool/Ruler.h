/* This file is part of the KDE project
 * Copyright (C) 2008 Florian Merz <florianmerz@gmx.de>
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

#ifndef RULER_H
#define RULER_H

#include "RulerFragment.h"

#include <KoUnit.h>

#include <QColor>
#include <QLineF>
#include <QList>
#include <QMatrix>
#include <QObject>
#include <QString>

class QPainter;
class QPointF;

/* A Ruler manages several values like the minimum value, the maximum value
 * the current value and the old value of a ruler, which can then be used
 * by the RulerControl class to display the Ruler on the screen.
 * A Ruler can be painted at more than one place by using multiple
 * RulerControl instances with a single Ruler instance.
 */
class Ruler : public QObject
{
    Q_OBJECT
public:
    Ruler(QObject *parent = NULL);

    ~Ruler() {}

    KoUnit unit() const {
        return m_unit;
    }
    void setUnit(KoUnit unit);

    qreal value() const;
    QString valueString() const;
    void setValue(qreal value);
    void moveTo(qreal value, bool smooth);
    void resetValue();

    qreal oldValue() const {
        return m_oldValue;
    }
    void setOldValue(qreal value) {
        m_oldValue = value;
    }

    // distance in points between steps when moving the ruler,
    // set this to 0.0 to disable stepping
    qreal stepValue() const {
        return m_stepValue;
    }
    void setStepValue(qreal stepValue) {
        m_stepValue = stepValue;
    }

    // the ruler cannot be dragged to a value lower than this value
    qreal minimumValue() const;
    void setMinimumValue(qreal value);

    // the value cannot be dragged to a value higher than this value
    qreal maximumValue() const;
    void setMaximumValue(qreal value);

    // these options specify how the ruler will be drawn
    enum Options {
        noOptions = 0,
        drawSides = 1 << 0
    };

    int options() const {
        return m_options;
    }
    void setOptions(int options) {
        m_options = m_options | options;
    }

    bool isActive() const {
        return m_active;
    }
    void setActive(bool active);

    bool isFocused() const {
        return m_focused;
    }
    void setFocused(bool focused);

    bool isHighlighted() const {
        return m_highlighted;
    }
    void setHighlighted(bool highlighted);

    bool isEnabled() const {
        return m_enabled;
    }
    void setEnabled(bool enabled);

    bool isVisible() const;

    void increaseByStep();
    void decreaseByStep();

    QColor activeColor() const {
        return QColor(100, 148, 255);
    }
    QColor highlightColor() const {
        return QColor(78, 117, 201);
    }
    QColor normalColor() const {
        return QColor(100, 100, 100);
    }
    QColor focusColor() const {
        return QColor(100, 148, 255);
    }

    void addFragment(RulerFragment fragment) {
        fragment.setRuler(this);
        m_fragments.append(fragment);
    }

    void clearFragments() {
        m_fragments.clear();
    }

    RulerFragment *hitTest(const QPointF &point);

    void paint(QPainter &painter) const;

    QString name() const {
        return m_name;
    }
    
    void setName(const QString& name) {
        m_name = name;
    }

signals:
    // emitted when value has been changed via the user interface
    // (in contrast to via the setValue() method)
    void valueChanged(qreal value);

    // emitted when the ruler needs to be repainted
    void needsRepaint();

private:
    QList<RulerFragment> m_fragments;
    QString m_name;

    // the value of a ruler is the distance between the baseline and
    // the ruler line (the one that can be dragged)
    // all values are always measured in points
    qreal m_value;
    qreal m_oldValue;
    qreal m_stepValue;
    qreal m_minValue;
    qreal m_maxValue;
    KoUnit m_unit;

    bool m_active;
    bool m_focused;
    bool m_highlighted;
    bool m_enabled;
    int m_options;
};

#endif

