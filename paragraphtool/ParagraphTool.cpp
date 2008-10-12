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

#include "ParagraphTool.h"
#include "dialogs/OptionWidget.h"

#include <KoCanvasBase.h>
#include <KoParagraphStyle.h>
#include <KoPointerEvent.h>
#include <KoShape.h>
#include <KoShapeBorderModel.h>
#include <KoShapeManager.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoUnit.h>
#include <KoViewConverter.h>

#include <KDebug>
#include <KAction>
#include <KLocalizedString>

#include <QAbstractTextDocumentLayout>
#include <QCheckBox>
#include <QColor>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLine>
#include <QVectorIterator>

/* FIXME:
 * - tab iterates over all rulers, maybe try to restrict to visible rulers
 *
 * TODO:
 * - add undo support (QTextDocument::undo(QTextCursor*) could help)
 * - add accessibility support
 * - remove hard-coded colors
 *   (use KDE theme or adjust to document/shape background?)
 * - add RTL support
 * - add linespacing support
 * - add more feature via the options docker
 * - add proper relayouting
 *   (the tools layouting doesn't flow around other shapes)
 * - highlight the paragraph over which the mouse is currently hovering
 *   (best would be if this included the top and bottom margins, too)
 * - think about a method to give instructions to the users
 *   (the bubble used by okular might be a good way to do this)
 */
static bool shapeContainsBlock(const KoShape *shape, QTextBlock textBlock)
{
    QTextLayout *layout = textBlock.layout();
    qreal blockStart = layout->lineAt(0).y();

    QTextLine endLine = layout->lineAt(layout->lineCount() - 1);
    qreal blockEnd = endLine.y() + endLine.height();

    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
    if (textShapeData == NULL) {
        return false;
    }

    qreal shapeStart = textShapeData->documentOffset();
    qreal shapeEnd = shapeStart + shape->size().height();

    return (blockEnd >= shapeStart && blockStart < shapeEnd);
}

ParagraphTool::ParagraphTool(KoCanvasBase *canvas)
        : KoTool(canvas),
        m_paragraphHighlighter(this, canvas),
        m_paragraphStyle(NULL),
        m_activeRuler(noRuler),
        m_focusedRuler(noRuler),
        m_highlightedRuler(noRuler),
        m_needsRepaint(false),
        m_smoothMovement(false)
{
    initializeRuler(m_rulers[firstIndentRuler]);
    initializeRuler(m_rulers[followingIndentRuler]);
    initializeRuler(m_rulers[rightMarginRuler]);
    initializeRuler(m_rulers[topMarginRuler], Ruler::drawSides);
    initializeRuler(m_rulers[bottomMarginRuler], Ruler::drawSides);

    KAction *action = new KAction(i18n("Apply parent style to ruler"), this);
    action->setShortcut(Qt::ALT + Qt::CTRL + Qt::Key_P);
    addAction("apply_parent_style_to_active_ruler", action);
    connect(action, SIGNAL(triggered()), this, SLOT(applyParentStyleToActiveRuler()));
}

ParagraphTool::~ParagraphTool()
{}

// helper function to initialize rulers
void ParagraphTool::initializeRuler(Ruler &ruler, int options)
{
    ruler.setParent(this);
    ruler.setOptions(options);
    ruler.setUnit(canvas()->unit());
    ruler.setMinimumValue(0.0);
    connect(&ruler, SIGNAL(needsRepaint()),
            this, SLOT(scheduleRepaint()));
    connect(&ruler, SIGNAL(valueChanged(qreal)),
            this, SLOT(updateLayout()));
}

QWidget *ParagraphTool::createOptionWidget()
{
    OptionWidget *optionWidget = new OptionWidget();

    connect(this, SIGNAL(styleNameChanged(const QString&)), optionWidget, SLOT(setStyleName(const QString &)));

    connect(this, SIGNAL(smoothMovementChanged(bool)), optionWidget, SLOT(setSmoothMovement(bool)));
    connect(optionWidget, SIGNAL(smoothMovementChanged(bool)), this, SLOT(setSmoothMovement(bool)));

    return optionWidget;
}

