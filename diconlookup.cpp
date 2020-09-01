#include "diconlookup.h"

#include <QDir>
#include <QDirIterator>
#include <DThemeManager>
#include <QDebug>
#include <QIcon>

const QString p_assets =    ":/assets/images/";
const QString p_bit =       ":/icons/deepin/builtin/texts/";
const QString p_bii =       ":/icons/deepin/builtin/icons/";
const QString p_lightI =    ":/icons/deepin/builtin/light/icons/";
const QString p_darkI =     ":/icons/deepin/builtin/dark/icons/";
const QString p_chameleon = ":/chameleon/";

DIconic::DIconic(QObject *parent) : QObject(parent) {}

DIconic::~DIconic()
{
    //QObject::~QObject();
}

QIcon *DIconic::fromString(const QString icon_name, IconScopes scope)
{
    QStringList dirs;

    switch (scope) {
    case (int)IconScopes::Assets: dirs << p_assets; break;
    case (int)IconScopes::Chameleon: dirs << p_chameleon; break;
    case (int)IconScopes::BuiltInIcon: dirs << p_bii; break;
    case (int)IconScopes::BuiltInText: dirs << p_bit; break;
    case (int)IconScopes::BuiltIn: dirs << p_bii << p_bit; break;
    case (int)IconScopes::Dark: dirs << p_darkI; break;
    case (int)IconScopes::Light: dirs << p_lightI; break;
    case (int)IconScopes::All: dirs << p_assets << p_lightI << p_darkI << p_bit << p_bii << p_chameleon; break;
    }

    QIcon missing = QIcon::fromTheme(icon_name, QIcon(":/missing.svg"));

    int i = 0;
    while (i<dirs.length()) {
        QDirIterator *it = new QDirIterator(dirs.at(i), QDirIterator::Subdirectories);
        while (it->hasNext()) {
            QString path = it->next().toUtf8().data();
            if (path == QString(dirs.at(i) + icon_name)) {
                return new QIcon(path);
            }
            //path.~QString();
        }
        //it->~QDirIterator();
        i++;
    }
    //dirs.~QStringList();
    qDebug() << "icon " << icon_name << " notfound.";
    return new QIcon(missing);
}

QStringList DIconic::listPaths()
{
    QStringList dirs;
    dirs << p_assets << p_lightI << p_darkI << p_bit << p_bii << p_chameleon;
    return dirs;
}

QStringList DIconic::list()
{
    QStringList data;

    int i = 0;
    QStringList dirs = listPaths();
    while (i<dirs.length()) {
        QDirIterator *it = new QDirIterator(dirs.at(i), QDirIterator::Subdirectories);
        while (it->hasNext()) {
            data << it->next();
        }
        //it->~QDirIterator();
        i++;
    }
    //dirs.~QStringList();
    return data;
}
