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

#include "PatternChooser.h"

#include <KPatternBackground.h>
#include <KShape.h>
#include <KoResourceServerAdapter.h>
#include <KoResourceItemChooser.h>
#include <KoPattern.h>
#include <KoResourceServerProvider.h>


PatternChooser::PatternChooser(QSet<KShape*> shapes, QWidget *parent)
    : QWidget(parent),
    m_shapes(shapes)
{
    setObjectName("PatternChooser");
    Q_ASSERT(!shapes.isEmpty());
    m_background = dynamic_cast<KPatternBackground*>((*shapes.begin())->background());
    widget.setupUi(this);

    Q_ASSERT(m_background);
    m_background->ref();
    widget.editCheckbox->setEnabled(!m_background->pattern().isNull());

    KoAbstractResourceServerAdapter* adapter = new KoResourceServerAdapter<KoPattern>(
            KoResourceServerProvider::instance()->patternServer());
    KoResourceItemChooser *chooser = new KoResourceItemChooser(adapter, widget.selectionPage);
    widget.selectionPageLayout->addWidget(chooser);

    connect (widget.repeatType, SIGNAL(currentIndexChanged(int)), this, SLOT(setRepeatType(int)));
    connect (widget.zoomToFit, SIGNAL(clicked()), this, SLOT(zoomToFit()));
    connect (widget.zoomIn, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect (widget.zoomOut, SIGNAL(clicked()), this, SLOT(zoomOut()));
    connect (chooser, SIGNAL(resourceSelected(KoResource*)),
            this, SLOT(setPattern(KoResource*)));
    connect (widget.editCheckbox, SIGNAL(clicked(bool)), this, SLOT(setEditMode(bool)));

    // TODO store editCheckbox state persistently
    setEditMode(false);
}

PatternChooser::~PatternChooser()
{
    if (m_background && !m_background->deref())
        delete m_background;
}

void PatternChooser::setRepeatType(int index)
{
    if (index == 0)
        m_background->setRepeat(KPatternBackground::Original);
    else if (index == 1)
        m_background->setRepeat(KPatternBackground::Tiled);
    else if (index == 2)
        m_background->setRepeat(KPatternBackground::Stretched);

    widget.sizeGroupBox->setEnabled(index == 1); // only useful if tiled

    emit changed();
}

void PatternChooser::zoomToFit()
{
    Q_ASSERT(m_background->repeat() == KPatternBackground::Tiled);
    if (m_shapes.isEmpty())
        return;
    m_background->setReferencePointOffset(QPointF());

    QSizeF size((*m_shapes.begin())->size());
    if (widget.aspectButton->keepAspectRatio()) {
        if (size.width() > size.height())
            size.setWidth(size.height());
        else
            size.setHeight(size.width());
    }

    m_background->setPatternDisplaySize(size);
    emit changed();
}

void PatternChooser::zoomIn()
{
    QSizeF size = m_background->patternDisplaySize();
    m_background->setPatternDisplaySize(QSizeF(size.width() * 1.25, size.height() * 1.25));
    emit changed();
}

void PatternChooser::zoomOut()
{
    QSizeF size = m_background->patternDisplaySize();
    m_background->setPatternDisplaySize(QSizeF(size.width() * 0.8, size.height() * 0.8));
    emit changed();
}

void PatternChooser::setPattern(KoResource *resource)
{
    Q_ASSERT(resource);
    KoPattern *kopattern = dynamic_cast<KoPattern*>(resource);
    Q_ASSERT(kopattern);
    m_background->setPattern(kopattern->image());
    widget.editCheckbox->setEnabled(true);

    emit changed();
}

void PatternChooser::setEditMode(bool on)
{
    widget.editCheckbox->setChecked(on);
    widget.stackedWidget->setCurrentIndex(on ? 0 : 1);
}

void PatternChooser::shapeChanged(KShape *shape, KShape::ChangeType type)
{
    if (!m_shapes.contains(shape))
        return;
    if (type == KShape::Deleted) {
        m_shapes.remove(shape);
    } else if (type == KShape::BackgroundChanged) {
        if (m_background && !m_background->deref())
            delete m_background;
        m_background = dynamic_cast<KPatternBackground*>(shape->background());
        if (m_background)
            m_background->ref();
    }
}
