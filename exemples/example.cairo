#setoutputsize(800, 600);
#setpngoutput();
#initmessage("Génération de l'image de démonstration", 1);

// 1. Définition des couleurs de fond et de base
setbgcolor(30, 30, 40, 255);

// 2. Création d'un Pattern (Dégradé Radial pour le soleil/fond)
bg_gradient = newradial(400, 300, 50, 400, 300, 450) {
addstop(0.0, 255, 200, 100, 255);
addstop(0.5, 200, 80, 120, 200);
addstop(1.0, 20, 20, 40, 0);
}

// Dessin de l'arrière-plan avec le motif
usepattern(bg_gradient);
rectangle(0, 0, 800, 600);
fill();

// 3. Définition d'un Module Réutilisable pour dessiner une étoile / fleur géométrique
module FleurSpatiale(cx,cy, petales,rayon) {
angle_step = (2 * pi()) /petales;
// CanvaBloc avec sauvegarde d'état et transformation
translate(cx,cy) {
    for (i = [0 :petales - 1]) {
        rotate(angle_step * i) {
            // Modification dynamique de la couleur et de la ligne
            color(255 - (i * 15), 100 + (i * 10), 220, 180);
            setlinewidth(2);
            setdash([8, 4], 0);

            // Tracé d'une pétale avec courbe de Bézier
            moveto(0, 0);
            curve(rayon * 0.5, -rayon * 0.3,rayon, 0, rayon,rayon * 0.2);
            lineto(0, 0);
            stroke();
        }
    }

    // Appel de la structure enfant si elle existe
    children();
}


}

// 4. Utilisation des Groupes et Masques Alpha
etoile_group = group() {
color(255, 255, 255, 255);
rectangle(-50, -50, 100, 100);
fill();
}

// 5. Instanciation du module avec un bloc enfant (children)
FleurSpatiale(400, 300, 12, 180) {
// Ce bloc est exécuté par children() au centre de la fleur
color(255, 215, 0, 255);
arc(0, 0, 25, 0, 2 * pi());
fillpreserve();

settextcolor(40, 40, 40, 255);
setfontsize(14);
setfont("Sans", "Normal", "Bold");

// Recalage au centre pour le texte
metrics = gettextextents("Cairo");
// metrics[2] contient width, metrics[3] height
moveto(-metrics[2] / 2,metrics[3] / 4);
text("Cairo");


}

// 6. Boucle decorative et transformations sur le bord inférieur
setlinewidth(4);
setautostroke();
setautostrokecolor(100, 200, 255, 200);

for (x = [50 : 750 : 100]) {
save();
translate(x, 520);
scale(0.6, 0.6);

// Dessin d'une forme avec polyline
polyline([0, 0, 20, -40, 40, 0, 60, -20]);
restore();


}

// Message de fin
message("Rendu terminé avec succès !", gettimeinsecs());
