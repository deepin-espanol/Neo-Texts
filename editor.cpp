#include "editor.h"
#include "imagepopup.h"

#include <QDebug>
#include <QApplication>
#include <QFontDatabase>
#include <QActionGroup>
#include <QFileDialog>
#include <QImageReader>
#include <QAbstractTextDocumentLayout>
#include <QMimeData>

const QRegExp imageSupport("[\\w\\-.]+\\.(jpg|JPG|png|PNG|gif|GIF|jpeg|JPEG)");

Editor::Editor(QWidget *parent) : QTextEdit(parent)
{
    this->setContent("That's wondeful bruh!");
    this->setMargin(50);
    m_storage = new MobileStorage;
    Q_EMIT this->newCursorAvailable(this->format(), false);

    connect(m_storage, &MobileStorage::requestChanges, this, &Editor::applyTextCursor);
    connect(this, &Editor::newCursorAvailable, m_storage, &MobileStorage::setTextCharFormat);
    connect(document(), &QTextDocument::cursorPositionChanged, this, [this](QTextCursor cursored) {
        Q_EMIT this->newCursorAvailable(cursored.charFormat(), cursored.selectedText().isEmpty());
    });
    connect(m_storage, &MobileStorage::addImage, this, &Editor::imageInsertion);
}

Editor::~Editor() {}

void Editor::setContent(QString data)
{
    this->setHtml(data);
}

void Editor::setMargin(qreal v)
{
    QTextFrame* root = document()->rootFrame();
    QTextFrameFormat format = root->frameFormat();
    format.setMargin(v);
    root->setFrameFormat(format);
}

QTextCharFormat Editor::format()
{
    return this->textCursor().charFormat();
}

void Editor::applyTextCursor(QTextCharFormat cur)
{
    this->textCursor().mergeCharFormat(cur);
}

MobileStorage *Editor::storage()
{
    return m_storage;
}

void Editor::imageInsertion()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
                                  ".", tr("Bitmap Files (*.bmp)\n"
                                    "JPEG (*.jpg *jpeg)\n"
                                    "GIF (*.gif)\n"
                                    "PNG (*.png)\n"));
    QUrl Uri(QString("file://%1").arg(file));
    imageInsertionByPath(Uri);
 }

void Editor::imageInsertionByPath(QUrl URI)
{
    QImage image = QImageReader(URI.toString()).read();
    qDebug() << image.isNull();
    document()->addResource(QTextDocument::ImageResource, URI, QVariant(image));
    QTextCursor cursor = textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setName(URI.toString());
    imageFormat.setWidth(250);
    imageFormat.setHeight(250);
    cursor.insertImage(imageFormat);
}

bool Editor::canInsertFromMimeData(const QMimeData *source) const
{
    int i = 0;
    while (i<source->urls().length()) {
        if (source->urls().at(i).toString().contains(imageSupport)) {
            return true;
        }
        i++;
    }
    return false;
}

void Editor::insertFromMimeData(const QMimeData *source)
{
    int i = 0;
    bool oneIsNotSupported = false;
    while (i<source->urls().length()) {
        if (source->urls().at(i).toString().contains(imageSupport)) {
            imageInsertionByPath(source->urls().at(i));
            source->urls().removeAt(i);
        } else {
            oneIsNotSupported = true;
        }
        i++;
    }
    if (oneIsNotSupported == true) {
        if (QTextEdit::canInsertFromMimeData(source)) {
            QTextEdit::insertFromMimeData(source);
        }
    }
}

void Editor::mouseReleaseEvent(QMouseEvent * event)
{
    int pos = this->document()->documentLayout()->hitTest(event->pos(), Qt::HitTestAccuracy::ExactHit);
    qDebug() << "Clicke>d" << pos;
    if(pos>-1)
    {
        QTextCursor cursor(document());
        cursor.setPosition(pos);
        if(!cursor.atEnd())
        {
            cursor.setPosition(pos+1);
            QTextFormat format = cursor.charFormat();
            if(format.isImageFormat())
            {
                ImageViewer viewer;
                viewer.open(format.toImageFormat().name());
                viewer.exec();
            }
        }
    }
    QTextEdit::mouseReleaseEvent(event);
}

