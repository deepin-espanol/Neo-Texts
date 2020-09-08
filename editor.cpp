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
#include <QNetworkReply>
#include <QDesktopServices>
#include <QNetworkDiskCache>
#include <QDir>
#include <DDialog>
#include <DLineEdit>
#include <DPushButton>

const QRegExp imageSupport("[\\w\\-.]+\\.(jpg|JPG|png|PNG|gif|GIF|jpeg|JPEG)");
QRegExp imageInclusion("<img src=\"(http|https|ftp|file)://([a-zA-Z.]*)/([a-zA-Z0-9.?%/_\\-]*).(png|jpg|gif|JPEG|PNG|GIF)\" width=\"([0-9]*)\" height=\"([0-9]*)\" />");
const QString imgp = "https://upload.wikimedia.org/wikipedia/commons/thumb/b/b6/Image_created_with_a_mobile_phone.png/250px-Image_created_with_a_mobile_phone.png";

const QRegularExpression imageURIP("(http|https|ftp|file)://([a-zA-Z0-9.]+)([a-zA-Z0-9.?%/_#\\\\-]+).(png|jpg|gif|JPEG|PNG|GIF)",
                                   QRegularExpression::MultilineOption
                                   | QRegularExpression::DotMatchesEverythingOption
                                   | QRegularExpression::CaseInsensitiveOption);

Editor::Editor(QWidget *parent) : QTextEdit(parent)
{
    m_storage = new MobileStorage;
    m_manager = new QNetworkAccessManager;
    m_storage->editor = this;

    imageInclusion.setMinimal(true);

    Q_EMIT this->newCursorAvailable(this->format(), false);

    connect(this, &Editor::newCursorAvailable, m_storage, &MobileStorage::setTextCharFormat);
    connect(m_storage, &MobileStorage::requestChanges, this, &Editor::applyTextCursor);
    connect(m_storage, &MobileStorage::addImage, this, &Editor::imageInsertion);
    connect(m_storage, &MobileStorage::makeOutput, this, &Editor::generateOutput);
    connect(m_storage, &MobileStorage::reqMenuPopup, this, &Editor::popupMenu);
    connect(m_storage, &MobileStorage::reqRmUnderCursor, this, &Editor::removeUnderCursor);
    //connect(m_storage, &MobileStorage::requestImageByURL, this, &Editor::parseImageResourceRequest);
    connect(document(), &QTextDocument::cursorPositionChanged, this, [this](QTextCursor cursored) {
        Q_EMIT this->newCursorAvailable(cursored.charFormat(), cursored.selectedText().isEmpty());
    });
    this->setContent(QString("<p>That's wondeful bruh!</p><img src=\""+imgp+"\" width=\"250\" height=\"250\" />"));
}

Editor::~Editor() {}

void Editor::parseImageResourceRequest(QUrl u, bool handleInsertion)
{
    qDebug() << "Adding to the document";
    if (QFile("/tmp/neotexts/"+u.toString().replace("://", ":/").replace("/", "%")).exists() == false) {
        if (waitingRequests.contains(u) == false) {
            qDebug() << "No waiting calls for this ressource.";
            waitingRequests << u;
            _reply = m_manager->get(generate(u));
            connect(_reply, &QNetworkReply::finished, this, &Editor::finished);
            connect(_reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, &Editor::error);
            connect(_reply, &QNetworkReply::downloadProgress, this, &Editor::updateProgress);
            qDebug() << "Next";
            toprocess++;
            connect(_reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, [this](QNetworkReply::NetworkError e) {qDebug() << e << ": " << _reply->errorString();});
        }
    } else {
        if (handleInsertion == true) {
            qDebug() << "image already exists!";
            QString value = "/tmp/neotexts/"+u.toString().replace("://", ":/").replace("/", "%");
            QImage image = QImageReader(value).read();
            qDebug() << image.isNull();
            document()->addResource(QTextDocument::ImageResource, value, QVariant(image));
            QTextCursor cursor = textCursor();
            QTextImageFormat imageFormat;
            imageFormat.setName(value);
            imageFormat.setWidth(250);
            imageFormat.setHeight(250);
            cursor.insertImage(imageFormat);
        }
    }
}

