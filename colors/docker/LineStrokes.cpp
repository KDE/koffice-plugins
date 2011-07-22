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
#include "LineStrokes.h"

#include <KIcon>
#include <QPen>

LineStrokes::LineStrokes(QWidget *parent)
    : QWidget(parent),
    m_caps(new QButtonGroup(this)),
    m_joins(new QButtonGroup(this))
{
    widget.setupUi(this);

    m_caps->addButton(widget.squareCap, Qt::SquareCap);
    m_caps->addButton(widget.roundCap, Qt::FlatCap);
    m_caps->addButton(widget.flatCap, Qt::RoundCap);
    widget.squareCap->setIcon(KIcon("cap_square"));
    widget.roundCap->setIcon(KIcon("cap_round"));
    widget.flatCap->setIcon(KIcon("cap_butt"));

    m_joins->addButton(widget.roundJoin, Qt::RoundJoin);
    m_joins->addButton(widget.miterJoin, Qt::MiterJoin);
    m_joins->addButton(widget.bevelJoin, Qt::BevelJoin);
    widget.roundJoin->setIcon(KIcon("join_round"));
    widget.miterJoin->setIcon(KIcon("join_miter"));
    widget.bevelJoin->setIcon(KIcon("join_bevel"));

    connect (widget.style, SIGNAL(currentIndexChanged(int)), this, SLOT(setLineStyle(int)));
    connect (m_caps, SIGNAL(buttonClicked(int)), this, SLOT(setCaps(int)));
    connect (m_joins, SIGNAL(buttonClicked(int)), this, SLOT(setJoins(int)));
}

void LineStrokes::setUnit(const KUnit &unit)
{
    widget.widthSpinbox->setUnit(unit);
}

void LineStrokes::setPen(const QPen &pen)
{
    m_caps->button(pen.capStyle())->setChecked(true);
    m_joins->button(pen.joinStyle())->setChecked(true);
    widget.widthSpinbox->changeValue(pen.widthF());
    widget.style->setCurrentIndex(pen.style());
    widget.miterLimit->setValue(pen.miterLimit() * 100);
}

void LineStrokes::setLineStyle(int index)
{
}

void LineStrokes::setCaps(int)
{
}

void LineStrokes::setJoins(int)
{
}
