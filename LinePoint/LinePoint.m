
clear;
syms x y x0 y0 A B C r;
mans = solve([(x0-x)^2+(y0-y)^2==r^2,r^2*(A^2+B^2)==A*x+B*y+C] ,[x,y] );