MobileStorage::MobileStorage(QObject *parent) : QObject(parent) {
    colorHash[tr("Yellow")] = CYELLOW;
    colorHash[tr("Orange")] = CORANGE;
    colorHash[tr("Red")] = CRED;
    colorHash[tr("Pink")] = CPINK;
    colorHash[tr("Purple")] = CPURPLE;
    colorHash[tr("Cyan")] = CCYAN;
    colorHash[tr("Azure")] = CAZURE;
    colorHash[tr("Blue")] = CBLUE;
    colorHash[tr("Marin Blue")] = CMARINBLUE;
    colorHash[tr("Green")] = CGREEN;
    colorHash[tr("Pear")] = CPEAR;
    colorHash[tr("White")] = CWHITE;
    colorHash[tr("Gray")] = CGRAY;
    colorHash[tr("Black")] = CBLACK;

    base = new QFontDatabase;
    textStyles = new DOpenIconButton;
    changeCase = new DOpenIconButton;
    foregroundc = new DOpenIconButton;
    backgroundc = new DOpenIconButton;
    extras = new DOpenIconButton;

    textStyles->setMenuAnchors(Qt::AnchorPoint::AnchorLeft, Qt::AnchorPoint::AnchorLeft);
    changeCase->setMenuAnchors(Qt::AnchorPoint::AnchorLeft, Qt::AnchorPoint::AnchorLeft);
    foregroundc->setMenuAnchors(Qt::AnchorPoint::AnchorLeft, Qt::AnchorPoint::AnchorLeft);
    extras->setMenuAnchors(Qt::AnchorPoint::AnchorLeft, Qt::AnchorPoint::AnchorLeft);
    backgroundc->setMenuAnchors(Qt::AnchorPoint::AnchorLeft, Qt::AnchorPoint::AnchorLeft);

    backgroundc->setIcon(QIcon(":/icons/highlighter-solid.svg"));
    foregroundc->setIcon(QIcon(":/icons/palette-solid.svg"));
    textStyles->setIcon(QIcon(":/icons/font-solid.svg"));
    extras->setIcon(QIcon(":/icons/style.svg"));
    changeCase->setIcon(QIcon(":/icons/foreground.svg"));

    pointSize = new DDoubleSpinBox;
    fontWeight = new QSpinBox;
    family = new DComboBox;
    pointSize->setMinimum(1);
    pointSize->setMaximum(500);
    pointSize->setValue(11);

    family->addItems(base->families());

    connect(pointSize, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &MobileStorage::changeFontPointSize);
    connect(fontWeight, qOverload<int>(&QSpinBox::valueChanged), this, &MobileStorage::changeFontWeight);
    connect(family, &DComboBox::currentTextChanged, this, &MobileStorage::changeFontFamily);

    connect(extras, &DOpenIconButton::requestMenu, this, [this]() {extras->handleMenuRequest(mextras());});
    connect(textStyles, &DOpenIconButton::requestMenu, this, [this]() {textStyles->handleMenuRequest(mtextStyles());});
    connect(changeCase, &DOpenIconButton::requestMenu, this, [this]() {changeCase->handleMenuRequest(mchangeCase());});
    connect(foregroundc, &DOpenIconButton::requestMenu, this, [this]() {foregroundc->handleMenuRequest(mforegroundc());});
    connect(backgroundc, &DOpenIconButton::requestMenu, this, [this]() {backgroundc->handleMenuRequest(mbackgroundc());});

    textStyles->setToolTip(tr("Font style"));
    changeCase->setToolTip(tr("Text case options"));
    family->setToolTip(tr("Font family"));
    pointSize->setToolTip(tr("Font's point size"));
    fontWeight->setToolTip(tr("Character's weight"));
    foregroundc->setToolTip(tr("Text color"));
    backgroundc->setToolTip(tr("Text background"));
    extras->setToolTip(tr("Extra options"));
}

MobileStorage::~MobileStorage() {
    base->~QFontDatabase();
    m_textStyles->~QMenu();
    m_foregroundc->~QMenu();
    m_backgroundc->~QMenu();
    m_changeCase->~QMenu();
    oldFont.~QString();
    fontWeight->~QSpinBox();
    pointSize->~QDoubleSpinBox();
    family->~QComboBox();
}

void MobileStorage::setTextCharFormat(QTextCharFormat cur, bool empty) {
    skipNext = empty;
    m_format = cur;
    pointSize->setValue(cur.fontPointSize());
    family->setCurrentText(cur.fontFamily());
    fontWeight->setValue(cur.fontWeight());
}

void makeColorActions(QHash<QString, QColor> hash, QActionGroup *m)
{
    int i = 0;
    while (i<hash.keys().length()) {
        m->addAction(hash.keys().at(i));
        i++;
    }
}

QList<QAction*> makeStyles(QStringList styles, QActionGroup *group)
{
    qDebug() << "making style actions with: " << styles;
    QList<QAction *> data;
    int i = 0;
    while(i<styles.length()) {
        data << group->addAction(styles.at(i));
        qDebug() << styles.at(i);
        i++;
    }
    return data;
}

QMenu *MobileStorage::mtextStyles()
{
    //skipNext = true;
    if (m_textStyles == nullptr) {
        m_textStyles = new QMenu;
        QAction *act = m_textStyles->addAction(tr("Add image"));
        connect(act, &QAction::triggered, this, &MobileStorage::addImage);
    }
    if (styleGroup == nullptr) {
        styleGroup = new QActionGroup(m_textStyles);
    }

    if (oldFont != m_format.font().family() || firstShown == true) {
        if (firstShown == false) {
            int i = 0;
            int len = styleGroup->actions().length();
            while(i<len) {
                styleGroup->removeAction(styleGroup->actions().at(0));
                i++;
            }
        }
        oldFont = m_format.font().family();
        m_textStyles->addActions(makeStyles(base->styles(oldFont), styleGroup));
        firstShown = false;
    }
    return m_textStyles;
}

