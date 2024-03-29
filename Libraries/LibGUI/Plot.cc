#include "matplotlibcpp.h"

#include <LibGUI/Plot.hh>
#include <LibInterpolation/LinearGenerator.hh>

namespace plt = matplotlibcpp;

namespace Uni::Gui
{

void plot_init()
{
  plt::backend("TkAgg");
}

void suptitle(const std::string& title)
{
  plt::suptitle(title);
}

void subplot(const std::uint32_t rows,
             const std::uint32_t cols,
             const std::uint32_t plot_number)
{
  plt::subplot(rows, cols, plot_number);
}

void legend()
{
  plt::legend();
}

void plot(const std::vector<Point<f64>>& points)
{
  const auto size = points.size();
  auto xs = std::vector<double>(size);
  auto ys = std::vector<double>(size);

  for (const auto& [x, y] : points)
  {
    xs.push_back(x);
    ys.push_back(y);
  }

  plt::scatter(xs, ys, 3.0, {{"marker", "x"}, {"color", "black"}});
}

void plot(const Polynomial& polynomial,
          const double left,
          const double right,
          const std::size_t detalization)
{
  auto generator = LinearGenerator<double>(left, right, detalization);

  auto xs = std::vector<double>{};
  auto ys = std::vector<double>{};
  xs.reserve(detalization);
  ys.reserve(detalization);

  for (auto x : generator)
  {
    xs.push_back(x);
    ys.push_back(polynomial(x));
  }

  plt::plot(xs, ys);
}

// FIXME: remove code duplication
void named_plot(const std::string& name,
                const Polynomial& polynomial,
                const double left,
                const double right,
                const std::size_t detalization)
{
  auto generator = LinearGenerator<double>(left, right, detalization);

  auto xs = std::vector<double>{};
  auto ys = std::vector<double>{};
  xs.reserve(detalization);
  ys.reserve(detalization);

  for (auto x : generator)
  {
    xs.push_back(x);
    ys.push_back(polynomial(x));
  }

  plt::named_plot(name, xs, ys);
}

void named_plot(const std::string& name, const std::vector<Point<f64>>& points)
{
  auto xs = std::vector<double>{};
  auto ys = std::vector<double>{};
  xs.reserve(points.size());
  ys.reserve(points.size());

  for (const auto& point : points)
  {
    xs.push_back(point.x);
    ys.push_back(point.y);
  }

  plt::named_plot(name, xs, ys);
}

void named_plot(const std::string& name,
                const std::vector<std::size_t>& xs,
                const std::vector<std::size_t>& ys)
{
  plt::named_plot(name, xs, ys);
}
void named_plot(const std::string& name,
                const std::vector<long double>& xs,
                const std::vector<long double>& ys)
{
  plt::named_plot(name, xs, ys);
}

void show_plot()
{
  plt::show();
}

}  // namespace Uni::Gui
