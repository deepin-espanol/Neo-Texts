#include "editor.h"
#include "imagepopup.h"
#include "helpers.h"
#include "ioop.h"

#include <QDebug>
#include <QImageReader>
#include <QDir>
#include <QLineEdit>
#include <DPushButton>
#include <DDialog>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include <QTextTable>
#include <QAbstractTextDocumentLayout>

const QRegExp imageSupport("[\\w\\-.]+\\.(jpg|JPG|png|PNG|gif|GIF|jpeg|JPEG)");
QRegExp imageInclusion("<img src=\"(http|https|ftp|file)://([a-zA-Z.]*)/([a-zA-Z0-9.?%/_\\-]*).(png|jpg|gif|JPEG|PNG|GIF)\" width=\"([0-9]*)\" height=\"([0-9]*)\" />");
const QString imgp = "https://upload.wikimedia.org/wikipedia/commons/thumb/b/b6/Image_created_with_a_mobile_phone.png/250px-Image_created_with_a_mobile_phone.png";

const QRegularExpression imageURIP("(http|https|ftp|file)://([a-zA-Z0-9.]+)([a-zA-Z0-9.?%/_#\\\\-]+).(png|jpg|gif|JPEG|PNG|GIF)",
                                   QRegularExpression::MultilineOption
                                   | QRegularExpression::DotMatchesEverythingOption
                                   | QRegularExpression::CaseInsensitiveOption);

const QString imageBegin = "<img src=\"data:image/";
const QString imageEnding = "\" height=\"250\" width=\"250\"/>";

Editor::Editor(QWidget *parent) : QTextEdit(parent)
{
    m_storage = new MobileStorage;
    m_manager = new QNetworkAccessManager;
    m_storage->editor = this;

    imageInclusion.setMinimal(true);

    Q_EMIT newCursorAvailable(format(), false);

    connect(this, &Editor::newCursorAvailable, m_storage, &MobileStorage::setTextCharFormat);
    connect(m_storage, &MobileStorage::requestChanges, this, &Editor::applyTextCursor);
    connect(m_storage, &MobileStorage::addImage, this, &Editor::imageInsertion);
    connect(m_storage, &MobileStorage::makeOutput, this, &Editor::generateOutput);
    connect(m_storage, &MobileStorage::reqMenuPopup, this, &Editor::popupMenu);
    connect(m_storage, &MobileStorage::reqRmUnderCursor, this, &Editor::removeUnderCursor);
    connect(m_storage, &MobileStorage::reqAddCells, this, &Editor::addCells);
    connect(m_storage, &MobileStorage::reqRmCells, this, &Editor::removeCells);
    connect(m_storage, &MobileStorage::reqAddTable, this, &Editor::insertTable);
    connect(document(), &QTextDocument::cursorPositionChanged, this, [this](QTextCursor cursored) {
        Q_EMIT newCursorAvailable(cursored.charFormat(), cursored.selectedText().isEmpty());
    });
    setContent(QString("<p>That's wondeful bruh!</p><img src=\""+imgp+"\" width=\"250\" height=\"250\" />"));

    connect(IOOP::instance(), &IOOP::fileOpenedPath, this, &Editor::setFilePath);
    connect(IOOP::instance(), &IOOP::fileDataRead, this, &Editor::setContent);
}

Editor::~Editor() {}

