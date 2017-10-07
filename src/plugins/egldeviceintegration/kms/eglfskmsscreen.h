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

#ifndef LIRI_EGLFSKMSSCREEN_H
#define LIRI_EGLFSKMSSCREEN_H

#include <QtCore/QList>
#include <QtCore/QMutex>

#include <LiriEglFS/EglFSScreen>

#include "eglfskmsintegration.h"

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

namespace Liri {

namespace Platform {

class EglFSKmsDevice;
class EglFSKmsCursor;
class EglFSKmsInterruptHandler;

struct EglFSKmsOutput
{
    QString name;
    uint32_t connector_id;
    uint32_t crtc_id;
    QSizeF physical_size;
    int mode; // index of selected mode in list below
    int preferred_mode; // index of preferred mode in list below
    bool mode_set;
    drmModeCrtcPtr saved_crtc;
    QList<drmModeModeInfo> modes;
    drmModePropertyPtr dpms_prop;
    drmModePropertyBlobPtr edid_blob;
};

struct EglFSKmsEdid
{
    QString identifier;
    QString manufacturer;
    QString model;
    QString serialNumber;
    QSizeF physicalSize;
};

class EglFSKmsScreen : public EglFSScreen
{
public:
    EglFSKmsScreen(EglFSKmsIntegration *integration,
                   EglFSKmsDevice *device,
                   EglFSKmsOutput output,
                   QPoint position);
    ~EglFSKmsScreen();

    QRect geometry() const override;
    int depth() const override;
    QImage::Format format() const override;

    QSizeF physicalSize() const override;
    QDpi logicalDpi() const override;
    qreal pixelDensity() const override;
    Qt::ScreenOrientation nativeOrientation() const override;
    Qt::ScreenOrientation orientation() const override;

    QString name() const override;

    QPlatformCursor *cursor() const override;

    qreal refreshRate() const override;

    QList<QPlatformScreen *> virtualSiblings() const override { return m_siblings; }
    void setVirtualSiblings(QList<QPlatformScreen *> sl) { m_siblings = sl; }

    EglFSKmsIntegration *integration() const { return m_integration; }
    EglFSKmsDevice *device() const { return m_device; }

    gbm_surface *surface() const { return m_gbm_surface; }
    gbm_surface *createSurface();
    void destroySurface();

    void suspend();
    void resume();

    void waitForFlip();
    void flip();
    void flipFinished();

    bool isResizingSurface() const { return m_pendingMode >= 0; }
    void resizeSurface();

    EglFSKmsOutput &output() { return m_output; }
    void restoreMode();

    EglFSScreen::PowerState powerState() const override;
    void setPowerState(EglFSScreen::PowerState state) override;

    QVector<EglFSScreen::Mode> modes() const override;

    int currentMode() const override;
    void setCurrentMode(int modeId) override;

    int preferredMode() const override;

    QString manufacturer() const override;
    QString model() const override;
    QString serialNumber() const override;

private:
    EglFSKmsIntegration *m_integration;
    EglFSKmsDevice *m_device;
    gbm_surface *m_gbm_surface;

    gbm_bo *m_gbm_bo_current;
    gbm_bo *m_gbm_bo_next;

    bool m_suspend;

    int m_pendingMode;
    EglFSKmsEdid m_edid;
    EglFSKmsOutput m_output;
    QPoint m_pos;
    QScopedPointer<EglFSKmsCursor> m_cursor;

    QList<QPlatformScreen *> m_siblings;

    PowerState m_powerState;

    struct FrameBuffer {
        FrameBuffer() : fb(0) {}
        uint32_t fb;
    };
    static void bufferDestroyedHandler(gbm_bo *bo, void *data);
    FrameBuffer *framebufferForBufferObject(gbm_bo *bo);

    static QMutex m_waitForFlipMutex;

    EglFSKmsInterruptHandler *m_interruptHandler;

    enum EdidDescriptor {
        EdidDescriptorAlphanumericDataString = 0xfe,
        EdidDescriptorDisplayProductName = 0xfc,
        EdidDescriptorDisplayProductSerialNumber = 0xff
    };
    enum EdidOffset {
        EdidOffsetDataBlocks = 0x36,
        EdidOffsetLastBlock = 0x6c,
        EdidOffsetPnpId = 0x08,
        EdidOffsetSerial = 0x0c,
        EdidOffsetPhysicalWidth = 0x15,
        EdidOffsetPhysicalHeight = 0x16
    };
    bool parseEdid(EglFSKmsEdid &edid);
    QString parseEdidString(const quint8 *data);
};

} // namespace Platform

} // namespace Liri

#endif // LIRI_EGLFSKMSSCREEN_H
