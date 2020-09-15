#ifndef HELPERCLASS_H
#define HELPERCLASS_H

#include <QMenu>

class HelperClass
{
public:
    inline virtual QMenu *generateMenu() {return new QMenu;}
    inline virtual void helperRequest() {}
};

#endif // HELPERCLASS_H
