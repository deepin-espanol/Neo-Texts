#ifndef EDITOR_H
#define EDITOR_H

#include "dopeniconbutton.h"
#include "mobilestorage.h"

#include <QTextEdit>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextCursor>
#include <QNetworkAccessManager>
#include <QNetworkReply>

DWIDGET_USE_NAMESPACE

class Editor : public QTextEdit, public HelperClass
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor() override;
    void setContent(QString data);
    void setMargin(qreal value);

    QWidget *downBar = nullptr;
    QTextCharFormat format();
    MobileStorage *storage();
    static QNetworkRequest generate(QUrl p);
    QMenu *generateMenu() override;
    inline QUrl getFileUrl() {return openedFile;}

Q_SIGNALS:
    void newWidthAvailable(int w);
    void newCursorAvailable(QTextCharFormat format, bool emptySelection);

public Q_SLOTS:
    void applyTextCursor(QTextCharFormat format);
    void imageInsertion();
    void imageInsertionByPath(QUrl URI, bool isLocalFS = false);
    void generateOutput();
    void removeUnderCursor();
    void popupMenu(QMenu *);
    void error(QNetworkReply::NetworkError);
    void updateProgress(qint64 a, qint64 total);
    void finished();
    void parseImageResourceRequest(QUrl path, bool handleInsertion = false);
    void showURLInput();
    void insertTable();
    void saveRequest();
    void saveAsRequest();
    void setFilePath(QUrl p);
    void insertCells(Qt::AnchorPoint p, int cellsToAdd = 1);
    void addCells(Qt::AnchorPoint p, int cellsToAdd = 1);
    void removeCells(Qt::AnchorPoint p, int cellsToAdd = 1);
    void helperRequest() override;

protected:
    void insertFromMimeData(const QMimeData *source) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    bool handlePinch(QMouseEvent *e);

private:
    MobileStorage *m_storage = nullptr;
    bool lockMouse = false;
    int oldPos = 0;
    QNetworkAccessManager *m_manager;
    QNetworkRequest request;
    QNetworkReply *_reply;
    int toprocess = 0;
    int processed = 0;
    bool loadingData = false;
    QList<QUrl> waitingRequests;
    QUrl openedFile;
};

#endif // EDITOR_H
