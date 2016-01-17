/**
 * A Sky is the basic core of the game state, with a simple interface for
 * joining / quitting players, spawning and killing, etc. It is shared by
 * client and server. Non-universal subsystems such as graphics and delta
 * compression can be attached to a Sky with the subsystem framework.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include "physics.h"
#include "flight.h"
#include <memory>
#include "subsystem.h"

namespace sky {

class Sky {
private:
  std::vector<Subsystem *> subsystems;

public:
  Sky(const sf::Vector2f &dims);
  ~Sky();

  Physics physics;

  /**
   * Linking subsystems to the engine.
   */
  void linkSystem(Subsystem *);

  /**
   * Planes.
   */
  std::map<PID, std::unique_ptr<Plane>> planes;
  Plane *joinPlane(const PID pid, const PlaneTuning tuning);
  Plane *getPlane(const PID pid);
  void quitPlane(const PID pid);
  void spawnPlane(const PID pid, const sf::Vector2f pos, const float rot,
                  const PlaneTuning &tuning);
  void killPlane(const PID pid);

  /**
   * Laser guns: soon.
   */

  /**
   * Simulating.
   */
  void tick(float delta); // delta in seconds *everywhere*
};
}

#endif //SOLEMNSKY_SKY_H
