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

#ifndef LIRI_VTHANDLER_H
#define LIRI_VTHANDLER_H

#include <QtCore/QObject>

#include <LiriLogind/lirilogindglobal.h>

namespace Liri {

namespace Platform {

class VtHandlerPrivate;

class LIRILOGIND_EXPORT VtHandler : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(VtHandler)
public:
    VtHandler(QObject *parent = 0);

    bool isActive() const;

    void activate(quint32 nr);

Q_SIGNALS:
    void created();
    void activeChanged(bool active);
    void interrupted();
    void aboutToSuspend();
    void resumed();
};

} // namespace Platform

} // namespace Liri

#endif // LIRI_VTHANDLER_H
