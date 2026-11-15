# CairoDSL – Manuel de référence

## Chapitre 1 : Directives, structures de contrôle et blocs

### 1.1 Directives de préprocesseur et Prerunning

Ces directives permettent de configurer l'environnement de rendu ou
d'importer des fichiers externes avant l'exécution du dessin.

- *\#include "fichier.cairo";* : Inclut et analyse un fichier source
  externe.
- *\#setoutputsize(width, height);* : Définit la taille du canevas de
  sortie en pixels (640 × 480 par défaut).
- *\#setpngoutput();* / *\#setsvgoutput();* : Activent respectivement
  l'exportation au format PNG ou SVG. Par défaut, le format SVG est
  utilisé hors environnement SDL2.
- *\#initmessage("Texte", expression);* : Affiche un message
  d'initialisation lors de la phase de prerunning.
- **\#randinit(seed); / \#randinit(); : Initialise le générateur de
  nombres pseudo-aléatoires. Avec une graine explicite, elle permet de
  reproduire une séquence donnée. Sans argument, la graine est
  construite à partir de l'heure système et du PID du processus.**

Règles générales :

Toutes les directives commençant par *\#* doivent obligatoirement
commencer en début de ligne et se terminer par un point-virgule.

À l'exception de *\#include*, qui intervient pendant l'analyse
syntaxique du programme, les directives sont traitées lors de la phase
de prerunning.

Plusieurs directives *\#setoutputsize* peuvent être utilisées : seule la
dernière valeur définie avant l'initialisation de Cairo est retenue.

### 1.2 Structures de contrôle (conditions et boucles)

Le langage propose plusieurs structures conditionnelles et itératives
pour piloter l'exécution du code :

- *if (cond) { ... } else { ... }* : Exécute des blocs
  conditionnellement. L'expression évaluée doit retourner un nombre
  (*double*) : 0 est faux et toute autre valeur est vraie.
- *for (i = \[start:end:step\]) { ... }* : Boucle itérative permettant
  de parcourir un intervalle numérique avec un pas optionnel.
- *foreach (item in array) { ... }* : Permet de parcourir les éléments
  d'un tableau.
- *while (cond) { ... }* : Répète un bloc d'instructions tant qu'une
  condition est remplie.
- *break;* et *continue;* : Instructions de rupture ou de saut au sein
  des boucles.

Les accolades peuvent être omises lorsqu'une structure de contrôle ne
contient qu'une seule instruction. Cette possibilité est notamment
disponible avec *if*, *for*, *foreach* et *while*.

### 1.3 Notions de blocs (*{ ... }*)

Le langage distingue deux types de blocs d'instructions :

1.  Les blocs classiques : ensembles d'instructions standards utilisés
    notamment dans les structures de contrôle.
2.  Les CanvaBloc : blocs qui sauvegardent automatiquement l'état du
    contexte graphique (*save()*) avant leur exécution et le restaurent
    (*restore()*) à leur sortie.

Certains CanvaBloc disposent également d'un mécanisme d'auto-draw.
Celui-ci est activé dans deux situations :

- Les blocs associés à l'instruction *color*.
- Le programme principal, qui constitue lui-même un CanvaBloc global.

L'auto-draw permet notamment d'écrire :

*rectangle(10, 10, 100, 100);*

sans instruction de rendu explicite.

Ce mécanisme intervient uniquement lorsqu'un chemin reste disponible à
la sortie du bloc.

Les instructions *translate*, *rotate*, *scale*, *skewX/Y*,
*applymatrix*, *setlinewidth*, *setdash*, *setfont* et *setfontsize*,
ainsi que les appels de modules, utilisent également des CanvaBloc, mais
sans auto-draw.

Lorsqu'un CanvaBloc déclenche un auto-draw, celui-ci est exécuté juste
avant la restauration du contexte graphique (*restore()*). Les
opérations de rendu automatique n'affectent donc pas l'état graphique
conservé après la fermeture du bloc.

### 1.4 Déclaration et appel de modules

Les modules permettent de structurer le code en composants
réutilisables.

*module NomDuModule(arg1, arg2) {*

* // Instructions du module*

* children();*

*}*

*// Appel sans bloc enfant*

*NomDuModule(val1, val2);*

*// Appel avec bloc enfant*

*NomDuModule(val1, val2) {*

* // Instructions transmises à children()*

*}*

La fonction *children()* s'utilise à l'intérieur d'un module pour
exécuter le bloc d'instructions fourni lors de son appel.

Un module doit obligatoirement déclarer au moins un paramètre, même si
celui-ci n'est pas utilisé dans son corps.

Les variables locales d'un module sont réinitialisées à chaque
exécution.

L'emplacement de *children()* dans le corps du module détermine le
moment où le bloc enfant est exécuté.

## Chapitre 2 : Chemins et géométrie

Ce chapitre détaille les primitives de dessin géométrique et la gestion
des tracés vectoriels (chemins ou paths) dans le langage.

### 2.1 Formes géométriques de base

Le langage propose plusieurs primitives permettant de dessiner
directement des formes géométriques simples.

Certaines fonctions acceptent des paramètres scalaires individuels ou un
tableau numérique regroupant ces paramètres. Les fonctions *polygon()*
et *polyline()* acceptent uniquement un tableau de coordonnées.

- *rectangle(x, y, w, h)* ou *rectangle(\[params\])* : Trace un
  rectangle à partir du point *(x, y)* avec une largeur *w* et une
  hauteur *h*.
- *arc(x, y, r, a1, a2)* ou *arc(\[params\])* : Trace un arc circulaire
  de centre *(x, y)*, de rayon *r*, entre les angles *a1* et *a2*.
- *arcneg(x, y, r, a1, a2)* ou *arcneg(\[params\])* : Trace un arc
  circulaire dans le sens inverse de *arc()*.
- *curve(m1x, m1y, m2x, m2y, x, y)* ou *curve(\[params\])* : Trace une
  courbe de Bézier cubique avec deux points de contrôle et un point
  final.
- *polygon(\[params\])* : Trace un polygone fermé formé par une suite de
  points sous forme de tableau de coordonnées *(x1, y1, x2, y2, ...)*.
- *polyline(\[params\])* : Trace une ligne brisée ouverte formée par une
  suite de points sous forme de tableau de coordonnées *(x1, y1, x2, y2,
  ...)*.

Les angles utilisés par *arc()* et *arcneg()* sont exprimés en radians.
Leur sens de rotation suit directement les conventions de Cairo.

### 2.2 Gestion des tracés et des sous-chemins