void ParagraphTool::loadRulers()
{
    m_rulers[firstIndentRuler].setValue(m_paragraphStyle->leftMargin() + m_paragraphStyle->textIndent());
    m_rulers[followingIndentRuler].setValue(m_paragraphStyle->leftMargin());
    m_rulers[rightMarginRuler].setValue(m_paragraphStyle->rightMargin());
    m_rulers[topMarginRuler].setValue(m_paragraphStyle->topMargin());
    m_rulers[bottomMarginRuler].setValue(m_paragraphStyle->bottomMargin());

    scheduleRepaint();
}


void ParagraphTool::saveRulers()
{
    m_paragraphStyle->setLeftMargin(m_rulers[followingIndentRuler].value());
    m_paragraphStyle->setRightMargin(m_rulers[rightMarginRuler].value());
    m_paragraphStyle->setTopMargin(m_rulers[topMarginRuler].value());
    m_paragraphStyle->setBottomMargin(m_rulers[bottomMarginRuler].value());
    m_paragraphStyle->setTextIndent(m_rulers[firstIndentRuler].value() - m_rulers[followingIndentRuler].value());

    QTextBlockFormat format;
    m_paragraphStyle->applyStyle(format);

    m_activeCursor.mergeBlockFormat(format);
}

QString ParagraphTool::styleName()
{
    KoParagraphStyle *style = m_paragraphStyle;
    while (style != NULL) {
        QString name = style->name();
        if (!name.isNull() && !name.isEmpty()) {
            return name;
        }
        style = style->parentStyle();
    }

    return QString(i18n("None"));
}

void ParagraphTool::paintLabel(QPainter &painter, const KoViewConverter &converter) const
{
    RulerIndex ruler;
    RulerFragment *rulerFragment;
    QColor foregroundColor;

    if (hasActiveRuler()) {
        ruler = m_activeRuler;
        rulerFragment = m_activeRulerFragment;
        foregroundColor = m_rulers[ruler].activeColor();
    } else if (hasHighlightedRuler()) {
        ruler = m_highlightedRuler;
        rulerFragment = m_highlightedRulerFragment;
        foregroundColor = m_rulers[ruler].highlightColor();
    } else {
        return;
    }

    painter.save();

    QLineF unmapped(rulerFragment->labelConnector());
    QLineF connector(converter.documentToView(unmapped.p1()), converter.documentToView(unmapped.p2()));
    connector.setLength(10.0);

    QString text(m_rulers[ruler].valueString());

    painter.setBrush(Qt::white);
    painter.setPen(foregroundColor);

    QRectF label(connector.p2().x(), connector.p2().y(), 0.0, 0.0);
    label = painter.boundingRect(label, Qt::AlignCenter | Qt::AlignVCenter, text);
    label.adjust(-4.0, 0.0, 4.0, 0.0);

    // adjust label so that the connector ends on the edge of the label
    if (abs(connector.dx()) > abs(connector.dy())) {
        qreal halfWidth = connector.dy() < 0.0 ? label.width() / 2.0 : -label.width() / 2.0;
        label.adjust(halfWidth, 0.0, halfWidth, 0.0);
    } else {
        qreal halfHeight = connector.dy() >= 0.0 ? label.height() / 2.0 : -label.height() / 2.0;
        label.adjust(0.0, halfHeight, 0.0, halfHeight);
    }

    painter.drawLine(connector);
    painter.drawRoundRect(label, 720.0 / label.width(), 720.0 /  label.height());
    painter.drawText(label, Qt::AlignHCenter | Qt::AlignVCenter, text);

    painter.restore();
}

void ParagraphTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    m_needsRepaint = false;

    if (hasActiveTextBlock()) {
        painter.save();

        // transform painter from view coordinate system to document coordinate system
        QPointF trans = converter.documentToView(QPointF(1.0, 1.0));
        QMatrix matrix = QMatrix().translate(trans.x(), trans.y());
        painter.setMatrix(matrix * painter.matrix());
        KoShape::applyConversion(painter, converter);
        painter.setPen(Qt::darkGray);

        foreach (const ParagraphFragment &fragment, m_fragments) {
            fragment.paint(painter);
        }

        for (int ruler = 0; ruler != maxRuler; ++ruler) {
            m_rulers[ruler].paint(painter);
        }

        painter.restore();

        paintLabel(painter, converter);
    }

    if (m_paragraphHighlighter.hasActiveTextBlock()
            && m_paragraphHighlighter.textBlock() != textBlock()) {
        m_paragraphHighlighter.paint(painter, converter);
    }
}

