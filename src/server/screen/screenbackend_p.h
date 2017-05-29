/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL$
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1 or later as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPLv21 included in the packaging of
 * this file.  Please review the following information to ensure the
 * GNU Lesser General Public License version 2.1 requirements will be
 * met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 2.0 or later as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 included in the
 * packaging of this file.  Please review the following information to ensure
 * the GNU General Public License version 2.0 requirements will be
 * met: http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_SCREENBACKEND_P_H
#define LIRI_SCREENBACKEND_P_H

#include <QtCore/private/qobject_p.h>
#include <QtCore/QPoint>
#include <QtCore/QSize>

#include <LiriWaylandServer/ScreenBackend>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace Liri {

namespace WaylandServer {

class LIRIWAYLANDSERVER_EXPORT ScreenPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(Screen)
public:
    ScreenPrivate()
        : QObjectPrivate()
        , m_screen(nullptr)
        , m_refreshRate(60000)
        , m_subpixel(QWaylandOutput::SubpixelNone)
        , m_transform(QWaylandOutput::TransformNormal)
        , m_scaleFactor(1)
        , m_currentMode(-1)
        , m_preferredMode(-1)
    {
    }

    void setPosition(const QPoint &pos);
    void setSize(const QSize &size);
    void setRefreshRate(int refreshRate);
    void setPhysicalSize(const QSizeF &size);
    void setSubpixel(QWaylandOutput::Subpixel subpixel);
    void setTransform(QWaylandOutput::Transform transform);
    void setScaleFactor(int scale);
    void setCurrentMode(int modeId);
    void setPreferredMode(int modeId);
    void setModes(const QList<Screen::Mode> &modes);

    QScreen *m_screen;
    QString m_manufacturer;
    QString m_model;
    QPoint m_position;
    QSize m_size;
    int m_refreshRate;
    QSizeF m_physicalSize;
    QWaylandOutput::Subpixel m_subpixel;
    QWaylandOutput::Transform m_transform;
    int m_scaleFactor;
    int m_currentMode;
    int m_preferredMode;
    QList<Screen::Mode> m_modes;
};

class LIRIWAYLANDSERVER_EXPORT ScreenBackendPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(ScreenBackend)
public:
    ScreenBackendPrivate()
        : QObjectPrivate()
    {
    }

    QList<Screen *> screens;
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_SCREENBACKEND_P_H