Pour construire des chemins vectoriels personnalisés, le moteur utilise
un ensemble de commandes de contrôle de tracés :

- *moveto(x, y)* : Déplace le point de départ du chemin courant sans
  tracer de ligne.
- *lineto(x, y)* : Trace un segment de ligne droite depuis la position
  courante jusqu'au point *(x, y)*.
- *newpath()* : Détruit le chemin courant et en crée un nouveau.
- *newsubpath()* : Commence un nouveau sous-chemin sans supprimer les
  tracés déjà présents.
- *closepath()* : Clôture le sous-chemin courant en reliant le point
  final au point initial.
- *getpathextents()* : Renvoie un tableau contenant le rectangle
  englobant du chemin sous la forme *\[x1, y1, x2, y2\]*, correspondant
  aux coordonnées des coins opposés du rectangle.

### 2.3 Application du rendu

Une fois le chemin défini, différentes instructions permettent de
l'appliquer sur le canevas.

- *stroke()* / *strokepreserve()* : Dessinent le contour du chemin
  actuel, avec ou sans conservation du chemin.
- *fill()* / *fillpreserve()* : Remplissent l'intérieur du chemin
  actuel, avec ou sans conservation.
- *clip()* / *clippreserve()* / *resetclip()* : Définissent une zone de
  découpe (masquage géométrique), avec ou sans conservation ou
  réinitialisent la zone de découpe.
- *setautofill()* / *setautostroke()* / *setautofillstroke()* /
  *setautostrokefill()* : Configurent le comportement du rendu
  automatique des formes géométriques lors de l'auto-draw.

Les instructions *stroke(),* *fill()* et *clip()* consomment le chemin
courant après le rendu. Leurs variantes *preserve* conservent le chemin.

Les fonctions *setauto...()* sont des instructions sans paramètre qui
configurent respectivement l'ordre des opérations lors de l'auto-draw :

- *setautofill()* : remplissage uniquement.
- *setautostroke()* : contour uniquement.
- *setautofillstroke()* : remplissage puis contour.
- *setautostrokefill()* : contour puis remplissage.

## Chapitre 3 : Couleurs et styles

Ce chapitre aborde la configuration des couleurs, de l'arrière-plan, de
l'épaisseur des traits et des styles graphiques.

### 3.1 Gestion des couleurs

Les couleurs sont manipulées à l'aide de valeurs comprises entre 0 et
255 pour les composantes Rouge, Vert, Bleu et Alpha (RGBA).

Les instructions de réglage des couleurs acceptent trois formes d'appel
:

- Paramètres individuels : *(r, g, b, a)*
- Tableau avec alpha : *\[r, g, b, a\]*
- Tableau sans alpha : *\[r, g, b\]*

Lorsque la composante alpha est absente, sa valeur est fixée à 255
(opacité complète).

- *color(r, g, b, a)* : Définit la couleur courante pour le tracé et le
  remplissage. Cette instruction peut être utilisée seule ou sous forme
  de bloc d'instructions (CanvaBloc). Ce bloc dispose du mécanisme
  d'auto-draw.
- *setbgcolor(r, g, b, a)* : Définit la couleur de fond du canevas.
- *setautostrokecolor(r, g, b, a)* : Définit la couleur utilisée pour
  les contours automatiques.
- *setautofillcolor(r, g, b, a)* : Définit la couleur utilisée pour les
  remplissages automatiques.
- *settextcolor(r, g, b, a)* : Définit la couleur utilisée pour le rendu
  des textes. Cette couleur ne s'applique pas à *textpath()*, qui
  construit un chemin vectoriel.

Les fonctions *setautostrokecolor()* et *setautofillcolor()* agissent
sur le contexte interne de CairoDSL et non directement sur le contexte
graphique Cairo. Elles configurent les couleurs utilisées lors de
l'auto-draw et n'affectent pas directement l'état graphique courant.

### 3.2 Style des lignes

- *setlinewidth(width)* : Modifie l'épaisseur des lignes.
- *setdash(expression)* ou *setdash(expression, offset)* : Définit un
  motif de pointillé pour les lignes. Le premier argument est un tableau
  numérique définissant alternativement les longueurs des segments
  tracés et des espaces. Le second argument, facultatif, indique le
  décalage (*offset*) du motif.

Les instructions *setlinewidth()* et *setdash()* peuvent également être
utilisées sous forme de CanvaBloc. Elles sauvegardent et restaurent
l'état du contexte graphique, sans déclencher d'auto-draw.

# Chapitre 4 : Transformations

Les transformations géométriques permettent de manipuler le repère du
canevas afin d'effectuer des translations, des rotations, des mises à
l'échelle ou des déformations.

## 4.1 Transformations spatiales et matricielles

Les transformations peuvent être utilisées directement, suivies d'un
point-virgule, ou associées à un bloc d'instructions (CanvaBloc).

- *translate(x, y)* : applique une translation du repère selon les axes
  X et Y.
- *rotate(angle)* : applique une rotation du repère selon l'angle
  spécifié, exprimé en radians.
- *scale(x, y)* : applique une mise à l'échelle sur les axes X et Y.
- *applymatrix(m1, m2, m3, m4, m5, m6)* : applique une matrice de
  transformation personnalisée à six paramètres. Cette fonction reprend
  directement le mécanisme *cairo_apply_matrix* de Cairo.
- *skewX(angle)* / *skewY(angle)* : applique une inclinaison (*skew*)
  selon l'axe X ou Y. L'angle est exprimé en radians.

## 4.2 Sauvegarde et restauration de l'état

- *save()* : sauvegarde l'état actuel du contexte graphique.
- *restore()* : restaure le dernier état sauvegardé.

Ces deux fonctions sont des relais directs vers les fonctions
correspondantes de Cairo. Elles permettent notamment de gérer
explicitement la pile des états graphiques, indépendamment de
l'utilisation des CanvaBloc.

# Chapitre 5 : Textes et typographie

Ce chapitre décrit la configuration des polices, le rendu des chaînes de
caractères et l'accès aux métriques typographiques. CairoDSL s'appuie
directement sur les mécanismes de gestion du texte de Cairo.

## 5.1 Configuration de la police

- *setfont("Family", "Slant", "Weight")* : Définit la famille
  typographique, le style et la graisse de la police.

  - *Family* : nom de la famille de police, transmis directement à
    Cairo.
  - *Slant* : inclinaison du texte. Valeurs reconnues : *"normal"*,
    *"italic"* ou *"oblique"*.
  - *Weight* : graisse du texte. Valeurs reconnues : *"normal"* ou
    *"bold"*.

  Les comparaisons des paramètres de style et de graisse sont
  insensibles à la casse. Toute valeur non reconnue entraîne
  l'utilisation du style normal ou de la graisse normale correspondante.

