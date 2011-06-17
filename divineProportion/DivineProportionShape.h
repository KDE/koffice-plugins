/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#ifndef SHAPE_H
#define SHAPE_H

#include <KShape.h>

class QPainter;

#define DivineProportionShape_SHAPEID "DivineProportionShapeID"

class DivineProportionShape : public KShape {
public:
    DivineProportionShape();
    virtual ~DivineProportionShape();

    // Define where the disappearing point is.
    enum Orientation {
        BottomRight,
        BottomLeft,
        TopRight,
        TopLeft
    };

    /// reimplemented to be empty (this shape is fully non-printing)
    void paint(QPainter &painter, const KViewConverter &converter);
    /// reimplemented
    void paintDecorations(QPainter &painter, const KViewConverter &converter, const KCanvasBase *canvas);
    /// reimplemented
    virtual void saveOdf(KShapeSavingContext & context) const;
    /// reimplemented
    virtual bool loadOdf( const KXmlElement & element, KShapeLoadingContext &context );

    void setOrientation(Orientation orientation);
    Orientation orientation() const { return m_orientation; }

    void setPrintable(bool on);
    bool printable() const { return m_printable; }

private:
    void divideHorizontal(QPainter &painter, const QRectF &rect, bool top, bool left);
    void divideVertical(QPainter &painter, const QRectF &rect, bool top, bool left);
    void draw(QPainter &painter);

    const qreal m_divineProportion;
    Orientation m_orientation;
    bool m_printable;
};

#endif
