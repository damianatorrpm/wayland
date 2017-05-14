/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

#include <LiriEglFS/EglFSGlobal>

#include <QtCore/QElapsedTimer>
#include <QtCore/private/qobject_p.h>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformscreen.h>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandOutputMode>

#include "quickoutput.h"
#include "logging_p.h"
#include "extensions/screencaster.h"
#include "screen/screenbackend.h"

namespace Liri {

namespace WaylandServer {

/*
 * WindowFilter
 */

class WindowFilter : public QObject
{
public:
    WindowFilter(QObject *parent = Q_NULLPTR)
        : QObject(parent)
        , hotSpotLastTime(0)
        , hotSpotEntered(0)
    {
        quickOutput = static_cast<QuickOutput *>(parent);
        if (quickOutput) {
            connect(quickOutput, &QuickOutput::windowChanged, this, [this] {
                // Remove the event filter if it was previously installed
                if (!quickWindow.isNull()) {
                    quickWindow->removeEventFilter(this);
                    quickWindow.clear();
                }

                // Install the event filter when a window is set
                QQuickWindow *window = static_cast<QQuickWindow *>(quickOutput->window());
                if (window) {
                    quickWindow = window;
                    window->installEventFilter(this);
                    timer.restart();
                }
            });
        }
    }

    QuickOutput *quickOutput;
    QPointer<QQuickWindow> quickWindow;
    QElapsedTimer timer;
    quint64 hotSpotLastTime;
    quint64 hotSpotEntered;

protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE
    {
        // Do not filter other objects
        if (object != quickWindow)
            return QObject::eventFilter(object, event);

        // We are only interested in mouse move events
        if (event->type() != QEvent::MouseMove)
            return false;

        // We also cannot continue without the output
        if (!quickOutput)
            return false;

        // Coordinates
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        const QPoint pt = mouseEvent->localPos().toPoint();
        const QRect r = quickOutput->geometry();

        // Hot spots are triggered after the threshold
        qint64 time = timer.elapsed();
        if (time - hotSpotLastTime >= quickOutput->hotSpotThreshold()) {
            // Determine which hotspot was triggered
            QuickOutput::HotSpot hotSpot;
            const int w = quickOutput->hotSpotSize().width();
            const int h = quickOutput->hotSpotSize().height();
            bool triggered = true;
            if (pt.x() <= r.left() + w && pt.y() <= r.top() + h)
                hotSpot = QuickOutput::HotSpot::TopLeftHotSpot;
            else if (pt.x() >= r.right() - w && pt.y() <= r.top() + h)
                hotSpot = QuickOutput::HotSpot::TopRightHotSpot;
            else if (pt.x() <= r.left() + w && pt.y() >= r.bottom() - h)
                hotSpot = QuickOutput::HotSpot::BottomLeftHotSpot;
            else if (pt.x() >= r.right() - w && pt.y() >= r.bottom() - h)
                hotSpot = QuickOutput::HotSpot::BottomRightHotSpot;
            else {
                triggered = false;
                hotSpotEntered = 0;
            }

            // Trigger an action
            if (triggered) {
                if (hotSpotEntered == 0)
                    hotSpotEntered = time;
                else if (time - hotSpotEntered < quickOutput->hotSpotPushTime()) {
                    hotSpotLastTime = time;
                    Q_EMIT quickOutput->hotSpotTriggered(hotSpot);
                }
            }
        }

        return false;
    }
};

/*
 * OutputPrivate
 */

class QuickOutputPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QuickOutput)
public:
    QuickOutputPrivate()
        : initialized(false)
        , nativeScreen(Q_NULLPTR)
        , enabled(true)
        , hotSpotSize(QSize(5, 5))
        , hotSpotThreshold(1000)
        , hotSpotPushTime(50)
    {
    }

    bool initialized;
    Screen *nativeScreen;
    bool enabled;
    QSize hotSpotSize;
    quint64 hotSpotThreshold;
    quint64 hotSpotPushTime;
    QList<QObject *> objects;
};

/*
 * Output
 */

QuickOutput::QuickOutput()
    : QWaylandQuickOutput()
    , d_ptr(new QuickOutputPrivate())
{
    // Filter events on the output window
    new WindowFilter(this);
}

QuickOutput::QuickOutput(QWaylandCompositor *compositor)
    : QWaylandQuickOutput(compositor, Q_NULLPTR)
    , d_ptr(new QuickOutputPrivate())
{
    // Filter events on the output window
    new WindowFilter(this);

    // We cannot have multiple top level windows on the same screen
    // with our QPA plugin, hence set the screen as soon as possible
    connect(this, &QuickOutput::windowChanged, this, [this] {
        QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
        if (quickWindow && d_func()->nativeScreen)
            quickWindow->setScreen(d_func()->nativeScreen->screen());
    });
}

QQmlListProperty<QObject> QuickOutput::data()
{
    Q_D(QuickOutput);
    return QQmlListProperty<QObject>(this, d->objects);
}

Screen *QuickOutput::nativeScreen() const
{
    Q_D(const QuickOutput);
    return d->nativeScreen;
}