- *setfontsize(size)* : Définit la taille de la police courante.

Ces instructions modifient l'état typographique du contexte graphique et
peuvent être utilisées directement, suivies d'un point-virgule, ou
associées à un bloc d'instructions (CanvaBloc).

## 5.2 Rendu du texte et création de tracés

CairoDSL propose deux façons distinctes d'utiliser du texte :

\- *text(expression)* : Affiche directement le texte à la position
courante du contexte graphique. La couleur utilisée est celle
précédemment définie par *settextcolor()*.

*Settextcolor* enregistre la couleur dans le contexte CairoDSL et ne
modifie le contexte cairo que lors de l’appel à text(), le tout encadré
par save et restore.**

\- *textpath(expression)* : Convertit les caractères en tracés
vectoriels et les ajoute au chemin courant. Aucun rendu direct n'est
effectué. Le texte peut ensuite être manipulé comme n'importe quel autre
chemin, notamment avec *fill()*, *stroke()*, les variantes *preserve* ou
les mécanismes de découpe.

Contrairement à *text()*, *textpath()* ne tient pas compte de la couleur
définie par *settextcolor()*. Il utilise la source graphique courante de
Cairo.

Les deux instructions utilisent le point courant du contexte graphique
pour positionner le texte, et positionnent ensuite le point courant au
bout du texte.

À retenir : *text()* réalise un affichage typographique direct avec la
couleur dédiée au texte, tandis que *textpath()* construit une géométrie
vectorielle soumise aux mécanismes habituels de rendu des chemins.

## 5.3 Métriques typographiques

CairoDSL expose deux fonctions permettant d'obtenir des informations
numériques sur les dimensions du texte et les caractéristiques de la
police.

- *gettextextents(expression)* : Renvoie les métriques d'une chaîne de
  caractères sous la forme d'un tableau numérique de six éléments :

  \| Index \| Valeur \| Description \|  
  \|---:\|---\|---\|  
  \| 0 \| *x_bearing* \| Décalage horizontal de la boîte du texte par
  rapport à la position courante. \|  
  \| 1 \| *y_bearing* \| Décalage vertical de la boîte du texte par
  rapport à la position courante. \|  
  \| 2 \| *width* \| Largeur de la boîte englobante du texte. \|  
  \| 3 \| *height* \| Hauteur de la boîte englobante du texte. \|  
  \| 4 \| *x_advance* \| Avancement horizontal du point courant après le
  texte. \|  
  \| 5 \| *y_advance* \| Avancement vertical du point courant après le
  texte. \|

- *getfontextents()* : Renvoie les métriques générales de la police
  courante sous la forme d'un tableau numérique de cinq éléments :

  \| Index \| Valeur \| Description \|  
  \|---:\|---\|---\|  
  \| 0 \| *ascent* \| Hauteur au-dessus de la ligne de base. \|  
  \| 1 \| *descent* \| Profondeur sous la ligne de base. \|  
  \| 2 \| *height* \| Hauteur recommandée pour l'espacement des lignes.
  \|  
  \| 3 \| *max_x_advance* \| Avancement horizontal maximal des
  caractères. \|  
  \| 4 \| *max_y_advance* \| Avancement vertical maximal des caractères.
  \|

Les valeurs retournées correspondent directement aux métriques fournies
par Cairo, sans transformation particulière, hormis leur conversion en
valeurs numériques de type *double*.

Important : Ces fonctions renvoient des tableaux numériques sans champs
nommés. Les valeurs doivent être récupérées par leur index, en
commençant à zéro.

Par ailleurs, *width* et *height* décrivent la boîte englobante des
caractères, tandis que *x_advance* et *y_advance* décrivent le
déplacement du point courant. Ces valeurs ne sont donc pas
nécessairement identiques.

Exemple :

*setfont("Sans", "normal", "bold");*

*setfontsize(24);*

*text("CairoDSL");*

*metrics = gettextextents("CairoDSL");*

*largeur = metrics\[2\];*

*hauteur = metrics\[3\];*

*avancement = metrics\[4\];*

# Chapitre 6 : Groupes, patterns et masques

CairoDSL permet de créer et de manipuler des sources graphiques
réutilisables à partir de groupes de dessin et de patterns. Ces objets
peuvent être utilisés pour appliquer des rendus complexes, notamment des
dégradés ou des masques alpha.

Les groupes et les patterns sont référencés par des identifiants
numériques internes à CairoDSL, de type *double*. Ils partagent le même
espace de stockage interne : du point de vue de Cairo, ces objets sont
tous représentés par des patterns.

## 6.1 Les groupes

Les groupes permettent d'enregistrer une séquence d'instructions
graphiques sous la forme d'une source réutilisable, sans la dessiner
immédiatement sur le canevas.

Deux syntaxes permettent de créer un groupe.

**Création explicite :**

*pushgroup();*

*// Instructions graphiques*

*rectangle(10, 10, 100, 100);*

*id = popgroup();*

- *pushgroup()* : Commence la création d'un groupe graphique. Cette
  instruction est un relais vers *cairo_push_group()*.
- *popgroup()* : Termine la création du groupe et retourne son
  identifiant numérique interne à CairoDSL.

**Création par bloc :**

*id = group() {*

* rectangle(10, 10, 100, 100);*

* rectangle(40, 40, 50, 50);*

*}*

*group()* constitue un CanvaBloc : il sauvegarde automatiquement l'état
du contexte graphique avant l'exécution du bloc et le restaure à sa
sortie.

L'identifiant retourné permet de conserver et de réutiliser le groupe.

**Attention syntaxique :** contrairement à une instruction classique, la
fermeture du bloc *group()* ne doit actuellement pas être suivie d'un
point-virgule.

### Utilisation des groupes

- *usegroup(id)* : Sélectionne le groupe comme source graphique
  courante.
- *paint()* : Applique la source graphique courante sur l'ensemble du
  canevas.
- *paint(alpha)* : Applique la source graphique courante avec une valeur
  d'alpha global.

*usegroup()* ne réalise aucun rendu à lui seul. Il sélectionne
simplement la source qui sera utilisée par les instructions de rendu
suivantes.

## 6.2 Les patterns : dégradés linéaires et radiaux

Les patterns permettent de définir des sources graphiques évolutives,
notamment sous la forme de dégradés de couleurs.

