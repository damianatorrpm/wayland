/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QtCore/QThread>
#include <QtTest/QtTest>

#include <LiriWaylandClient/Buffer>
#include <LiriWaylandClient/ClientConnection>
#include <LiriWaylandClient/Compositor>
#include <LiriWaylandClient/Registry>
#include <LiriWaylandClient/Shm>
#include <LiriWaylandClient/ShmPool>

#include <QtWaylandCompositor/QWaylandCompositor>

using namespace Liri;

static const QString s_socketName = QStringLiteral("liri-test-0");

class TestShmPool : public QObject
{
    Q_OBJECT
public:
    TestShmPool(QObject *parent = nullptr)
        : QObject(parent)
        , m_compositor(nullptr)
        , m_thread(nullptr)
        , m_display(nullptr)
        , m_shm(nullptr)
        , m_shmPool(nullptr)
    {
    }

private:
    QWaylandCompositor *m_compositor;
    QThread *m_thread;
    WaylandClient::ClientConnection *m_display;
    WaylandClient::Shm *m_shm;
    WaylandClient::ShmPool *m_shmPool;

private Q_SLOTS:
    void init()
    {
        delete m_compositor;
        m_compositor = new QWaylandCompositor(this);
        m_compositor->setSocketName(s_socketName.toUtf8());
        m_compositor->create();

        delete m_display;
        m_display = new WaylandClient::ClientConnection();
        m_display->setSocketName(s_socketName);

        delete m_thread;
        m_thread = new QThread(this);
        m_display->moveToThread(m_thread);
        m_thread->start();

        QSignalSpy connectedSpy(m_display, SIGNAL(connected()));
        QSignalSpy failedSpy(m_display, SIGNAL(failed()));
        m_display->initializeConnection();
        QVERIFY(connectedSpy.wait());
        QCOMPARE(connectedSpy.count(), 1);
        QCOMPARE(failedSpy.count(), 0);
        QVERIFY(m_display->display());

        WaylandClient::Registry registry;
        registry.create(m_display->display());
        QSignalSpy shmAnnounced(&registry, SIGNAL(shmAnnounced(quint32,quint32)));
        QVERIFY(shmAnnounced.isValid());
        registry.setup();

        QVERIFY(shmAnnounced.wait());

        const quint32 shmName = shmAnnounced.first().first().value<quint32>();
        const quint32 shmVersion = shmAnnounced.first().last().value<quint32>();
        delete m_shm;
        m_shm = registry.createShm(shmName, shmVersion, this);
        QVERIFY(m_shm);

        delete m_shmPool;
        m_shmPool = m_shm->createPool(1024);
    }

    void cleanup()
    {
        delete m_shmPool;
        m_shmPool = nullptr;

        delete m_shm;
        m_shm = nullptr;

        delete m_compositor;
        m_compositor = nullptr;

        if (m_thread) {
            m_thread->quit();
            m_thread->wait();
            delete m_thread;
            m_thread = nullptr;
        }

        delete m_display;
        m_display = nullptr;
    }

    void testCreateEmptyBuffer()
    {
        QSize size(0, 0);
        QVERIFY(size.isNull());
        QVERIFY(!m_shmPool->createBuffer(size, 0));
    }

    void testCreateBufferFromNullImage()
    {
        QImage image;
        QVERIFY(image.isNull());
        QVERIFY(!m_shmPool->createBuffer(image));
    }

    void testCreateBufferInvalidSize()
    {
        QSize size;
        QVERIFY(!size.isValid());
        QVERIFY(!m_shmPool->createBuffer(size, 0));
    }

    void testCreateBufferFromImage()
    {
        QImage image(42, 42, QImage::Format_RGB888);
        image.fill(Qt::red);
        QVERIFY(!image.isNull());

        WaylandClient::BufferSharedPtr buffer = m_shmPool->createBuffer(image).toStrongRef();
        QVERIFY(buffer);
        QCOMPARE(image.byteCount(), buffer->stride() * buffer->size().height());
        QCOMPARE(image.size(), buffer->size());

        QImage image2(buffer->address(), buffer->size().width(),
                      buffer->size().height(), QImage::Format_RGB888);
        QCOMPARE(image, image2);
    }

    void testCreateBufferFromData()
    {
        QImage image(42, 42, QImage::Format_RGB888);
        image.fill(Qt::red);
        QVERIFY(!image.isNull());

        WaylandClient::BufferSharedPtr buffer = m_shmPool->createBuffer(image.size(),
                                                                 image.bytesPerLine(), image.constBits()).toStrongRef();
        QVERIFY(buffer);
        QCOMPARE(image.byteCount(), buffer->stride() * buffer->size().height());
        QCOMPARE(image.size(), buffer->size());

        QImage image2(buffer->address(), buffer->size().width(),
                      buffer->size().height(), QImage::Format_RGB888);
        QCOMPARE(image, image2);
    }

    void testReuseBuffer()
    {
        QImage image(42, 42, QImage::Format_RGB888);
        image.fill(Qt::red);
        QVERIFY(!image.isNull());

        WaylandClient::BufferSharedPtr buffer = m_shmPool->createBuffer(image).toStrongRef();
        QVERIFY(buffer);
        buffer->setReleased(true);
        buffer->setUsed(false);

        // The second buffer should be the same because it's using the same image
        WaylandClient::BufferSharedPtr buffer2 = m_shmPool->createBuffer(image).toStrongRef();
        QCOMPARE(buffer, buffer2);
        buffer2->setReleased(true);
        buffer2->setUsed(false);

        // A bigger image should get a new buffer
        QImage image2(96, 96, QImage::Format_RGB888);
        image2.fill(Qt::red);
        QVERIFY(!image2.isNull());
        QVERIFY(image2 != image);
        WaylandClient::BufferSharedPtr buffer3 = m_shmPool->createBuffer(image2).toStrongRef();
        QVERIFY(buffer3 != buffer2);

        // An image with a different format should get a new buffer
        QImage image3(42, 42, QImage::Format_ARGB32);
        image3.fill(Qt::red);
        QVERIFY(!image3.isNull());
        QVERIFY(image3 != image);
        QVERIFY(image3 != image2);
        WaylandClient::BufferSharedPtr buffer4 = m_shmPool->createBuffer(image3).toStrongRef();
        QVERIFY(buffer4 != buffer2);
        QVERIFY(buffer4 != buffer3);
    }
};

QTEST_MAIN(TestShmPool)

#include "tst_shmpool.moc"
