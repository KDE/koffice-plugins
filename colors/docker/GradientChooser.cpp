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
#include "GradientChooser.h"
#include "GradientEditor.h"

#include <klocale.h>
#include <KGradientBackground.h>
#include <KShapeBorderBase.h>

#include <QGridLayout>

GradientChooser::GradientChooser(QSet<KShape*> shapes, SelectorWidget::FillStroke type, QWidget *parent)
    : QWidget(parent),
    m_background(0),
    m_shapeBorder(0),
    m_shapes(shapes)
{
    Q_ASSERT(!shapes.isEmpty());
    if (type == SelectorWidget::Fill)
        m_background = dynamic_cast<KGradientBackground*>((*shapes.begin())->background());
    else
        m_shapeBorder = (*shapes.begin())->border();
    m_gradientEditor = new GradientEditor(this);

    if (m_background) {
        m_background->ref();
        m_gradientEditor->setGradientEdit(const_cast<QGradient*>(m_background->gradient()));
    } else if (m_shapeBorder) {
        m_shapeBorder->ref();
        m_brush = m_shapeBorder->pen().brush();
        m_gradientEditor->setGradientEdit(const_cast<QGradient*>(m_brush.gradient()));
    }
    QGridLayout *lay = new QGridLayout(this);
    lay->setMargin(0);
    lay->addWidget(m_gradientEditor);
    setLayout(lay);

    connect (m_gradientEditor, SIGNAL(changed()), this, SIGNAL(changed()));
    connect (m_gradientEditor, SIGNAL(changedGradientType(const QGradient&)),
        this, SLOT(gradientChanged(const QGradient&)));
}

GradientChooser::~GradientChooser()
{
    if (m_background && !m_background->deref())
        delete m_background;
    if (m_shapeBorder && !m_shapeBorder->deref())
        delete m_shapeBorder;
}

void GradientChooser::gradientChanged(const QGradient &gradient)
{
    if (m_background) {
        m_background->setGradient(gradient);

        // the const_cast here is essentially a dirty trick to edit the gradient without
        // having to do a 'new' for every user action.  Its save since we did a rev() on
        // the background and we are the only ones that will edit the background anyway.
        m_gradientEditor->setGradientEdit(const_cast<QGradient*>(m_background->gradient()));
    } else if (m_shapeBorder) {
        QPen pen = m_shapeBorder->pen();
        pen.setBrush(gradient);
        m_shapeBorder->setPen(pen);
        m_brush = QBrush(gradient);
        m_gradientEditor->setGradientEdit(const_cast<QGradient*>(m_brush.gradient()));
    }
}

void GradientChooser::shapeChanged(KShape *shape, KShape::ChangeType type)
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
            m_gradientEditor->setEnabled(false);
        }
    }
}

#include <GradientChooser.moc>
