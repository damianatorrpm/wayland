/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LIRI_COMPOSITORSETTINGS_H
#define LIRI_COMPOSITORSETTINGS_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>

#include <LiriWaylandServer/liriwaylandserverglobal.h>

class QWaylandCompositor;

namespace Liri {

namespace WaylandServer {

class CompositorSettingsPrivate;

class LIRIWAYLANDSERVER_EXPORT CompositorSettings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_DECLARE_PRIVATE(CompositorSettings)
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_PROPERTY(Qt::KeyboardModifier windowActionKey READ windowActionKey WRITE setWindowActionKey NOTIFY windowActionKeyChanged)
    Q_PROPERTY(quint32 keyboardRepeatRate READ keyboardRepeatRate WRITE setKeyboardRepeatRate NOTIFY keyboardRepeatRateChanged)
    Q_PROPERTY(quint32 keyboardRepeatDelay READ keyboardRepeatDelay WRITE setKeyboardRepeatDelay NOTIFY keyboardRepeatDelayChanged)
public:
    explicit CompositorSettings(QObject *parent = nullptr);
    ~CompositorSettings();

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

    Qt::KeyboardModifier windowActionKey() const;
    void setWindowActionKey(Qt::KeyboardModifier mod);

    quint32 keyboardRepeatRate() const;
    void setKeyboardRepeatRate(quint32 rate);

    quint32 keyboardRepeatDelay() const;
    void setKeyboardRepeatDelay(quint32 delay);

Q_SIGNALS:
    void compositorChanged();
    void windowActionKeyChanged();
    void keyboardRepeatRateChanged();
    void keyboardRepeatDelayChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    CompositorSettingsPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_setupKeyboard())
};

} // namespace WaylandServer

} // namespace Liri

#endif // LIRI_COMPOSITORSETTINGS_H
