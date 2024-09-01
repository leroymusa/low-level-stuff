#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "../inc/hypervisor.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
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
    void on_createButton_clicked();
    void on_destroyButton_clicked();
    void on_messageButton_clicked();
    void updateTable();

private:
    Ui::MainWindow *ui;
    Hypervisor hypervisor;
};
#endif // MAINWINDOW_H
