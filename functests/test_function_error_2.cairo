#setoutputsize(50, 50);
function foo(a)
{
	a=4;
}


function bar(a, b)
{
	c=a;
	return c+b;
}

message("foo", foo(1));
message("bar", bar(2, 3));
