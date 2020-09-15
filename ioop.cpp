#include "ioop.h"

#include <QUrl>
#include <QMessageBox>
#include <QFile>
#include <DNotifySender>
#include <QFileDialog>
#include <QDebug>

using Sender = Dtk::Core::DUtil::DNotifySender;

IOOP::IOOP(QObject *parent) : QObject(parent)
{
    box = new QMessageBox;
}

IOOP::~IOOP()
{
    box->~QMessageBox();
    path.~QUrl();
    data.~QString();
    if (file != nullptr) {
        file->~QFile();
    }
}

void IOOP::writeToFile(QUrl p, QString d)
{
    path = p;
    data = d;
    if (file != nullptr) {
        file->~QFile();
    }
    file = new QFile(path.path());
    opened = file->open(QIODevice::OpenModeFlag::NewOnly);
    if (opened != false) {
        continueOW();
    }
}

void IOOP::continueOW()
{
    if (opened == false) {
        opened = file->open(QIODevice::OpenModeFlag::Truncate);
    }
    if (opened == true) {
        file->write(data.toLatin1());
        file->flush();
    } else {
        Sender send("Failed writting to file");
        send.timeOut(3000);
        send.appIcon("preferences-system");
        send.appName("TLA");
        send.appBody("Error: " + file->errorString() + "\nFile: \"" + path.url() + "\"");
        send.call();
        send.~Sender();
    }
    clearIn();
}

void IOOP::clearIn()
{
    if (file != nullptr) {
        file->close();
        file->~QFile();
    }
    data.clear();
    path.clear();
}

void IOOP::openFile(QUrl p)
{
    file = new QFile(p.path());
    bool failed = true;
    if (file->open(QIODevice::OpenModeFlag::ReadOnly)) {
        if (file->size() > 0) {
            Q_EMIT fileDataRead(file->readAll());
            failed = false;
        }
    }
    if (failed == true) {
        Sender send("Failed to open file");
        send.timeOut(3000);
        send.appIcon("preferences-system");
        send.appName("TLA");
        send.appBody("Error: " + file->errorString() + "\nFile:\"" + p.url() + "\"");
        send.call();
        send.~Sender();
    }
    clearIn();
}

void IOOP::uiOpenFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, tr("Open a file"),
                                                "~/Documents", tr("Web pages (*.html)\n"
                                                  "Custom Data Type (*.cdt)\n"
                                                  "MarkDown (*.md)\n"));
    if (!file.isEmpty()) {
        openFile(QUrl(QString("file://%1").arg(file)));
    }
}

void IOOP::uiSaveFile(QString data)
{
    QString file = QFileDialog::getSaveFileName(nullptr, tr("Select a file"), "~/Documents", tr("Custom Data Type (*.cdt)\n"));
    if (!file.isEmpty()) {
        writeToFile(QUrl(QString("file://%1").arg(file)), data);
    }
}
