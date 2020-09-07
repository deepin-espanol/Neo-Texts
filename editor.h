#ifndef EDITOR_H
#define EDITOR_H

#include "dopeniconbutton.h"

#include <QTextEdit>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextCursor>
#include <QSpinBox>
#include <QMenu>
#include <DSpinBox>
#include <DComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define CYELLOW Qt::yellow
#define CORANGE QColor::fromRgb(244, 67, 54)
#define CRED Qt::red
#define CPINK QColor::fromRgb(255, 80, 140)
#define CPURPLE QColor::fromRgb(156, 39, 176)
#define CCYAN QColor::fromRgb(0, 204, 175)
#define CAZURE QColor::fromRgb(0, 188, 212)
#define CBLUE Qt::blue
#define CBLACK Qt::black
#define CPEAR QColor::fromRgb(139, 195, 74)
#define CGREEN Qt::green
#define CGRAY Qt::gray
#define CWHITE Qt::white
#define CMARINBLUE QColor::fromRgb(63, 81, 181)

DWIDGET_USE_NAMESPACE

class Editor;

class MobileStorage : public QObject {
    Q_OBJECT
public:
    explicit MobileStorage(QObject *parent = nullptr);
    ~MobileStorage();

    QList<QWidget *> getBar();

    QMenu *mtextStyles();
    QMenu *mchangeCase();
    QMenu *mforegroundc();
    QMenu *mbackgroundc();
    QMenu *mextras();
    QMenu *selectionOptions();

    QHash<QString, QColor> colorHash;
    QHash<QString, QAction*> styleHash;
    bool isAnImage = false;

    Editor *editor = nullptr;

Q_SIGNALS:
    void requestChanges(QTextCharFormat format);
    void addImage();
    void makeOutput();
    void reqRmUnderCursor();
    void reqMenuPopup(QMenu*);
    void requestImageByURL(QUrl u);

public Q_SLOTS:
    void setTextCharFormat(QTextCharFormat format);
    void update();
    void selectionMenuPopup();

protected Q_SLOTS:
    void changeBg(QAction *);
    void changeFg(QAction *);
    void changeFontFamily(QString key);
    void changeFontStyle(QAction *);
    void changeFontPointSize(double v);
    void changeFontWeight(int v);
    void makeUpperCase();
    void makeSmallCaps();
    void makeLowerCase();
    void makeCapitalization();
    void changeFontCaps(QFont::Capitalization cap);
    void toggleUnderline();
    void toggleItalic();
    void toggleOverline();
    void openInBrowser();

protected:
    DOpenIconButton *textStyles = nullptr;
    DOpenIconButton *foregroundc = nullptr;
    DOpenIconButton *backgroundc = nullptr;
    DOpenIconButton *changeCase = nullptr;
    DOpenIconButton *extras = nullptr;
    DComboBox *family = nullptr;
    QDoubleSpinBox *pointSize = nullptr;
    QSpinBox *fontWeight = nullptr;
    QFontDatabase *base = nullptr;

    QMenu *m_textStyles = nullptr;
    QMenu *m_foregroundc = nullptr;
    QMenu *m_backgroundc = nullptr;
    QMenu *m_changeCase = nullptr;
    QMenu *m_extras = nullptr;
    QMenu *mimageOptions = nullptr;
    QMenu *m_otherHandling = nullptr;

    QAction *overline = nullptr;
    QAction *italic = nullptr;
    QAction *underline = nullptr;

    QActionGroup *styleGroup = nullptr;
    QTextImageFormat *m_if = nullptr;

    QTextCharFormat m_format;
    QString oldFont;

    bool firstShown = true;
};

class Editor : public QTextEdit
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();
    void setContent(QString data);
    void setMargin(qreal value);

    QWidget *downBar = nullptr;
    QTextCharFormat format();
    MobileStorage *storage();
    static QNetworkRequest generate(QUrl p);

Q_SIGNALS:
    void newWidthAvailable(int w);
    void newCursorAvailable(QTextCharFormat format, bool emptySelection);

public Q_SLOTS:
    void applyTextCursor(QTextCharFormat format);
    void imageInsertion();
    void imageInsertionByPath(QUrl URI);
    void generateOutput();
    void removeUnderCursor();
    void popupMenu(QMenu *);
    void error(QNetworkReply::NetworkError);
    void updateProgress(qint64 a, qint64 total);
    void finished();
    void parseImageResourceRequest(QUrl path);

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
    QSet<QUrl> waitingRequests;
};

#endif // EDITOR_H
