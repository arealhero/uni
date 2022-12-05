#pragma once

#include <LibMath/Point.hh>
#include <LibMath/Polynomial.hh>
#include <string>
#include <vector>

namespace Uni::Gui
{

void plot_init();

void suptitle(const std::string& title);

void subplot(const std::size_t rows,
             const std::size_t cols,
             const std::size_t plot_number);
void legend();

void plot(const std::vector<Point>& points);
void plot(const Polynomial& polynomial,
          const double left,
          const double right,
          const std::size_t detalization = 1000);
void named_plot(const std::string& name,
                const Polynomial& polynomial,
                const double left,
                const double right,
                const std::size_t detalization = 1000);
void show_plot();

}  // namespace Uni
