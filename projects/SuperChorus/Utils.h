#pragma once

namespace utils
{
 
// Fast log2 for x within [1.0, 2.0]
inline float fast_log2(float x)
{
    float t = x - 1.0f;

    return t * (
        1.3465552f +
    t * (
       -0.3606740f +
        0.01411931f * t));
}

} // namespace utils