function result = areIdentical(lhs, rhs)
  result = isequal(isAlways(lhs == rhs), ones(size(lhs)));
end
