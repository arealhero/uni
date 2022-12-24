pkg load symbolic;

clear;
clc;

EXAMPLE = 2;

switch EXAMPLE
  case 1
                                % Пример 3.1
    P = [0 1 0 0; 0 0 0 1; 1 0 0 0; 0 0 1 0];
    Q = [0 0 0 1]';
    f = [0 0 0 0]';

    m = 2;
    x_0 = [0 0 0 0]';
    x_m = [0 1 0 -1]';
  case 2
                                % Пример 3.2
    P = [0 1 0 0; 0 0 1 0; 0 0 0 1; 1 0 0 0];
    Q = [1 0; 0 1; 1 0; 0 1];
    f = [0 0 0 0]';

    m = 2;
    x_0 = [0 0 0 0]';
    x_m = [1 1 1 1]';
  case 3
                                % Пример 3.3
    P = [0 1 0 0; 0 0 1 0; 0 0 0 1; 1 0 0 0];
    Q = [1 0; 1 1; 1 0; 0 1];
    f = [0 0 0 0]';

    m = 2;
    x_0 = [0 1 1 0]';
    x_m = [1 0 0 1]';
  otherwise
    fprintf("Пример #%d не найден\n", EXAMPLE);
    return;
end

P
Q
f
m
x_0
x_m

n = size(x_0, 1)
r = size(Q, 2)

S = Q;
m_dash = min(m, n);
for i = 1:m_dash-1
  S = [S P^i*Q];
end
S

rank_S = rank(S);
isFullyControllable = rank(S) == n;
if isFullyControllable
  fprintf("rank S (%d) = n (%d) => система полностью управляема.\n", rank_S, n)
else
  fprintf("rank S (%d) < n (%d) => система не полностью управляема.\n", rank_S, n);
end

A = Q;
for i = 1:m-1
  A = [P^i*Q A];
end
A

factor = 1;
for i = 0:m-1
  factor = factor * subs(P, i);
end
eta = x_m - factor * x_0;
for i = 1:m
  factor = 1;
  for j = i:m-1
    factor = factor * subs(P, j);
  end
  eta = eta - factor * subs(f, i-1);
end
eta

rank_A = rank(A);

if rank_A ~= rank([A eta])
  fprintf("rank A (%d) != rank [A eta] (%d) => система несовместна\n", rank_A, rank([A eta]));
  return;
elseif rank_A ~= m*r
  fprintf("rank A (%d) != m*r (%d) => система неопределена\n", rank_A, m*r);
  numberOfBasisVectors = m*r - rank_A;
  fprintf(" Найдём ФСР (количество векторов: %d)\n", numberOfBasisVectors);
  solutions = null(sym(A));
  disp(solutions)
  syms c [numberOfBasisVectors 1];
  [result, R] = linsolve(A, eta);
  result = solutions * c + result;
else
  fprintf("Система совместна и определена\n");
  result = A \ eta;
end

fprintf("Решение:\n");
disp(result);

if all(result == 0)
  fprintf("Система не требует управления.\n");
else
  for i = 1:m
    fprintf("u(%d):\n", i-1);
    disp(result(i:i+r-1));
  end
end