void Editor::parseImageResourceRequest(QUrl u, bool handleInsertion)
{
    qDebug() << u.toString() << "\n" << relativeToStaticPath(u.toString(), "../kk.png");
    if (QFile("/tmp/neotexts/"+u.toString().replace("://", ":/").replace("/", "%")).exists() == false) {
        if (waitingRequests.contains(u) == false) {
            waitingRequests << u;
            _reply = m_manager->get(generate(u));
            connect(_reply, &QNetworkReply::finished, this, &Editor::finished);
            connect(_reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, &Editor::error);
            connect(_reply, &QNetworkReply::downloadProgress, this, &Editor::updateProgress);
            toprocess++;
            connect(_reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, [this](QNetworkReply::NetworkError e) {qDebug() << e << ": " << _reply->errorString();});
        }
    } else {
        if (handleInsertion == true) {
            QString value = "/tmp/neotexts/"+u.toString().replace("://", ":/").replace("/", "%");
            QImage image = QImageReader(value).read();
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
    setMargin(50);
    imageInclusion.indexIn(data);
    int MaxCaptures = imageInclusion.matchedLength();
    for (int i = 0; i < MaxCaptures; ++i) {
        if (imageInclusion.cap(i).contains(imageInclusion)) {
            parseImageResourceRequest(imageURIP.match(imageInclusion.cap(0)).captured(0), false);
        }
    }
    QString translated = data;
    imageInclusion.indexIn(data);
    for (int i = 0; i < MaxCaptures; ++i) {
        if (imageInclusion.cap(i).contains(imageInclusion)) {
            QString filePath = imageURIP.match(imageInclusion.cap(0)).captured(0);
            QString result = "/tmp/neotexts/"+filePath.replace("://", ":/").replace("/", "%");
            translated.replace(imageURIP.match(imageInclusion.cap(0)).captured(0), result);
        }
    }
    setHtml(translated);
    setMargin(50);
    document()->clearUndoRedoStacks();
}

QNetworkRequest Editor::generate(QUrl p)
{
    QNetworkRequest r(p);
    r.setAttribute(QNetworkRequest::Attribute::FollowRedirectsAttribute, true);
    return r;
}

void Editor::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);
}

void Editor::error(QNetworkReply::NetworkError err)
{
    qDebug() << "Error while downloading data: " << err;
    _reply->deleteLater();
}

void Editor::updateProgress(qint64 read, qint64 total)
{
    if (total != 0) {
        qDebug() << "Download state: " << read*100/total;
    } else {
        qDebug() << "Download state: " << read << "/" << total;
    }
}

void Editor::finished()
{
    processed++;
    QByteArray b = _reply->readAll();
    QString result = "/tmp/neotexts/"+_reply->url().toString().replace("://", ":/").replace("/", "%");
    QFile file(result);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    file.write(b);
    file.close();
    QImage i = QImageReader(b).read();
    document()->addResource(QTextDocument::ImageResource, QUrl(result), QVariant(i));
    setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); //Only way to make it reload our images (sadly)
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
        parseImageResourceRequest(edit->text(), true);
        container->~QWidget();
        d->close();
    });
    connect(submit, &DPushButton::clicked, this, [d, edit, container, this] {
        parseImageResourceRequest(edit->text(), true);
        container->~QWidget();
        d->close();
    });

    d->exec();
}

void Editor::generateOutput()
{
    qDebug() << toHtml() << "\n";
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
    return textCursor().charFormat();
}

void Editor::applyTextCursor(QTextCharFormat cur)
{
    textCursor().mergeCharFormat(cur);
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
        imageInsertionByPath(QUrl(QString("file://%1").arg(file)), true);
    }
 }

void Editor::imageInsertionByPath(QUrl URI, bool isLocalFS)
{
    if (isLocalFS == true) {
        QFile f(URI.path());
        f.open(QIODevice::OpenModeFlag::ReadOnly);
        textCursor().insertHtml(binded64(f.readAll(), imageBegin + URI.toString().split(".").last().toLower() + ";base64,", imageEnding));
        setLineWrapColumnOrWidth(lineWrapColumnOrWidth());
    } else {
        QImage image = QImageReader(URI.toString()).read();
        document()->addResource(QTextDocument::ImageResource, URI, QVariant(image));
        QTextCursor cursor = textCursor();
        QTextImageFormat imageFormat;
        imageFormat.setName(URI.toString());
        imageFormat.setWidth(250);
        imageFormat.setHeight(250);
        cursor.insertImage(imageFormat);
    }
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
    return QTextEdit::canInsertFromMimeData(source);
}

void Editor::insertFromMimeData(const QMimeData *source)
{
    int i = 0;
    if (source->hasText() && source->hasText() == false) {
        textCursor().insertText(source->text());
    } else if (source->hasHtml() && source->hasUrls() == false) {
        textCursor().insertHtml(source->html());
    } else if (source->hasUrls()) {
        while (i<source->urls().length()) {
           if (source->urls().at(i).toString().contains(imageSupport)) {
               imageInsertionByPath(source->urls().at(i));
               source->urls().removeAt(i);
           }
           i++;
       }
    }
}

