#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "../inc/hypervisor.h"

// I declared in order they were initialized to prevent
// the ISO C++ warnings
Hypervisor hypervisor = {
    .vms = {{
        .stack = {0},              // Initialize the stack array
        .sp = 0,                   // Stack pointer
        .ip = 0,                   // Instruction pointer
        .running = 0,              // Running state
        .program = nullptr,        // Program pointer
        .memory = {0},             // Initialize memory array
        .message_queue = {{0}},    // Initialize message queue
        .msg_count = 0,            // Message count
        .priority = 0              // Priority level
    }},
    .current_vm = 0,
    .vm_count = 0,
    .active_vms = 0
};

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
