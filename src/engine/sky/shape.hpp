/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Definition of an arbitrary geometric shape. Used especially by Physics, the box2d wrapper.
 */
#pragma once
#include "util/types.hpp"

namespace sky {

/**
 * A shape defines the local boundaries of a potentially non-convex zone. It can be
 * broken down into contex polygons via polypartition and turned into a box2d fixture for
 * various applications in the physics engine.
 */
struct Shape: public VerifyStructure {
 private:
  enum class Type {
    Circle,
    Rectangle,
    Polygon
  } type;

  // Circle.
  optional<float> radius;

  // Rectangle.
  optional<sf::Vector2f> dimensions;

  // Polygon.
  std::vector<sf::Vector2f> vertices;

  Shape(const Type type);

 public:
  Shape() = delete;

  static Shape Circle()
  static Shape Rectangle(const sf::Vector2f dimensions);
  static Shape Polygon(const std::vector<sf::Vector2f> vertices);

  // Cereal serialization.
  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Circle: {
        ar(radius);
        break;
      }
      case Type::Rectangle: {
        ar(dimensions);
        break;
      }
      case Type::Polygon: {
        ar(vertices);
        break;
      }
    }
  }

  // VerifyStructure impl.
  virtual bool verifyStructure() const;

};

}