void ParagraphTool::repaintDecorations()
{
    if (!needsRepaint() && !m_paragraphHighlighter.needsRepaint()) {
        return;
    }

    // TODO: should add previous and current label positions to the
    // repaint area
    QRectF repaintRectangle = m_storedRepaintRectangle;
    m_storedRepaintRectangle = QRectF();
    foreach(const ParagraphFragment &fragment, m_fragments) {
        QRectF boundingRect(QPointF(0, 0), fragment.shape()->size());

        if (fragment.shape()->border()) {
            KoInsets insets;
            fragment.shape()->border()->borderInsets(fragment.shape(), insets);
            boundingRect.adjust(-insets.left, -insets.top, insets.right, insets.bottom);
        }

        // adjust for arrow heads and label
        // (although we can't be sure about the label)
        boundingRect.adjust(-50.0, -50.0, 50.0, 50.0);

        boundingRect = fragment.shape()->absoluteTransformation(0).mapRect(boundingRect);

        m_storedRepaintRectangle |= boundingRect;
    }
    repaintRectangle |= m_paragraphHighlighter.dirtyRectangle();
    repaintRectangle |= m_storedRepaintRectangle;

    canvas()->updateCanvas(repaintRectangle);
}

void ParagraphTool::scheduleRepaint()
{
    m_needsRepaint = true;
}

bool ParagraphTool::needsRepaint() const
{
    return m_needsRepaint;
}

bool ParagraphTool::createFragments()
{
    m_fragments.clear();
    m_rulers[firstIndentRuler].clearFragments();
    m_rulers[followingIndentRuler].clearFragments();
    m_rulers[rightMarginRuler].clearFragments();
    m_rulers[topMarginRuler].clearFragments();
    m_rulers[bottomMarginRuler].clearFragments();

    KoTextDocumentLayout *layout = static_cast<KoTextDocumentLayout*>(textBlock().document()->documentLayout());
    if (layout == NULL) {
        return false;
    }

    QList<KoShape*> shapes = layout->shapes();
    foreach(KoShape *shape, shapes) {
        if (shapeContainsBlock(shape, textBlock())) {
            m_fragments << ParagraphFragment(m_rulers, shape, textBlock(), m_paragraphStyle);
        }
    }

    return true;
}

/* slot which is called when the value of one of the rulers changed
 * causes storing and reloading of the positions and values of the rulers
 * from the file */
void ParagraphTool::updateLayout()
{
    saveRulers();

    static_cast<KoTextDocumentLayout*>(textBlock().document()->documentLayout())->layout();

    if (createFragments()) {
        loadRulers();
    } else {
        deactivateTextBlock();
    }

    repaintDecorations();
}

// try to find and activate a text block below the mouse position
// return true if successful
bool ParagraphTool::activateTextBlockAt(const QPointF &point)
{
    KoShape *shape = dynamic_cast<KoShape*>(canvas()->shapeManager()->shapeAt(point));
    if (shape == NULL) {
        // the shape below the mouse position is not a text shape
        return false;
    }

    KoTextShapeData *textShapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
    if (textShapeData == NULL) {
        return false;
    }

    QTextDocument *document = textShapeData->document();

    QPointF p = shape->transformation().inverted().map(point);
    p += QPointF(0.0, textShapeData->documentOffset());

    int position = document->documentLayout()->hitTest(p, Qt::ExactHit);
    if (position == -1) {
        // there is no text below the mouse position
        return false;
    }

    QTextBlock block(document->findBlock(position));
    if (!block.isValid()) {
        // the text block is not valid, this shouldn't really happen
        return false;
    }

    // the textblock is already activated, no need for a repaint and all that
    if (hasActiveTextBlock() && block == textBlock()) {
        return true;
    }

    m_activeCursor = QTextCursor(block);
    delete m_paragraphStyle;
    m_paragraphStyle = KoParagraphStyle::fromBlock(m_activeCursor.block(), this);

    if (createFragments()) {
        loadRulers();
        emit styleNameChanged(styleName());

    } else {
        deactivateTextBlock();
        return false;
    }

    return true;
}

