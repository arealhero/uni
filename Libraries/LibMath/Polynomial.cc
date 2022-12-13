#include "Polynomial.hh"

namespace Uni
{

auto operator*(double number, const Polynomial& polynomial) -> Polynomial
{
  Polynomial result = polynomial;
  return result *= number;
}

auto operator*(Polynomial lhs, const Polynomial& rhs) -> Polynomial
{
  return lhs *= rhs;
}

auto operator+(Polynomial lhs, const Polynomial& rhs) -> Polynomial
{
  return lhs += rhs;
}
auto operator-(Polynomial lhs, const Polynomial& rhs) -> Polynomial
{
  return lhs -= rhs;
}

auto operator==(const Polynomial& lhs, const Polynomial& rhs) -> bool
{
  if (lhs.degree() != rhs.degree())
  {
    return false;
  }

  for (std::size_t i = 0; i < lhs.degree(); ++i)
  {
    if (lhs.coeff(i) != rhs.coeff(i))  // FIXME: change to approximate equality
    {
      return false;
    }
  }

  return true;
}

auto operator<<(std::ostream& out, const Polynomial& polynomial)
    -> std::ostream&
{
  for (std::size_t i = 0; i <= polynomial.degree(); ++i)
  {
    auto coefficient = polynomial.at(i);
    if (i != 0)
    {
      if (coefficient < 0)
      {
        out << "- ";
      }
      else
      {
        out << "+ ";
      }
      out << std::abs(coefficient);
    }
    else
    {
      out << coefficient;
    }

    if (i != 0)
    {
      out << " x^{" << i << '}';
    }
    out << ' ';
  }
  return out;
}

}  // namespace Uni