void Editor::setContent(QString data)
{
    QDir d("/tmp/neotexts");
    if (!d.exists()) {
        d.mkdir("/tmp/neotexts");
    }
    this->setMargin(50);
    imageInclusion.indexIn(data);
    int MaxCaptures = imageInclusion.matchedLength();
    for (int i = 0; i < MaxCaptures; ++i) {
        if (imageInclusion.cap(i).contains(imageInclusion)) {
            qDebug() << "Image data path found";
            parseImageResourceRequest(imageURIP.match(imageInclusion.cap(0)).captured(0), false);
        }
    }
    QString translated = data;
    imageInclusion.indexIn(data);
    for (int i = 0; i < MaxCaptures; ++i) {
        if (imageInclusion.cap(i).contains(imageInclusion)) {
            QString filePath = imageURIP.match(imageInclusion.cap(0)).captured(0);
            QString result = "/tmp/neotexts/"+filePath.replace("://", ":/").replace("/", "%");
            qDebug() << "\nLooking for:" << imageURIP.match(imageInclusion.cap(0)).captured(0) << "\n\nTo replace with:" << result << "\n";
            translated.replace(imageURIP.match(imageInclusion.cap(0)).captured(0), result);
        }
    }
    qDebug() << translated;
    this->setHtml(translated);
    this->setMargin(50);
    this->document()->clearUndoRedoStacks();
}

QNetworkRequest Editor::generate(QUrl p)
{
    QNetworkRequest r(p);
    r.setAttribute(QNetworkRequest::Attribute::FollowRedirectsAttribute, true);
    return r;
}

void Editor::resizeEvent(QResizeEvent *e)
{
   // Q_EMIT sizeChanged(QSize(e->size().width(), e->size().height()-80));
    this->QTextEdit::resizeEvent(e);
}

void Editor::error(QNetworkReply::NetworkError err)
{
    qDebug() << "Error while downloading data: " << err;
    _reply->deleteLater();
}

void Editor::updateProgress(qint64 read, qint64 total)
{
    qDebug() << "Download state: " << read << "/" << total;
}

void Editor::finished()
{
    qDebug() << "Processing received request";
    processed++;
    QByteArray b = _reply->readAll();
    QString result = "/tmp/neotexts/"+_reply->url().toString().replace("://", ":/").replace("/", "%");
    qDebug() << result;
    QFile file(result);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    file.write(b);
    file.close();
    qDebug() << "Save ended";
    QImage i = QImageReader(b).read();
    document()->addResource(QTextDocument::ImageResource, QUrl(result), QVariant(i));
    qDebug() << "Updating layout visual";
    this->setLineWrapColumnOrWidth(this->lineWrapColumnOrWidth()); //Only way to make it reload our images (sadly)
    _reply->deleteLater();
    if (toprocess == processed) {
        toprocess = 0;
        processed = 0;
        waitingRequests.clear();
    }
}

void Editor::showURLInput()
{
    DDialog *d = new DDialog;
    QVBoxLayout *mainL = new QVBoxLayout(d);
    QHBoxLayout *subL = new QHBoxLayout(d);
    DLabel *l = new DLabel(d);
    DPushButton *cancel = new DPushButton(d);
    DPushButton *submit = new DPushButton(d);
    QWidget *container = new QWidget(d);
    QLineEdit *edit = new QLineEdit(d);
    DIconButton *clear = new DIconButton(d);
    QHBoxLayout *editL = new QHBoxLayout(d);

    submit->setText(tr("Confirm"));
    cancel->setText(tr("Cancel"));
    l->setText(tr("Select the image URl"));

    clear->setIconSize(QSize(16, 16));
    clear->setFixedSize(16, 16);
    clear->setVisible(false);
    clear->setFlat(true);
    clear->setAccessibleName("Clear");
    connect(clear, &DIconButton::clicked, edit, &QLineEdit::clear);

    editL->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    editL->addWidget(clear);
    edit->setPlaceholderText(tr("http://path.to.image/gif/png/jpg.jpeg"));
    edit->setLayout(editL);

    mainL->addWidget(l);
    mainL->addWidget(edit);
    mainL->addLayout(subL);
    subL->addWidget(submit);
    subL->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    subL->addWidget(cancel);
    container->setLayout(mainL);
    d->addContent(container);

    connect(cancel, &DPushButton::clicked, this, [d]() {d->~DDialog();});
    connect(edit, &QLineEdit::returnPressed, this, [d, edit, container, this]() {
        this->parseImageResourceRequest(edit->text(), true);
        container->~QWidget();
        d->close();
    });
    connect(submit, &DPushButton::clicked, this, [d, edit, container, this] {
        this->parseImageResourceRequest(edit->text(), true);
        container->~QWidget();
        d->close();
    });

    d->exec();
}

void Editor::generateOutput()
{
    qDebug() << this->toHtml() << "\n";
    imageInclusion.indexIn(this->toHtml());
    int MaxCaptures = imageInclusion.matchedLength();
    for (int i = 0; i < MaxCaptures; ++i) {
        if (imageInclusion.cap(i).contains(imageInclusion)) {
            qDebug() << imageInclusion.cap(i);
        }
    }
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
    if (file != "file://") {
        imageInsertionByPath(QUrl(QString("file://%1").arg(file)));
    }
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
    //Maybe QTextEdit handles something we support ourselves, less work!
    return this->QTextEdit::canInsertFromMimeData(source);
}

