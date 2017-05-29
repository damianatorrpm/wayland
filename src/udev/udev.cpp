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

#include <qplatformdefs.h>

#include "logging_p.h"
#include "udev.h"
#include "udev_p.h"
#include "udevdevice.h"

namespace Liri {

namespace Platform {

/*
 * UdevPrivate
 */

UdevPrivate::UdevPrivate()
{
    udev = udev_new();
    if (!udev)
        qCWarning(lcUdev, "Unable to get udev library context: no devices can be detected");
}

UdevPrivate::~UdevPrivate()
{
    if (udev)
        udev_unref(udev);
}

UdevPrivate *UdevPrivate::get(Udev *u)
{
    return u ? u->d_func() : nullptr;
}

/*
 * Udev
 */

Udev::Udev()
    : d_ptr(new UdevPrivate)
{
}

Udev::~Udev()
{
    delete d_ptr;
}

bool Udev::isValid() const
{
    Q_D(const Udev);
    return d->udev;
}

UdevDevice *Udev::deviceFromFileName(const QString &fileName) const
{
    Q_D(const Udev);

    if (!isValid())
        return nullptr;

    QT_STATBUF sb;

    if (QT_STAT(qPrintable(fileName), &sb) != 0)
        return nullptr;

    udev_device *dev = nullptr;

    if (S_ISBLK(sb.st_mode))
        dev = udev_device_new_from_devnum(d->udev, 'b', sb.st_rdev);
    else if (S_ISCHR(sb.st_mode))
        dev = udev_device_new_from_devnum(d->udev, 'c', sb.st_rdev);

    if (!dev)
        return nullptr;

    return new UdevDevice(dev);
}

UdevDevice *Udev::deviceFromSubSystemAndName(const QString &subSystem, const QString &name) const
{
    Q_D(const Udev);

    if (!isValid())
        return nullptr;

    udev_device *dev = udev_device_new_from_subsystem_sysname(d->udev,
                                                              qPrintable(subSystem),
                                                              qPrintable(name));
    return new UdevDevice(dev);
}

UdevDevice *Udev::deviceFromSysfsPath(const QString &sysfsPath) const
{
    Q_D(const Udev);

    if (!isValid())
        return nullptr;

    udev_device *dev = udev_device_new_from_syspath(d->udev, qPrintable(sysfsPath));
    return new UdevDevice(dev);
}

} // namespace Platform

} // namespace Liri
