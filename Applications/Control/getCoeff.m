function coeff = getCoeff(row, col, eigenvalue, n)
  if col < row
    coeff = 0;
  else
    coeff = factorial(col - 1) / factorial(col - row) * eigenvalue^(col - row);
  end
end
