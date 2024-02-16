#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_player = new music_player(ui, this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_player;
}
