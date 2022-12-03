#include "Polynomial.hh"

namespace Uni
{

Polynomial operator*(double number, const Polynomial& polynomial)
{
  Polynomial result = polynomial;
  return result *= number;
}

Polynomial operator*(Polynomial lhs, const Polynomial& rhs)
{
  return lhs *= rhs;
}

Polynomial operator+(Polynomial lhs, const Polynomial& rhs)
{
  return lhs += rhs;
}

bool operator==(const Polynomial& lhs, const Polynomial& rhs)
{
  if (lhs.degree() != rhs.degree())
    return false;

  for (std::size_t i = 0; i < lhs.degree(); ++i)
  {
    if (lhs.coeff(i) != rhs.coeff(i))  // FIXME: change to approximate equality
    {
      return false;
    }
  }

  return true;
}

std::ostream& operator<<(std::ostream& out, const Polynomial& polynomial)
{
  for (std::size_t i = 0; i <= polynomial.degree(); ++i)
  {
    auto coefficient = polynomial.at(i);
    if (i != 0)
    {
      if (coefficient < 0)
        out << "- ";
      else
        out << "+ ";
      out << std::abs(coefficient);
    }
    else
    {
      out << coefficient;
    }

    if (i != 0)
      out << " x^{" << i << '}';
    out << ' ';
  }
  return out;
}

}  // namespace Uni
