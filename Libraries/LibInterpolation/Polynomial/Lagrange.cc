#include <LibInterpolation/Polynomial/Lagrange.hh>

namespace Uni
{

auto LagrangeInterpolator::operator()(const std::vector<Point<f64>>& points)
    -> Polynomial
{
  Polynomial result;

  for (std::size_t k = 0; k < points.size(); ++k)
  {
    const auto& [x_k, y_k] = points.at(k);

    Polynomial term(1.0);
    for (std::size_t i = 0; i < points.size(); ++i)
    {
      if (i == k)
      {
        continue;
      }

      auto x_i = points.at(i).x;
      term *= Polynomial({-x_i, 1.0});
    }

    const auto factor = y_k / term(x_k);
    result += factor * term;
  }

  return result;
}

}  // namespace Uni