QMenu *MobileStorage::mchangeCase()
{
    //skipNext = true;
    if (m_changeCase == nullptr) {
        m_changeCase = new QMenu;
        QAction *capitol = m_changeCase->addAction(tr("Capitalize"));
        QAction *upper = m_changeCase->addAction(tr("Set as uppercase"));
        QAction *lower = m_changeCase->addAction(tr("Set as lowercase"));
        QAction *small = m_changeCase->addAction(tr("Use small caps"));
        connect(upper, &QAction::triggered, this, &MobileStorage::makeUpperCase);
        connect(lower, &QAction::triggered, this, &MobileStorage::makeLowerCase);
        connect(small, &QAction::triggered, this, &MobileStorage::makeSmallCaps);
        connect(capitol, &QAction::triggered, this, &MobileStorage::makeCapitalization);
    }
    return m_changeCase;
}

QMenu *MobileStorage::mbackgroundc()
{
    //skipNext = true;
    if (m_backgroundc == nullptr) {
        m_backgroundc = new QMenu;
        QActionGroup *group = new QActionGroup(m_backgroundc);
        makeColorActions(colorHash, group);
        connect(group, &QActionGroup::triggered, this, &MobileStorage::changeBg);
        m_backgroundc->addActions(group->actions());
        group->setExclusive(true);
    }
    return m_backgroundc;
}

QMenu *MobileStorage::mforegroundc()
{
    //skipNext = true;
    if (m_foregroundc == nullptr) {
        m_foregroundc = new QMenu;
        QActionGroup *group = new QActionGroup(m_foregroundc);
        makeColorActions(colorHash, group);
        connect(group, &QActionGroup::triggered, this, &MobileStorage::changeFg);
        m_foregroundc->addActions(group->actions());
        group->setExclusive(true);
    }
    return m_foregroundc;
}

QMenu *MobileStorage::mextras()
{
    //skipNext = true;
    if (m_extras == nullptr) {
        m_extras = new QMenu;
    }
    if (overline == nullptr) {
        overline = m_extras->addAction(tr("Overline"));
        connect(overline, &QAction::triggered, this, &MobileStorage::toggleOverline);
    }
    if (underline == nullptr) {
        underline = m_extras->addAction(tr("Underline"));
        connect(underline, &QAction::triggered, this, &MobileStorage::toggleUnderline);
        underline->setIcon(QIcon(":/icons/underline-solid.svg"));
    }
    if (italic == nullptr) {
        italic = m_extras->addAction(tr("Italic"));
        connect(italic, &QAction::triggered, this, &MobileStorage::toggleItalic);
        italic->setIcon(QIcon(":/icons/italic-solid.svg"));
    }
    return m_extras;
}

QList<QWidget*> MobileStorage::getBar()
{
    QList<QWidget*> list;
    list << family
         << pointSize
         << fontWeight
         << textStyles
         << foregroundc
         << backgroundc
         << extras
         << changeCase;
    return list;
}

void MobileStorage::update()
{
    skipNext = false;
    Q_EMIT requestChanges(m_format);
}

void MobileStorage::changeBg(QAction *act)
{
    m_format.setBackground(colorHash[act->text()]);
    update();
}

void MobileStorage::changeFg(QAction *act)
{
    m_format.setForeground(colorHash[act->text()]);
    update();
}

void MobileStorage::changeFontPointSize(double v)
{
    m_format.setFontPointSize(v);
    update();
}

void MobileStorage::changeFontFamily(QString key)
{
    m_format.setFontFamily(key);
    update();
}

void MobileStorage::changeFontStyle(QAction *act)
{
    m_format.font().setStyle((QFont::Style)QMetaEnum::fromType<QFont::Style>().keyToValue(act->text().toLocal8Bit()));
    update();
}

void MobileStorage::changeFontCaps(QFont::Capitalization cap)
{
    m_format.setFontCapitalization(cap);
    update();
}

void MobileStorage::changeFontWeight(int v)
{
    m_format.font().setWeight(v);
    update();
}

void MobileStorage::makeLowerCase() {changeFontCaps(QFont::Capitalization::AllLowercase);}

void MobileStorage::makeSmallCaps() {changeFontCaps(QFont::Capitalization::SmallCaps);}

void MobileStorage::makeUpperCase() {changeFontCaps(QFont::Capitalization::AllUppercase);}

void MobileStorage::makeCapitalization() {changeFontCaps(QFont::Capitalization::Capitalize);}

void MobileStorage::toggleItalic()
{
    m_format.setFontItalic(!m_format.fontItalic());
    update();
}

void MobileStorage::toggleOverline()
{
    m_format.setFontOverline(!m_format.fontOverline());
    update();
}

void MobileStorage::toggleUnderline()
{
    m_format.setUnderlineColor(m_format.foreground().color());
    m_format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::SingleUnderline);
    m_format.font().setUnderline(m_format.font().underline());
    update();
}
