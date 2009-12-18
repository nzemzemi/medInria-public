/* medViewContainer.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:54:57 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 16:26:19 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 20
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer.h"

#include <QtGui>

#include <dtkCore/dtkAbstractView.h>

class medViewContainerPrivate
{
public:
    QGridLayout *layout;

    QHash<QWidget *, dtkAbstractView*> views;
};

medViewContainer::medViewContainer(QWidget *parent) : QWidget(parent), d(new medViewContainerPrivate)
{
    d->layout = new QGridLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(2);

    s_current = this;

    static int id = 0;

    this->setObjectName(QString("medViewContainer%1").arg(id++));

    this->setFocusPolicy(Qt::ClickFocus);
}

medViewContainer::~medViewContainer(void)
{
    QHash<QWidget *, dtkAbstractView*>::iterator it = d->views.begin();

    while (it!=d->views.end()) {
        delete (*it);
        ++it;
    }    

    d->views.clear();
}

medViewContainer *medViewContainer::current(void)
{
    return s_current;
}

void medViewContainer::split(int rows, int cols)
{
    medViewContainer *current = this->current();

    if (!current)
        return;

    if (current->d->layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            current->d->layout->addWidget(new medViewContainer(current), i, j);

    s_current = 0;
}

dtkAbstractView *medViewContainer::view(void)
{
    QWidget *widget = NULL;

    if(d->layout->count())
        widget = d->layout->itemAt(0)->widget();

    if(!widget)
        return NULL;

    if(!d->views.keys().contains(widget))
        return NULL;

    return d->views.value(widget);
}

void medViewContainer::setView(dtkAbstractView *view)
{
    if (!view)
        return;
  
    medViewContainer *current = this->current();

    if (!current)
        return;

    if (current->d->layout->count())
        return;
    
    if(QWidget *widget = view->widget()) {
        widget->setParent(current);
        current->d->layout->setContentsMargins(1, 1, 1, 1);
        current->d->layout->addWidget(widget, 0, 0);
        d->views.insert(widget, view);
    }
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    qDebug() << __FILE__ << __func__;

    s_current = this;

    if(dtkAbstractView *view = this->view())
        emit focused(view);

    this->update();
}

void medViewContainer::focusOutEvent(QFocusEvent *event)
{
    qDebug() << __FILE__ << __func__;

    this->update();
}

void medViewContainer::paintEvent(QPaintEvent *event)
{
    if(this->layout()->count() > 1)
        return;

    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    if (s_current == this)
        painter.setPen(QColor(0x9a, 0xb3, 0xd5));
    else
        painter.setPen(Qt::darkGray);
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
    painter.end();
}

medViewContainer *medViewContainer::s_current = NULL;