#ifndef IMAGEPOPUP_H
#define IMAGEPOPUP_H

#include <DLabel>
#include <DDialogCloseButton>
#include <QDialog>

DWIDGET_USE_NAMESPACE

class ImageViewer : public QDialog
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer() override;

public slots:
    void open(QImage &image);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();

    DLabel *img_label_ = nullptr;
    DDialogCloseButton *close_button_;
    QImage data;

    using QDialog::open;
};
#endif // IMAGEPOPUP_H
