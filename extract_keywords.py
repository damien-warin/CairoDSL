import sys
import re

def main():
    if len(sys.argv) < 3:
        print("Usage: python extract_keywords.py <lexer.l> <output.h>")
        sys.exit(1)

    lexer_path = sys.argv[1]
    output_path = sys.argv[2]

    keywords = []
    try:
        with open(lexer_path, 'r', encoding='utf-8') as f:
            for line in f:
                # Recherche des mots entre guillemets typiques des règles flex
                match = re.search(r'\"([a-zA-Z_][a-zA-Z0-9_]*)\"', line)
                if match:
                    kw = match.group(1)
                    if kw not in keywords:
                        keywords.append(kw)
    except Exception as e:
        print(f"Erreur lors de la lecture de {lexer_path}: {e}")
        sys.exit(1)

    try:
        with open(output_path, 'w', encoding='utf-8') as out:
            out.write('#ifndef CAIRO_DSL_KEYWORDS_H\n')
            out.write('#define CAIRO_DSL_KEYWORDS_H\n')
            out.write('#include <QStringList>\n\n')
            out.write('inline QStringList getCairoDslKeywords() {\n')
            out.write('    return QStringList{\n')
            for kw in keywords:
                out.write(f'        "\\\\b{kw}\\\\b",\n')
            out.write('    };\n')
            out.write('}\n\n')
            out.write('#endif // CAIRO_DSL_KEYWORDS_H\n')
    except Exception as e:
        print(f"Erreur lors de l'écriture de {output_path}: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()