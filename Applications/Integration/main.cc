#include <LibGUI/Plot.hh>
#include <LibMath/Integration.hh>
#include <LibMath/Math.hh>
#include <LibMath/Point.hh>
#include <VS/Types.hh>
#include <iostream>
#include <cmath>

#include <string>
#include <utility>
#include <vector>

#define DEBUG_PRINT

constexpr f64 f(f64 x)
{
  // FIXME: погрешности оставляют желать лучшего...
  /* constexpr f64 eps = 1e-15; */
  /* return 2 * cos(2.5 * x, eps) * exp(x / 3.0, eps) + */
  /*        4 * sin(3.5 * x, eps) * exp(-3.0 * x, eps) + x; */
  return 2 * std::cos(2.5 * x) * std::exp(x / 3.0) +
         4 * std::sin(3.5 * x) * std::exp(-3.0 * x) + x;
}

constexpr f64 absolute_value = 7.258L;

int main()
{
  using namespace LibMath::Numerical::Integration;

  const f64 a = 1.5;
  const f64 b = 3.3;

  const std::vector<std::pair<Method, std::string>> methods = {
      {middle_rectangle, "Middle rectangle"},
      {left_rectangle, "Left rectangle"},
      {trapezoid, "Trapezoid"},
      {simpson, "Simpson"},
  };

  Uni::Gui::plot_init();

  for (auto&& [method, name] : methods)
  {
    std::vector<Uni::Point<f64>> errors;

#ifdef DEBUG_PRINT
    std::cout << name << '\n';
#endif
    for (u32 partitions = 100; partitions <= 5000; partitions += 100)
    {
      const auto result = uniform_partition(f, a, b, partitions, method);
#ifdef DEBUG_PRINT
      std::cout << partitions << ": " << result << '\n';
#endif
      errors.emplace_back(partitions, std::abs(absolute_value - result));
    }
#ifdef DEBUG_PRINT
    std::cout << "\n --- Errors --- \n";
    for (auto&& [partitions, error] : errors)
    {
      std::cout << partitions << ": " << error << '\n';
    }
    std::cout << '\n';
#endif

    Uni::Gui::named_plot(name, errors);
  }

  Uni::Gui::legend();
  Uni::Gui::show_plot();

  return 0;
}
