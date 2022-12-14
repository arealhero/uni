#pragma once

#include <LibMath/Function.hh>
#include <initializer_list>
#include <ostream>
#include <vector>

namespace Uni
{

class Polynomial : public Function
{
 public:
  Polynomial() = default;
  Polynomial(double number) : m_coeffs(1, number) { shrink_to_fit(); }
  Polynomial(const std::vector<double>& coeffs) : m_coeffs(coeffs)
  {
    shrink_to_fit();
  }
  Polynomial(std::vector<double>&& coeffs) : m_coeffs(std::move(coeffs))
  {
    shrink_to_fit();
  }
  Polynomial(const std::initializer_list<double>& coeffs) : m_coeffs(coeffs)
  {
    shrink_to_fit();
  }
  Polynomial(std::initializer_list<double>&& coeffs) : m_coeffs(coeffs)
  {
    shrink_to_fit();
  }

  [[nodiscard]] auto coeff(std::size_t i) -> double& { return m_coeffs.at(i); }
  [[nodiscard]] auto coeff(std::size_t i) const -> const double&
  {
    return m_coeffs.at(i);
  }

  [[nodiscard]] auto at(std::size_t i) const -> double
  {
    if (i <= degree())
    {
      return m_coeffs[i];
    }

    return 0;
  }

  [[nodiscard]] auto degree() const -> std::size_t
  {
    return m_coeffs.size() - 1;
  }

  [[nodiscard]] auto d() const -> Polynomial
  {
    if (degree() == 0)
    {
      return {0};
    }

    std::vector<double> coeffs(m_coeffs.size() - 1, 0);

    for (std::size_t i = 0; i < coeffs.size(); ++i)
    {
      coeffs[i] = m_coeffs[i + 1] * static_cast<double>(i + 1);
    }

    auto result = Polynomial{coeffs};
    result.shrink_to_fit();
    return result;
  }

  [[nodiscard]] constexpr auto operator()(double x) const -> double override
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

  auto operator+=(const Polynomial& other) -> Polynomial&
  {
    resize_if_needed(other.degree());

    for (std::size_t i = 0; i <= degree(); ++i)
    {
      m_coeffs[i] += other.at(i);
    }

    shrink_to_fit();
    return *this;
  }
  auto operator-=(const Polynomial& other) -> Polynomial&
  {
    resize_if_needed(other.degree());

    for (std::size_t i = 0; i <= degree(); ++i)
    {
      m_coeffs[i] -= other.at(i);
    }

    shrink_to_fit();
    return *this;
  }
  auto operator*=(double number) -> Polynomial&
  {
    for (auto& coeff : m_coeffs)
    {
      coeff *= number;
    }

    shrink_to_fit();
    return *this;
  }
  auto operator*=(const Polynomial& other) -> Polynomial&
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

  void shrink_to_fit(const double error = 1e-9)
  {
    if (degree() == 0)
    {
      return;
    }
    std::size_t i = m_coeffs.size() - 1;
    while (i > 0 && std::abs(m_coeffs[i]) < error)
    {
      m_coeffs.pop_back();
      --i;
    }
  }

 private:
  void resize_if_needed(std::size_t new_degree)
  {
    if (degree() < new_degree)
    {
      resize(new_degree);
    }
  }
  void resize(std::size_t new_degree) { m_coeffs.resize(new_degree + 1); }

  // FIXME: change std::vector to VS::Vector
  std::vector<double> m_coeffs = {0};
};

auto operator*(double number, const Polynomial& polynomial) -> Polynomial;
auto operator*(Polynomial lhs, const Polynomial& rhs) -> Polynomial;

auto operator+(Polynomial lhs, const Polynomial& rhs) -> Polynomial;
auto operator-(Polynomial lhs, const Polynomial& rhs) -> Polynomial;
auto operator==(const Polynomial& lhs, const Polynomial& rhs) -> bool;

auto operator<<(std::ostream& out, const Polynomial& polynomial)
    -> std::ostream&;

}  // namespace Uni
