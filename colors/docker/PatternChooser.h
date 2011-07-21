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

#ifndef PATTERN_CHOOSER_H
#define PATTERN_CHOOSER_H

#include <QtGui/QWidget>
#include <ui_PatternChooser.h>

#include <KShape.h>

class PatternEditor;
class KPatternBackground;
class KoResource;

class PatternChooser : public QWidget
{
    Q_OBJECT
public:
    PatternChooser(QSet<KShape*> shapes, QWidget *parent = 0);
    ~PatternChooser();

public slots:
    void shapeChanged(KShape *shape, KShape::ChangeType type);

signals:
    void changed();

private slots:
    void setRepeatType(int);
    void zoomToFit();
    void zoomIn();
    void zoomOut();
    void setPattern(KoResource*);
    void setEditMode(bool);

private:
    PatternEditor *m_patternEditor;
    KPatternBackground *m_background;
    QSet<KShape*> m_shapes;

    Ui::PatternChooser widget;
};

#endif
