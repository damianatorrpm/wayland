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

#include <QGuiApplication>
#include <QWaylandOutputMode>
#include <qpa/qwindowsysteminterface.h>

#include "quickoutput.h"
#include "screenmodel.h"

QuickOutput::QuickOutput()
    : QWaylandQuickOutput()
{
}

QQmlListProperty<ScreenMode> QuickOutput::screenModes()
{
    auto appendFunc = [](QQmlListProperty<ScreenMode> *prop, ScreenMode *mode) {
        auto output = static_cast<QuickOutput *>(prop->object);;
        if (!output->m_initialized)
            output->m_modes.append(mode);
    };
    auto countFunc = [](QQmlListProperty<ScreenMode> *prop) {
        return static_cast<QuickOutput *>(prop->object)->m_modes.count();
    };
    auto atFunc = [](QQmlListProperty<ScreenMode> *prop, int index) {
        return static_cast<QuickOutput *>(prop->object)->m_modes.at(index);
    };
    auto clearFunc = [](QQmlListProperty<ScreenMode> *prop) {
        auto output = static_cast<QuickOutput *>(prop->object);
        if (!output->m_initialized)
            output->m_modes.clear();
    };
    return QQmlListProperty<ScreenMode>(this, this, appendFunc, countFunc, atFunc, clearFunc);
}

int QuickOutput::currentModeIndex() const
{
    return m_currentModeIndex;
}

void QuickOutput::setCurrentModeIndex(int index)
{
    if (m_initialized)
        return;
    if (index == m_currentModeIndex)
        return;

    m_currentModeIndex = index;
    Q_EMIT currentModeIndexChanged();
}

int QuickOutput::preferredModeIndex() const
{
    return m_preferredModexIndex;
}

void QuickOutput::setPreferredModeIndex(int index)
{
    if (m_initialized)
        return;
    if (index == m_preferredModexIndex)
        return;

    m_preferredModexIndex = index;
    Q_EMIT preferredModeIndexChanged();
}

void QuickOutput::initialize()
{
    // Modes cannot change past initialization
    m_initialized = true;

    // Make sure size is dictated by the current mode
    setSizeFollowsWindow(false);

    int index = 0;
    for (const auto mode : m_modes) {
        QWaylandOutputMode wlMode(mode->resolution(), mode->refreshRate());
        addMode(wlMode, m_preferredModexIndex == index);

        if (m_currentModeIndex == index)
            setCurrentMode(wlMode);

        index++;
    }

    QWaylandQuickOutput::initialize();

    // Center mouse cursor
    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (quickWindow && quickWindow->screen() == QGuiApplication::primaryScreen())
        QWindowSystemInterface::handleMouseEvent(quickWindow, geometry().center(),
                                                 geometry().center(), Qt::NoButton);
}
