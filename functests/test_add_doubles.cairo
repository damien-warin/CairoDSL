a=1;
b=2;
c=[3, 4];
d=[5, 6];

message("simple+simple", a+b);
message("simple+array", a+c);
message("array+simple", c+b);
message("array+array", c+d);
message("simple+array+array+simple", a+c+d+b);
message("simple+array+simple+array", a+c+b+d);