void QuickOutput::setNativeScreen(Screen *screen)
{
    Q_D(QuickOutput);

    if (d->nativeScreen == screen)
        return;

    if (d->initialized) {
        qCWarning(gLcCore)
                << "Setting Liri::WaylandServer::Output::nativeScreen "
                   "after initialization has no effect";
        return;
    }

    d->nativeScreen = screen;
    Q_EMIT nativeScreenChanged();

    // We cannot have multiple top level windows on the same screen
    // with our QPA plugin, hence set the screen as soon as possible
    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (quickWindow)
        quickWindow->setScreen(d->nativeScreen->screen());
}

bool QuickOutput::isEnabled() const
{
    Q_D(const QuickOutput);
    return d->enabled;
}

void QuickOutput::setEnabled(bool value)
{
    Q_D(QuickOutput);

    if (d->enabled == value)
        return;

    d->enabled = value;
    Q_EMIT enabledChanged();
}

QuickOutput::PowerState QuickOutput::powerState() const
{
    Q_D(const QuickOutput);

    // Power state is supported only with native screens and our QPA
    QPlatformScreen *screen = Q_NULLPTR;
    if (d->nativeScreen && d->nativeScreen->screen())
        screen = static_cast<QPlatformScreen *>(
                    d->nativeScreen->screen()->handle());
    if (!screen)
        return PowerStateOn;

    return static_cast<QuickOutput::PowerState>(screen->powerState());
}

void QuickOutput::setPowerState(PowerState state)
{
    Q_D(QuickOutput);

    // Power state is supported only with native screens and our QPA
    QPlatformScreen *screen = Q_NULLPTR;
    if (d->nativeScreen && d->nativeScreen->screen())
        screen = static_cast<QPlatformScreen *>(
                    d->nativeScreen->screen()->handle());
    if (!screen)
        return;

    QPlatformScreen::PowerState pstate =
            static_cast<QPlatformScreen::PowerState>(state);
    if (screen->powerState() != pstate) {
        screen->setPowerState(pstate);
        Q_EMIT powerStateChanged();
    }
}

QSize QuickOutput::hotSpotSize() const
{
    Q_D(const QuickOutput);
    return d->hotSpotSize;
}

void QuickOutput::setHotSpotSize(const QSize &size)
{
    Q_D(QuickOutput);

    if (d->hotSpotSize == size)
        return;

    d->hotSpotSize = size;
    Q_EMIT hotSpotSizeChanged();
}

quint64 QuickOutput::hotSpotThreshold() const
{
    Q_D(const QuickOutput);
    return d->hotSpotThreshold;
}

void QuickOutput::setHotSpotThreshold(quint64 value)
{
    Q_D(QuickOutput);

    if (d->hotSpotThreshold == value)
        return;

    d->hotSpotThreshold = value;
    Q_EMIT hotSpotThresholdChanged();
}

quint64 QuickOutput::hotSpotPushTime() const
{
    Q_D(const QuickOutput);
    return d->hotSpotPushTime;
}

void QuickOutput::setHotSpotPushTime(quint64 value)
{
    Q_D(QuickOutput);

    if (d->hotSpotThreshold == value)
        return;

    d->hotSpotPushTime = value;
    Q_EMIT hotSpotPushTimeChanged();
}

QuickOutput *QuickOutput::fromResource(wl_resource *resource)
{
    return qobject_cast<QuickOutput *>(QWaylandOutput::fromResource(resource));
}

void QuickOutput::initialize()
{
    Q_D(QuickOutput);

    QWaylandQuickOutput::initialize();

    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (!quickWindow) {
        qCWarning(gLcCore,
                  "Could not locate QQuickWindow on initializing"
                  "Liri::WaylandServer::Output %p.\n", this);
        return;
    }

    // We want to read contents for the screencaster
    connect(quickWindow, &QQuickWindow::afterRendering,
            this, &QuickOutput::readContent);

    // Add modes
    QList<Screen::Mode> modes = d->nativeScreen->modes();
    if (d->nativeScreen && modes.size() > 0 && !sizeFollowsWindow()) {
        int modeId = 0;
        Q_FOREACH (const Screen::Mode &mode, modes) {
            QWaylandOutputMode wlMode(mode.size, qRound(mode.refreshRate * 1000));
            addMode(wlMode, modeId == d->nativeScreen->preferredMode());
            modeId++;
        }

        // Set current mode
        int currentModeId = d->nativeScreen->currentMode();
        if (currentModeId < 0)
            currentModeId = 0;
        Screen::Mode currentMode = modes.at(currentModeId);
        setCurrentMode(QWaylandOutputMode(currentMode.size, qRound(currentMode.refreshRate * 1000)));
    }

    // Set the window visible now
    quickWindow->setVisible(true);

    d->initialized = true;
}

void QuickOutput::readContent()
{
    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (!quickWindow)
        return;

    QWaylandCompositorExtension *e = compositor()->extension(Screencaster::interfaceName());
    Screencaster *screencaster = qobject_cast<Screencaster *>(e);
    if (screencaster && screencaster->isInitialized())
        screencaster->recordFrame(quickWindow);
}

} // namespace WaylandServer

} // namespace Liri

#include "moc_quickoutput.cpp"