CairoDSL propose deux types de patterns :

- *newlinear(x1, y1, x2, y2)* : Crée un dégradé linéaire défini par deux
  points qui déterminent son axe.
- *newradial(x1, y1, r1, x2, y2, r2)* : Crée un dégradé radial défini
  par deux cercles, chacun caractérisé par son centre et son rayon.

Ces fonctions retournent un identifiant numérique interne à CairoDSL.

### Ajout des points d'arrêt

La fonction *addstop()* permet d'ajouter des points d'arrêt de couleur
au dégradé.

**Hors bloc :**

*p = newlinear(0, 0, 200, 0);*

*addstop(p, 0, 255, 0, 0, 255);*

*addstop(p, 1, 0, 0, 255, 255);*

Syntaxe :

*addstop(id, position, r, g, b, a)*

- *id* : identifiant du pattern.
- *position* : position du point d'arrêt dans le dégradé, comprise entre
  0 et 1.
- *r*, *g*, *b*, *a* : composantes de couleur RGBA, comprises entre 0 et
  255.

**Dans un bloc de création :**

*p = newlinear(0, 0, 200, 0) {*

* addstop(0, 255, 0, 0, 255);*

* addstop(1, 0, 0, 255, 255);*

*}*

Dans ce contexte, l'identifiant du pattern est implicite. La fonction
*addstop()* utilise directement le pattern en cours de création.

Cette syntaxe est également disponible avec *newradial()*.

**Attention syntaxique :** comme pour *group()*, le bloc de création
d'un pattern ne doit actuellement pas être suivi d'un point-virgule
après son accolade fermante.

### Utilisation des patterns

- *usepattern(id)* : Sélectionne un pattern comme source graphique
  courante.
- *paint()* : Applique le pattern sélectionné sur le canevas.
- *paint(alpha)* : Applique le pattern sélectionné avec un alpha global.

*usepattern()* et *usegroup()* sont deux alias reconnus par CairoDSL.
Ils effectuent la même opération de sélection d'une source graphique.

## 6.3 Les masques

La fonction *mask(id)* permet d'utiliser un groupe ou un pattern comme
masque alpha.

Elle récupère le pattern correspondant à l'identifiant fourni dans le
stockage interne de CairoDSL, puis applique le mécanisme *cairo_mask()*
de Cairo.

Le masque utilise le canal alpha du pattern pour déterminer la
transparence appliquée à la source graphique courante.

Syntaxe :

*mask(id);*

Le masque est appliqué à la source graphique courante sur la zone
définie par le clipping actif. Le chemin courant n'intervient pas dans
cette opération.

Cela permet notamment d'utiliser un dégradé ou un groupe comme masque de
transparence pour un autre dessin.

## 6.4 Gestion des identifiants

Les identifiants des groupes et des patterns sont des valeurs numériques
de type *double*, gérées par CairoDSL.

Ils partagent un même espace de stockage interne. Cette organisation est
cohérente avec le fonctionnement de Cairo, qui représente aussi bien les
groupes que les dégradés sous forme de patterns.

Il est donc possible d'utiliser les fonctions de sélection et de
masquage avec les identifiants correspondants, sans distinction de
catégorie au niveau de ce stockage.

# Chapitre 7 : Importation SVG

CairoDSL permet d'importer des fichiers vectoriels SVG et de les
intégrer directement dans le rendu CairoDSL. L'importateur analyse le
document SVG et en interprète les éléments graphiques pris en charge.

L'importation repose sur l'analyse XML du document. Elle permet
notamment de réutiliser un dessin vectoriel existant sans devoir le
retranscrire dans le langage CairoDSL.

## 7.1. Importer un fichier SVG

La fonction *import()* permet de charger un fichier SVG.

Deux modes d'utilisation sont possibles.

### Importation et dessin immédiat

*import("dessin.svg");*

Le fichier est importé et son contenu est dessiné immédiatement sur le
canevas.

### Importation avec conservation d'un identifiant

*id = import("dessin.svg");*

Dans ce cas, l'importation est enregistrée et la fonction retourne un
identifiant numérique permettant de la réutiliser ultérieurement.

La fonction *drawimport()* permet alors de dessiner ou redessiner
l'importation :

*id = import("dessin.svg");*

*drawimport(id);*

L'identifiant peut être conservé dans une variable et réutilisé autant
que nécessaire.

### Résolution des chemins

Les chemins relatifs sont résolus à partir du répertoire de travail
courant (*Current Working Directory*, ou CWD) contenant le fichier
source CairoDSL qui a été ouvert.

## 7.2. Éléments SVG pris en charge

L'importateur reconnaît plusieurs catégories d'éléments SVG.

Parmi les éléments graphiques pris en charge figurent notamment :

- *\<g\>* : groupes d'éléments ;
- *\<rect\>* : rectangles ;
- *\<path\>* : tracés vectoriels ;
- *\<line\>* : segments ;
- *\<circle\>* : cercles ;
- *\<ellipse\>* : ellipses ;
- *\<text\>* : texte.

Cette liste n'est pas exhaustive. La prise en charge des éléments SVG ne
signifie toutefois pas que l'ensemble de la spécification SVG est
implémenté.

## 7.3. Transformations

Les transformations SVG suivantes sont prises en charge :

- *translate* : translation ;
- *rotate* : rotation ;
- *scale* : mise à l'échelle ;
- *matrix* : transformation matricielle ;
- *skewX* et *skewY* : inclinaison horizontale ou verticale.

Les transformations sont interprétées selon les conventions du format
SVG. En particulier, les angles exprimés dans les attributs SVG sont
convertis lorsque nécessaire pour leur application dans Cairo.

Les transformations peuvent être combinées selon la syntaxe SVG.

## 7.4. Couleurs et styles

L'importateur prend en charge plusieurs attributs permettant de définir
l'apparence des éléments :

- *fill* : couleur de remplissage ;
- *stroke* : couleur du contour ;
- *fill-opacity* : opacité du remplissage ;
- *stroke-opacity* : opacité du contour ;
- *style* : déclaration de styles directement dans l'attribut.

Les couleurs peuvent être définies à l'aide des formats reconnus par
l'importateur.

Certaines propriétés de style, comme *stroke-dasharray*, sont gérées.

**Limite importante :** l'importateur n'est pas un moteur SVG/CSS
entièrement conforme aux spécifications. Toutes les propriétés CSS et
toutes les règles de cascade et d'héritage des styles SVG ne sont pas
implémentées. Il ne faut donc pas considérer qu'un fichier SVG complexe
sera nécessairement rendu à l'identique de son affichage dans un
navigateur.

