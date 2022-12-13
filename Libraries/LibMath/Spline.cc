#include <LibMath/Spline.hh>

namespace Uni
{

auto operator<<(std::ostream& out, const Spline& spline) -> std::ostream&
{
  for (const auto& [interval, polynomial] : spline.get_polynomials())
  {
    out << polynomial << '{' << interval.left << " <= x <= " << interval.right
        << "}\n";
  }
  return out;
}

}  // namespace Uni