void ParagraphTool::deactivateTextBlock()
{
    if (!hasActiveTextBlock())
        return;

    emit styleNameChanged(QString(i18n("n/a")));

    deactivateRuler();
    dehighlightRuler();

    // invalidate active cursor
    m_activeCursor = QTextCursor();
    scheduleRepaint();
}

bool ParagraphTool::activateRulerAt(const QPointF &point)
{
    if (!hasActiveTextBlock()) {
        // can't activate a ruler without a textblock
        return false;
    }

    for (int ruler = 0; ruler != maxRuler; ++ruler) {
        RulerFragment *fragment = m_rulers[ruler].hitTest(point);
        if (fragment != NULL) {
            activateRuler(static_cast<RulerIndex>(ruler), fragment);
            return true;
        }
    }

    m_activeRuler = noRuler;
    m_activeRulerFragment = NULL;
    return false;
}

void ParagraphTool::activateRuler(RulerIndex ruler, RulerFragment *fragment)
{
    m_activeRuler = ruler;
    m_activeRulerFragment = fragment;
    m_rulers[m_activeRuler].setActive(true);

    // disable hovering if we have an active ruler
    // hovering over the wrong ruler confuses the user
    if (hasHighlightedRuler()) {
        dehighlightRuler();
    }
}

void ParagraphTool::deactivateRuler()
{
    if (!hasActiveRuler()) {
        return;
    }

    RulerIndex activeRuler = m_activeRuler;
    m_activeRuler = noRuler;
    m_activeRulerFragment = NULL;
    m_rulers[activeRuler].setActive(false);

    focusRuler(activeRuler);

    // there's no active ruler anymore, so we have to check if we hover
    // over any other ruler
    highlightRulerAt(m_mousePosition);
}

void ParagraphTool::resetActiveRuler()
{
    if (hasActiveRuler()) {
        m_rulers[m_activeRuler].resetValue();
        deactivateRuler();
    }
}

void ParagraphTool::moveActiveRulerTo(const QPointF &point)
{
    m_activeRulerFragment->moveTo(point, smoothMovement());
}

void ParagraphTool::focusRuler(RulerIndex ruler)
{
    if (m_focusedRuler == ruler) {
        return;
    }

    if (hasFocusedRuler()) {
        m_rulers[m_focusedRuler].setFocused(false);
    }

    m_focusedRuler = ruler;
    m_rulers[m_focusedRuler].setFocused(true);
}

void ParagraphTool::defocusRuler()
{
    if (!hasFocusedRuler()) {
        return;
    }

    m_rulers[m_focusedRuler].setFocused(false);
    m_focusedRuler = noRuler;
}

void ParagraphTool::highlightRulerAt(const QPointF &point)
{
    // check if we were already hovering over an element
    if (hasHighlightedRuler()) {
        // check if we are still over the same element
        if (m_highlightedRulerFragment->hitTest(point)) {
            return;
        } else {
            // stop hovering over the element
            dehighlightRuler();
        }
    }

    // check if we are hovering over a new control
    for (int ruler = 0; ruler != maxRuler; ++ruler) {
        RulerFragment *fragment = m_rulers[ruler].hitTest(point);
        if (fragment != NULL) {
            m_highlightedRuler = static_cast<RulerIndex>(ruler);
            m_highlightedRulerFragment = fragment;
            m_rulers[m_highlightedRuler].setHighlighted(true);
            break;
        }
    }
}

void ParagraphTool::dehighlightRuler()
{
    if (hasHighlightedRuler()) {
        m_rulers[m_highlightedRuler].setHighlighted(false);
        m_highlightedRuler = noRuler;
        m_highlightedRulerFragment = NULL;
    }
}

