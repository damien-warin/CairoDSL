module rotate_around(center, angle)
{
        translate(center)rotate(angle)translate(-center)
        {
                children();
        }
}


a=newradial(300, 300, 10, 300, 300, 300);
addstop(a, 0.0, 255, 0, 0, 255);
addstop(a, 1.0, 0, 0, 255, 255);
addstop(a, 0.5, 0, 255, 0, 255);
usepattern(a);
paint();

b=newlinear(0, 0, 100, 100);
addstop(b, 0.0, 255, 0, 0, 255);
addstop(b, 0.5, 0, 255, 0, 127);
addstop(b, 1.0, 0, 0, 255, 255);



usepattern(b);
rectangle(0, 0, 100, 100);
fill();

translate(100, 70)rotate_around([50, 50], pi()/4)
{
usepattern(b);
rectangle(0, 0, 100, 100);
fill();
}

translate(200, 0)rotate_around([50, 50], pi()/2)
{
usepattern(b);
rectangle(0, 0, 100, 100);
fill();
}

translate(300, 70)rotate_around([50, 50], 3*pi()/4)
{
usepattern(b);
rectangle(0, 0, 100, 100);
fill();
}
