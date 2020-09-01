#ifndef DSPLITEDWINDOW_H
#define DSPLITEDWINDOW_H

#include <DBlurEffectWidget>

DWIDGET_BEGIN_NAMESPACE

class DSplitedBar;

/**
 * @brief Class that provides modern Ui with splited (left/right) window.
 */

class DSplitedWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DSplitedWindow(QWidget *parent = nullptr, bool leftBlurEnabled = true, DBlurEffectWidget::BlendMode blendMode = DBlurEffectWidget::BlendMode::BehindWindowBlend);

    QWidget *leftWidget() const;
    QWidget *rightWidget() const;
    QWidget *bottomWidget() const;
    DSplitedBar *splitedbar() const;

    void setLeftBlur(bool leftBlur);
    void setLeftBlurMode(DBlurEffectWidget::BlurMode);
    void setLeftBlendMode(DBlurEffectWidget::BlendMode);
    void setLeftBlurColor(QColor color);

    void setRightWidget(QWidget *w);
    void setBottomWidget(QWidget *w);

    void setLeftAreaWidth(int width);
    int leftAreaWidth();

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    DSplitedBar *m_bar;
    DBlurEffectWidget *m_leftw = nullptr;
    QWidget *m_bottomw = nullptr;
    QWidget *m_rightw = nullptr;
    bool isOriginal = true;
    int m_leftwidth = 250;
};

DWIDGET_END_NAMESPACE

#endif // DSPLITEDWINDOW_H
