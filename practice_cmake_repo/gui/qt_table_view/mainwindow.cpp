#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#define LOGGIN_MODE 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    //as far as I can see it begins from 0 counter.
    int lastRow = ui->tableWidget->rowCount();
#ifdef LOGGIN_MODE
    std::cout << lastRow << std::endl;
#endif

    QTableWidgetItem  *ID = new QTableWidgetItem("new id");
    QTableWidgetItem  *Connection  = new QTableWidgetItem("new connection");
    QTableWidgetItem  *osInfo = new QTableWidgetItem("New OS Info");
    QTableWidgetItem  *taskDescription = new QTableWidgetItem("New Task desc");

    ui->tableWidget->insertRow(lastRow);

    ui->tableWidget->setItem(lastRow, 0, ID);
    ui->tableWidget->setItem(lastRow, 1, Connection);
    ui->tableWidget->setItem(lastRow, 2, osInfo);
    ui->tableWidget->setItem(lastRow, 3, taskDescription);
#ifdef LOGGIN_MODE
    std::cout << "new rowCount() = " << ui->tableWidget->rowCount() << std::endl;
#endif
}

void MainWindow::on_pushButton_clicked()
{
    int lastRow = ui->tableWidget->rowCount() - 1;

    if(lastRow < 1) {
      return;
    }

#ifdef LOGGIN_MODE
    std::cout << "last row id for removin =  " << lastRow << std::endl;
#endif
    ui->tableWidget->removeRow(lastRow);
    ui->tableWidget->update();
}
