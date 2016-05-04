/**
 * The state and logic of Planes.
 */
#pragma once
#include <Box2D/Box2D.h>
#include <bitset>
#include <forward_list>
#include "prop.h"
#include "physics.h"
#include "util/types.h"

namespace sky {

class Sky;

/**
 * Vectors of action to control a SkyPlayer. Passed along with a state,
 * corresponding to whether the action was begun or ended.
 */
enum class Action {
  Thrust,
  Reverse,
  Left,
  Right,
  Primary,
  Secondary,
  Special,
  Suicide,
  MAX
};

std::string showAction(const Action action);
optional<Action> readAction(const std::string &string);

/**
 * The POD static state of a GamePlane.
 */
struct PlaneTuning {
  PlaneTuning(); // constructs with sensible defaults

  template<class Archive>
  void serialize(Archive &ar) {
    ar(energy.thrustDrain, energy.recharge, energy.laserGun);
    ar(stall.maxRotVel, stall.maxVel, stall.thrust, stall.damping);
    ar(flight.maxRotVel, flight.airspeedFactor, flight.throttleInfluence,
       flight.throttleEffect, flight.gravityEffect, flight.afterburnDrive,
       flight.leftoverDamping, flight.threshold, throttleSpeed);
  }

  sf::Vector2f hitbox; // x axis parallel with flight direction
  float maxHealth, throttleSpeed;

  struct Energy {
    Energy();
    // energy mechanics
    float thrustDrain, recharge, laserGun;
  } energy;

  struct Stall {
    Stall();
    // mechanics when stalled
    float maxRotVel, // how quickly we can turn, (deg / s)
        maxVel, // our terminal velocity (px / s)
        thrust, // thrust acceleration (ps / s^2)
        damping; // how quickly we approach our terminal velocity
    float threshold; // the minimum airspeed that we need to enter flight
  } stall;

  struct Flight {
    Flight();
    // mechanics when not stalled
    float maxRotVel = 180, // how quickly we can turn
        airspeedFactor,
        throttleInfluence,
        throttleEffect,
        gravityEffect,
        afterburnDrive,
        leftoverDamping;
    float threshold; // the maximum airspeed that we need to enter stall
  } flight;

};

/**
 * The POD variable game state of a GamePlane, that's necessary to sync over
 * the network.
 */
struct PlaneState {
  PlaneState(); // for packing
  PlaneState(const PlaneTuning &tuning,
             const sf::Vector2f &pos,
             const float rot);

  template<class Archive>
  void serialize(Archive &ar) {
    ar(physical);
    ar(stalled, afterburner, leftoverVel, airspeed, throttle);
    ar(energy, health, primaryCooldown);
  }

  /**
   * State.
   */
  PhysicalState physical;
  bool stalled;
  Clamped afterburner;
  sf::Vector2f leftoverVel;
  Clamped airspeed, throttle;

  Clamped energy, health;
  Cooldown primaryCooldown;

  /**
   * Helper methods.
   */
  float forwardVelocity() const;
  float velocity() const;
  // returns true if energy was drawn
  bool requestDiscreteEnergy(const float reqEnergy);
  // returns the fraction of the requested energy that was drawn
  float requestEnergy(const float reqEnergy);
};

/**
 * The persistent control state of a SkyPlayer; synchronized along with the
 * rest of the SkyPlayer's state.
 */
struct PlaneControls {
 private:
  std::bitset<size_t(Action::MAX)> controls;

 public:
  PlaneControls();

  template<typename Archive>
  void serialize(Archive &ar) {
    bool x;
    for (size_t i = 0; i < size_t(Action::MAX); ++i) {
      x = controls[i];
      ar(x);
      controls[i] = x;
      // hahahah
    }
  }

  void doAction(const Action action, const bool actionState);
  bool getState(const Action action) const;

  template<Action action>
  bool getState() const { return controls[size_t(action)]; }

  Movement rotMovement() const;
};

/**
 * Initializer for SkyPlayer's Networked implementation.
 */
struct SkyPlayerInit {
  SkyPlayerInit();
  SkyPlayerInit(const PlaneControls &controls,
                const PlaneTuning &tuning,
                const PlaneState &state);
  SkyPlayerInit(const PlaneControls &controls);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(spawn, controls);
  }

  optional<std::pair<PlaneTuning, PlaneState>> spawn;
  PlaneControls controls;
};

/**
 * Delta for SkyPlayer's Networked implementation.
 */
struct SkyPlayerDelta: public VerifyStructure {
  SkyPlayerDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(tuning, state, controls);
  }

  bool verifyStructure() const;

  optional<PlaneTuning> tuning; // if the plane spawned
  optional<PlaneState> state; // if the plane is alive
  optional<PlaneControls> controls; // if the controls changed
};

/**
 * The plane element that can be associated with a participation.
 */
class Plane {
 private:
  // Parameters.
  Sky &parent;
  Physics &physics;
  const PlaneControls &controls;

  // State.
  PlaneTuning tuning;
  PlaneState state;
  b2Body *const body;

  // Subroutines.
  void switchStall();
  void tickFlight(const float delta);
  void tickWeapons(const float delta);
  void writeToBody();
  void readFromBody();

  // Sky API.
  void prePhysics();
  void postPhysics(const float delta);
  void onBeginContact(const BodyTag &body);
  void onEndContact(const BodyTag &body);

 public:
  Plane() = delete;
  Plane(Sky &, PlaneControls &&, const PlaneTuning &,
        const PlaneState &) = delete; // `controls` must not be a temp
  Plane(Sky &parent,
        const PlaneControls &controls,
        const PlaneTuning &tuning,
        const PlaneState &state);
  ~Plane();

};

/**
 * A Player's participation in an active game.
 */
class Participation {
  friend class Sky;
  friend class SkyPlayer;
 private:
  // Parameters.
  Sky &parent;
  Physics &physics;
  const PlaneControls &controls;

  // State.


  // Sky API.
  void prePhysics();
  void postPhysics(const float delta);

 public:
  Participation() = delete;
  Participation(Sky &parent);

  Participation(const Participation &) = delete;
  Participation &operator=(const Participation &) = delete;

  // User API.
  const PlaneTuning &getTuning() const;
  const PlaneState &getState() const;

};

/**
 * Potential game state attached to a player: wraps optional<Participation>
 * and manages deltas.
 */
class SkyPlayer: private Networked<SkyPlayerInit, SkyPlayerDelta> {
  friend class Sky;
 private:
  // Parameters.
  Sky &parent;
  class Player &player; // associated player

  // State.
  bool newlyAlive; // for deltas
  optional<Participation> vital;
  PlaneControls controls;
  std::forward_list<Prop> props;

  // Internal helpers.
  void spawnWithState(const PlaneTuning &tuning, const PlaneState &state);

  // Sky API.
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f pos,
             const float rot);
  void doAction(const Action action, bool state);
  void reset();

  void applyDelta(const SkyPlayerDelta &delta) override;
  SkyPlayerInit captureInitializer() const;
  SkyPlayerDelta captureDelta();

 public:
  SkyPlayer() = delete;
  SkyPlayer(Sky &parent, class Player &player,
            const SkyPlayerInit &initializer);

  // User API.
  const optional<Participation> &getVital() const;
  const std::forward_list<Prop> &getProps() const;
  const PlaneControls &getControls() const;

  bool isSpawned() const;

};

}
