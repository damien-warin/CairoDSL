#setoutputsize(640, 480);

// Importation des dessins
leaf = import("leaf.svg");
star = import("star.svg");
drop = import("drop.svg");

// Tableau des identifiants
symbols = leaf;
symbols = symbols << star;
symbols = symbols << drop;

// Paramètres de la spirale
cx = 320;
cy = 240;
count = 24;

for (i = [0:count-1]) {

    // Angle de rotation
    angle = i * 0.52;

    // Rayon progressif
    radius = 12 + i * 6;

    // Position du motif
    x = cx + radius * cos(angle);
    y = cy + radius * sin(angle);

    // Sélection du dessin
    index = i - floor(i / 3) * 3;

    // Placement et orientation
		sc = [0.65, 0.65] * (i*0.1+1);
    translate(x, y) {
        rotate(angle + pi() / 2) {
            scale(sc) {
                drawimport(symbols[index]);
            }
        }
    }
}
