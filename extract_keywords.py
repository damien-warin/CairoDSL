import sys
import re
import os
import xml.etree.ElementTree as ET

def main():
    if len(sys.argv) < 3:
        print("Usage: python extract_keywords.py <lexer.l> [parser.xml] <output.h>")
        sys.exit(1)

    lexer_path = sys.argv[1]
    xml_path = None
    output_path = None
    
    if len(sys.argv) == 3:
        output_path = sys.argv[2]
    elif len(sys.argv) >= 4:
        xml_path = sys.argv[2]
        output_path = sys.argv[3]

    keywords = []
    token_to_literal = {}

    # 1. Lecture du lexer.l (Conserve exactement ton comportement d'origine pour les keywords)
    try:
        with open(lexer_path, 'r', encoding='utf-8') as f:
            for line in f:
                # Recherche des règles flex associant une chaîne à un token (ex: (?i:"color") { return COLOR; })
                match_token = re.search(r'(?:\(\?i:\s*)?"([a-zA-Z_][a-zA-Z0-9_]*)"(?:\s*\))?\s*\{\s*return\s+(\w+);\s*\}', line)
                if match_token:
                    kw, token = match_token.groups()
                    kw_lower = kw.lower()
                    token_to_literal[token] = kw_lower
                    if kw_lower not in keywords:
                        keywords.append(kw_lower)
                else:
                    # Fallback sur ta recherche simple d'origine
                    match_simple = re.search(r'\"([a-zA-Z_][a-zA-Z0-9_]*)\"', line)
                    if match_simple:
                        kw = match_simple.group(1).lower()
                        if kw not in keywords:
                            keywords.append(kw)
    except Exception as e:
        print(f"Erreur lors de la lecture de {lexer_path}: {e}")
        sys.exit(1)

    # 2. Analyse optionnelle du XML de Bison pour extraire les signatures de fonctions
    smart_completions = []
    if xml_path and os.path.exists(xml_path):
        try:
            tree = ET.parse(xml_path)
            root = tree.getroot()
            for rule in root.findall(".//grammar/rules/rule"):
                symbols = [s.text for s in rule.findall("rhs/symbol")]
                # Si la règle commence par un token terminal (MAJUSCULES) suivi de '('
                if len(symbols) > 1 and symbols[0].isupper() and symbols[1] == "'('":
                    token = symbols[0]
                    if token in token_to_literal:
                        literal = token_to_literal[token]
                        
                        # Reconstruction propre des arguments de la règle Bison
                        args = []
                        depth = 0
                        for sym in symbols[1:]:
                            if sym == "'('":
                                depth += 1
                                if depth > 1: args.append(sym)
                            elif sym == "')'":
                                depth -= 1
                                if depth == 0: break
                                args.append(sym)
                            elif depth > 0:
                                # Nettoyage des symboles (ex: 'expression' -> expression, ',' -> ,)
                                clean_sym = sym.strip("'")
                                args.append(clean_sym)
                                
                        signature = f"{literal}({''.join(args)})"
                        smart_completions.append((literal, signature))
                        
                # Si la règle commence par un IDENTIFIER '=' TOKEN '('
                if len(symbols) > 3 and symbols[0].isupper() and symbols[1] == "'='" and symbols[2].isupper() and symbols[3] == "'('":
                    token = symbols[2]
                    if token in token_to_literal:
                        literal = token_to_literal[token]
                        
                        # Reconstruction propre des arguments de la règle Bison
                        args = []
                        depth = 0
                        for sym in symbols[3:]:
                            if sym == "'('":
                                depth += 1
                                if depth > 1: args.append(sym)
                            elif sym == "')'":
                                depth -= 1
                                if depth == 0: break
                                args.append(sym)
                            elif depth > 0:
                                # Nettoyage des symboles (ex: 'expression' -> expression, ',' -> ,)
                                clean_sym = sym.strip("'")
                                args.append(clean_sym)
                                
                        signature = f"{literal}({''.join(args)})"
                        smart_completions.append((literal, signature))
        except Exception as e:
            print(f"Attention: Impossible de parser le XML Bison {xml_path}: {e}")

    # 3. Écriture du fichier Header C++
    try:
        with open(output_path, 'w', encoding='utf-8') as out:
            out.write('#ifndef CAIRO_DSL_KEYWORDS_H\n')
            out.write('#define CAIRO_DSL_KEYWORDS_H\n')
            out.write('#include <QStringList>\n')
            out.write('#include <QList>\n')
            out.write('#include <QString>\n\n')
            
            # --- FONCTION 1 : Inchangée pour ta coloration syntaxique ---
            out.write('inline QStringList getCairoDslKeywords() {\n')
            out.write('    return QStringList{\n')
            for kw in keywords:
                out.write(f'        "\\\\b{kw}\\\\b",\n')
            out.write('    };\n')
            out.write('}\n\n')

            out.write('inline QStringList getCairoSmartCompletions() {\n')
            out.write('    return QStringList{\n')
            for trigger, completion in smart_completions:
                out.write(f'        "{completion}",\n')
            out.write('    };\n')
            out.write('}\n\n')

            out.write('#endif // CAIRO_DSL_KEYWORDS_H\n')
            
        print(f"Génération réussie : {output_path} ({len(keywords)} mots-clés, {len(smart_completions)} fonctions intelligentes détectées)")
    except Exception as e:
        print(f"Erreur lors de l'écriture de {output_path}: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