En particulier, l'héritage des styles entre éléments et la gestion de
l'opacité globale ne sont pas entièrement pris en charge.

## 7.5. Texte SVG

Les éléments *\<text\>* sont pris en charge avec les attributs suivants
:

- *x* et *y* : position du texte ;
- *font-weight* : graisse de la police ;
- *font-size* : taille de la police.

Cette prise en charge reste limitée par rapport à la spécification SVG
complète.

Par exemple, l'attribut *text-anchor*, qui permet d'aligner le texte par
rapport à son point d'ancrage, n'est pas actuellement implémenté.

## 7.6. Dimensions de l'importation

La fonction *getimportextents(id)* permet de récupérer les dimensions
déclarées dans le *viewBox* du document SVG importé.

Elle retourne un tableau de quatre valeurs :

*extents = getimportextents(id);*

Le tableau contient :

|        |                           |
|--------|---------------------------|
| Indice | Valeur                    |
| 0      | Coordonnée X du *viewBox* |
| 1      | Coordonnée Y du *viewBox* |
| 2      | Largeur du *viewBox*      |
| 3      | Hauteur du *viewBox*      |

L'identifiant de l'importation doit être fourni en argument.

Si le document SVG ne contient pas d'attribut *viewBox*, la fonction
retourne :

*\[0, 0, 0, 0\]*

Il s'agit des dimensions déclarées dans le *viewBox*, et non
nécessairement des limites géométriques réelles des éléments dessinés.

## 7.7. À retenir

L'importateur SVG permet d'intégrer des ressources vectorielles externes
dans un programme CairoDSL, de les réutiliser grâce à leur identifiant
et de récupérer les dimensions déclarées dans leur *viewBox*.

Il prend en charge plusieurs formes géométriques, les transformations et
différentes propriétés de couleur et de style.

Il ne constitue cependant pas un moteur de rendu SVG complet : certaines
fonctionnalités de la spécification, notamment les dégradés, les motifs,
ainsi que certaines règles CSS, l'héritage des styles et la typographie,
restent hors de son périmètre actuel.

# Chapitre 8 : Opérateurs et fonctions mathématiques

CairoDSL met à disposition des opérateurs et des fonctions mathématiques
permettant de réaliser des calculs, des comparaisons et des opérations
logiques. Ces outils servent notamment à calculer dynamiquement les
dimensions, positions et métriques géométriques des dessins.

## 8.1. Opérateurs arithmétiques

Les opérateurs arithmétiques permettent d'effectuer les opérations
numériques classiques.

