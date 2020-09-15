#include "dsplitedwindow.h"
#include "multicolors.h"
#include "dopenapplication.h"
#include "diconlookup.h"
#include "dsplitedbar.h"
#include "editor.h"
#include "ioop.h"

#include <iostream>
#include <string>

#include <QDir>
#include <QDebug>
#include <QHBoxLayout>
#include <DIconButton>
#include <DBlurEffectWidget>

void addWidgets(QList<QWidget*> list, QLayout *l)
{
    int i = 0;
    while(i<list.length()) {
        DIconButton *btn = qobject_cast<DIconButton *>(list.at(i));
        if (btn != nullptr) {
            btn->setFixedSize(35,35);
        }
        l->addWidget(list.at(i));
        i++;
    }
}

DWIDGET_USE_NAMESPACE

const QString icon_path = ":/icons/palette-solid.svg";

int main(int argc, char *argv[])
{
    DOpenApplication::loadDXcbPlugin();
    DOpenApplication a(argc, argv);
    a.setWindowIcon(QIcon(icon_path));
    a.setApplicationLicense("GNU GPL v3");
    a.setProductIcon(QIcon(icon_path));
    a.setApplicationVersion("1.0");
    a.setApplicationDescription("Test application for DSplitedWindow");
    a.setApplicationAcknowledgementVisible(false);
    a.setApplicationName("TLA");
    a.setApplicationDisplayName("Neo Texts");

    DSplitedWindow wi;
    wi.splitedbar()->setCustomTitleAlign(Qt::AlignLeft);
    wi.splitedbar()->setCustomTitleAlign(Qt::AlignVCenter);

    DBlurEffectWidget *bl = new DBlurEffectWidget;
    bl->setBlendMode(DBlurEffectWidget::BlendMode::InWindowBlend);
    bl->setBlurEnabled(true);
    bl->setRadius(20);
    bl->setFixedHeight(50);

    Editor *editor = new Editor;
    wi.setBottomWidget(bl);
    wi.setRightWidget(editor);
    wi.show();

    QHBoxLayout *controls = new QHBoxLayout;
    bl->setLayout(controls);
    addWidgets(editor->storage()->getBar(), controls);
    QAction *act = wi.splitedbar()->menu()->addAction(QObject::tr("Clean FS cache"));

    QObject::connect(act, &QAction::triggered, editor, []() {QDir d("/tmp/neotexts"); qDebug() << "Cleaning"; d.rmdir("/tmp/neotexts");});

    wi.splitedbar()->leftMenu()->clear();
    // [TODO] save and saveAs's slots are exchanged because slot names don't corresponds!!
    QAction *saveAs = wi.splitedbar()->leftMenu()->addAction(QObject::tr("Save file"));
    QAction *save = wi.splitedbar()->leftMenu()->addAction(QObject::tr("Save file as"));
    QAction *open = wi.splitedbar()->leftMenu()->addAction(QObject::tr("Open local file"));

    QObject::connect(saveAs, &QAction::triggered, editor, &Editor::saveAsRequest);
    QObject::connect(save, &QAction::triggered, editor, &Editor::saveRequest);
    QObject::connect(open, &QAction::triggered, IOOP::instance(), &IOOP::uiOpenFile);

    save->setShortcut(QKeySequence(Qt::Key_Control + Qt::Key_Shift + Qt::Key_S));
    saveAs->setShortcut(QKeySequence(Qt::Key_Control + Qt::Key_S));
    open->setShortcut(QKeySequence(Qt::Key_Control + Qt::Key_O));

    return a.exec();
}
