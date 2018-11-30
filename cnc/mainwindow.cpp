#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testing.h"

#include <iostream>
#include <QInputDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bool ok;
    QString text = QInputDialog::getText(this, tr("Input Interface Name"),
                                         tr("Interface:"), QLineEdit::Normal,
                                         "eno1", &ok);
    if (ok && !text.isEmpty()){
        interfaceName = text.toStdString();
    }
    else{
        QApplication::quit();
    }

    ok = false;
    int port = QInputDialog::getInt(this, tr("Input Listen Port"),
                                         tr("Listen Port:"),
                                         9000, 0, 65535, 1, &ok);
    if (ok && !text.isEmpty()){

        listenPort = port;
        std::cout << "listenPort: " << listenPort << std::endl;
    }
    else{
        QApplication::quit();
    }

    QObject::connect(&cnc_channel, SIGNAL(appendText(QString)),
                     this, SLOT(appendText(QString)));

    //Testing t;
    //t.run_tests();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendText(QString text){
    std::cout << "appendText: " << text.toStdString() << std::endl;
    ui->textEdit_Shell->append(text);
}

void MainWindow::on_pushButton_Enter_clicked()
{
    string cmd = ui->lineEdit_Command->text().toStdString();
    cnc_channel.send_run_cmd(cmd);
}

void MainWindow::on_pushButton_Test_clicked()
{
    string ip = ui->lineEdit_IP->text().toStdString();
    int port = ui->lineEdit_Port->text().toInt();
    cnc_channel.start(ip, port, interfaceName, listenPort);
}
