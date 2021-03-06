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
 * Interface used on server-side to push callbacks into the Sky game logic.
 */
#pragma once
#include "engine/arena.hpp"
#include "plane.hpp"
#include "physics/physics.hpp"

namespace sky {

class SkyListener {
 protected:
  virtual void onCollide(Plane &plane, const BodyTag &body);
  virtual void onEndCollide(Plane &plane, const BodyTag &body);

};

}