|                      |                          |
|----------------------|--------------------------|
| Opérateur / Fonction | Description              |
| *+*                  | Addition                 |
| *-*                  | Soustraction             |
| *\**                 | Multiplication           |
| */*                  | Division                 |
| *power(base, exp)*   | Élévation à la puissance |
| *mod(a, b)*          | Modulo                   |

Les fonctions *power()* et *mod()* acceptent des valeurs numériques.

La fonction *mod()* permet notamment de calculer un reste avec des
nombres flottants.

### Gestion des valeurs particulières

Les opérations arithmétiques ne disposent pas encore d'un mécanisme
spécifique de traitement des valeurs *NaN* (*Not a Number*) et *Inf*
(infini).

Par exemple, une division par zéro peut produire une valeur infinie
(*Inf*) ou indéterminée (*NaN*), selon les opérandes. Ces valeurs sont
ensuite propagées dans les calculs suivants selon les règles habituelles
des opérations flottantes.

Il appartient donc au programmeur de tenir compte de ces éventuelles
valeurs dans ses calculs.

## 8.2. Opérateurs de comparaison

Les opérateurs de comparaison disponibles sont :

|           |                   |
|-----------|-------------------|
| Opérateur | Signification     |
| *==*      | Égalité           |
| *!=*      | Différence        |
| *\<*      | Inférieur         |
| *\<=*     | Inférieur ou égal |
| *\>*      | Supérieur         |
| *\>=*     | Supérieur ou égal |

Les comparaisons *==* et *!=* prennent en compte le type et la structure
des valeurs comparées.

Le fonctionnement est le suivant :

1.  Les deux valeurs doivent être de même type.
2.  Dans le cas des tableaux, ils doivent également contenir le même
    nombre d'éléments.
3.  Si ces conditions sont satisfaites, les contenus sont comparés
    jusqu'à la première différence.

Ainsi, deux chaînes sont comparées sur leur contenu, et deux tableaux
sur leurs éléments respectifs.

Les opérateurs *==* et *!=* retournent donc un résultat faux ou vrai
selon que les valeurs sont identiques ou différentes, après vérification
de leur compatibilité structurelle.

Les comparaisons numériques (*\<*, *\<=*, *\>* et *\>=*) s'appliquent
uniquement aux valeurs numériques simples (ne s’applique pas aux
tableaux).

## 8.3. Opérateurs logiques

CairoDSL propose quatre opérateurs logiques :

- *and* : ET logique ;
- *or* : OU logique ;
- *xor* : OU exclusif ;
- *not* : négation logique.

Ces opérateurs travaillent uniquement sur des valeurs numériques
scalaires.

La convention utilisée est la suivante :

- *0* représente une valeur fausse ;
- toute autre valeur représente une valeur vraie.

Les opérateurs logiques retournent un résultat numérique correspondant
au résultat de l'opération.

Ils ne constituent pas des opérateurs de comparaison de chaînes ou de
tableaux.

## 8.4. Trigonométrie et conversions d'angles

CairoDSL propose les fonctions trigonométriques classiques.

|             |              |
|-------------|--------------|
| Fonction    | Description  |
| *cos(rad)*  | Cosinus      |
| *sin(rad)*  | Sinus        |
| *tan(rad)*  | Tangente     |
| *acos(val)* | Arc cosinus  |
| *asin(val)* | Arc sinus    |
| *atan(val)* | Arc tangente |

Les angles utilisés par les fonctions trigonométriques sont exprimés en
radians. Les fonctions inverses retournent également des radians.

Deux fonctions permettent de convertir les angles :

- *radians(deg)* : convertit un angle exprimé en degrés vers les radians
  ;
- *degrees(rad)* : convertit un angle exprimé en radians vers les
  degrés.

Ces fonctions peuvent prendre un tableau et retournent alors un tableau.

### Constantes mathématiques

Deux fonctions sans argument donnent accès à des constantes
mathématiques :

- *pi()* : retourne π, approximativement égal à 3,14159 ;
- *phi()* : retourne le nombre d'or φ, approximativement égal à 1,61803.

## 8.5. Arrondis et racines

Trois fonctions sont disponibles pour les opérations courantes d'arrondi
et de calcul de racine :

|            |                                            |
|------------|--------------------------------------------|
| Fonction   | Description                                |
| *floor(x)* | Retourne la partie entière inférieure de x |
| *ceil(x)*  | Retourne la partie entière supérieure de x |
| *sqrt(x)*  | Calcule la racine carrée de x              |

Ces fonctions peuvent prendre un tableau et retournent alors un tableau.

## 8.6. Génération de nombres aléatoires

La fonction *rand()* permet de générer des valeurs aléatoires.

Deux formes sont disponibles :

*valeur = rand();*

*valeur = rand(max);*

- *rand()* génère une valeur aléatoire entre 0 et 1.
- *rand(max)* génère une valeur comprise entre *0* et *max*, bornes
  incluses.

### Initialisation du générateur

La directive *\#randinit* permet d'initialiser la graine (*seed*) du
générateur aléatoire.

Avec une graine explicite :

*\#randinit(1234);*

Sans argument :

*\#randinit();*

Dans ce second cas, la graine est construite à partir de l'heure système
et du PID (*Process ID*) du processus.

L'utilisation d'une graine fixe permet de retrouver une même séquence
pseudo-aléatoire lors des exécutions, dans les mêmes conditions.

## 8.7. Conversion de types

Deux fonctions permettent de convertir des valeurs entre chaînes de
caractères et nombres.

### Conversion d'une chaîne en nombre

*valeur = strtod("12.3");*

La fonction *strtod()* utilise directement la fonction standard C du
même nom.

Elle convertit le début de la chaîne en nombre flottant (*double*).

Si la chaîne ne contient pas de représentation numérique valide, strtod
donne 0.

### Conversion d'un nombre en chaîne

*texte = dtostr(12.3);*

La fonction *dtostr()* convertit un nombre flottant en chaîne de
caractères en utilisant le format *%g*.

Ce format adapte automatiquement la représentation du nombre, en
choisissant une écriture décimale ou scientifique selon sa valeur.

Ces deux fonctions peuvent prendre un tableau de leur type d’entrée et
retournent alors un tableau de leur type de sortie.

valeurs=strtod(*"12.3", "21", "42");//Valeurs contient \[12.3, 21, 42\]*

# Chapitre 9 : Variables, tableaux et chaînes de caractères

Ce chapitre présente les différents types de données manipulés par
CairoDSL, ainsi que les mécanismes de déclaration, d'affectation et de
manipulation des variables, tableaux et chaînes de caractères.

## 9.1 Types de données et affectation

CairoDSL utilise cinq types de données internes :

|         |                                         |
|---------|-----------------------------------------|
| Type    | Description                             |
| NONE    | Variable non initialisée ou vidée.      |
| DOUBLE  | Valeur numérique flottante (double).    |
| DOUBLES | Tableau de valeurs numériques (double). |
| STRING  | Chaîne de caractères.                   |
| STRINGS | Tableau de chaînes de caractères.       |

Les tableaux sont homogènes : un tableau contient exclusivement des
valeurs numériques ou exclusivement des chaînes de caractères. Les
tableaux mixtes et les tableaux imbriqués ne sont pas pris en charge.

### Déclaration et affectation

Une variable est créée ou modifiée par simple affectation :

*x = 12.5;*

*texte = "Bonjour";*

*nombres = \[1, 2, 3\];*

*mots = \["Bonjour", "CairoDSL"\];*

Le type de la variable est déterminé automatiquement par la valeur
affectée.

L'instruction *unset()* permet de vider une variable et de réinitialiser
son type :

*unset(x);*

Après cette opération, la variable retrouve le type NONE et peut être
réutilisée pour stocker une autre catégorie de données.

## 9.2 Génération de tableaux numériques

Les tableaux numériques peuvent être construits directement à l'aide de
crochets ou générés dynamiquement à partir d'un intervalle.

### Syntaxe des intervalles

Deux syntaxes sont disponibles :

*tableau = \[min:max\];*

*tableau = \[min:max:step\];*

- *min* : valeur initiale de l'intervalle.
- *max* : valeur maximale visée.
- *step* : incrément appliqué entre deux éléments.

Lorsque le pas est omis, sa valeur est fixée à 1.

Les bornes sont inclusives : la valeur maximale est intégrée au tableau
lorsqu'elle est atteinte exactement par la progression.

Exemples :

*a = \[0:5\]; // \[0, 1, 2, 3, 4, 5\]*

*b = \[0:10:2\]; // \[0, 2, 4, 6, 8, 10\]*

*c = \[0:10:3\]; // \[0, 3, 6, 9\]*

*d = \[5:0:-1\]; // \[5, 4, 3, 2, 1, 0\]*

Un pas négatif permet de parcourir un intervalle dans l'ordre
décroissant.

**Attention :** le moteur ne vérifie actuellement pas la cohérence entre
le sens de progression et les bornes. Un intervalle dont le pas ne
permet pas d'atteindre la borne dans le bon sens, par exemple
*\[0:10:-1\]*, peut entraîner une tentative d'allocation mémoire
démesurée et provoquer un échec (*malloc failed*).

Il convient donc de toujours adapter le signe du pas au sens de
progression souhaité.

## 9.3 Opérateurs sur les nombres, tableaux et chaînes

L'opérateur *+* dispose de plusieurs comportements selon les types des
opérandes.

### Addition numérique

Deux valeurs numériques simples sont additionnées normalement :

*resultat = 10 + 5;*

### Opérations sur les tableaux numériques

Deux tableaux numériques de même taille peuvent être additionnés élément
par élément :

*a = \[1, 2, 3\];*

*b = \[10, 20, 30\];*

*c = a + b; // \[11, 22, 33\]*

Les deux tableaux doivent avoir le même nombre d'éléments.

### Concaténation de chaînes

Deux chaînes simples peuvent être concaténées :

*texte = "Cairo" + "DSL"; // "CairoDSL"*

### Opérations sur les tableaux de chaînes

Deux tableaux de chaînes de même taille peuvent être combinés élément
par élément. Chaque élément du premier tableau est concaténé avec
l'élément correspondant du second :

*a = \["Bon", "Cairo"\];*

*b = \["jour", "DSL"\];*

*c = a + b; // \["Bonjour", "CairoDSL"\]*

Il est également possible de combiner une chaîne simple avec un tableau
de chaînes :

- *STRING + STRINGS* : préfixe la chaîne à chaque élément du tableau.
- *STRINGS + STRING* : suffixe la chaîne à chaque élément du tableau.

Exemple :

*mots = \["Bonjour", "Bonsoir"\];*

*prefixes = "Hello " + mots;*

*// \["Hello Bonjour", "Hello Bonsoir"\]*

