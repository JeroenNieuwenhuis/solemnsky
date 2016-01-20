#include "value.h"
#include <cmath>
#include "sysutil.h"

/**
 * Cooldown.
 */
void Cooldown::reset() { cooldown = period; }

void Cooldown::prime() { cooldown = 0; }

bool Cooldown::cool(const float delta) {
  cooldown = std::max(0.0f, cooldown - delta);
  return cooldown == 0;
}

Cooldown::Cooldown(const float period) : period(period), cooldown(period) { }

/**
 * Clamped.
 */
Clamped::Clamped(const float min, const float max) :
    min(min), max(max), value(min) { }

Clamped::Clamped(const float min, const float max, const float value) :
    min(min), max(max), value(clamp(min, max, value)) { }

Clamped &Clamped::operator=(const float x) {
  value = clamp(min, max, x);
  return *this;
}

Clamped &Clamped::operator+=(const float x) {
  value = clamp(min, max, value + x);
  return *this;
}

Clamped &Clamped::operator-=(const float x) {
  value = clamp(min, max, value - x);
  return *this;
}

/**
 * Cyclic: this code could be combined with Clamped if I were using value
 * templates, which I'm not, because it would make me use more value
 * templates in other places in the code, which would be bad because ...
 * well, I'm still working on this rationalization.
 */
Cyclic::Cyclic(const float min, const float max) :
    min(min), max(max), value(min) { }

Cyclic::Cyclic(const float min, const float max, const float value) :
    min(min), max(max), value(cyclicClamp(min, max, value)) { }

Cyclic &Cyclic::operator=(const float x) {
  value = cyclicClamp(min, max, x);
  return *this;
}

Cyclic &Cyclic::operator+=(const float x) {
  value = cyclicClamp(min, max, value + x);
  return *this;
}

Cyclic &Cyclic::operator-=(const float x) {
  value = cyclicClamp(min, max, value - x);
  return *this;
}

bool cyclicApproach(Cyclic &x, const float target, const float amount) {
  const float distance = cyclicDistance(x, target);
  x += sign(distance) * std::min(amount, std::abs(distance));
  return amount < distance;
}

float cyclicDistance(const Cyclic x, const float y) {
  const float range = x.max - x.min;
  const float diffUp = Cyclic(0, x.max - x.min, y - x);
  return (diffUp < range / 2) ? diffUp : diffUp - range;
}

/**
 * Switched.
 */
Switched &Switched::operator=(const float x) {
  for (float possible : states)
    if (x == possible) {
      value = x;
      return *this;
    }

  appLog(LogType::Error, "Bad assignment to Switched value!");
  assert(false); // no need for exceptions, this should be
  // 'practically a compile-time error'
}

/**
 * Angle
 */
Angle::Angle(const float x) : value(cyclicClamp<float>(0, 360, x)) { }

Angle &Angle::operator=(const float x) {
  value = cyclicClamp<float>(0, 360, x);
  return *this;
}

Angle &Angle::operator+=(const float x) {
  value = cyclicClamp<float>(0, 360, x + value);
  return *this;
}

Angle &Angle::operator-=(const float x) {
  value = cyclicClamp<float>(0, 360, x - value);
  return *this;
}

float linearTween(const float begin, const float end, const float time) {
  return begin + time * (end - begin);
}