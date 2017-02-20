#include <Arduino.h>

#include "Vec2d.h"

template<>
Vec2d<float>::Vec2d() : mX(0.0f), mY(0.0f) {}

template<>
Vec2d<double>::Vec2d() : mX(0.0), mY(0.0) {}