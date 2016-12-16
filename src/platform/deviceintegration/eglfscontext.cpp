/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini
 * Copyright (C) 2016 The Qt Company Ltd.
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "eglfsglobal.h"

#include <QtGui/QSurface>
#include <QtEglSupport/private/qeglconvenience_p.h>
#include <QtEglSupport/private/qeglpbuffer_p.h>

#include "logging.h"
#include "deviceintegration/egldeviceintegration.h"
#include "deviceintegration/eglfscontext.h"
#include "deviceintegration/eglfswindow.h"
#include "deviceintegration/eglfscursor.h"

namespace GreenIsland {

namespace Platform {

EglFSContext::EglFSContext(const QSurfaceFormat &format, QPlatformOpenGLContext *share, EGLDisplay display,
                           EGLConfig *config, const QVariant &nativeHandle)
    : QEGLPlatformContext(format, share, display, config, nativeHandle),
      m_tempWindow(0)
{
}

EGLSurface EglFSContext::eglSurfaceForPlatformSurface(QPlatformSurface *surface)
{
    if (surface->surface()->surfaceClass() == QSurface::Window)
        return static_cast<EglFSWindow *>(surface)->surface();
    else
        return static_cast<QEGLPbuffer *>(surface)->pbuffer();
}

EGLSurface EglFSContext::createTemporaryOffscreenSurface()
{
    if (egl_device_integration()->supportsPBuffers())
        return QEGLPlatformContext::createTemporaryOffscreenSurface();

    if (!m_tempWindow) {
        m_tempWindow = egl_device_integration()->createNativeOffscreenWindow(format());
        if (!m_tempWindow) {
            qCWarning(lcDeviceIntegration, "EglFSContext: Failed to create temporary native window");
            return EGL_NO_SURFACE;
        }
    }
    EGLConfig config = q_configFromGLFormat(eglDisplay(), format());
    return eglCreateWindowSurface(eglDisplay(), config, m_tempWindow, 0);
}

void EglFSContext::destroyTemporaryOffscreenSurface(EGLSurface surface)
{
    if (egl_device_integration()->supportsPBuffers()) {
        QEGLPlatformContext::destroyTemporaryOffscreenSurface(surface);
    } else {
        eglDestroySurface(eglDisplay(), surface);
        egl_device_integration()->destroyNativeWindow(m_tempWindow);
        m_tempWindow = 0;
    }
}

void EglFSContext::runGLChecks()
{
    // Note that even though there is an EGL context current here,
    // QOpenGLContext and QOpenGLFunctions are not yet usable at this stage

    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    // Warn about limited performances that llvmpipe users might expect
    if (renderer && strstr(renderer, "llvmpipe"))
        qCWarning(lcDeviceIntegration, "Running on a software rasterizer (LLVMPipe), expect limited performance.");
}

void EglFSContext::swapBuffers(QPlatformSurface *surface)
{
    // draw the cursor
    if (surface->surface()->surfaceClass() == QSurface::Window) {
        QPlatformWindow *window = static_cast<QPlatformWindow *>(surface);
        if (EglFSCursor *cursor = qobject_cast<EglFSCursor *>(window->screen()->cursor()))
            cursor->paintOnScreen();
    }

    egl_device_integration()->waitForVSync(surface);
    if (egl_device_integration()->isResizingSurface(surface)) {
        QEGLPlatformContext::doneCurrent();
        egl_device_integration()->resizeSurface(surface);
        QEGLPlatformContext::makeCurrent(surface);
    }
    QEGLPlatformContext::swapBuffers(surface);
    egl_device_integration()->presentBuffer(surface);
}

} // namespace Platform

} // namespace GreenIsland
