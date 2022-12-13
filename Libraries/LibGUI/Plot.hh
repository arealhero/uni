#pragma once

#include <LibMath/Point.hh>
#include <LibMath/Polynomial.hh>
#include <string>
#include <vector>

namespace Uni::Gui
{

void plot_init();

void suptitle(const std::string& title);

void subplot(std::uint32_t rows, std::uint32_t cols, std::uint32_t plot_number);
void legend();

void plot(const std::vector<Point>& points);
void plot(const Polynomial& polynomial,
          double left,
          double right,
          std::size_t detalization = 1000);
void named_plot(const std::string& name,
                const Polynomial& polynomial,
                double left,
                double right,
                std::size_t detalization = 1000);
void show_plot();

}  // namespace Uni::Gui
