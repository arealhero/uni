clc;
clear;

pkg load symbolic;

syms t real;

## P = [1 1; 0 1];
## Q = [2; 0];
## f = [0; 0];
##
## T = 1;
## x_0 = [0; 0];
## x_T = [1; 1];

## P = [2 1; -1 2];
## Q = [exp(2*t); 0];
## f = [0; exp(2*t)*sin(t)];
##
## T = 2*pi;
## x_0 = [1; 0];
## x_T = [0; 0];

P = [1 1; 0 1];
Q = [0; 1];
f = [0; 0];

T = 2;
x_0 = [0; 0];
x_T = [100; -100];

## P = [0 1; 0 0];
## Q = [t 0; 0 1];
## f = [0; 0];
##
## T = 5;
## x_0 = [1; 1];
## x_T = [0; 1];

N = length(f);
Y = fundmat(P);
fprintf("\n\n");
Y_inv = Y^-1;

B = Y_inv * Q;
fprintf(" -> B:\n");
disp(B);
B_t = B.';

A = int(B * B_t, 0, T);
fprintf(" -> A:\n");
disp(A);

fprintf(" -> det(A):\n");
det_A = det(A);
disp(det_A);

eta = subs(Y_inv, T) * x_T - x_0 - int(Y_inv * f, 0, T);
fprintf(" -> eta:\n");
disp(eta);

if ~areIdentical(det_A, 0)
  fprintf("det(A) != 0 => система полностью управляема\n\n");
else
  rank_A = rank(A);
  rank_A_eta = rank([A eta]);

  fprintf(" -> rank A:\n");
  disp(rank_A);
  fprintf(" -> rank [A eta]:\n");
  disp(rank_A_eta);

  if ~areIdentical(rank_A, rank_A_eta)
    error("rank A != rank [A eta] => пара точек {x_0, x_T} неуправляема.");
  else
    fprintf("rank A = rank [A eta] => пара точек управляема.\n\n");
  end
end

C = A \ eta;
fprintf(" -> C:\n");
disp(C);

u = B_t * C;
u = simplify(u)

syms x(t) [N 1];
sode = diff(x) == P * x + Q * u + f;
conds = [subs(x, 't', 0) == x_0; subs(x, 't', T) == x_T];
solution = dsolve(sode, conds);

if N > 1
  fprintf(" -> упрощаем x1...\n");
  solution.x1 = simplify(solution.x1);
  fprintf(" -> упрощаем x2...\n");
  solution.x2 = simplify(solution.x2);
elseif N > 0
  fprintf(" -> упрощаем x1...\n");
  solution.x1 = simplify(solution.x1);
end
solution

fprintf("\n-— ПРОВЕРКА —-\n");
fprintf(" 1. \n");
fprintf(" x_0:\n");
disp(x_0);
fprintf(" solution(0):\n");
disp(subs(solution, 't', 0));
fprintf(" 2. \n");
fprintf(" x_T:\n");
disp(x_T);
fprintf(" solution(T):\n");
disp(subs(solution, 't', T));

if N == 2
  fplot3(solution.x1, solution.x2, t, [0 T]);
  hold on;
  scatter3(x_0(1), x_0(2), 0);
  scatter3(x_T(1), x_T(2), T);
  grid on;
  ax = gca;
  chart = ax.Children(2);
  datatip(chart,x_0(1),x_0(2),0);
  chart = ax.Children(1);
  datatip(chart,x_T(1),x_T(2),T);
end
