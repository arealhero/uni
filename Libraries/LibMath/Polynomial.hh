#pragma once

#include <initializer_list>
#include <ostream>
#include <vector>

namespace Uni
{

class Polynomial
{
 public:
  Polynomial() = default;
  Polynomial(double number) : m_coeffs(1, number) {}
  Polynomial(const std::vector<double>& coeffs) : m_coeffs(coeffs) {}
  Polynomial(const std::initializer_list<double>& coeffs) : m_coeffs(coeffs) {}
  Polynomial(std::initializer_list<double>&& coeffs)
      : m_coeffs(std::move(coeffs))
  {
  }

  double& coeff(std::size_t i) { return m_coeffs.at(i); }
  const double& coeff(std::size_t i) const { return m_coeffs.at(i); }

  double at(std::size_t i) const
  {
    if (i <= degree())
      return m_coeffs[i];
    else
      return 0;
  }

  std::size_t degree() const { return m_coeffs.size() - 1; }

  Polynomial d() const
  {
    if (degree() == 0)
      return {0};

    std::vector<double> coeffs(m_coeffs.size() - 1, 0);

    for (std::size_t i = 0; i < coeffs.size(); ++i)
    {
      coeffs[i] = m_coeffs[i + 1] * (i + 1);
    }

    auto result = Polynomial{coeffs};
    result.shrink_to_fit();
    return result;
  }

  constexpr double operator()(double x) const
  {
    double result = 0;
    double power = 1;

    for (const auto& coeff : m_coeffs)
    {
      result += coeff * power;
      power *= x;
    }

    return result;
  }

  Polynomial& operator+=(const Polynomial& other)
  {
    resize_if_needed(other.degree());

    for (std::size_t i = 0; i <= degree(); ++i)
    {
      m_coeffs[i] += other.at(i);
    }

    shrink_to_fit();
    return *this;
  }
  Polynomial& operator-=(const Polynomial& other)
  {
    resize_if_needed(other.degree());

    for (std::size_t i = 0; i <= degree(); ++i)
    {
      m_coeffs[i] -= other.at(i);
    }

    shrink_to_fit();
    return *this;
  }
  Polynomial& operator*=(double number)
  {
    for (auto& coeff : m_coeffs)
      coeff *= number;
    return *this;
  }
  Polynomial& operator*=(const Polynomial& other)
  {
    const auto max_degree = degree() + other.degree();
    std::vector<double> coeffs(max_degree + 1, 0.0);

    for (std::size_t i = 0; i <= degree(); ++i)
    {
      for (std::size_t j = 0; j <= other.degree(); ++j)
      {
        coeffs.at(i + j) += at(i) * other.at(j);
      }
    }

    m_coeffs = std::move(coeffs);

    shrink_to_fit();
    return *this;
  }

  void shrink_to_fit()
  {
    if (degree() == 0)
      return;
    std::size_t i = m_coeffs.size() - 1;
    while (i > 0 && m_coeffs[i] == 0)
    {
      m_coeffs.pop_back();
      --i;
    }
  }

 private:
  void resize_if_needed(std::size_t new_degree)
  {
    if (degree() < new_degree)
      resize(new_degree);
  }
  void resize(std::size_t new_degree) { m_coeffs.resize(new_degree + 1); }

  std::vector<double> m_coeffs = {0};
};

Polynomial operator*(double number, const Polynomial& polynomial);
Polynomial operator*(Polynomial lhs, const Polynomial& rhs);

bool operator==(const Polynomial& lhs, const Polynomial& rhs);

std::ostream& operator<<(std::ostream& out, const Polynomial& polynomial);

}  // namespace Uni
