#include "mainwindow.h"
#include "components/emulatorwindow/EmulatorWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{

    // Set default window size
    resize(800, 450);

    m_socket = new QTcpSocket(this);

    // Add these connection lines
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerResponse);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &MainWindow::handleSocketError);


    // Title
    QLabel *titleLabel = new QLabel("<h1>Online Game Cloud</h1>");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Instructions
    QLabel *instructions = new QLabel("<h3>Choose An Emulator To Begin!</h3>");
    instructions->setAlignment(Qt::AlignCenter);

    // Create the buttons
    emulatorBtn1 = new QPushButton("Choose Emulator 1", this);
    emulatorBtn2 = new QPushButton("Choose Emulator 2", this);

    // Set button sizes (making them bigger)
    emulatorBtn1->setMinimumSize(200, 100);
    emulatorBtn2->setMinimumSize(200, 100);

    // Connect buttons to actions (slots)
    connect(emulatorBtn1, &QPushButton::clicked, this, &MainWindow::chooseEmulator1);
    connect(emulatorBtn2, &QPushButton::clicked, this, &MainWindow::chooseEmulator2);

    // Set up a vertical layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(titleLabel);
    layout->addWidget(instructions);
    layout->addWidget(emulatorBtn1);
    layout->addWidget(emulatorBtn2);

    // Create a central widget and set the layout
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Set the window title
    setWindowTitle("Emulator Selection");
}

MainWindow::~MainWindow()
{
}

void MainWindow::connectToServer()
{
    QString serverAddress = "20.150.209.32";  // Your Azure VM IP
    quint16 serverPort = 12345;  // Match the server port

    // Disconnect if already connected
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForDisconnected(1000);
    }

    // Connect to the server
    m_socket->connectToHost(serverAddress, serverPort);

    // Check connection status
    if (m_socket->waitForConnected(5000)) {
        qDebug() << "Connected to server successfully!";
    } else {
        qDebug() << "Could not connect to server";
        QMessageBox::warning(this, "Connection Failed",
                             "Could not connect to server: " + m_socket->errorString());
    }
}

void MainWindow::handleSocketError(QAbstractSocket::SocketError error)
{
    QString errorMessage = "Network error: " + m_socket->errorString();
    QMessageBox::critical(this, "Connection Error", errorMessage);
}

void MainWindow::handleServerResponse()
{
    QByteArray data = m_socket->readAll();
    qDebug() << "Raw server response:" << data;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON response";
        QMessageBox::warning(this, "Error", "Invalid response from server");
        return;
    }

    QJsonObject response = doc.object();
    qDebug() << "Parsed response:" << response;

    if (response["status"].toString() == "success") {
        qDebug() << "Success response received";
        EmulatorWindow* emulatorWindow = new EmulatorWindow(m_socket->peerAddress().toString(), this);
        emulatorWindow->setAttribute(Qt::WA_DeleteOnClose);
        emulatorWindow->show();
    } else {
        QString errorMsg = response["message"].toString();
        qDebug() << "Error response received:" << errorMsg;
        QMessageBox::warning(this, "Error",
                             "Failed to launch emulator: " + errorMsg);
    }
}

void MainWindow::chooseEmulator1()
{
    connectToServer();

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray message = "EMULATOR1";
        m_socket->write(message);
        if (m_socket->waitForBytesWritten(1000)) {
            qDebug() << "Emulator 1 selection sent to server";
        } else {
            qDebug() << "Failed to send data to server";
            QMessageBox::warning(this, "Send Failed",
                                 "Failed to send selection to server");
        }
    }
}

void MainWindow::chooseEmulator2()
{
    connectToServer();

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray message = "EMULATOR2";
        m_socket->write(message);
        if (m_socket->waitForBytesWritten(1000)) {
            qDebug() << "Emulator 2 selection sent to server";
        } else {
            qDebug() << "Failed to send data to server";
            QMessageBox::warning(this, "Send Failed",
                                 "Failed to send selection to server");
        }
    }
}