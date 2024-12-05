#include "EmulatorWindow.h"
#include <QMessageBox>
#include <QLabel>
#include <QApplication>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCloseEvent>

EmulatorWindow::EmulatorWindow(const QString& serverAddress, QWidget *parent)
        : QMainWindow(parent)
        , m_serverAddress(serverAddress)
        , m_socket(new QTcpSocket(this))
{
    setupUI();
    connectToEmulator();
}


EmulatorWindow::~EmulatorWindow()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void EmulatorWindow::updateScreen()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Socket state:" << m_socket->state()
                 << "Bytes available:" << m_socket->bytesAvailable()
                 << "Bytes to write:" << m_socket->bytesToWrite();

        QJsonObject request;
        request["type"] = "screen_request";
        QJsonDocument doc(request);
        QByteArray data = doc.toJson();
        qint64 written = m_socket->write(data);
        qDebug() << "Screen request sent, wrote" << written << "bytes";

        // Force the write
        m_socket->flush();
    }
}

void EmulatorWindow::connectToEmulator()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
    }

    connect(m_socket, &QTcpSocket::errorOccurred, this, &EmulatorWindow::handleConnectionError);
    connect(m_socket, &QTcpSocket::readyRead, this, &EmulatorWindow::handleServerData);

    m_socket->connectToHost(m_serverAddress, 12345);
}


void EmulatorWindow::handleConnectionError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error:" << error << "-" << m_socket->errorString();
    if (error == QAbstractSocket::RemoteHostClosedError) {
        // Try to reconnect
        QTimer::singleShot(1000, this, &EmulatorWindow::connectToEmulator);
    } else {
        QMessageBox::warning(this, "Connection Error",
                             "Lost connection to server: " + m_socket->errorString());
    }
}

void EmulatorWindow::handleServerData()
{
    QByteArray data = m_socket->readAll();
    qDebug() << "Received data bytes:" << data.size();
    qDebug() << "Raw data:" << data;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (doc.isNull()) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        qDebug() << "at offset:" << parseError.offset;
        return;
    }

    QJsonObject response = doc.object();
    qDebug() << "Response status:" << response["status"].toString();

    if (response["status"].toString() == "screen_data") {
        qDebug() << "Got screen data - width:" << response["width"].toInt()
                 << "height:" << response["height"].toInt()
                 << "data length:" << response["data"].toString().length();
        processScreenData(response);
    }
}

void EmulatorWindow::keepAlive()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject ping;
        ping["status"] = "ping";
        ping["type"] = "keepalive";  // Add this to distinguish from commands
        m_socket->write(QJsonDocument(ping).toJson());
    } else {
        qDebug() << "Socket disconnected, attempting to reconnect...";
        connectToEmulator();
    }
}
void EmulatorWindow::processScreenData(const QJsonObject& response)
{
    qDebug() << "Begin processing screen data";

    if (!response.contains("width") || !response.contains("height") || !response.contains("data")) {
        qDebug() << "Missing required fields in response:" << response.keys();
        return;
    }

    int width = response["width"].toInt();
    int height = response["height"].toInt();
    QString base64Data = response["data"].toString();

    qDebug() << "Screen data dimensions:" << width << "x" << height;
    qDebug() << "Base64 data length:" << base64Data.length();

    if (base64Data.isEmpty()) {
        qDebug() << "Received empty base64 data";
        return;
    }

    QByteArray imageData = QByteArray::fromBase64(base64Data.toLatin1());
    qDebug() << "Decoded image data size:" << imageData.size();

    QImage image;
    if (!image.loadFromData(imageData, "JPEG")) {
        qDebug() << "Failed to load JPEG image from data";
        // Try PNG format as fallback
        if (!image.loadFromData(imageData, "PNG")) {
            qDebug() << "Failed to load PNG image from data";
            return;
        }
    }

    qDebug() << "Successfully created image:" << image.size();

    if (image.isNull()) {
        qDebug() << "Created image is null";
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmap.isNull()) {
        qDebug() << "Failed to create pixmap from image";
        return;
    }

    QSize labelSize = m_screenLabel->size();
    qDebug() << "Label size for scaling:" << labelSize;

    QPixmap scaled = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qDebug() << "Created scaled pixmap:" << scaled.size();

    m_screenLabel->setPixmap(scaled);
    qDebug() << "Set pixmap to label";
}

void EmulatorWindow::setupUI()
{
    setWindowTitle("Emulator Stream");
    resize(1024, 768);

    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout(m_centralWidget);

    // Create and configure screen label
    m_screenLabel = new QLabel(this);
    m_screenLabel->setMinimumSize(640, 480);
    m_screenLabel->setAlignment(Qt::AlignCenter);
    m_screenLabel->setStyleSheet("QLabel { background-color: black; border: 2px solid red; }"); // Make it very visible
    m_screenLabel->setText("Waiting for screen data..."); // Add initial text
    m_screenLabel->setStyleSheet("QLabel { color: white; background-color: black; }");
    m_layout->addWidget(m_screenLabel);

    setCentralWidget(m_centralWidget);

    m_keepAliveTimer = new QTimer(this);
    connect(m_keepAliveTimer, &QTimer::timeout, this, &EmulatorWindow::keepAlive);
    m_keepAliveTimer->start(1000);

    m_screenUpdateTimer = new QTimer(this);
    connect(m_screenUpdateTimer, &QTimer::timeout, this, &EmulatorWindow::updateScreen);
    m_screenUpdateTimer->start(33); // ~30 FPS

    qDebug() << "UI setup complete. Window size:" << size();
    qDebug() << "Screen label size:" << m_screenLabel->size();
}