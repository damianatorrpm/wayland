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

#ifndef LIRI_LOGIND_H
#define LIRI_LOGIND_H

#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>

#include <LiriLogind/lirilogindglobal.h>

namespace Liri {

namespace Platform {

class LogindPrivate;

class LIRILOGIND_EXPORT Logind : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool hasSessionControl READ hasSessionControl NOTIFY hasSessionControlChanged)
    Q_PROPERTY(bool sessionActive READ isSessionActive NOTIFY sessionActiveChanged)
    Q_PROPERTY(bool inhibited READ isInhibited NOTIFY inhibitedChanged)
    Q_PROPERTY(int vtNumber READ vtNumber NOTIFY vtNumberChanged)
    Q_DECLARE_PRIVATE(Logind)
public:
    enum InhibitFlag {
        InhibitShutdown = 0x01,
        InhibitSleep = 0x02,
        InhibitIdle = 0x04,
        InhibitPowerKey = 0x08,
        InhibitSuspendKey = 0x10,
        InhibitHibernateKey = 0x20,
        InhibitLidSwitch = 0x40
    };
    Q_DECLARE_FLAGS(InhibitFlags, InhibitFlag)

    enum InhibitMode {
        Block = 0,
        Delay
    };

    static Logind *instance();

    bool isConnected() const;
    bool hasSessionControl() const;
    bool isSessionActive() const;
    bool isInhibited() const;
    int vtNumber() const;

public Q_SLOTS:
    void inhibit(const QString &who, const QString &why, InhibitFlags flags, InhibitMode mode);
    void uninhibit(int fd);

    void lockSession();
    void unlockSession();

    void takeControl();
    void releaseControl();

    int takeDevice(const QString &fileName);
    void releaseDevice(int fd);

    void pauseDeviceComplete(quint32 devMajor, quint32 devMinor);

    void switchTo(quint32 vt);

Q_SIGNALS:
    void connectedChanged(bool);
    void hasSessionControlChanged(bool);
    void sessionActiveChanged(bool);
    void inhibitedChanged(bool);
    void vtNumberChanged(int);

    void prepareForSleep(bool before);
    void prepareForShutdown(bool before);

    void lockSessionRequested();
    void unlockSessionRequested();

    void inhibited(int fd);
    void uninhibited(int fd);

    void devicePaused(quint32 major, quint32 minor, const QString &type);
    void deviceResumed(quint32 major, quint32 minor, int fd);

protected:
    explicit Logind(const QDBusConnection &connection, QObject *parent = 0);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_serviceRegistered())
    Q_PRIVATE_SLOT(d_func(), void _q_serviceUnregistered())
    Q_PRIVATE_SLOT(d_func(), void _q_sessionPropertiesChanged())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Logind::InhibitFlags)

} // namespace Platform

} // namespace Liri

#endif // LIRI_LOGIND_H
