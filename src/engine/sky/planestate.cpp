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
#include "planestate.hpp"
#include "util/methods.hpp"

/**
 * Action.
 */

namespace sky {


/**
 * PlaneTuning.
 */

PlaneTuning::PlaneTuning() :
    hitbox(75, 30),
    maxHealth(10),
    throttleSpeed(1.5) { }

PlaneTuning::Energy::Energy() :
    thrustDrain(1),
    recharge(0.5),
    laserGun(0.3),
    primaryRecharge(4) { }

PlaneTuning::Stall::Stall() :
    maxRotVel(200),
    maxVel(300),
    thrust(250),
    damping(0.8),
    threshold(130) { }

PlaneTuning::Flight::Flight() :
    maxRotVel(190),
    airspeedFactor(350),
    throttleInfluence(0.65),
    throttleEffect(0.3),
    throttleBrakeEffect(0.6),
    throttleGlideDamper(0.5),
    gravityEffect(0.6),
    afterburnDrive(0.8),
    leftoverDamping(0.3),
    threshold(100) { }

float *PlaneTuning::accessParamByName(const std::string &name) {
  // TODO: access this using the cereal JSON serialization?
  if (name == "hitbox.x") return &hitbox.x;
  if (name == "hitbox.y") return &hitbox.y;
  if (name == "maxHealth") return &maxHealth;
  if (name == "throttleSpeed") return &throttleSpeed;

  if (name == "energy.thrustDrain") return &energy.thrustDrain;
  if (name == "energy.recharge") return &energy.recharge;
  if (name == "energy.laserGun") return &energy.laserGun;
  if (name == "energy.primaryRecharge") return &energy.primaryRecharge;

  if (name == "stall.maxRotVel") return &stall.maxRotVel;
  if (name == "stall.maxVel") return &stall.maxVel;
  if (name == "stall.thrust") return &stall.thrust;
  if (name == "stall.damping") return &stall.damping;
  if (name == "stall.threshold") return &stall.threshold;

  if (name == "flight.maxRotVel") return &flight.maxRotVel;
  if (name == "flight.airspeedFactor") return &flight.airspeedFactor;
  if (name == "flight.throttleInfluence") return &flight.throttleInfluence;
  if (name == "flight.throttleEffect") return &flight.throttleEffect;
  if (name == "flight.throttleBrakeEffect") return &flight.throttleBrakeEffect;
  if (name == "flight.throttleGlideDamper") return &flight.throttleGlideDamper;
  if (name == "flight.gravityEffect") return &flight.gravityEffect;
  if (name == "flight.afterburnDrive") return &flight.afterburnDrive;
  if (name == "flight.leftoverDamping") return &flight.leftoverDamping;
  if (name == "flight.threshold") return &flight.threshold;

  return nullptr;
}

std::string PlaneTuning::toString() const {
  std::ostringstream stream;
  cereal::JSONOutputArchive ar(stream);
  ar(cereal::make_nvp("tuning", *this));
  return stream.str();
}

/**
 * PlaneStateClient.
 */

PlaneStateClient::PlaneStateClient(const PlaneState &state) :
    physical(state.physical),
    airspeed(state.airspeed),
    throttle(state.throttle),
    stalled(state.stalled) { }

/**
 * PlaneStateServer.
 */

PlaneStateServer::PlaneStateServer(const PlaneState &state) :
    energy(state.energy),
    health(state.health),
    primaryCooldown(state.primaryCooldown) { }

/**
 * PlaneState.
 */

PlaneState::PlaneState() :
    stalled(false),
    afterburner(0),
    throttle(1),
    energy(1),
    health(1) { }

PlaneState::PlaneState(const PlaneTuning &tuning,
                       const sf::Vector2f &pos,
                       const float rot) :
    physical(pos,
             tuning.flight.airspeedFactor * VecMath::fromAngle(rot),
             rot, 0),

    stalled(false),
    airspeed(tuning.flight.throttleInfluence),
    afterburner(0),
    throttle(1),
    leftoverVel(0, 0),

    energy(1),
    health(1) { }

float PlaneState::forwardVelocity() const {
  return velocity() *
      (const float) cos(
          toRad(physical.rot) - std::atan2(physical.vel.y, physical.vel.x));
}

float PlaneState::velocity() const {
  return VecMath::length(physical.vel);
}

void PlaneState::applyClient(const PlaneStateClient &client) {
  physical = client.physical;
  airspeed = client.airspeed;
  throttle = client.throttle;
  stalled = client.stalled;
}

void PlaneState::applyServer(const PlaneStateServer &server) {
  energy = server.energy;
  health = server.health;
  primaryCooldown = server.primaryCooldown;
}

/**
 * PlaneControls.
 */

PlaneControls::PlaneControls() { }

void PlaneControls::doAction(const Action action, const bool actionState) {
  controls.set(action, actionState);
}

bool PlaneControls::getState(const Action action) const {
  return controls.get(action);
}

Movement PlaneControls::rotMovement() const {
  return addMovement(getState<Action::Left>(),
                     getState<Action::Right>());
}

bool operator==(const PlaneControls &x, const PlaneControls &y) {
  return x.controls == y.controls;
}

bool operator!=(const PlaneControls &x, const PlaneControls &y) {
  return !(x == y);
}

}

