#ifndef DSPLITEDBAR_H
#define DSPLITEDBAR_H

#include <dtkwidget_global.h>
#include <DGuiApplicationHelper>

#include <DObjectPrivate>

#include <dwindowclosebutton.h>
#include <dwindowmaxbutton.h>
#include <dwindowminbutton.h>
#include <dwindowoptionbutton.h>
#include <dplatformwindowhandle.h>
#include <daboutdialog.h>
#include <dapplication.h>
#include <dmainwindow.h>
#include <DHorizontalLine>
#include <dimagebutton.h>
#include <dblureffectwidget.h>
#include <dwidgetstype.h>
#include <dlabel.h>

#include <QMenu>
#include <QFrame>

class QHBoxLayout;

DGUI_USE_NAMESPACE
DWIDGET_BEGIN_NAMESPACE

class DSplitedWindow;
class DSplitedBarPrivate;

/**
 * @brief Modern splited bar for DSplitedWindow
 */

class LIBDTKWIDGETSHARED_EXPORT DSplitedBar : public QFrame, public DTK_CORE_NAMESPACE::DObject
{
    Q_OBJECT
    Q_PROPERTY(bool blurBackground READ blurBackground WRITE setBlurBackground)

public:
    explicit DSplitedBar(QWidget *parent = Q_NULLPTR);

#ifndef QT_NO_MENU
    QMenu *menu() const;
    QMenu *leftMenu() const;
    void setLeftMenu(QMenu *menu);
    void setMenu(QMenu *menu);
#endif

    QWidget *customWidget() const;
    void setCustomWidget(QWidget *, bool fixCenterPos = false);

    void addWidget(QWidget *w, Qt::Alignment alignment = Qt::Alignment());
    void removeWidget(QWidget *w);

    int buttonAreaWidth() const;
    bool separatorVisible() const;

    bool autoHideOnFullscreen() const;
    void setAutoHideOnFullscreen(bool autohide);

    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void setEmbedMode(bool embed);

    bool menuIsVisible() const;
    void setMenuVisible(bool visible);

    bool menuIsDisabled() const;
    void setMenuDisabled(bool disabled);

    bool quitMenuIsDisabled() const;
    void setQuitMenuDisabled(bool disabled);
    void setQuitMenuVisible(bool visible);

    bool switchThemeMenuIsVisible() const;
    void setSwitchThemeMenuVisible(bool visible);

    void setDisableFlags(Qt::WindowFlags flags);
    Qt::WindowFlags disableFlags() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    bool blurBackground() const;

    int leftMargin() const;
    void setLeftMargin(int margin);

Q_SIGNALS:
    void optionClicked();
    void doubleClicked();
    void mousePressed(Qt::MouseButtons buttons);
    void mouseMoving(Qt::MouseButton button);

#ifdef DTK_TITLE_DRAG_WINDOW
    void mousePosPressed(Qt::MouseButtons buttons, QPoint pos);
    void mousePosMoving(Qt::MouseButton button, QPoint pos);
#endif

public Q_SLOTS:
    void setFixedHeight(int h);
    void setBackgroundTransparent(bool transparent);
    void setSeparatorVisible(bool visible);
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    /// Maximized/Minumized
    void toggleWindowState();
    void setIconMenu(QMenu *);
    void setBlurBackground(bool blurBackground);
    void setCustomTitleAlign(Qt::Alignment);

private Q_SLOTS:
#ifndef QT_NO_MENU
    void showMenu();
    void showIconMenu();
#endif

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *e) override;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    int left_margin = 250;
    bool isFirstMarginSet = true;
    QMenu *m_iconMenu;

    D_DECLARE_PRIVATE(DSplitedBar)
    D_PRIVATE_SLOT(void _q_toggleWindowState())
    D_PRIVATE_SLOT(void _q_showMinimized())
    D_PRIVATE_SLOT(void _q_onTopWindowMotifHintsChanged(quint32))

#ifndef QT_NO_MENU
    D_PRIVATE_SLOT(void _q_addDefaultMenuItems())
    D_PRIVATE_SLOT(void _q_helpActionTriggered())
    D_PRIVATE_SLOT(void _q_aboutActionTriggered())
    D_PRIVATE_SLOT(void _q_quitActionTriggered())
    D_PRIVATE_SLOT(void _q_switchThemeActionTriggered(QAction*))
#endif
};

/**
 * @brief Private class of DSplitedBar, stores data.
 */

class DSplitedBarPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
protected:
    DSplitedBarPrivate(DSplitedBar *qq);

private:
    void init();
    QWidget *targetWindow();
    QWidget *titleSource;
    // FIXME: get a batter salution
    // hide title will make eventFilter not work, instead set Height to zero
    bool isVisableOnFullscreen();
    void hideOnFullscreen();
    void showOnFullscreen();

    void updateFullscreen();
    void updateButtonsState(Qt::WindowFlags type);
    void updateButtonsFunc();
    void updateCenterArea();

    void handleParentWindowStateChange();
    void handleParentWindowIdChange();
    void _q_toggleWindowState();
    void _q_showMinimized();
    void _q_onTopWindowMotifHintsChanged(quint32 winId);

#ifndef QT_NO_MENU
    void _q_addDefaultMenuItems();
    void _q_helpActionTriggered();
    void _q_aboutActionTriggered();
    void _q_quitActionTriggered();
    void _q_switchThemeActionTriggered(QAction*action);
#endif

    void setIconVisible(bool visible);
    void updateTabOrder();

    QHBoxLayout         *mainLayout;
    QWidget             *leftArea;
    QHBoxLayout         *leftLayout;
    QWidget             *rightArea;
    QHBoxLayout         *rightLayout;
    DLabel              *centerArea;
    QHBoxLayout         *centerLayout;
    DIconButton         *iconLabel;
    QWidget             *buttonArea;
    DWindowMinButton    *minButton;
    DWindowMaxButton    *maxButton;
    DWindowCloseButton  *closeButton;
    DWindowOptionButton *optionButton;
    DImageButton        *quitFullButton;
    DLabel              *titleLabel;
    QWidget             *customWidget = nullptr;

    DHorizontalLine     *separatorTop;
    DHorizontalLine     *separator;

    DBlurEffectWidget   *blurWidget = nullptr;

#ifndef QT_NO_MENU
    QMenu               *menu             = Q_NULLPTR;
    QAction             *helpAction       = Q_NULLPTR;
    QAction             *aboutAction      = Q_NULLPTR;
    QAction             *quitAction       = Q_NULLPTR;
    bool                canSwitchTheme    = true;
    QAction             *themeSeparator   = nullptr;
    QMenu               *switchThemeMenu  = nullptr;
    QAction             *autoThemeAction  = nullptr;
    QAction             *lightThemeAction = nullptr;
    QAction             *darkThemeAction  = nullptr;
    QMenu               *automateIconMenu();
    QMenu               *m_iconmenu       = nullptr;
#endif

    QWindow            *targetWindowHandle = Q_NULLPTR;

    Qt::WindowFlags     disableFlags;
    bool                mousePressed    = false;
    bool                embedMode       = false;
    bool                autoHideOnFullscreen = false;

    Q_DECLARE_PUBLIC(DSplitedBar)
};

DWIDGET_END_NAMESPACE

#endif // DSPLITEDBAR_H
