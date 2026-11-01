#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class CairoDslHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    CairoDslHighlighter(QTextDocument *parent = nullptr);
    // Méthode pour mettre à jour les symboles dynamiques trouvés dans les includes
    void setDynamicSymbols(const QSet<QString> &modules, const QSet<QString> &variables);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    // Formats de style
    QTextCharFormat keywordFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat numberFormat;

    QVector<HighlightingRule> staticRules;   // Règles fixes (lexer, nombres, etc.)
    QVector<HighlightingRule> dynamicRules;  // Règles dynamiques (variables, modules)
    QSet<QString> lastKnownModules;
    QSet<QString> lastKnownVariables;
};
