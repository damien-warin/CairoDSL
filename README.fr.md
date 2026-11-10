# CairoInterpreter

[English](README.md) | [Français](README.fr.md)

**CairoInterpreter** est un petit interpréteur de langage de script construit au-dessus de la bibliothèque graphique [Cairo](https://www.cairographics.org/).

Il permet de décrire des opérations graphiques à l'aide d'un langage dédié, puis de produire des sorties graphiques, notamment au format SVG ou PNG.

Le projet propose plusieurs modes d'utilisation :
- **Bash** : exécution en ligne de commande.
- **SDL2** : affichage graphique interactif.
- **Qt** : interface graphique dédiée à l'édition et à l'exécution des scripts.
- **CGI** : exécution depuis un serveur web avec une sortie XML adaptée à une interface cliente.

---

## 1. Dépendances

### Dépendances principales

Le projet nécessite :
- **Cairo** : bibliothèque graphique.
- **Flex** et **Bison** : génération de l'analyseur lexical et syntaxique.
- **libxml2** : gestion des documents XML et import SVG.
- **uthash** : bibliothèque de tables de hachage.
- **SDL2** : affichage graphique (facultatif).
- **libb64** : encodage Base64 des images PNG en mode serveur.
- **Python 3** : utilisé par certains outils de génération du projet.
- **CMake** et un compilateur C : compilation.

La bibliothèque [log.c](https://github.com/rxi/log.c) est utilisée pour la gestion des journaux (logs).

### Installation sous Ubuntu / Debian

Pour installer les dépendances principales :

```bash
sudo apt install build-essential cmake bison flex python3 \
    libcairo2-dev libxml2-dev libsdl2-dev libb64-dev uthash-dev
```

### Dépendances supplémentaires pour Qt

L'interface graphique Qt nécessite Qt 6 et quelques bibliothèques complémentaires :

```bash
sudo apt install qt6-base-dev qt6-svg-dev \
    libqt6widgets6 libqt6xml6 libqt6svgwidgets6 \
    qt6-tools-dev linguist-qt6 librsvg2-dev
```

---

## 2. Options de compilation

Plusieurs options CMake permettent d'adapter la compilation aux différents environnements.

| Option | Description |
|---|---|
| `NOSDL` | Désactive la prise en charge de SDL2 pour la sortie graphique. L'interpréteur fonctionne alors sans dépendance à SDL2, notamment en mode Bash ou CGI. |
| `CGIMODE` | Active le mode serveur CGI : génération d'une sortie XML complète avec les en-têtes HTTP et prise en compte de `CONTENT_LENGTH` pour la lecture des données en entrée. |
| `QTGUI` | Compile l'interface graphique Qt. Actuellement incompatible avec la compilation SDL2. |
| `BUILD_UNITTESTS` | Active ou désactive la compilation des tests unitaires. |
| `UPDISTANT` | Option de compilation complémentaire du projet. |

---

## 3. Compilation

### Compilation avec l'interface Qt

Voici un exemple de configuration permettant de compiler rapidement l'interpréteur et son interface graphique Qt :

```bash
mkdir build
cd build

cmake ../ \
    -DCMAKE_VERBOSE_MAKEFILE=false \
    -DBUILD_UNITTESTS=false \
    -DUPDISTANT=true \
    -DNOSDL=true \
    -DCGIMODE=false \
    -DQTGUI=true

make
```

Cette configuration génère notamment :
- `CairoInterpreter` : l'interpréteur.
- `CairoDSLEditor` : l'interface graphique Qt.

Pour une compilation sans interface Qt, il suffit de désactiver `QTGUI`.

---

## 4. Utilisation

### 4.1. Mode Bash

L'interpréteur peut être utilisé directement depuis un terminal.

**Avec SDL2 :**

```bash
./CairoInterpreter < fichier
```

**Sans SDL2 :**

```bash
./CairoInterpreter < fichier > output
```

Dans ce second cas, la sortie graphique est récupérée dans le fichier `output`.

### 4.2. Interface graphique Qt

L'éditeur Qt permet de travailler directement avec les scripts CairoDSL.

Pour le lancer :

```bash
./CairoDSLEditor
```

La partie graphique de cette interface a été initialement générée à l'aide d'une IA, puis intégrée au projet.

### 4.3. Mode serveur CGI

En mode CGI, l'interpréteur attend que la variable d'environnement `CONTENT_LENGTH` indique la taille des données reçues en entrée.

Pour effectuer un test depuis le terminal :

```bash
export CONTENT_LENGTH=$(wc -c < fichier)
./CairoInterpreter < fichier
```

Le mode CGI produit une réponse XML complète, incluant les en-têtes nécessaires à son utilisation depuis un serveur web.

---

## 5. Interface web

Le projet contient également une interface web dans le fichier :

```text
Gui/Editor.html
```

Cette interface peut communiquer avec une version CGI de CairoInterpreter installée sur un serveur distant.

Pour cela, il suffit de modifier la variable `URL_DE_LA_CGI` directement dans `Editor.html`, en indiquant l'adresse du script CGI.

### Tester l'interface web en local sous Linux

Il est possible de tester l'interface web localement, sans avoir à installer un véritable serveur CGI.

1. Compiler CairoInterpreter avec `CGIMODE=true` et sans SDL2.
2. Copier l'exécutable dans le répertoire CGI :

```bash
cp build/CairoInterpreter Gui/cgi-bin/cairo.cgi
```

3. Modifier `URL_DE_LA_CGI` dans `Gui/Editor.html` :

```javascript
URL_DE_LA_CGI = "http://localhost:8000/cgi-bin/cairo.cgi";
```

4. Depuis le répertoire `Gui`, lancer un serveur HTTP local :

```bash
python3 -m http.server --cgi 8000
```

5. Ouvrir l'adresse suivante dans un navigateur :

```text
http://localhost:8000/Editor.html
```

**Remarque :** le support CGI du serveur HTTP intégré à Python est réservé aux versions qui proposent encore ce module. Pour les versions récentes de Python, un autre serveur compatible CGI peut être nécessaire.

---

## 6. Documentation

Le fichier `doc.odt` contient la documentation du langage CairoDSL.

Il présente les principales instructions, les fonctionnalités du langage et leur utilisation.

---

## 7. Langues

L'interface est disponible en deux langues :
- Français si la langue du système est le français.
- Anglais dans les autres cas.
