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

#ifndef STROKEFILLSELECTORS_H
#define STROKEFILLSELECTORS_H

#include <KoCheckerBoardPainter.h>
#include <QFrame>

class QEvent;
class QPaintEvent;
class QBrush;
class KShapeBorderBase;
class KShapeBackgroundBase;

/// A widget to preview stroke and fill of a shape
class StrokeFillSelector : public QFrame
{
    Q_OBJECT

public:
    /// Constructs preview widget with given parent
    explicit StrokeFillSelector(QWidget* parent = 0L);

    /// Destroys the preview widget
    ~StrokeFillSelector();

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

    /**
     * Updates the preview with the given fill and stroke
     * @param stroke the stroke to preview
     * @param fill the fill to preview
     */
    void update(KShapeBorderBase * stroke, KShapeBackgroundBase * fill);

    virtual bool eventFilter(QObject* object, QEvent* event);

    /**
     * Returns whether the stroke (true) or the fill (false) is selected.
     */
    bool strokeIsSelected() const { return m_strokeWidget; }

signals:
    /// Is emitted as soon as the stroke was changed
    void strokeChanged(const KShapeBorderBase &);
    /// Is emitted as soon as the fill was changed
    void fillChanged(const QBrush&);
    /// Is emitted as soon as the fill is selected
    void fillSelected();
    /// Is emitted as soon as the stroke is selected
    void strokeSelected();

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    void drawFill(QPainter & painter, const KShapeBackgroundBase * fill);
    void drawStroke(QPainter & painter, const KShapeBorderBase*);

    bool m_strokeWidget; ///< shows if stroke or fill is selected
    KShapeBackgroundBase * m_background; ///< the fill to preview
    KShapeBorderBase * m_stroke; ///< the stroke to preview
    QRectF m_strokeRect;
    QRectF m_fillRect;
    KoCheckerBoardPainter m_checkerPainter;
};

#endif
