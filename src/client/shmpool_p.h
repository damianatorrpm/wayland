/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef LIRIWAYLANDCLIENT_SHMPOOL_P_H
#define LIRIWAYLANDCLIENT_SHMPOOL_P_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QTemporaryFile>
#include <QtCore/private/qobject_p.h>

#include <LiriWaylandClient/ShmPool>
#include "qwayland-wayland.h"

#include <unistd.h>
#include <sys/mman.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

Q_DECLARE_LOGGING_CATEGORY(WLSHMPOOL)

namespace Liri {

namespace WaylandClient {

class LIRIWAYLANDCLIENT_EXPORT ShmPoolPrivate
        : public QObjectPrivate
        , public QtWayland::wl_shm_pool
{
    Q_DECLARE_PUBLIC(ShmPool)
public:
    ShmPoolPrivate();
    ~ShmPoolPrivate();

    bool createPool(Shm *shm, size_t createSize);
    bool resizePool(size_t newSize);

    QVector<BufferSharedPtr>::iterator reuseBuffer(const QSize &s, qint32 stride, Shm::Format format);

    static ShmPoolPrivate *get(ShmPool *pool) { return pool->d_func(); }

    Shm *shm;
    QScopedPointer<QTemporaryFile> file;
    uchar *data;
    qint32 size;
    qint32 offset;
    QVector<BufferSharedPtr> buffers;
};

} // namespace WaylandClient

} // namespace Liri

#endif // LIRIWAYLANDCLIENT_SHMPOOL_P_H
