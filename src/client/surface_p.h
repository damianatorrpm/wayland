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

#ifndef LIRIWAYLANDCLIENT_SURFACE_P_H
#define LIRIWAYLANDCLIENT_SURFACE_P_H

#include <QtCore/private/qobject_p.h>

#include <LiriWaylandClient/Surface>
#include <LiriWaylandClient/private/qwayland-wayland.h>

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

namespace WaylandClient {

class LIRIWAYLANDCLIENT_EXPORT SurfacePrivate
        : public QObjectPrivate
        , public QtWayland::wl_surface
{
    Q_DECLARE_PUBLIC(Surface)
public:
    SurfacePrivate();

    bool frameCallbackInstalled;

    void handleFrameCallback();

    static void frameCallback(void *data, wl_callback *callback, uint32_t time);
    static const wl_callback_listener callbackListener;

    static Surface *fromWlSurface(struct ::wl_surface *surface);
    static SurfacePrivate *get(Surface *surface) { return surface->d_func(); }

protected:
    void surface_enter(wl_output *output) override;
    void surface_leave(wl_output *output) override;
};

} // namespace WaylandClient

} // namespace Liri

#endif // LIRIWAYLANDCLIENT_SURFACE_P_H
