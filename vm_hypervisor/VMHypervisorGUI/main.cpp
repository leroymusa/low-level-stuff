#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "../inc/hypervisor.h"

Hypervisor hypervisor = {.current_vm = 0, .vm_count = 0, .active_vms = 0};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "VMHypervisorGUI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
