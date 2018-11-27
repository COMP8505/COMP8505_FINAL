#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testing.h"
#include "covert_channel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Testing t;
    //t.run_tests();

    cnc_channel.start("eno1");

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    cnc_channel.send_run_cmd("ifconfig");
}
