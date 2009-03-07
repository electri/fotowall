/***************************************************************************
 *                                                                         *
 *   This file is part of the FotoWall project,                            *
 *       http://code.google.com/p/fotowall                                 *
 *                                                                         *
 *   Copyright (C) 2009 by Enrico Ros <enrico.ros@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "PlasmaFrame.h"
#include <QLinearGradient>
#include <QPainter>
#include <QSvgRenderer>

struct PlasmaFramePrivate {
    QSvgRenderer * svg;
    int w1, w2, w3;
    int h1, h2, h3;
    int padL, padT, padR, padB;
    bool stretchBorders;

    PlasmaFramePrivate()
        : svg(0),w1(0),w2(0),w3(0)
        ,h1(0),h2(0),h3(0)
        ,padL(0),padT(0),padR(0),padB(0)
        ,stretchBorders(false)
    {
    }
    ~PlasmaFramePrivate()
    {
        delete svg;
    }
    int widthOf(const QString & element, int value = 0) const
    {
        if (!svg->elementExists(element))
            return value;
        return svg->boundsOnElement(element).width();
    }
    int heightOf(const QString & element, int value = 0) const
    {
        if (!svg->elementExists(element))
            return value;
        return svg->boundsOnElement(element).height();
    }
    void draw(QPainter * painter, const QRect & frameRect)
    {
        // left column
        int l = frameRect.left();
        int t = frameRect.top();
        int w = frameRect.width();
        int h = frameRect.height();
        int wx = w - w1 - w3;
        int hx = h - h1 - h3;
        svg->render(painter, "topleft",     QRect(l,        t,      w1, h1));
        svg->render(painter, "top",         QRect(l+w1,     t,      wx, h1));
        svg->render(painter, "topright",    QRect(l+w1+wx,  t,      w3, h1));
        svg->render(painter, "left",        QRect(l,        t+h1,   w1, hx));
        if (padL > w1 || padT > h1 || padR > w3 || padB > h3)
            svg->render(painter, "center",  QRect(l+w1,     t+h1,   wx, hx));
        svg->render(painter, "right",       QRect(l+w1+wx,  t+h1,   w3, hx));
        svg->render(painter, "bottomleft",  QRect(l,        t+h1+hx,w1, h3));
        svg->render(painter, "bottom",      QRect(l+w1,     t+h1+hx,wx, h3));
        svg->render(painter, "bottomright", QRect(l+w1+wx,  t+h1+hx,w3, h3));
    }
};


PlasmaFrame::PlasmaFrame(const QString & fileName)
    : d(new PlasmaFramePrivate())
{
    // create the renderer
    d->svg = new QSvgRenderer(fileName);
    if (!d->svg->isValid()) {
        qWarning("PlasmaFrame::PlasmaFrame: invalid file %s", qPrintable(fileName));
        delete d->svg;
        d->svg = 0;
        return;
    }

    // find out inital params
    d->w1 = d->widthOf("topleft");
    d->w2 = d->widthOf("top");
    d->w3 = d->widthOf("topright");
    d->h1 = d->heightOf("topleft");
    d->h2 = d->heightOf("left");
    d->h3 = d->heightOf("bottomleft");
    d->padL = d->widthOf("hint-left-margin", d->w1);
    d->padT = d->heightOf("hint-top-margin", d->h1);
    d->padR = d->widthOf("hint-right-margin", d->w3);
    d->padB = d->heightOf("hint-bottom-margin", d->h3);
    d->stretchBorders = d->svg->elementExists("hint-stretch-borders");
}

PlasmaFrame::~PlasmaFrame()
{
    delete d;
}

bool PlasmaFrame::isValid() const
{
    return d->svg;
}

QRect PlasmaFrame::contentsRect(const QRect & frameRect) const
{
    return frameRect.adjusted(d->padL, d->padT, -d->padR, -d->padB);
}

void PlasmaFrame::layoutButtons(QList<QGraphicsItem *> buttons, const QRect & frameRect) const
{
    const int spacing = 4;
    int bottom = frameRect.bottom() - d->padB;
    int right = frameRect.right() - d->padR;
    foreach (QGraphicsItem * button, buttons) {
        QSize bSize = button->boundingRect().size().toSize();
        button->setPos(right - bSize.width() / 2, bottom - bSize.height() / 2);
        right -= bSize.width() + spacing;
    }
}

void PlasmaFrame::layoutText(QGraphicsItem * textItem, const QRect & frameRect) const
{
    textItem->setPos( frameRect.left() + d->padL, frameRect.center().y() - textItem->boundingRect().size().height() / 2 );
}

void PlasmaFrame::paint(QPainter * painter, const QRect & frameRect)
{
    //painter->fillRect(frameRect,Qt::red);
    d->draw(painter, frameRect);
}