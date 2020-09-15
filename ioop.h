#ifndef IOOP_H
#define IOOP_H

#include <QFile>
#include <QUrl>

class QMessageBox;

class IOOP : public QObject
{
    Q_OBJECT
public:
    explicit IOOP(QObject *parent = nullptr);
    ~IOOP();

    inline static IOOP *instance() {
        static IOOP *inst = new IOOP;
        return inst;
    }

signals:
    void fileDataRead(QString data);
    void fileOpenedPath(QUrl p);

public slots:
    void writeToFile(QUrl path, QString data);
    void openFile(QUrl path);
    void uiOpenFile();
    void uiSaveFile(QString data);

private Q_SLOTS:
    void continueOW();
    void clearIn();

private:
    QMessageBox *box;
    QFile *file = nullptr;
    QUrl path;
    QString data;
    bool opened = false;
};

#endif // IOOP_H
