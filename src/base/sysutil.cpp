#include <sstream>

#define _USE_MATH_DEFINES // for M_PI

#include <math.h>
#include <iostream>
#include <cmath>
#include "sysutil.h"
#include "util.h"

void appLog(LogType type, const std::string contents) {
  static std::vector<std::string> prefixes =
      {"INFO:   ",
       "NOTICE: ",
       "ERROR:  ",
       "DEBUG:  ",
       "        "};

  std::stringstream stream(contents);
  std::string line;

  bool isFirstLine(true);
  while (getline(stream, line, '\n')) {
    if (isFirstLine) {
      std::cout << prefixes[(int) type] + line + "\n";
      isFirstLine = false;
    } else {
      std::cout << prefixes[4] + line + "\n";
    }
  }
}

inline uint8 mix(const uint8 x1, const uint8 x2, const float degree) {
  const float _x1(x1), _x2(x2);
  return (uint8) (std::round(degree * (_x2 - _x1)) + _x1);
}

float VecMath::length(const sf::Vector2f &vec) {
  return (float) sqrt(vec.x * vec.x + vec.y * vec.y);
}

sf::Vector2f VecMath::fromAngle(const float angle) {
  const float rad = toRad(angle);
  return sf::Vector2f(std::cos(rad), std::sin(rad));
}

float VecMath::angle(const sf::Vector2f &vec) {
  return toDeg(std::atan2(vec.y, vec.x));
}

float toDeg(const float x) {
  constexpr float factor = (float) (180 / M_PI);
  return x * factor;
}

float toRad(const float x) {
  constexpr float factor = (float) (M_PI / 180);
  return x * factor;
}