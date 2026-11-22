#setoutputsize(50, 50);
function foo(a)
{
	return a*2;
}


function bar(a, b)
{
	c=3*a;
	return c+b;
}

rectangle(0, 0, foo(10), bar(10, 5));
