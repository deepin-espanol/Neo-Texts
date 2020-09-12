#ifndef FUNCTION_HELPERS_H
#define FUNCTION_HELPERS_H

#include <QStringList>
#include <QString>
#include <QDebug>

static QString relativeToStaticPath(QString sourcePath, QString relativePath)
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

QString binded64(QByteArray toBind, QString before = "", QString after = "")
{
    return QString(before + toBind.toBase64() + after);
}

#endif // FUNCTION_HELPERS_H
