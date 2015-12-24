/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef GREENISLANDCLIENT_TOUCH_P_H
#define GREENISLANDCLIENT_TOUCH_P_H

#include <QtCore/private/qobject_p.h>

#include <GreenIsland/Client/Seat>
#include <GreenIsland/Client/Surface>
#include <GreenIsland/Client/Touch>
#include <GreenIsland/client/private/qwayland-wayland.h>

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

namespace GreenIsland {

namespace Client {

class GREENISLANDCLIENT_EXPORT TouchPrivate
        : public QObjectPrivate
        , public QtWayland::wl_touch
{
    Q_DECLARE_PUBLIC(Touch)
public:
    TouchPrivate();
    ~TouchPrivate();

    Seat *seat;
    bool active;
    QVector<TouchPoint *> points;

    TouchPoint *getPressedPoint(qint32 id) const;

    static Touch *fromWlTouch(struct ::wl_touch *touch);
    static TouchPrivate *get(Touch *touch) { return touch->d_func(); }

protected:
    void touch_down(uint32_t serial, uint32_t time,
                    struct ::wl_surface *surface, int32_t id,
                    wl_fixed_t x, wl_fixed_t y) Q_DECL_OVERRIDE;
    void touch_up(uint32_t serial, uint32_t time, int32_t id) Q_DECL_OVERRIDE;
    void touch_motion(uint32_t time, int32_t id,
                      wl_fixed_t x, wl_fixed_t y) Q_DECL_OVERRIDE;
    void touch_frame() Q_DECL_OVERRIDE;
    void touch_cancel() Q_DECL_OVERRIDE;
};

class GREENISLANDCLIENT_EXPORT TouchPointPrivate
{
    Q_DECLARE_PUBLIC(TouchPoint)
public:
    TouchPointPrivate();

    qint32 id;
    quint32 upSerial, downSerial;
    QPointer<Surface> surface;
    QVector<QPointF> positions;
    QVector<quint32> timestamps;
    bool down;

private:
    TouchPoint *q_ptr;
};

} // namespace Client

} // namespace GreenIsland

#endif // GREENISLANDCLIENT_TOUCH_P_H