function Y = fundmat(P)
  pkg load symbolic;
  SHOULD_TEST = 1;
  SHOULD_SIMPLIFY = 1;

  syms t real;

  N = length(P);
  P = sym(P);
  b = sym('b', [N 1]);

  fprintf(" -> P:\n");
  disp(P);
  eig_vec = eig(P);
  eigenvalues = unique(eig_vec).';
  fprintf("\n -> eigenvalues:\n");
  disp(eigenvalues);

  A = sym('A', size(P));
  e = sym('e', size(b));

  ignored_eigenvalues = [];
  index = 1;
  for i = 1:numel(eigenvalues)
    eigenvalue = eigenvalues(i);
    fprintf("\n -> processing eigenvalue: ");
    disp(eigenvalue);
    if ~isempty(find(ignored_eigenvalues==eigenvalue, 1))
      fprintf("   -> ignoring\n");
      continue
    end

    algmult = length(find(eig_vec==eigenvalue));
    fprintf("   -> algebraic multiplicity: %d\n", algmult);
    if imag(eigenvalue) == 0
      for row = 1:algmult
        e(index) = t^(row - 1) * exp(eigenvalue * t);
        for col = 1:N
          A(index, col) = getCoeff(row, col, eigenvalue);
        end

        index = index + 1;
      end
    else
      syms z;
      lhs = exp(z * t);
      rhs = sym('rhs', [1 N]);

      for row = 1:algmult
        for col = 1:N
          rhs(col) = getCoeff(row, col, eigenvalue);
        end

        lhs_sub = subs(lhs, z, eigenvalue);
        e(index) = real(lhs_sub);
        A(index, 1:N) = real(rhs);
        index = index + 1;

        e(index) = imag(lhs_sub);
        A(index, 1:N) = imag(rhs);
        index = index + 1;

        lhs = diff(lhs, z);
      end

      ignored_eigenvalues = [ignored_eigenvalues; conj(eigenvalue)];
      fprintf("   -> ignored_eigenvalues updated:\n");
      disp(ignored_eigenvalues);
    end

  end
  fprintf("\n -> calculating b...");
  b = A \ e;
  fprintf("\n -> b:\n");
  disp(b);
  if SHOULD_SIMPLIFY
    fprintf(" -> simplifying b...");
    b = simplify(b);
    fprintf("\n -> b:\n");
    disp(b);
  end
  Y = zeros(size(P));
  Y = sym(Y);

  fprintf("\n -> calculating Y...");
  for k = 0:N-1
    Y = Y + b(k+1) * P^k;
  end
  fprintf("\n -> Y:\n");
  disp(Y);
  if SHOULD_SIMPLIFY
    fprintf(" -> simplifying Y...");
    Y = simplify(Y);
    fprintf("\n -> Y:\n");
    disp(Y);
  end
  if SHOULD_TEST
    fprintf("\n--- TEST ---\n");
    fprintf(" 1. Y' = PY\n")
    fprintf("    Y' = PY: %s\n", mat2str(areIdentical(diff(Y), P * Y)))
    fprintf(" 1. Y(0) = E\n")
    fprintf("    Y(0) = E: %s\n", mat2str(areIdentical(subs(Y, 0), eye(N))))
  end
end
