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

#ifndef LIRI_SCREENCASTER_H
#define LIRI_SCREENCASTER_H

#include <QtCore/QObject>
#include <QtCore/QMultiHash>
#include <QtCore/QMutex>

#include <QtWaylandCompositor/QWaylandCompositorExtension>

#include <LiriWaylandServer/liriwaylandserverglobal.h>

class QQuickWindow;

namespace Liri {

namespace WaylandServer {

class Screencast;
class ScreencastPrivate;
class ScreencasterPrivate;

class LIRIWAYLANDSERVER_EXPORT Screencaster : public QWaylandCompositorExtensionTemplate<Screencaster>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Screencaster)
public:
    Screencaster();
    Screencaster(QWaylandCompositor *compositor);

    void initialize() override;

    void recordFrame(QQuickWindow *window);

    static const struct wl_interface* interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void captureRequested(Screencast *screencast);
};

class LIRIWAYLANDSERVER_EXPORT Screencast : public QWaylandCompositorExtensionTemplate<Screencast>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Screencast)
public:
    ~Screencast();

    static const struct wl_interface* interface();
    static QByteArray interfaceName();

protected:
    bool event(QEvent *event) override;

private:
    explicit Screencast();

    friend class ScreencasterPrivate;
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_SCREENCASTER_H
