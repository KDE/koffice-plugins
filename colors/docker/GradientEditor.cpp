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

#include "GradientEditor.h"

#include <KoResourceServerAdapter.h>
#include <KoAbstractGradient.h>
#include <KoResourceServerProvider.h>
#include <KoResourceItemChooser.h>

#include <QGridLayout>

GradientEditor::GradientEditor(QWidget* parent)
    : QWidget(parent),
    m_type(QGradient::LinearGradient),
    m_spread(QGradient::PadSpread),
    m_gradientEdit(0)
{
    setObjectName("GradientEditor");
    widget.setupUi(this);

    KoAbstractResourceServerAdapter *adapter = new KoResourceServerAdapter<KoAbstractGradient>
        (KoResourceServerProvider::instance()->gradientServer());
    KoResourceItemChooser *chooser = new KoResourceItemChooser(adapter, widget.selectionPage);
    chooser->showButtons(false);
    widget.selectionPageLayout->addWidget(chooser, 0, 0);
    //chooser->setObjectName("GradientChooser");
    //chooser->setColumnCount(1);

    connect (widget.editCheckbox, SIGNAL(clicked(bool)), this, SLOT(setEditMode(bool)));
    connect (widget.gradientEditor, SIGNAL(changed(const QGradientStops&)),
            this, SLOT(updateStops(const QGradientStops&)));
    connect (widget.gradientType, SIGNAL(currentIndexChanged(int)), this, SLOT(setGradientType(int)));
    connect (widget.gradientRepeat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setGradientRepeat(int)));
    connect(chooser, SIGNAL(resourceSelected(KoResource *)),
            this, SLOT(setGradient(KoResource *)));

    // TODO store editCheckbox state persistently
    setEditMode(false);

}

GradientEditor::~GradientEditor()
{
}

void GradientEditor::setGradientEdit(QGradient *gradient)
{
    m_gradientEdit = 0;
    if (gradient) {
        setGradientRepeat((int) (gradient->spread()));
        widget.gradientEditor->setStops(gradient->stops());
        setGradientType((int) (gradient->type()));
    }
    m_gradientEdit = gradient;
}

void GradientEditor::setEditMode(bool on)
{
    widget.editCheckbox->setChecked(on);
    widget.stackedWidget->setCurrentIndex(on ? 0 : 1);
}

void GradientEditor::setGradientType(int type)
{
    QGradient::Type t = static_cast<QGradient::Type>(type);
    if (m_type == t)
        return;
    QPointF oldStart(0.3, 0.5);
    if (m_gradientEdit) {
        switch (m_gradientEdit->type()) {
        case QGradient::LinearGradient:
            oldStart = static_cast<QLinearGradient*>(m_gradientEdit)->start();
            break;
        case QGradient::RadialGradient:
            oldStart = static_cast<QRadialGradient*>(m_gradientEdit)->center();
            break;
        case QGradient::ConicalGradient:
            oldStart = static_cast<QConicalGradient*>(m_gradientEdit)->center();
            break;
        default: break;
        }
    }
    m_type = t;
    widget.gradientType->setCurrentIndex(type);
    // since this is a differnt type; I have to delete the m_gradientEdit
    if (m_gradientEdit) {
        m_gradientEdit = 0; // we don't own it. Don't delete it!

        QGradient *gradient = 0;
        switch (m_type) {
        case QGradient::LinearGradient:
            gradient = new QLinearGradient(oldStart.x(), oldStart.y(), 1., 0.5);
            break;
        case QGradient::RadialGradient:
            gradient = new QRadialGradient(oldStart.x(), oldStart.y(), 0.5);
            break;
        case QGradient::ConicalGradient:
            gradient = new QConicalGradient(oldStart.x(), oldStart.y(), 45.);
            break;
        case QGradient::NoGradient:
            Q_ASSERT(0);
            return;
        }
        gradient->setSpread(m_spread);
        gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
        gradient->setStops(widget.gradientEditor->stops());
        emit changedGradientType(*gradient);
        delete gradient;
        emit changed();
    }
}

void GradientEditor::setGradientRepeat(int repeat)
{
    QGradient::Spread s = static_cast<QGradient::Spread>(repeat);
    if (m_spread == s)
        return;
    m_spread = s;
    widget.gradientRepeat->setCurrentIndex(repeat);
    if (m_gradientEdit) {
        m_gradientEdit->setSpread(m_spread);
        emit changed();
    }
}

void GradientEditor::updateStops(const QGradientStops &stops)
{
    if (m_gradientEdit) {
        m_gradientEdit->setStops(stops);
        emit changed();
    }
}

void GradientEditor::setGradient(KoResource *resource)
{
    Q_ASSERT(resource);
    KoAbstractGradient *gradientResource = dynamic_cast<KoAbstractGradient*>(resource);
    Q_ASSERT(gradientResource);
    if (!gradientResource)
        return;

    QGradient *gradient = gradientResource->toQGradient();
    updateStops(gradient->stops());
    widget.gradientEditor->setStops(gradient->stops());
    delete gradient;
}

#include "GradientEditor.moc"
