#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hypervisor.h"

extern Hypervisor hypervisor;
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    int program[] = {PUSH, 10, PUSH, 5, ADD, PRINT, HALT};
    if (hypervisor.vm_count < MAX_VMS)
    {
        create_vm(&hypervisor, program, 1);
        updateTable();
        ui->statusLabel->setText("VM Created Successfully");
    }
    else
    {
        ui->statusLabel->setText("Error: Maximum VM limit reached.");
    }
}

void MainWindow::on_destroyButton_clicked()
{
    if (hypervisor.vm_count > 0)
    {
        destroy_vm(&hypervisor, hypervisor.vm_count - 1);
        updateTable();
        ui->statusLabel->setText("VM Destroyed Successfully");
    }
    else
    {
        ui->statusLabel->setText("Error: No VM to destroy.");
    }
}

void MainWindow::on_messageButton_clicked()
{
    if (hypervisor.vm_count > 1)
    {
        send_message(&hypervisor, 0, 1, "Hello from GUI");
        ui->statusLabel->setText("Message Sent Successfully");
    }
    else
    {
        ui->statusLabel->setText("Error: Not enough VMs to send message.");
    }
}

void MainWindow::updateTable()
{
    ui->vmTable->setRowCount(hypervisor.vm_count);
    for (int i = 0; i < hypervisor.vm_count; ++i)
    {
        if (i >= MAX_VMS) break;

        QString runningState = hypervisor.vms[i].running ? "Running" : "Stopped";
        int priority = hypervisor.vms[i].priority;

        if (priority < 0 || priority > 10)
        {
            priority = 0;
        }

        ui->vmTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        ui->vmTable->setItem(i, 1, new QTableWidgetItem(runningState));
        ui->vmTable->setItem(i, 2, new QTableWidgetItem(QString::number(priority)));
    }
}
