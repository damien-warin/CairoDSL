//a=newradial(100, 100, 50, 100, 100, 150);
//addstop(a, 0.0, 1, 0, 0, 255);
//addstop(a, 1.0, 0, 0, 255, 255);
//usepattern(a);
//paint();


b=newradial(100, 100, 50, 100, 100, 150)
{
addstop(0.0, 1, 0, 0, 255);
addstop(1.0, 0, 0, 255, 255);
addstop([0.5, 0, 255, 0, 83]);
}
usepattern(b);
paint();
