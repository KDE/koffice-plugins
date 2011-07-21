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
#include "ColorChooser.h"
#include "ColorPicker.h"

#include <klocale.h>
#include <KColorBackground.h>
#include <KoColorSpaceRegistry.h>
#include <KShapeBorderBase.h>

#include <QGridLayout>

ColorChooser::ColorChooser(QSet<KShape*> shapes, SelectorWidget::FillStroke type, QWidget *parent)
    : QWidget(parent),
    m_background(0),
    m_shapeBorder(0),
    m_shapes(shapes)
{
    Q_ASSERT(!shapes.isEmpty());
    if (type == SelectorWidget::Fill)
        m_background = dynamic_cast<KColorBackground*>((*shapes.begin())->background());
    else
        m_shapeBorder = (*shapes.begin())->border();

    m_colorChooser = new ColorPicker(this, false);
    m_colorChooser->changeLayout(ColorPicker::SimpleLayout); // TODO fix the default layout to be nice

    if (m_background) {
        m_background->ref();
        m_colorChooser->setColor(KoColor(m_background->color(), KoColorSpaceRegistry::instance()->rgb8()));
    } else if (m_shapeBorder) {
        m_shapeBorder->ref();
        m_colorChooser->setColor(KoColor(m_shapeBorder->pen().color(), KoColorSpaceRegistry::instance()->rgb8()));
    }

    connect (m_colorChooser, SIGNAL(colorChanged(const KoColor&)), this, SLOT(updateColor(const KoColor&)));

    QGridLayout *lay = new QGridLayout(this);
    lay->setMargin(0);
    lay->addWidget(m_colorChooser);
    setLayout(lay);

    setMinimumWidth(194);
}

ColorChooser::~ColorChooser()
{
    if (m_background && !m_background->deref())
        delete m_background;
    if (m_shapeBorder && !m_shapeBorder->deref())
        delete m_shapeBorder;
}

void ColorChooser::updateColor(const KoColor &color)
{
    if (m_background) {
        m_background->setColor(color.toQColor());
        emit changed();
    } else if (m_shapeBorder) {
        QPen pen = m_shapeBorder->pen();
        pen.setColor(color.toQColor());
        m_shapeBorder->setPen(pen);
        emit changed();
    }
}

void ColorChooser::shapeChanged(KShape *shape, KShape::ChangeType type)
{
    if (m_shapes.contains(shape) && (type == KShape::Deleted || type == KShape::BackgroundChanged)) {
        m_shapes.remove(shape);
        if (m_shapes.isEmpty()) {
            if (m_background && !m_background->deref())
                delete m_background;
            m_background = 0;
            if (m_shapeBorder && !m_shapeBorder->deref())
                delete m_shapeBorder;
            m_shapeBorder = 0;
            m_colorChooser->setEnabled(false);
        }
    }
}

#include <ColorChooser.moc>
