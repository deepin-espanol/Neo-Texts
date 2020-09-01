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

    QHash<QString, QColor> colorHash;
    QHash<QString, QAction*> styleHash;

Q_SIGNALS:
    void requestChanges(QTextCharFormat format);
    void addImage();

public Q_SLOTS:
    void setTextCharFormat(QTextCharFormat format, bool emptySelection);
    void update();

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

    QAction *overline = nullptr;
    QAction *italic = nullptr;
    QAction *underline = nullptr;

    QActionGroup *styleGroup = nullptr;
    QTextCharFormat m_format;
    QString oldFont;

    bool firstShown = true;
    bool skipNext = false; //Used for issue when opening a QMenu.
};

class Editor : public QTextEdit
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();
    void setContent(QString data);
    void setMargin(qreal value);

    QTextCharFormat format();
    MobileStorage *storage();

Q_SIGNALS:
    void newWidthAvailable(int w);
    void newCursorAvailable(QTextCharFormat format, bool emptySelection);

public Q_SLOTS:
    void applyTextCursor(QTextCharFormat format);
    void imageInsertion();
    void imageInsertionByPath(QUrl URI);

protected:
    void insertFromMimeData(const QMimeData *source) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QString privateTest = "LOL it works bruh";
    MobileStorage *m_storage = nullptr;
};

#endif // EDITOR_H
