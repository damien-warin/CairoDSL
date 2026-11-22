#pragma once


#include <QString>
#include <QList>

extern "C"
{
#include <String.h>
#include <Op.h>

int cairo_parse(String *buffer, String *prefix, String *out, QList<struct MessageItem> &msgs, int *is_xml, int *width, int *height);
}

struct MessageItem {
	OpRunningState state;
    QString text;
    QString filename;
    int firstLine = -1;
    int firstColumn = -1;
    int lastLine = -1;
    int lastColumn = -1;
};
// Méthode simulée qui prend le code DSL et retourne le XML
int processlib(const QString& code, const QString &filepath, QList<struct MessageItem> &msgs, QByteArray &output, bool &is_xml, int *width, int *height);