*suffixes = mots + " !";*

*// \["Bonjour !", "Bonsoir !"\]*

Ces opérations produisent un nouveau tableau et ne modifient pas
directement les opérandes d'origine.

## 9.4 Concaténation et ajout d'éléments

CairoDSL propose deux formes équivalentes pour construire ou compléter
des tableaux :

*concat(tableau, element);*

*tableau \<\< element;*

L'opérateur *\<\<* constitue un alias syntaxique de *concat()*.

Ces opérations permettent :

- De concaténer deux tableaux de même type.
- D'ajouter un élément à la fin d'un tableau.
- D'initialiser progressivement un tableau qui n'existe pas encore.

Exemple :

*nombres = \[1, 2\];*

*nombres = concat(nombres,3);*

*nombres = nombres \<\< 4;*

*// nombres = \[1, 2, 3, 4\]*

Initialisation dynamique :

*nombres = nombres \<\< 10;*

*nombres = nombres \<\< 20;*

*nombres = nombres \<\< 30;*

Le tableau est automatiquement créé lors de son premier ajout.

Les règles de compatibilité des types s'appliquent : les tableaux
numériques et les tableaux de chaînes restent distincts.

## 9.5 Extraction et consultation des collections

Plusieurs fonctions permettent d'interroger le contenu des tableaux et
des chaînes.

### getfirst() et getlast()

*premier = getfirst(tableau);*

*dernier = getlast(tableau);*

- *getfirst()* retourne le premier élément.
- *getlast()* retourne le dernier élément.

Lorsque la collection est vide :

- Pour un tableau numérique, le résultat est *NAN*.
- Pour une chaîne ou un tableau de chaînes, le résultat est une chaîne
  vide *""*.

### getsize()

*taille = getsize(collection);*

La fonction retourne :

- Pour un tableau : le nombre d'éléments.
- Pour une chaîne : la taille mémoire allouée à la chaîne, en octets,
  terminateur nul compris.

### getlen()

*longueur = getlen(texte);*

Cette fonction retourne la longueur de la chaîne de caractères, selon le
fonctionnement de *strlen()* en C.

Contrairement à *getsize()*, le terminateur nul n'est pas comptabilisé.

## 9.6 Accès direct par index

Les éléments d'un tableau peuvent être consultés directement à l'aide de
leur index :

*tableau = \[10, 20, 30\];*

*valeur = tableau\[0\]; // 10*

*valeur = tableau\[2\]; // 30*

L'indexation commence à zéro.

L'expression utilisée comme index est convertie en entier non signé
(*unsigned int*).

Si l'index obtenu dépasse les limites du tableau, le moteur génère une
erreur.

Il convient donc de vérifier les limites avant d'accéder à un élément,
notamment lors de parcours dynamiques de tableaux.

# Chapitre 10 : Gestion du temps et de l'environnement

CairoDSL fournit des fonctions permettant d'accéder à l'horloge système,
de convertir des timestamps en informations calendaires et de récupérer
des paramètres depuis l'environnement d'exécution.

## 10.1 Horloge système et timestamps

La fonction *gettimeinsecs()* permet de récupérer l'heure système sous
forme d'un timestamp UNIX.

*timestamp = gettimeinsecs();*

Elle retourne le nombre de secondes écoulées depuis l'époque UNIX (1er
janvier 1970 à 00:00:00 UTC).

La valeur retournée est un nombre de type DOUBLE.

## 10.2 Conversion des timestamps en informations calendaires

Deux fonctions permettent de décomposer un timestamp en informations
calendaires :

- *getgmttime(timestamp)* : convertit le timestamp en heure UTC (GMT).
- *getlocaltime(timestamp)* : convertit le timestamp en heure locale du
  système d'exécution.

Ces deux fonctions retournent un tableau numérique de neuf éléments,
organisé selon l'ordre des champs de la structure *struct tm* du langage
C.

|       |       |                          |            |
|-------|-------|--------------------------|------------|
| Index | Champ | Description              | Valeurs    |
| 0     | sec   | Secondes                 | 0 à 60     |
| 1     | min   | Minutes                  | 0 à 59     |
| 2     | hour  | Heures                   | 0 à 23     |
| 3     | mday  | Jour du mois             | 1 à 31     |
| 4     | mon   | Mois de l'année          | 0 à 11     |
| 5     | year  | Année depuis 1900        | Variable   |
| 6     | wday  | Jour de la semaine       | 0 à 6      |
| 7     | yday  | Jour dans l'année        | 0 à 365    |
| 8     | isdst | Indicateur d'heure d'été | -1, 0 ou 1 |

### Particularités des valeurs retournées

Certaines conventions héritées de *struct tm* méritent une attention
particulière :

- **Mois (*****mon*****)** : commence à zéro. Janvier correspond donc à
  0 et décembre à 11.

- **Année (*****year*****)** : représente le nombre d'années écoulées
  depuis 1900. Pour obtenir l'année calendaire, il faut ajouter 1900.

- **Jour de la semaine (*****wday*****)** : dimanche vaut 0, lundi vaut
  1, jusqu'à samedi qui vaut 6.

- **Jour dans l'année (*****yday*****)** : commence à zéro. Le premier
  janvier correspond à 0.

- **Heure d'été (*****isdst*****)** :

  - *1* : l'heure d'été est active.
  - *0* : l'heure d'été n'est pas active.
  - *-1* : l'information n'est pas disponible.

### Exemple d'utilisation

*now = gettimeinsecs();*

*utc = getgmttime(now);*

*local = getlocaltime(now);*

*heure = local\[2\];*

*minute = local\[1\];*

*jour = local\[3\];*

*mois = local\[4\] + 1;*

*annee = local\[5\] + 1900;*

Les deux fonctions utilisent le même timestamp, mais produisent des
résultats différents selon le fuseau horaire appliqué.

## 10.3 Variables d'environnement

La fonction *getenv()* permet de récupérer le contenu des variables
d'environnement du système d'exécution.

Elle accepte deux formes d'appel.

### Récupération d'une variable

*valeur = getenv("FOO");*

La fonction retourne la valeur de la variable d'environnement demandée
sous forme de chaîne de caractères (*STRING*).

