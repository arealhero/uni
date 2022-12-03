#pragma once

#include <LibInterpolation/Interpolator.hh>
#include <LibMath/Spline.hh>

namespace Uni
{

class SplineInterpolator : public Interpolator<Spline>
{
};

}  // namespace Uni
