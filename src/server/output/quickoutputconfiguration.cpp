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

#include "outputconfiguration_p.h"
#include "quickoutputconfiguration.h"
#include "quickoutputconfiguration_p.h"

namespace Liri {

namespace WaylandServer {

/*
 * QuickOutputConfigurationPrivate
 */

QuickOutputConfigurationPrivate::QuickOutputConfigurationPrivate()
{
}

QQmlListProperty<OutputChangeset> QuickOutputConfigurationPrivate::changes()
{
    Q_Q(QuickOutputConfiguration);
    return QQmlListProperty<OutputChangeset>(q, nullptr, changesCount, changesAt);
}

int QuickOutputConfigurationPrivate::changesCount(QQmlListProperty<OutputChangeset> *prop)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return OutputConfigurationPrivate::get(that)->changes.values().count();
}

OutputChangeset *QuickOutputConfigurationPrivate::changesAt(QQmlListProperty<OutputChangeset> *prop, int index)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return OutputConfigurationPrivate::get(that)->changes.values().at(index);
}

/*
 * QuickOutputConfiguration
 */

QuickOutputConfiguration::QuickOutputConfiguration()
    : OutputConfiguration()
{
}

QQmlListProperty<QObject> QuickOutputConfiguration::data()
{
    Q_D(QuickOutputConfiguration);
    return QQmlListProperty<QObject>(this, d->objects);
}

} // namespace WaylandServer

} // namespace Liri

#include "moc_quickoutputconfiguration.cpp"
