pkg load symbolic;

clear;
clc;

EXAMPLE = 9;

switch EXAMPLE
  case 1
                                % Пример 12.1
    P = [1 -1 0; 0 1 -1; 2 1 1];
    Q = [0 0 1]';
  case 2
                                % Пример 12.2
    P = [1 -2 2; -1 2 -1; 1 1 0];
    Q = [0 0 1]';
  case 3
                                % Пример 12.3
    P = [2 1 0; 1 -1 1; 0 1 1];
    Q = [1 0 0]';
  case 4
                                % Пример 14.1
    P = [1 -2 0; 0 -1 0; 1 1 2];
    Q = [1 0 0]';
  case 5
                                % Пример 14.2
    P = [1 -2 0; 0 -1 0; 1 1 2];
    Q = [0 0 1]';
  case 6
                                % Пример 14.3
    P = [-1 0 0 1; 0 -1 1 -1; 1 1 0 0; -1 0 0 0];
    Q = [0 0 1 0]';
  case 7
                                % Вариант 1, задача 1
    P = [-1 0 1 0; 1 0 0 2; 1 0 0 0; 0 -1 0 0];
    Q = [0 0 1 0]';
  case 8
                                % Вариант 1, задача 2
    P = [1 0 0; 1 -2 0; -1 0 2];
    Q = [0 1; 1 1; 1 0];
  case 9
                                % Вариант 1, задача 3
    P = [-1 0 0 0; 0 -1 1 -1; 1 1 0 0; -1 0 0 1];
    Q = [0 0 1 1]';
  otherwise
    fprintf("Пример %d не найден\n", EXAMPLE);
    return;
end

n = length(Q);

syms t real;
x = sym('x', [n 1]);
% syms x(t) [n 1];

S = Q;
for i = 1:n-1
  S = [S P^i*Q];
end
S

m = rank(S)

isFullyControllable = m == n;

if isFullyControllable
  fprintf("rank S = n => система полностью управляема\n");
else
  fprintf("rank S < n => система не полностью управляема\n");
end

if isFullyControllable
  T = S;
else
  [r, basisIndices] = rref(S);
  T = S(:,basisIndices);
  canon = eye(n);
  for i = 1:n
    v = canon(:,i);
    if rank(T) < rank([T v])
      T = [T v];
    end
  end
end
T

T_inv = T^-1;

P_tilde = T_inv * P * T
Q_tilde = T_inv * Q

P11 = P_tilde(1:m,1:m)
P22 = P_tilde(m+1:n,m+1:n);

if ~isFullyControllable
  P22
  fprintf("Проверка неуправляемой подсистемы на асимптотическую устойчивость ");
  if ~all(real(eig(P22)) < 0)
    fprintf("не пройдена => система нестабилизируема.\n");
    fprintf("");
    return;
  end
  fprintf("пройдена.\n");
end

K_1 = sym(eye(size(P11)));
p = charpoly(sym(P11))
for i = 1:m-1
  K_1(i, i+1:end) = p(2:end-i);
end
K_1

K_2 = eye(size(P22));
if ~isFullyControllable
  K_2
end

eigenvalues = 1:m;
for i = 1:m
  while eigenvalues(i) >= 0
    prompt = sprintf("Введите собственное число (%d из %d): ", i, m);
    eigenvalues(i) = input(prompt);
    if eigenvalues(i) >= 0
      fprintf("Ошибка: собственные числа должны быть меньше нуля\n");
    end
  end
end
eigenvalues

psi = charpoly(sym(diag(eigenvalues)))
Gamma = sym(1:n);
for i = 1:m
  Gamma(i) = p(i+1) - psi(i+1);
end
Gamma(m+1:n) = zeros(1, n-m)

K = [K_1 sym(zeros(m, n-m)); sym(zeros(n-m, m)) K_2]

C = Gamma * (T * K)^-1
