#ifndef MULTICOLORS_H
#define MULTICOLORS_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

/**
 * @brief Class used for Ui test, generates multicolor squares
 */

class MultiColors : public QWidget
{
    Q_OBJECT
public:
    explicit MultiColors(QWidget *parent = nullptr);

    /**
     * @brief rectangleSize, the size of the squares rendered
     */
    int rectangleSize = 80;

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override {QWidget::resizeEvent(e); update(); }
};

#endif // MULTICOLORS_H
