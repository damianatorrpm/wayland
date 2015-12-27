/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef RECORDER_H
#define RECORDER_H

#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtGui/QRgb>

class Recorder
{
public:
    Recorder(const QString &fileName);
    ~Recorder();

    bool open();

    void write(quint32 time, const QSize &size,
               qint32 stride, const QImage &image);

private:
    QDataStream m_stream;
    QFile *m_file;
};

#endif // RECORDER_H