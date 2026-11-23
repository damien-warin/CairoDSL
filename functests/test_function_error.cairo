#setoutputsize(50, 50);
function foo(a)
{
	return a*2;
}


function bar(a, b)
{
	c=foo(a);
	return c+b;
}

message("foo", foo(1));
message("bar", bar(2, 3));
