#include "mainwindow.h"
#include <QApplication>
#include <QtGlobal>
#include <iostream>

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);
    std::cout << msg.toStdString() << std::endl;

    QString formattedMsg = msg;
    if (type == QtWarningMsg) {
        formattedMsg = QString("<span style='color:#ffb74d;'>[WARN] %1</span>").arg(msg);
    } else if (type == QtCriticalMsg || type == QtFatalMsg) {
        formattedMsg = QString("<span style='color:#ff5252; font-weight:bold;'>[ERR] %1</span>").arg(msg);
    } else if (msg.contains("Error") || msg.contains("aborted") || msg.contains("invalid")) {
        formattedMsg = QString("<span style='color:#ff5252;'>%1</span>").arg(msg);
    } else if (msg.contains("completed") || msg.contains("ready") || msg.contains("valid")) {
        formattedMsg = QString("<span style='color:#69f0ae;'>%1</span>").arg(msg);
    }

    MainWindow::logOutput(formattedMsg);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    qInstallMessageHandler(customLogHandler);

    MainWindow w;
    w.show();

    return a.exec();
}
