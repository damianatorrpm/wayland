/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 The Qt Company Ltd.
 *
 * $BEGIN_LICENSE:QTLGPL$
 *
 * GNU Lesser General Public License Usage
 * This file may be used under the terms of the GNU Lesser General
 * Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPLv3 included in the
 * packaging of this file. Please review the following information to
 * ensure the GNU Lesser General Public License version 3 requirements
 * will be met: https://www.gnu.org/licenses/lgpl.html.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 2.0 or (at your option) the GNU General
 * Public license version 3 or any later version approved by the KDE Free
 * Qt Foundation. The licenses are as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 and LICENSE.GPLv3
 * included in the packaging of this file. Please review the following
 * information to ensure the GNU General Public License requirements will
 * be met: https://www.gnu.org/licenses/gpl-2.0.html and
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_EGLFSX11INTEGRATION_H
#define LIRI_EGLFSX11INTEGRATION_H

#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QtGui/qpa/qplatformwindow.h>

#include <Liri/Platform/EGLDeviceIntegration>

#include <xcb/xcb.h>

namespace Liri {

namespace Platform {

namespace Atoms {
    enum {
        _NET_WM_NAME = 0,
        UTF8_STRING,
        WM_PROTOCOLS,
        WM_DELETE_WINDOW,
        _NET_WM_STATE,
        _NET_WM_STATE_FULLSCREEN,

        N_ATOMS
    };
}

class EventReader;

class EglFSX11Integration : public EGLDeviceIntegration
{
public:
    EglFSX11Integration() : m_connection(0), m_window(0), m_eventReader(0) {}

    void platformInit() Q_DECL_OVERRIDE;
    void platformDestroy() Q_DECL_OVERRIDE;
    EGLNativeDisplayType platformDisplay() const Q_DECL_OVERRIDE;

    bool usesVtHandler() Q_DECL_OVERRIDE;
    bool handlesInput() Q_DECL_OVERRIDE;

    QSize screenSize() const Q_DECL_OVERRIDE;
    EGLNativeWindowType createNativeWindow(QPlatformWindow *window,
                                           const QSize &size,
                                           const QSurfaceFormat &format) Q_DECL_OVERRIDE;
    void destroyNativeWindow(EGLNativeWindowType window) Q_DECL_OVERRIDE;
    bool hasCapability(QPlatformIntegration::Capability cap) const Q_DECL_OVERRIDE;

    xcb_connection_t *connection() { return m_connection; }
    const xcb_atom_t *atoms() const { return m_atoms; }
    QPlatformWindow *platformWindow() { return m_platformWindow; }

private:
    void sendConnectionEvent(xcb_atom_t a);

    void *m_display;
    xcb_connection_t *m_connection;
    xcb_atom_t m_atoms[Atoms::N_ATOMS];
    xcb_window_t m_window;
    EventReader *m_eventReader;
    xcb_window_t m_connectionEventListener;
    QPlatformWindow *m_platformWindow;
    mutable QSize m_screenSize;
};

} // namespace Platform

} // namespace Liri

#endif // LIRI_EGLFSX11INTEGRATION_H
