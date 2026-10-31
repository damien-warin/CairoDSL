setautofill();

//Create group
pushgroup();

color(blue())rectangle(0, 0, 50, 50);
color(red())arc(50, 50, 30, 0, 2*PI());

//Finish group in var a
a=popgroup();

//Create anoher group wither GROUP instruction
b=group()
{
color(red())rectangle(0, 0, 50, 50);
color(blue())arc(50, 50, 30, 0, 2*PI());
}

translate(45, 45)color(green())rectangle(0, 0, 100, 100);

translate(50, 50)
{
//Recall group a
    usegroup(a);
    paint(0.5);
}

translate(145, 45)color(green())rectangle(0, 0, 100, 100);

translate(150, 50)
{
    usegroup(a);
    paint();
}

translate(45, 45+100)color(green())rectangle(0, 0, 100, 100);

translate(50, 50+100)
{
    usegroup(b);
    paint();
}

translate(145, 45+100)color(green())rectangle(0, 0, 100, 100);

translate(150, 50+100)
{
    usegroup(b);
    paint(0.5);
}
