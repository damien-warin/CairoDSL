#include "CairoDslHighlighter.h"
#include "CairoDslKeywords.h" // Fichier généré automatiquement par CMake

CairoDslHighlighter::CairoDslHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

    HighlightingRule rule;

    // 1. Application automatique de tous les mots-clés extraits de lexer.l
    keywordFormat.setForeground(QColor("#0000FF")); // Bleu (sur fond blanc)
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns = getCairoDslKeywords();
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        staticRules.append(rule);
    }

    // 2. Format pour les nombres
    numberFormat.setForeground(QColor("#098658")); // Vert
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?\\b");
    rule.format = numberFormat;
    staticRules.append(rule);

    // Chaînes de caractères (entre guillemets doubles ou simples)
	// On les place en dernier pour qu'elles "écrasent" tout ce qui est à l'intérieur
	QTextCharFormat stringFormat;
	stringFormat.setForeground(QColor("#A31515")); // Rouge brique classique pour les strings
	rule.pattern = QRegularExpression("\".*?\"");
	rule.format = stringFormat;
	staticRules.append(rule);

    // Format pour les includes
    commentFormat.setForeground(QColor("#800080")); // Magenta
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    staticRules.append(rule);


    // Format pour les commentaires
    commentFormat.setForeground(QColor("#008000")); // Vert doux
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    staticRules.append(rule);
}

void CairoDslHighlighter::setDynamicSymbols(const QSet<QString> &modules, const QSet<QString> &variables) {
    // 1. Si les symboles n'ont pas changé, on s'arrête net (coupe la boucle !)
    if (modules == lastKnownModules && variables == lastKnownVariables) {
        return;
    }

    lastKnownModules = modules;
    lastKnownVariables = variables;

    // 2. On vide uniquement les règles dynamiques précédentes
    dynamicRules.clear();

    QTextCharFormat moduleFormat;
    moduleFormat.setForeground(QColor("#267F99"));
    moduleFormat.setFontWeight(QFont::Bold);

    QTextCharFormat varFormat;
    //varFormat.setForeground(QColor("#001080"));
    varFormat.setForeground(QColor("#FF1080"));

    HighlightingRule rule;

    for (const QString &mod : modules) {
        rule.pattern = QRegularExpression("\\b" + mod + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = moduleFormat;
        dynamicRules.append(rule);
    }

    for (const QString &var : variables) {
        rule.pattern = QRegularExpression("\\b" + var + "\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = varFormat;
        dynamicRules.append(rule);
    }

    // 3. On ne force le re-highlight qu'en cas de vrai changement
    rehighlight();
}

void CairoDslHighlighter::highlightBlock(const QString &text) {
    // Combiner les deux listes pour le rendu de la ligne
    QVector<HighlightingRule> allRules = dynamicRules + staticRules;

    for (const HighlightingRule &rule : qAsConst(allRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
