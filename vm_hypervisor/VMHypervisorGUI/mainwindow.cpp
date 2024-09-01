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
    qDebug() << "Create VM button clicked";
    int program[] = {PUSH, 10, PUSH, 5, ADD, PRINT, HALT};
    create_vm(&hypervisor, program, 1);
    updateTable();
    ui->statusLabel->setText("VM Created");
    qDebug() << "VM Created. Total VMs:" << hypervisor.vm_count;
}

void MainWindow::on_destroyButton_clicked()
{
    qDebug() << "Destroy VM button clicked";
    if (hypervisor.vm_count > 0)
    {
        destroy_vm(&hypervisor, hypervisor.vm_count - 1);
        updateTable();
        ui->statusLabel->setText("VM Destroyed");
        qDebug() << "VM Destroyed. Total VMs:" << hypervisor.vm_count;
    }
    else
    {
        ui->statusLabel->setText("No VMs to Destroy");
        qDebug() << "No VMs to destroy";
    }
}

void MainWindow::on_messageButton_clicked()
{
    qDebug() << "Send Message button clicked";
    if (hypervisor.vm_count > 1)
    {
        send_message(&hypervisor, 0, 1, "Hello from GUI");
        ui->statusLabel->setText("Message Sent");
        qDebug() << "Message sent from VM 0 to VM 1";
    }
    else
    {
        ui->statusLabel->setText("Not Enough VMs to Send Message");
        qDebug() << "Not enough VMs to send a message";
    }
}

void MainWindow::updateTable()
{
    if (!ui->vmTable)
    {
        return;
    }

    int rowCount = hypervisor.vm_count;
    if (rowCount > MAX_VMS)
    {
        rowCount = MAX_VMS;
    }

    ui->vmTable->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i)
    {
        VM *vm = &hypervisor.vms[i];

        if (vm->priority < 0 || vm->priority > 100)
        { // Example validation
            printf("Invalid VM state detected. Skipping VM index %d\n", i);
            continue;
        }

        qDebug() << "Updating table for VM index:" << i;
        qDebug() << "VM Running State:" << vm->running;
        qDebug() << "VM Priority:" << vm->priority;

        ui->vmTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        ui->vmTable->setItem(i, 1, new QTableWidgetItem(vm->running ? "Running" : "Stopped"));
        ui->vmTable->setItem(i, 2, new QTableWidgetItem(QString::number(vm->priority)));
    }
}