void Editor::insertFromMimeData(const QMimeData *source)
{
    int i = 0;
    bool oneIsNotSupported = false;
    if (source->hasText() && source->hasText() == false) {
        textCursor().insertText(source->text());
    } else if (source->hasHtml() && source->hasUrls() == false) {
        textCursor().insertHtml(source->html());
    } else if (source->hasUrls()) {
        while (i<source->urls().length()) {
           if (source->urls().at(i).toString().contains(imageSupport)) {
               imageInsertionByPath(source->urls().at(i));
               source->urls().removeAt(i);
           } else {
               oneIsNotSupported = true;
           }
           i++;
       }
    }
}

// Editor::handlePinch() sometimes leads to issues when you have an image and some text or just \n at bottom
bool Editor::handlePinch(QMouseEvent *event)
{
    int pos = this->document()->documentLayout()->hitTest(event->pos(), Qt::HitTestAccuracy::ExactHit);
    m_storage->isAnImage = false;
    if(pos>-1)
    {
        QTextCursor cursor(document());
        cursor.setPosition(pos);
        if(!cursor.atEnd())
        {
            cursor.setPosition(pos+1);
            qDebug() << cursor.position();
            QTextFormat format = cursor.charFormat();
            if(format.isImageFormat())
            {
                if (event->button() == Qt::MouseButton::LeftButton) {

                    ImageViewer viewer;
                    QImage i = document()->resource(QTextDocument::ImageResource, format.toImageFormat().name()).value<QImage>();
                    qDebug() << "values for the image: " << i.isNull()
                             << " | " << format.toImageFormat().name()
                             << " | " << document()->resource(QTextDocument::ImageResource, format.toImageFormat().name())
                             << " | " << document()->resource(QTextDocument::ImageResource, format.toImageFormat().name()).value<QImage>();
                    viewer.open(i);
                    viewer.exec();
                }
                m_storage->isAnImage = true;
            }
        }
    }
    return false;
}

void Editor::popupMenu(QMenu *m)
{
    m->exec(this->cursor().pos());
}

void Editor::mouseReleaseEvent(QMouseEvent * event)

{
    if (lockMouse == false) {
        QTextEdit::mouseReleaseEvent(event);
    }
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    oldPos = this->textCursor().position();
    lockMouse = handlePinch(event);
    if (lockMouse == false) {
        QTextEdit::mousePressEvent(event);
    }
}

void Editor::removeUnderCursor()
{
    if (oldPos == this->document()->characterCount()-1) {oldPos--;}
    QTextCursor tmp = this->textCursor();
    tmp.setPosition(oldPos, QTextCursor::MoveMode::KeepAnchor);
    this->setTextCursor(tmp);
    this->textCursor().movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveMode::KeepAnchor);
    this->textCursor().deleteChar();
}

void Editor::contextMenuEvent(QContextMenuEvent *event)
{
    m_storage->selectionOptions()->exec(event->globalPos());
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

void MobileStorage::setTextCharFormat(QTextCharFormat cur) {
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
    if (m_textStyles == nullptr) {
        m_textStyles = new QMenu;
        QAction *out = m_textStyles->addAction(tr("Output as HTML"));
        connect(out, &QAction::triggered, this, &MobileStorage::makeOutput);
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

QMenu *MobileStorage::selectionOptions()
{
    if (isAnImage == true) {
        if (mimageOptions == nullptr) {
            mimageOptions = new QMenu;
            QAction *rm = mimageOptions->addAction(tr("Remove image"));
            QAction *bwr = mimageOptions->addAction(tr("Show file"));

            connect(rm, &QAction::triggered, this, &MobileStorage::reqRmUnderCursor);
            connect(bwr, &QAction::triggered,  this, &MobileStorage::openInBrowser);
        }
        return mimageOptions;
    } else {
        QMenu *ma = editor->createStandardContextMenu();
        QMenu *m = new QMenu(tr("Add an image"));
        QAction *act = m->addAction(tr("Locally"));
        QAction *act2 = m->addAction(tr("From th web"));
        connect(act, &QAction::triggered, this, &MobileStorage::addImage);
        connect(act2, &QAction::triggered, editor, &Editor::showURLInput);
        ma->addMenu(m);
        ma->deleteLater();
        return ma;
    }
}

void MobileStorage::selectionMenuPopup()
{
    Q_EMIT reqMenuPopup(selectionOptions());
}

QMenu *MobileStorage::mforegroundc()
{
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

void MobileStorage::openInBrowser()
{
    QDesktopServices::openUrl(m_format.toImageFormat().name());
}
