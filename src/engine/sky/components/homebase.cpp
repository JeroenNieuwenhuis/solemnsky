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
#include "homebase.hpp"

namespace sky {

/**
 * HomeBaseState.
 */

HomeBaseState::HomeBaseState(const sf::Vector2f &dimensions,
                             const sf::Vector2f &pos,
                             const Angle rot,
                             const float damage,
                             const SwitchSet<Team> friendly) :
    dimensions(dimensions),
    pos(pos),
    rot(rot),
    damage(damage),
    friendly(friendly) { }

/**
 * HomeBase.
 */

void HomeBase::prePhysics() {

}

void HomeBase::postPhysics(const TimeDiff delta) {

}

HomeBase::HomeBase(const HomeBaseState &state, Physics &physics) :
    Component(state, physics) { }

void HomeBase::applyDelta(const HomeBaseDelta &delta) {

}

HomeBaseState HomeBase::captureInitializer() const {
  return state;
}

optional<HomeBaseDelta> HomeBase::collectDelta() {
  return {};
}

}
