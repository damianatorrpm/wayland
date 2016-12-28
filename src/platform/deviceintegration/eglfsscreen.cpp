/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include <QtCore/qtextstream.h>
#include <QtGui/qwindow.h>
#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QtGui/qpa/qplatformcursor.h>
#include <QtPlatformCompositorSupport/private/qopenglcompositor_p.h>

#include "logging.h"
#include "deviceintegration/egldeviceintegration.h"
#include "deviceintegration/eglfsscreen.h"
#include "deviceintegration/eglfswindow.h"

namespace Liri {

namespace Platform {

EglFSScreen::EglFSScreen(EGLDisplay dpy)
    : m_dpy(dpy),
      m_pointerWindow(0),
      m_surface(EGL_NO_SURFACE),
      m_cursor(0)
{
    m_cursor = egl_device_integration()->createCursor(this);
}

EglFSScreen::~EglFSScreen()
{
    delete m_cursor;
    QOpenGLCompositor::destroy();
}

QRect EglFSScreen::geometry() const
{
    QRect r = rawGeometry();

    static int rotation = qEnvironmentVariableIntValue("LIRI_QPA_EGLFS_ROTATION");
    switch (rotation) {
    case 0:
    case 180:
    case -180:
        break;
    case 90:
    case -90: {
        int h = r.height();
        r.setHeight(r.width());
        r.setWidth(h);
        break;
    }
    default:
        qWarning("Invalid rotation %d specified in LIRI_QPA_EGLFS_ROTATION", rotation);
        break;
    }

    return r;
}

QRect EglFSScreen::rawGeometry() const
{
    return QRect(QPoint(0, 0), egl_device_integration()->screenSize());
}

int EglFSScreen::depth() const
{
    return egl_device_integration()->screenDepth();
}

QImage::Format EglFSScreen::format() const
{
    return egl_device_integration()->screenFormat();
}

QSizeF EglFSScreen::physicalSize() const
{
    return egl_device_integration()->physicalScreenSize();
}

QDpi EglFSScreen::logicalDpi() const
{
    return egl_device_integration()->logicalDpi();
}

qreal EglFSScreen::pixelDensity() const
{
    return egl_device_integration()->pixelDensity();
}

Qt::ScreenOrientation EglFSScreen::nativeOrientation() const
{
    return egl_device_integration()->nativeOrientation();
}

Qt::ScreenOrientation EglFSScreen::orientation() const
{
    return egl_device_integration()->orientation();
}

QPlatformCursor *EglFSScreen::cursor() const
{
    return m_cursor;
}

qreal EglFSScreen::refreshRate() const
{
    return egl_device_integration()->refreshRate();
}

void EglFSScreen::setPrimarySurface(EGLSurface surface)
{
    m_surface = surface;
}

void EglFSScreen::handleCursorMove(const QPoint &pos)
{
    const QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    const QList<QOpenGLCompositorWindow *> windows = compositor->windows();

    // Generate enter and leave events like a real windowing system would do.
    if (windows.isEmpty())
        return;

    // First window is always fullscreen.
    if (windows.count() == 1) {
        QWindow *window = windows[0]->sourceWindow();
        if (m_pointerWindow != window) {
            m_pointerWindow = window;
            QWindowSystemInterface::handleEnterEvent(window, window->mapFromGlobal(pos), pos);
        }
        return;
    }

    QWindow *enter = 0, *leave = 0;
    for (int i = windows.count() - 1; i >= 0; --i) {
        QWindow *window = windows[i]->sourceWindow();
        const QRect geom = window->geometry();
        if (geom.contains(pos)) {
            if (m_pointerWindow != window) {
                leave = m_pointerWindow;
                m_pointerWindow = window;
                enter = window;
            }
            break;
        }
    }

    if (enter && leave)
        QWindowSystemInterface::handleEnterLeaveEvent(enter, leave, enter->mapFromGlobal(pos), pos);
}

QPixmap EglFSScreen::grabWindow(WId wid, int x, int y, int width, int height) const
{
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    const QList<QOpenGLCompositorWindow *> windows = compositor->windows();
    Q_ASSERT(!windows.isEmpty());

    QImage img;

    if (static_cast<EglFSWindow *>(windows.first()->sourceWindow()->handle())->isRaster()) {
        // Request the compositor to render everything into an FBO and read it back. This
        // is of course slow, but it's safe and reliable. It will not include the mouse
        // cursor, which is a plus.
        img = compositor->grab();
    } else {
        // Just a single OpenGL window without compositing. Do not support this case for now. Doing
        // glReadPixels is not an option since it would read from the back buffer which may have
        // undefined content when calling right after a swapBuffers (unless preserved swap is
        // available and enabled, but we have no support for that).
        qCWarning(lcDeviceIntegration)
                << "grabWindow: Not supported for non-composited OpenGL content."
                << "Use QQuickWindow::grabWindow() instead.";
        return QPixmap();
    }

    if (!wid) {
        const QSize screenSize = geometry().size();
        if (width < 0)
            width = screenSize.width() - x;
        if (height < 0)
            height = screenSize.height() - y;
        return QPixmap::fromImage(img).copy(x, y, width, height);
    }

    foreach (QOpenGLCompositorWindow *w, windows) {
        const QWindow *window = w->sourceWindow();
        if (window->winId() == wid) {
            const QRect geom = window->geometry();
            if (width < 0)
                width = geom.width() - x;
            if (height < 0)
                height = geom.height() - y;
            QRect rect(geom.topLeft() + QPoint(x, y), QSize(width, height));
            rect &= window->geometry();
            return QPixmap::fromImage(img).copy(rect);
        }
    }

    return QPixmap();
}

/*!
  Returns the list of modes.

  The default implementation always returns a list with
  only one mode from the current screen size and refresh rate.

  \sa QPlatformScreen::geometry
  \sa QPlatformScreen::refreshRate
*/
QList<EglFSScreen::Mode> EglFSScreen::modes() const
{
    QList<EglFSScreen::Mode> list;
    list.append({geometry().size(), refreshRate()});
    return list;
}

/*!
  Returns the index of the current mode from the modes list.

  The default implementation always returns 0.

  \sa QPlatformScreen::modes
*/
int EglFSScreen::currentMode() const
{
    return 0;
}

/*!
  Sets the index of the current mode from the modes list.

  The default implementation does nothing.

  \sa QPlatformScreen::modes
*/
void EglFSScreen::setCurrentMode(int modeId)
{
    Q_UNUSED(modeId);
}

/*!
  Returns the index of the preferred mode from the modes list.

  The default implementation always returns 0.

  \sa QPlatformScreen::modes
*/
int EglFSScreen::preferredMode() const
{
    return 0;
}

/*!
  Sets the index of the preferred mode from the modes list.

  The default implementation does nothing.

  \sa QPlatformScreen::modes
*/
void EglFSScreen::setPreferredMode(int modeId)
{
    Q_UNUSED(modeId);
}

/*!
  Returns the screen identifier.

  The default implementation always returns an empty string.
*/
QString EglFSScreen::identifier() const
{
    return QString();
}

/*!
  Returns the screen manufacturer.

  The default implementation always returns an empty string.
*/
QString EglFSScreen::manufacturer() const
{
    return QString();
}

/*!
  Returns the screen model.

  The default implementation always returns an empty string.
*/
QString EglFSScreen::model() const
{
    return QString();
}

/*!
  Returns the screen serial number.

  The default implementation always returns an empty string.
*/
QString EglFSScreen::serialNumber() const
{
    return QString();
}

} // namespace Platform

} // namespace Liri
