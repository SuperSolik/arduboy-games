#pragma once

template <typename T>
struct Point2D {
  T x;
  T y;
};

using PointS16 = Point2D<int8_t>;