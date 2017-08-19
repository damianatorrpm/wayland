/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LIRI_FAKESCREENBACKEND_H
#define LIRI_FAKESCREENBACKEND_H

#include <LiriWaylandServer/ScreenBackend>

namespace Liri {

namespace WaylandServer {

class LIRIWAYLANDSERVER_EXPORT FakeScreenBackend : public ScreenBackend
{
    Q_OBJECT
public:
    FakeScreenBackend(QObject *parent = nullptr);

    void setConfiguration(const QString &fileName);

    void acquireConfiguration() override;

private:
    QString m_fileName;
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_FAKESCREENBACKEND_H
