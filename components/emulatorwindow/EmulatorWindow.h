#ifndef EMULATORWINDOW_H
#define EMULATORWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QLabel>

class EmulatorWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit EmulatorWindow(const QString& serverAddress, QWidget *parent = nullptr);
    ~EmulatorWindow();

private slots:
    void handleConnectionError(QAbstractSocket::SocketError error);
    void handleServerData();
    void keepAlive();
    void updateScreen();

private:
    void setupUI();
    void connectToEmulator();
    void processScreenData(const QJsonObject& response);

    QTcpSocket *m_socket;
    QString m_serverAddress;
    QWidget *m_centralWidget;
    QVBoxLayout *m_layout;
    QLabel *m_screenLabel;
    QTimer *m_keepAliveTimer;
    QTimer *m_screenUpdateTimer;
};

#endif // EMULATORWINDOW_H