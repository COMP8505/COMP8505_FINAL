#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cnc_channel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Cnc_Channel cnc_channel;

private slots:
    void appendText(QString text);
    void on_pushButton_Enter_clicked();
    void on_pushButton_Test_clicked();
    void on_pushButton_Upload_clicked();

    void on_pushButton_Download_clicked();

private:
    bool promptForInfo();

    Ui::MainWindow *ui;
    std::string interfaceName;
    int listenPort;
};

#endif // MAINWINDOW_H
