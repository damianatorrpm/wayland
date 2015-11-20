/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDQUICKITEM_P_H
#define QWAYLANDQUICKITEM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/private/qquickitem_p.h>

#include "qwaylandquickitem.h"

QT_BEGIN_NAMESPACE

class QWaylandSurfaceTextureProvider;
class QMutex;

class QWaylandQuickItemPrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(QWaylandQuickItem)
public:
    QWaylandQuickItemPrivate()
        : QQuickItemPrivate()
        , view(Q_NULLPTR)
        , oldSurface(Q_NULLPTR)
        , provider(Q_NULLPTR)
        , paintEnabled(true)
        , touchEventsEnabled(false)
        , inputEventsEnabled(true)
        , newTexture(false)
        , focusOnClick(true)
        , sizeFollowsSurface(true)
        , connectedWindow(Q_NULLPTR)
        , origin(QWaylandSurface::OriginTopLeft)
    {
    }

    void init()
    {
        Q_Q(QWaylandQuickItem);
        if (!mutex)
            mutex = new QMutex;

        view.reset(new QWaylandView(q));
        q->setFlag(QQuickItem::ItemHasContents);

        q->update();

        q->setSmooth(true);

        q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton |
                                   Qt::ExtraButton1 | Qt::ExtraButton2 | Qt::ExtraButton3 | Qt::ExtraButton4 |
                                   Qt::ExtraButton5 | Qt::ExtraButton6 | Qt::ExtraButton7 | Qt::ExtraButton8 |
                                   Qt::ExtraButton9 | Qt::ExtraButton10 | Qt::ExtraButton11 |
                                   Qt::ExtraButton12 | Qt::ExtraButton13);
        q->setAcceptHoverEvents(true);

        QObject::connect(q, &QQuickItem::windowChanged, q, &QWaylandQuickItem::updateWindow);
        QObject::connect(view.data(), &QWaylandView::surfaceChanged, q, &QWaylandQuickItem::surfaceChanged);
        QObject::connect(view.data(), &QWaylandView::surfaceChanged, q, &QWaylandQuickItem::handleSurfaceChanged);
        QObject::connect(view.data(), &QWaylandView::surfaceDestroyed, q, &QWaylandQuickItem::surfaceDestroyed);
    }

    bool shouldSendInputEvents() const { return view->surface() && inputEventsEnabled; }

    static QMutex *mutex;

    QScopedPointer<QWaylandView> view;
    QWaylandSurface *oldSurface;
    mutable QWaylandSurfaceTextureProvider *provider;
    bool paintEnabled;
    bool touchEventsEnabled;
    bool inputEventsEnabled;
    bool newTexture;
    bool focusOnClick;
    bool sizeFollowsSurface;

    QQuickWindow *connectedWindow;
    QWaylandSurface::Origin origin;
};

QT_END_NAMESPACE

#endif  /*QWAYLANDQUICKITEM_P_H*/