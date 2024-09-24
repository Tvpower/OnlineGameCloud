#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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

void MainWindow::chooseEmulator1()
{
    // Handle the first emulator selection
    // For now, just print to the console
    qDebug("Emulator 1 chosen");
}

void MainWindow::chooseEmulator2()
{
    // Handle the second emulator selection
    // For now, just print to the console
    qDebug("Emulator 2 chosen");
}
