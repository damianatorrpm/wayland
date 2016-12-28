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

#ifndef LIRI_APPLICATIONMANAGER_P_H
#define LIRI_APPLICATIONMANAGER_P_H

#include <QtCore/QMultiMap>
#include <QtQuick/QQuickItem>

#include <QtWaylandCompositor/private/qwaylandcompositorextension_p.h>

#include <Liri/WaylandServer/ApplicationManager>
#include <Liri/waylandserver/private/qwayland-server-liri.h>

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

class ClientWindow;

class LIRIWAYLANDSERVER_EXPORT ApplicationManagerPrivate
        : public QWaylandCompositorExtensionPrivate
        , public QtWaylandServer::liri_applications
{
    Q_DECLARE_PUBLIC(ApplicationManager)
public:
    ApplicationManagerPrivate();
    ~ApplicationManagerPrivate();

    void registerWindow(ClientWindow *window);
    void unregisterWindow(ClientWindow *window);

    void recalculateVirtualGeometry();

    void _q_outputAdded(QWaylandOutput *);
    void _q_outputRemoved(QWaylandOutput *);

    void _q_appIdChanged();
    void _q_activatedChanged();

    QQmlListProperty<ClientWindow> windows();

    static int windowsCount(QQmlListProperty<ClientWindow> *prop);
    static ClientWindow *windowsAt(QQmlListProperty<ClientWindow> *prop, int index);

    static ApplicationManagerPrivate *get(ApplicationManager *appMan) { return appMan->d_func(); }

    QWaylandCompositor *compositor;
    QQuickItem *rootItem;
    ClientWindow *focusedWindow;
    QVector<ClientWindow *> windowsList;
    QStringList appIds;
    QMap<QString, QString> appIdMap;

protected:
    void applications_quit(Resource *resource, const QString &app_id) Q_DECL_OVERRIDE;
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_APPLICATIONMANAGER_P_H