void ParagraphTool::applyParentStyleToActiveRuler()
{
    if (!hasActiveRuler()) {
        return;
    }

    if (m_activeRuler == firstIndentRuler) {
        m_paragraphStyle->remove(QTextFormat::TextIndent);
        m_rulers[m_activeRuler].setValue(m_paragraphStyle->textIndent());
    } else if (m_activeRuler == followingIndentRuler) {
        m_paragraphStyle->remove(QTextFormat::BlockLeftMargin);
        m_rulers[m_activeRuler].setValue(m_paragraphStyle->leftMargin());
    } else if (m_activeRuler == rightMarginRuler) {
        m_paragraphStyle->remove(QTextFormat::BlockRightMargin);
        m_rulers[m_activeRuler].setValue(m_paragraphStyle->rightMargin());
    } else if (m_activeRuler == topMarginRuler) {
        m_paragraphStyle->remove(QTextFormat::BlockTopMargin);
        m_rulers[m_activeRuler].setValue(m_paragraphStyle->topMargin());
    } else if (m_activeRuler == bottomMarginRuler) {
        m_paragraphStyle->remove(QTextFormat::BlockBottomMargin);
        m_rulers[m_activeRuler].setValue(m_paragraphStyle->bottomMargin());
    }

    deactivateRuler();

    // we need to call the updateLayout() slot manually, it is not emitted
    // if the ruler has been changed by calling setValue()
    // TODO: maybe setValue() should emit valueChanged(), too but make
    // sure that this doesn't trigger additional repaints or relayouts
    updateLayout();
}

void ParagraphTool::mousePressEvent(KoPointerEvent *event)
{
    m_paragraphHighlighter.mousePressEvent(event);

    m_mousePosition = event->point;

    if (event->button() == Qt::LeftButton) {

        bool activated = activateRulerAt(event->point);

        if (!activated) {

            if (hasFocusedRuler()) {
                defocusRuler();
            } else {
                deactivateTextBlock();
                activateTextBlockAt(event->point);
            }
        }
    } else if (event->button() == Qt::RightButton) {
        resetActiveRuler();
    } else if (event->button() == Qt::MidButton) {
        applyParentStyleToActiveRuler();
    }

    repaintDecorations();
}

void ParagraphTool::mouseReleaseEvent(KoPointerEvent *event)
{
    m_paragraphHighlighter.mouseReleaseEvent(event);

    m_mousePosition = event->point;

    if (hasActiveTextBlock()) {
        deactivateRuler();
    }

    repaintDecorations();
}

void ParagraphTool::mouseMoveEvent(KoPointerEvent *event)
{
    m_paragraphHighlighter.mouseMoveEvent(event);

    m_mousePosition = event->point;

    if (hasActiveTextBlock()) {
        if (hasActiveRuler()) {
            moveActiveRulerTo(event->point);
        } else {
            highlightRulerAt(event->point);
        }

    }

    repaintDecorations();
}

void ParagraphTool::keyPressEvent(QKeyEvent *event)
{
    if (hasActiveRuler()) {
        switch (event->key()) {
        case Qt::Key_Shift:
            toggleSmoothMovement();
            break;
        case Qt::Key_Escape:
            resetActiveRuler();
            break;
        case Qt::Key_Delete:
        case Qt::Key_BackSpace:
            applyParentStyleToActiveRuler();
            break;
        default:
            break;
        }
    } else if (hasFocusedRuler()) {
        switch (event->key()) {
        case Qt::Key_Plus:
            focusedRuler().increaseByStep();
            break;
        case Qt::Key_Minus:
            focusedRuler().decreaseByStep();
            break;
        case Qt::Key_PageUp:
            break;
        case Qt::Key_PageDown:
            break;
        case Qt::Key_Tab:
            focusNextRuler();
            break;
        default:
            break;
        }
    } else {
        switch (event->key()) {
        case Qt::Key_Tab:
            focusRuler(topMarginRuler);
            break;
        default:
            break;
        }
    }

    repaintDecorations();
}

void ParagraphTool::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
        toggleSmoothMovement();
}

void  ParagraphTool::activate(bool)
{
    // don't know why force=true is needed and what it does,
    // but almost everyone else uses it...
    useCursor(Qt::ArrowCursor, true);
}

void ParagraphTool::deactivate()
{
    // the document might have changed, so we have to deactivate the textblock
    deactivateTextBlock();
}

