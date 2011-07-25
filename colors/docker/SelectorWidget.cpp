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

#include "SelectorWidget.h"
#include "ColorChooser.h"
#include "GradientChooser.h"
#include "PatternChooser.h"
#include "LineStrokes.h"

#include <KCanvasBase.h>
#include <KColorBackground.h>
#include <KDebug>
#include <KGradientBackground.h>
#include <KImageCollection.h>
#include <KLineBorder.h>
#include <KoFlake.h>
#include <KPatternBackground.h>
#include <KResourceManager.h>
#include <KSelection.h>
#include <KShapeBorderRegistry.h>
#include <KShapeController.h>
#include <KShapeManager.h>

#include <QMenu>
#include <QShowEvent>

SelectorWidget::SelectorWidget(QWidget *parent)
    : QWidget(parent),
    m_type(TypeNoFill),
    m_state(Fill),
    m_current(0),
    m_shapeSelection(0),
    m_imageCollection(0),
    m_showStrokeWidget(false)
{
    widget.setupUi(this);

    QMenu *menu = new QMenu(widget.typePopup);
    QAction *action = menu->addAction(i18n("Stroke"));
    connect (action, SIGNAL(triggered()), this, SLOT(switchToStroke()));
    action = menu->addAction(i18n("Fill"));
    connect (action, SIGNAL(triggered()), this, SLOT(switchToFill()));
    widget.typePopup->setMenu(menu);

    switchToFill();
    shapeSelectionChanged();

    connect (widget.selector, SIGNAL(fillSelected()), this, SLOT(switchToFill()));
    connect (widget.selector, SIGNAL(strokeSelected()), this, SLOT(switchToStroke()));
    connect (widget.typeChoice, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    connect (widget.transparencySlider, SIGNAL(valueChanged(int)), this, SLOT(setShapeTransparency(int)));
}

void SelectorWidget::switchToFill()
{
    widget.type->setText(i18n("Fill"));
    widget.tabWidget->setTabText(1, i18n("Fill"));
    m_state = Fill;

    shapeSelectionChanged(); // force refresh.
}

void SelectorWidget::switchToStroke()
{
    widget.type->setText(i18n("Stroke"));
    widget.tabWidget->setTabText(1, i18n("Color"));
    m_state = Stroke;

    shapeSelectionChanged(); // force refresh.
}

void SelectorWidget::setType(int type)
{
    Type newType = static_cast<Type>(type);
    if (m_type == newType)
        return;
    m_type = newType;
    widget.typeChoice->setCurrentIndex(type);

    repaintAllShapes();
    foreach (KShape *shape, m_openShapes) {
        switch (m_type) {
        case TypeNoFill:
            switch (m_state) {
            case Fill:
                if (shape->background()) {
                    shape->setBackground(0);
                }
                break;
            case Stroke:
                m_showStrokeWidget = false;
                if (shape->border()) {
                    update();
                    shape->setBorder(0);
                }
            }
            break;
        case TypeColors:
            switch (m_state) {
            case Fill:
                if (dynamic_cast<KColorBackground*>(shape->background()) == 0) {
                    shape->setBackground(new KColorBackground(QColor(0x79, 0xE3, 0xA0)));
                    shape->update();
                }
                break;
            case Stroke: {
                KShapeBorderBase *border = shape->border();
                if (border == 0) {
                    shape->setBorder(new KLineBorder());
                } else {
                    QPen pen = border->pen();
                    pen.setBrush(Qt::SolidPattern);
                    border->setPen(pen);
                    shape->update();
                }
                break;
            }
            }
            break;
        case TypeGradients: {
            QLinearGradient gradient(0.3, 0.5, 1, 0.5);
            gradient.setColorAt(0, QColor(10, 40, 80)); // default
            gradient.setColorAt(0.8, QColor(25, 80, 160));
            gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
            switch (m_state) {
            case Fill:
                if (dynamic_cast<KGradientBackground*>(shape->background()) == 0) {
                    shape->setBackground(new KGradientBackground(gradient));
                    shape->update();
                }
                break;
            case Stroke: {
                QPen pen;
                KShapeBorderBase *border = 0;
                if (shape->border()) {
                    pen = shape->border()->pen();
                    KShapeBorderFactoryBase *factory = KShapeBorderRegistry::instance()->get(shape->border()->borderId());
                    if (factory)
                        border = factory->createBorder(shape);
                }
                if (border == 0)
                    border = new KLineBorder();
                pen.setBrush(gradient);
                border->setPen(pen);
                shape->setBorder(border);
                shape->update();
                break;
            }
            }
            break;
        }
        case TypePatterns:
            switch (m_state) {
            case Fill:
                if (dynamic_cast<KPatternBackground*>(shape->background()) == 0) {
                    Q_ASSERT(m_imageCollection);
                    KPatternBackground *bg = new KPatternBackground(m_imageCollection);
                    bg->setPatternDisplaySize(shape->size());
                    shape->setBackground(bg);
                    shape->update();
                }
                break;
            case Stroke: {
            /* TODO
                KShapeBorderBase *border = shape->border();
                if (border == 0) {
                    border = new KLineBorder();
                    shape->setBorder(border);
                }
                QPen pen = border->pen();
                pen.setBrush(QBrush::SolidPattern);
                border->setPen(pen);
            */
                break;
            }
            }
            break;
        }
    }
    repaintAllShapes();
    updateUi();
}

void SelectorWidget::shapeSelectionChanged()
{
    m_openShapes.clear();
    delete m_current;
    m_current = 0;
    m_showStrokeWidget = false;
    if (isVisible() == false || m_shapeSelection == 0) {
        setEnabled(false);
        return;
    }

    const QList<KShape*> selectedShapes = m_shapeSelection->selectedShapes(KoFlake::StrippedSelection);
    if (selectedShapes.isEmpty()) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
    // TODO use all shapes.
    //  based on the type I think we can check for all shapes that have the same background
    //   and/or border and group them.  If we end up with more than one group we can show
    //   a widget with all the different groups on the second tab and let the user choose one.
    //   Each child widget should thus know of only one background or border, BUT it can be that
    //   more than one shape owns it.
    foreach (KShape* shape, selectedShapes) {
        widget.selector->update(shape->border(), shape->background());
        if (m_state == Fill) {
            KShapeBackgroundBase *background = shape->background();
            if (background == 0)
                setType(TypeNoFill);
            else if (dynamic_cast<KColorBackground*>(background))
                setType(TypeColors);
            else if (dynamic_cast<KGradientBackground*>(background))
                setType(TypeGradients);
            else if (dynamic_cast<KPatternBackground*>(background))
                setType(TypePatterns);
        } else {
            KShapeBorderBase *border = shape->border();
            if (border == 0) {
                setType(TypeNoFill);
            } else {
                // find out what to put in the second tab.
                QPen pen = border->pen();
                switch (pen.brush().style()) {
                case Qt::LinearGradientPattern:
                case Qt::ConicalGradientPattern:
                case Qt::RadialGradientPattern:
                    setType(TypeGradients);
                    break;
                case Qt::TexturePattern:
                    setType(TypePatterns);
                    break;
                default:
                    setType(TypeColors);
                    break;
                }

                // find out if our border wants the pen-strokes configurable.
                KShapeBorderFactoryBase *factory = KShapeBorderRegistry::instance()->get(border->borderId());
                if (factory && factory->penStrokeConfigurable())
                    m_showStrokeWidget = true;

                // TODO get a border-specific UI for an extra tab.
            }
        }
        widget.transparencySlider->setValue(qRound(shape->transparency() * 100.));
        m_openShapes << shape;
        break;// only use first one for now...
    }

    updateUi();
}

void SelectorWidget::updateUi()
{
    delete m_current;
    m_current = 0;
    while (widget.tabWidget->count() > 2)
        widget.tabWidget->removeTab(2);
    if (m_shapeManager == 0)
        return;
    if (m_openShapes.isEmpty())
        return;

    switch (m_type) {
    case TypeNoFill:
        break;
    case TypeColors:
        m_current = new ColorChooser(m_openShapes, m_state, this);
        break;
    case TypeGradients:
        m_current = new GradientChooser(m_openShapes, m_state, this);
        break;
    case TypePatterns:
        if (m_state == Fill) // PatternChooser on a stroke is currently not supported. (saving won't work for instance)
            m_current = new PatternChooser(m_openShapes, this);
        break;
    }
    if (m_current) {
        connect (m_current, SIGNAL(changed()), this, SLOT(repaintAllShapes()));
        connect (m_shapeManager, SIGNAL(notifyShapeChanged(KShape*,KShape::ChangeType)),
                m_current, SLOT(shapeChanged(KShape*,KShape::ChangeType)));
        widget.contentsLayout->addWidget(m_current);
    }

    if (m_showStrokeWidget) {
        LineStrokes *lineStrokes = new LineStrokes(m_openShapes, this);
        widget.tabWidget->addTab(lineStrokes, i18n("Stroke"));
        connect (lineStrokes, SIGNAL(changed()), this, SLOT(repaintAllShapes()));
    }
}

void SelectorWidget::setCanvas(KCanvasBase *canvas)
{
    Q_ASSERT(canvas);
    KSelection *selection = canvas->shapeManager()->selection();
    m_shapeManager = canvas->shapeManager();
    Q_ASSERT(selection);
    if (m_shapeSelection)
        disconnect (m_shapeSelection, SIGNAL(selectionChanged()), this, SLOT(shapeSelectionChanged()));
    m_shapeSelection = selection;
    Q_ASSERT(canvas->shapeController());
    KResourceManager *man = canvas->shapeController()->resourceManager();
    Q_ASSERT(man);
    m_imageCollection = man->imageCollection();
    Q_ASSERT(m_imageCollection);

    connect (m_shapeSelection, SIGNAL(selectionChanged()), this, SLOT(shapeSelectionChanged()));
    shapeSelectionChanged();
}

void SelectorWidget::clearCanvas()
{
    m_shapeSelection = 0;
    m_imageCollection = 0;
    m_shapeManager = 0;
}

void SelectorWidget::repaintAllShapes()
{
    bool first = true;
    foreach (KShape *shape, m_openShapes) {
        shape->update();
        if (first) {
            widget.selector->update(shape->border(), shape->background());
            first = false;
        }
    }
}

void SelectorWidget::setShapeTransparency(int percent)
{
    foreach (KShape *shape, m_openShapes) {
        shape->setTransparency(percent / 100.);
        shape->update();
    }
}

void SelectorWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous() && m_shapeSelection)
        shapeSelectionChanged();
}

#include <SelectorWidget.moc>
