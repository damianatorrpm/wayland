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

#ifndef LIRI_CLIENTWINDOWQUICKITEM_P_H
#define LIRI_CLIENTWINDOWQUICKITEM_P_H

#include <QtWaylandCompositor/private/qwaylandquickshellsurfaceitem_p.h>

#include <Liri/WaylandServer/ClientWindowQuickItem>

//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

namespace Liri {

namespace WaylandServer {

class LIRIWAYLANDSERVER_EXPORT ClientWindowQuickItemPrivate : public QWaylandQuickShellSurfaceItemPrivate
{
    Q_DECLARE_PUBLIC(ClientWindowQuickItem)
public:
    ClientWindowQuickItemPrivate();
    ~ClientWindowQuickItemPrivate();

    static ClientWindowQuickItemPrivate *get(ClientWindowQuickItem *item) { return item->d_func(); }

    QQmlPropertyMap *savedProperties;
    bool isModifierHeld;
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_CLIENTWINDOWQUICKITEM_P_H

