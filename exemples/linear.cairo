a=newlinear(0, 0, 200, 200);
addstop(a, 0.0, 1, 0, 0, 1);
addstop(a, 1.0, 0, 0, 1, 1);
addstop(0.0, 1, 0, 0, 1);
usepattern(a);
paint();


b=newlinear(0, 0, 200, 200)
{
	addstop(0.0, 1, 0, 0, 1);
	addstop(1.0, 0, 0, 1, 1);
	addstop([0.5, 0, 1, 0, 0.3]);
}

usepattern(b);
paint();