// Editor::handlePinch() sometimes leads to issues, we miss some handlings with images and cells!
bool Editor::handlePinch(QMouseEvent *event)
{
    int pos = document()->documentLayout()->hitTest(event->pos(), Qt::HitTestAccuracy::ExactHit);
    m_storage->type = SelectionType::Char;
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
                if (event->button() == Qt::MouseButton::LeftButton) {

                    ImageViewer viewer;
                    QImage i = document()->resource(QTextDocument::ImageResource, format.toImageFormat().name()).value<QImage>();
                    viewer.open(i);
                    viewer.exec();
                }
                m_storage->type = SelectionType::Image;
            } else if (textCursor().currentTable() != nullptr || format.isTableCellFormat()) {
                m_storage->type = SelectionType::TableComponent;
            } else if (format.isListFormat()) {
                qDebug() << "Detected as a list";
            } else {
                qDebug() << "Detected as unhandled type";
            }
        }
    }
    return false;
}

void Editor::popupMenu(QMenu *m)
{
    m->exec(cursor().pos());
}

void Editor::mouseReleaseEvent(QMouseEvent * event)

{
    if (lockMouse == false) {
        QTextEdit::mouseReleaseEvent(event);
    }
    lockMouse = false;
}

void Editor::mousePressEvent(QMouseEvent *event)
{
    oldPos = textCursor().position();
    lockMouse = handlePinch(event);
    if (lockMouse == false) {
        QTextEdit::mousePressEvent(event);
    }
}

void Editor::removeUnderCursor()
{
    if (oldPos == document()->characterCount()-1) {oldPos--;}
    QTextCursor tmp = textCursor();
    tmp.setPosition(oldPos, QTextCursor::MoveMode::KeepAnchor);
    setTextCursor(tmp);
    textCursor().movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::MoveMode::KeepAnchor);
    textCursor().deleteChar();
}

void Editor::contextMenuEvent(QContextMenuEvent *event)
{
    m_storage->selectionOptions()->exec(event->globalPos());
}

void Editor::insertTable()
{
    QTextTableFormat f;
    f.setCellPadding(3);
    f.setBorder(2);
    f.setCellSpacing(0);
    f.setBorderStyle(QTextTableFormat::BorderStyle::BorderStyle_Double);
    f.setBorderBrush(QColor::fromRgb(158, 158, 158));
    textCursor().insertTable(2, 3, f);
}

void Editor::insertCells(Qt::AnchorPoint p, int toAdd)
{
    QTextTableCell c = textCursor().currentTable()->cellAt(textCursor());
    if (p == Qt::AnchorVerticalCenter) {
        textCursor().currentTable()->insertColumns(c.column(), toAdd);
    } else if (p == Qt::AnchorHorizontalCenter) {
        textCursor().currentTable()->insertRows(c.row(), toAdd);
    }
}

void Editor::addCells(Qt::AnchorPoint p, int toAdd)
{
    QTextTable *table = textCursor().currentTable();
    switch (p) {
        case Qt::AnchorTop : {
        table->insertRows(0, toAdd);
        break;
        }
        case Qt::AnchorLeft : {
        table->insertColumns(table->columns(), toAdd);
        break;
        }
        case Qt::AnchorBottom : {
        table->insertRows(table->rows(), toAdd);
        break;
        }
        case Qt::AnchorRight : {
        table->insertColumns(0, toAdd);
        break;
        }
        default: break;
    }
}

void Editor::removeCells(Qt::AnchorPoint p, int toRm)
{
    QTextTable *table = textCursor().currentTable();
    switch (p) {
        case Qt::AnchorTop:
        table->removeRows(0 - toRm, toRm);
        break;
        case Qt::AnchorLeft:
        table->removeColumns(table->columns() - toRm, toRm);
        break;
        case Qt::AnchorBottom:
        table->removeRows(table->rows() - toRm, toRm);
        break;
        case Qt::AnchorRight:
        table->removeColumns(0 + toRm, toRm);
        break;
        default: break;
    }
}

QMenu *Editor::generateMenu()
{
    return QTextEdit::createStandardContextMenu();
}

void Editor::helperRequest()
{
    showURLInput();
}

void Editor::saveRequest()
{
    IOOP::instance()->uiSaveFile(this->toHtml());
}

void Editor::saveAsRequest()
{
    IOOP::instance()->writeToFile(openedFile, this->toHtml());
}

void Editor::setFilePath(QUrl p)
{
    openedFile = p;
}
