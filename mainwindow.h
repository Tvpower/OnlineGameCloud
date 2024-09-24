#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
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
    void chooseEmulator1();
    void chooseEmulator2();
private:
    QPushButton *emulatorBtn1;
    QPushButton *emulatorBtn2;
};
#endif // MAINWINDOW_H
