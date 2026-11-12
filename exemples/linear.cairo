a=newlinear(0, 0, 200, 200);
addstop(a, 0.0, 255, 0, 0, 255);
addstop(a, 1.0, 0, 0, 255, 255);
addstop(0.0, 1, 0, 0, 255);
usepattern(a);
paint();


b=newlinear(0, 0, 200, 200)
{
	addstop(0.0, 1, 0, 0, 255);
	addstop(1.0, 0, 0, 255, 255);
	addstop([0.5, 0, 255, 0, 85]);
}

usepattern(b);
paint();
