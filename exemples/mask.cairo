// Source : dégradé linéaire
a = newlinear(0, 0, 200, 0);

addstop(a, 0.0, 255, 0, 0, 255);
addstop(a, 1.0, 0, 0, 255, 255);

// Masque : dégradé radial
m = newradial(100, 100, 10, 100, 100, 100);

addstop(m, 0.0, 0, 0, 0, 255);
addstop(m, 1.0, 0, 0, 0, 0);

// Application
usepattern(a);
mask(m);

// Le mask se sert du canal alpha pour masker plus ou moins le dessin. Ici transparence progressive du centre vers l'exterieur.
