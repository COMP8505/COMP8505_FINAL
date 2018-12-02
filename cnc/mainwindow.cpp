#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testing.h"

#include <iostream>
#include <QInputDialog>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!promptForInfo())
        return;

    QObject::connect(&cnc_channel, SIGNAL(appendText(QString)),
                     this, SLOT(appendText(QString)));

    Testing t;
    t.runtests();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::promptForInfo(){
    bool ok;
    QString text = QInputDialog::getText(this, tr("Input Interface Name"),
                                         tr("Interface:"), QLineEdit::Normal,
                                         "eno1", &ok);
    if (ok && !text.isEmpty()){
        interfaceName = text.toStdString();
    }
    else{
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        return false;
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
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        return false;
    }
    return true;
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

void MainWindow::on_pushButton_Upload_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);\
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    for ( const auto& i : fileNames  )
    {
        cnc_channel.send_file(i.toStdString());
    }
}

void MainWindow::on_pushButton_Download_clicked()
{
    string filepath = ui->lineEdit_Download->text().toStdString();
    cnc_channel.get_file(filepath);
}
