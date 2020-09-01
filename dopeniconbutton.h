#ifndef DOPENICONBUTTON_H
#define DOPENICONBUTTON_H

#include <QMenu>
#include <DIconButton>

class DOpenIconButton : public Dtk::Widget::DIconButton
{
    Q_OBJECT
public:
    explicit DOpenIconButton(QWidget *parent = nullptr, Qt::AnchorPoint xa = Qt::AnchorLeft, Qt::AnchorPoint ya = Qt::AnchorBottom);
    ~DOpenIconButton();

    int processAnchor(Qt::AnchorPoint a);
    Qt::AnchorPoint anchorX() const;
    Qt::AnchorPoint anchorY() const;
    QMenu *menu() const;

Q_SIGNALS:
    void clicked();
    void requestMenu();

public Q_SLOTS:
    void setMenuXAnchor(Qt::AnchorPoint a);
    void setMenuYAnchor(Qt::AnchorPoint a);
    void setMenuAnchors(Qt::AnchorPoint x, Qt::AnchorPoint y);
    void handleMenuRequest(QMenu *);

private:
    QMenu *m_menu = nullptr;
    Qt::AnchorPoint anchor_x;
    Qt::AnchorPoint anchor_y;
};

#endif // DOPENICONBUTTON_H
