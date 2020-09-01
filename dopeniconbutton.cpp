#include "dopeniconbutton.h"

DOpenIconButton::DOpenIconButton(QWidget *parent, Qt::AnchorPoint xa, Qt::AnchorPoint ya) : Dtk::Widget::DIconButton(parent)
{
    connect(this, &Dtk::Widget::DIconButton::clicked, this, &DOpenIconButton::requestMenu);
    anchor_x = xa;
    anchor_y = ya;
}

DOpenIconButton::~DOpenIconButton() {}

QMenu *DOpenIconButton::menu() const
{
    return m_menu;
}

int DOpenIconButton::processAnchor(Qt::AnchorPoint a)
{
    int v = 0;
    switch (a) {
    case Qt::AnchorTop: v = 0; break;
    case Qt::AnchorBottom: v = this->rect().height(); break;
    case Qt::AnchorLeft: v = 0; break;
    case Qt::AnchorRight: v = this->rect().width(); break;
    case Qt::AnchorVerticalCenter: v = this->rect().height()/2; break;
    case Qt::AnchorHorizontalCenter: v = this->rect().width()/2; break;
    default: break; //In case that you pass an int as the value and it is out of range!
    }
    return v;
}

void DOpenIconButton::handleMenuRequest(QMenu *menu)
{
    if (menu != nullptr) {
        QPoint center = mapToGlobal(QPoint(processAnchor(anchor_x), processAnchor(anchor_y)));
        int menuHeight = menu->sizeHint().height();
        int menuWidth = menu->sizeHint().width();
        center.setY(center.y() - menuHeight);
        center.setX(center.x() - menuWidth / 2);
        menu->exec(center);
    }
}

Qt::AnchorPoint DOpenIconButton::anchorX() const
{
    return anchor_x;
}

Qt::AnchorPoint DOpenIconButton::anchorY() const
{
    return anchor_y;
}

void DOpenIconButton::setMenuXAnchor(Qt::AnchorPoint p)
{
    anchor_x = p;
}

void DOpenIconButton::setMenuYAnchor(Qt::AnchorPoint p)
{
    anchor_y = p;
}

void DOpenIconButton::setMenuAnchors(Qt::AnchorPoint x, Qt::AnchorPoint y)
{
    anchor_x = x;
    anchor_y = y;
}
