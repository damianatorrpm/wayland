/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini
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

#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/QQmlComponent>


#include "xwayland.h"
#include "xwaylandserver.h"
#include "xwaylandshellsurface.h"
#include "xwaylandquickshellsurfaceitem.h"

class LiriXWaylandPluging : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    void registerTypes(const char *uri);
};

void LiriXWaylandPluging::registerTypes(const char *uri)
{
    // @uri Liri.XWayland
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.XWayland"));

    qmlRegisterType<XWayland>(uri, 1, 0, "XWayland");
    qmlRegisterUncreatableType<XWaylandShellSurface>(uri, 1, 0, "XWaylandWindow",
                                               QObject::tr("Cannot create instance of XWaylandWindow"));
    qmlRegisterType<XWaylandQuickShellSurfaceItem>(uri, 1, 0, "XWaylandShellSurfaceItem");
}

#include "plugin.moc"
