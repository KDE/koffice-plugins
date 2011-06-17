/* This file is part of the KDE project
 * Copyright (C) 2010  Vidhyapria  Arunkumar <vidhyapria.arunkumar@nokia.com>
 * Copyright (C) 2010  Amit Aggarwal <amit.5.aggarwal@nokia.com>
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

#include "PluginShape.h"

#include <KViewConverter.h>
#include <KShapeLoadingContext.h>
#include <KOdfLoadingContext.h>
#include <KShapeSavingContext.h>
#include <KXmlWriter.h>
#include <KOdfXmlNS.h>
#include <KOdfStorageDevice.h>
#include <KUnit.h>

#include <QPainter>
#include <kdebug.h>
#include <klocale.h>


PluginShape::PluginShape()
    : KFrameShape(KOdfXmlNS::draw, "plugin")
{
    setKeepAspectRatio(true);
}

PluginShape::~PluginShape()
{
}

void PluginShape::paint(QPainter &painter, const KViewConverter &converter)
{
    QRectF pixelsF = converter.documentToView(QRectF(QPointF(0,0), size()));
    painter.fillRect(pixelsF, QColor(Qt::yellow));
    painter.setPen(Qt::blue);
    QString mimetype = i18n("Unknown");
    if (!m_mimetype.isEmpty()) {
        mimetype = m_mimetype;
    }
    painter.drawText(pixelsF, Qt::AlignCenter, i18n("Plugin of mimetype: %1").arg(mimetype));
}

void PluginShape::saveOdf(KShapeSavingContext &context) const
{
    KXmlWriter &writer = context.xmlWriter();

    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);
    writer.startElement("draw:plugin");
    writer.addAttribute("draw:mime-type", m_mimetype);
    writer.addAttribute("xlink:type", m_xlinktype);
    writer.addAttribute("xlink:show", m_xlinkshow);
    writer.addAttribute("xlink:actuate", m_xlinkactuate);
    writer.addAttribute("xlink:href", m_xlinkhref);
    writer.addAttribute("xml:id", m_xmlid);

    QMap<QString,QString>::const_iterator itr = m_drawParams.constBegin();
    while (itr != m_drawParams.constEnd()) {
        writer.startElement("draw:param", true);
        writer.addAttribute("draw:name", itr.key());
        writer.addAttribute("draw:value", itr.value());
        writer.endElement(); // draw:param
        itr++;
    }
    writer.endElement(); // draw:plugin
    saveOdfCommonChildElements(context);
    writer.endElement(); // draw:frame

}

bool PluginShape::loadOdf(const KXmlElement &element, KShapeLoadingContext &context)
{
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool PluginShape::loadOdfFrameElement(const KXmlElement &element, KShapeLoadingContext &context)
{
    Q_UNUSED(context);
    if(element.isNull()) {
        return false;
    }

    if(element.localName() == "plugin") {
        m_mimetype  = element.attributeNS(KOdfXmlNS::draw, "mime-type", QString::null);
        m_xlinktype  = element.attributeNS(KOdfXmlNS::xlink, "type", QString::null);
        m_xlinkshow  = element.attributeNS(KOdfXmlNS::xlink, "show", QString::null);
        m_xlinkactuate  = element.attributeNS(KOdfXmlNS::xlink, "actuate", QString::null);
        m_xlinkhref  = element.attributeNS(KOdfXmlNS::xlink, "href", QString::null);
        m_xmlid = element.attribute("xml:id", QString::null);
        m_drawParams.clear();
        if(element.hasChildNodes()) {
            KXmlNode node = element.firstChild();
            while(!node.isNull()) {
                if(node.isElement()) {
                    KXmlElement nodeElement = node.toElement();
                    if(nodeElement.localName() == "param") {
                        QString name = nodeElement.attributeNS(KOdfXmlNS::draw, "name", QString::null);
                        if(!name.isNull()) {
                            m_drawParams.insert(name,nodeElement.attributeNS(KOdfXmlNS::draw, "value", QString::null));
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
        return true;
    }
    return false;
}
