#pragma once


#include <QString>

extern "C"
{
#include <String.h>

int cairo_parse(String *buffer, String *prefix, String *out, String *msgs, int *is_xml, int *width, int *height);
}

// Méthode simulée qui prend le code DSL et retourne le XML
int processlib(const QString& code, const QString &filepath, QString& msgs, QByteArray &output, bool &is_xml, int *width, int *height);
