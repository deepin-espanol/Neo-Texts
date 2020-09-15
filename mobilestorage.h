#ifndef MOBILESTORAGE_H
#define MOBILESTORAGE_H

#include "helperclass.h"
#include "dopeniconbutton.h"

#include <QMenu>
#include <QSpinBox>
#include <QMenu>
#include <DSpinBox>
#include <DComboBox>
#include <QTextCharFormat>
#include <QFontDatabase>
#include <QUrl>

enum SelectionType {
    Char,
    Image,
    TableComponent
};

DWIDGET_USE_NAMESPACE

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
    SelectionType type = SelectionType::Char;

    HelperClass *editor = nullptr;

Q_SIGNALS:
    void requestChanges(QTextCharFormat format);
    void addImage();
    void makeOutput();
    void reqRmUnderCursor();
    void reqMenuPopup(QMenu*);
    void requestImageByURL(QUrl u);
    void reqRmCells(Qt::AnchorPoint p, int toRm = 1);
    void reqAddCells(Qt::AnchorPoint p, int toAdd = 1);
    void reqInsertCells(Qt::AnchorPoint p, int toInsert = 1);
    void reqAddTable();

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
    QMenu *m_tableOptions = nullptr;
    QMenu *m_tInsertions = nullptr;

    QAction *overline = nullptr;
    QAction *italic = nullptr;
    QAction *underline = nullptr;

    QActionGroup *styleGroup = nullptr;
    QTextImageFormat *m_if = nullptr;

    QTextCharFormat m_format;
    QString oldFont;

    bool firstShown = true;
};
#endif // MOBILESTORAGE_H
