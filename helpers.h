#ifndef FUNCTION_HELPERS_H
#define FUNCTION_HELPERS_H

#include <QStringList>
#include <QString>
#include <QDebug>

#define CYELLOW Qt::yellow
#define CORANGE QColor::fromRgb(244, 67, 54)
#define CRED Qt::red
#define CPINK QColor::fromRgb(255, 80, 140)
#define CPURPLE QColor::fromRgb(156, 39, 176)
#define CCYAN QColor::fromRgb(0, 204, 175)
#define CAZURE QColor::fromRgb(0, 188, 212)
#define CBLUE Qt::blue
#define CBLACK Qt::black
#define CPEAR QColor::fromRgb(139, 195, 74)
#define CGREEN Qt::green
#define CGRAY Qt::gray
#define CWHITE Qt::white
#define CMARINBLUE QColor::fromRgb(63, 81, 181)

inline static QString relativeToStaticPath(QString sourcePath, QString relativePath)
{
    QStringList data = relativePath.split("/");
    int toRemove = data.first().count(".");
    QStringList pathed = sourcePath.split("/");
    int i = 0;
    while (i<toRemove) {
        pathed.removeLast();
        i++;
    }
    data.removeFirst();
    return pathed.join("/")+"/"+data.join("/");
}

inline QString binded64(QByteArray toBind, QString before = "", QString after = "")
{
    return QString(before + toBind.toBase64() + after);
}


#endif // FUNCTION_HELPERS_H
