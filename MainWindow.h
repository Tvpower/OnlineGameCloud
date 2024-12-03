#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QtNetwork/QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleServerResponse();
    void chooseEmulator1();
    void chooseEmulator2();
    void connectToServer();
private:
    QPushButton *emulatorBtn1;
    QPushButton *emulatorBtn2;
    QTcpSocket *m_socket;
};
#endif // MAINWINDOW_H