### Récupération de plusieurs variables

*valeurs = getenv(\["FOO", "BAR"\]);*

La fonction accepte également un tableau de noms de variables et
retourne un tableau de chaînes (*STRINGS*) contenant les valeurs
correspondantes, dans le même ordre que les noms demandés.

### Gestion des variables inexistantes

Si une variable d'environnement demandée n'existe pas, le moteur génère
une erreur.

Dans le cas d'une récupération multiple, la présence d'une seule
variable inexistante suffit à provoquer une erreur.

### Exemple : injection de paramètres externes

La combinaison de *getenv()* et de *strtod()* permet de transmettre
facilement des paramètres numériques au programme depuis l'environnement
d'exécution.

*angles = strtod(getenv(\["angle1", "angle2", "angle3"\]));*

Les valeurs récupérées sous forme de chaînes sont converties en nombres
et stockées dans un tableau numérique.

Cette technique permet notamment de modifier les paramètres d'un dessin
sans intervenir directement dans son code source.

# 11. Consultation, utilitaires et configuration système

Ce chapitre regroupe les fonctions permettant de consulter certains
paramètres du contexte graphique, de manipuler des couleurs prédéfinies,
de transmettre des informations au contexte CairoDSL et de connaître les
valeurs initiales de l'environnement de dessin.

## 11.1. Consultation des paramètres graphiques

Plusieurs fonctions permettent de récupérer des informations sur l'état
courant du contexte graphique.

### *getdash()*

Retourne le tableau numérique décrivant le motif de pointillés
actuellement défini.

Lorsque aucun motif de pointillés n'est défini, la fonction retourne
*NONE* (valeur non initialisée).

### *getdashoffset()*

Retourne le décalage courant du motif de pointillés.

Lorsque aucun motif de pointillés n'est défini, la fonction retourne
*0.*

### *getlinewidth()*

Retourne l'épaisseur courante du trait.

### *getoutputsize()*

Retourne un tableau numérique de deux éléments contenant les dimensions
de la surface de sortie :

- élément 0 : largeur ;
- élément 1 : hauteur.

Exemple :

*size = getoutputsize();*

*width = size\[0\];*

*height = size\[1\];*

## 11.2. Couleurs prédéfinies

CairoDSL fournit plusieurs fonctions retournant directement des couleurs
RGB sous forme de tableaux numériques.

|           |                     |
|-----------|---------------------|
| Fonction  | Valeur retournée    |
| *black()* | *\[0, 0, 0\]*       |
| *white()* | *\[255, 255, 255\]* |
| *red()*   | *\[255, 0, 0\]*     |
| *green()* | *\[0, 255, 0\]*     |
| *blue()*  | *\[0, 0, 255\]*     |

Ces fonctions retournent uniquement les trois composantes RGB, sans
composante alpha.

### *getrgb(colorname)*

Retourne les composantes RGB d'une couleur à partir de son nom.

La fonction reconnaît les 148 noms de couleurs CSS/SVG pris en charge
par CairoDSL. La recherche est insensible à la casse.

Le résultat est un tableau numérique de trois éléments *\[r, g, b\]*,
avec des composantes comprises entre 0 et 255.

Si le nom de couleur n'est pas reconnu, une erreur est déclenchée.

Les valeurs *transparent* et *currentColor* ne sont pas prises en
charge.

Exemple :

*mycolor = getrgb("CornflowerBlue");*

*color(mycolor) {*

* rectangle(50, 50, 100, 100);*

*}*

## 11.3. Transmission de messages

### *message(description, expression)*

Permet de transmettre une information à l’utilisateur, pour du debug par
exemple.

La fonction prend deux arguments :

- *description* : une chaîne de caractères décrivant l'information
  transmise ;
- *expression* : une expression évaluée dont la valeur est ajoutée au
  message.

Le second argument peut produire n'importe lequel des cinq types de
valeurs de CairoDSL :

- *NONE*
- *DOUBLE*
- *DOUBLES*
- *STRING*
- *STRINGS*

La fonction ne retourne pas de valeur exploitable.

Exemple :

*message("Valeur de phi", phi());*

*message("Résultat du calcul", phi() \* 42);*

*message("Tableau de valeurs", \[1, 2, 3, 4\]);*

Les messages sont conservés dans le contexte de CairoDSL, avec les
erreurs. Ils sont ensuite transmis par les interfaces selon leur mode de
fonctionnement.

- **Interface en ligne de commande :** les messages et les erreurs sont
  envoyés sur *stderr*, tandis que la sortie graphique est envoyée sur
  *stdout*.
- **Interface CGI :** les messages et les erreurs sont intégrés à la
  réponse XML.

## 11.4. Limitation de sécurité du mode CGI

Le mode CGI ne dispose actuellement d'aucun mécanisme interne de limitation du temps d'exécution des programmes.

En particulier, les boucles infinies ne sont pas détectées. Un programme contenant par exemple :

```cairo
while (1) {
    // ...
}
```

peut donc monopoliser indéfiniment le processus CGI qui l'exécute.

Le lancement simultané de plusieurs programmes de ce type peut entraîner une saturation des ressources du serveur et rendre le service indisponible. Ce comportement peut être exploité pour provoquer un déni de service (*Denial of Service*, DoS).

**En conséquence, le mode CGI ne doit pas être exposé à des utilisateurs non fiables sans protection complémentaire**, notamment sans mécanisme externe de limitation du temps d'exécution et du nombre d'exécutions simultanées.

## 11.5. Valeurs initiales de l'environnement graphique

À l'initialisation du contexte graphique, CairoDSL utilise les valeurs
par défaut suivantes :

|                        |                         |
|------------------------|-------------------------|
| Paramètre              | Valeur initiale         |
| Largeur de la surface  | 640 pixels              |
| Hauteur de la surface  | 480 pixels              |
| Couleur du trait       | Noir *\[0, 0, 0, 255\]* |
| Couleur de remplissage | Noir *\[0, 0, 0, 255\]* |
| Mode de rendu initial  | *stroke*                |
| Épaisseur du trait     | 5 pixels                |

Le moteur de rendu dépend de l'interface utilisée :

- **Avec SDL2 :** le rendu est effectué dans l'environnement SDL.
- **Sans SDL2 :** notamment dans les interfaces CGI, Qt ou autres, le
  format de sortie graphique par défaut est SVG.

Ces valeurs constituent l'état initial du contexte. Elles peuvent
ensuite être modifiées par les instructions et fonctions appropriées de
CairoDSL.
