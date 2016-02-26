/**
 * The core game engine / game state manager, along with an abstraction for
 * building subsystems that link into callbacks and augment its functionality.
 */
#ifndef SOLEMNSKY_SKY_H
#define SOLEMNSKY_SKY_H

#include <map>
#include <memory>
#include "physics.h"
#include "flight.h"
#include "map.h"
#include "delta.h"
#include "telegraph/pack.h"

namespace sky {

typedef unsigned int PID; // ID for elements in the game
const static tg::Pack<PID> pidPack = tg::BytePack<PID>();

/**
 * Subsystem abstraction, representing a module that attaches to Sky events
 * through a simple callback interface.
 *
 * To use a subsystem, we construct a Sky first, and then construct the
 * subsystem with a pointer to the sky. Finally, to tie the subsystem into
 * sky's events, we link it in with Sky::link().
 *
 * > Sky sky({1600, 900});
 * > <Subsystem> someSubsystem(&sky);
 * > sky.link(render);
 */
class Subsystem {
 protected:
  Sky *sky; // pointer to the sky it's attached to

 private:
  friend class Sky;

  // Callbacks are triggered after the respective event occurred.
  virtual void tick(const float delta) { }

  virtual void addPlane(const PID pid, Plane &plane) { }

  virtual void removePlane(const PID pid) { }

 public:
  Subsystem(Sky *sky) : sky(sky) { }

  // a Subsystem's life is never managed from a handle to a Subsystem, but
  // rather from non-virtual subclasses; we don't need a virtual dtor.
};

/**
 * The stuff we need to copy a sky over a connection.
 */
struct SkyInitializer {
  SkyInitializer();
  MapName mapName; // the map to load
  std::map<PID, PlaneInitializer> planes; // planes already in the arena
};

struct SkyInitializerPack: public tg::ClassPack<SkyInitializer> {
  SkyInitializerPack();
};

/**
 * A delta in the Sky. Since it holds essentially all the information
 */
struct SkySnapshot {
  SkySnapshot();

  std::map<PID, PlaneInitializer> addedPlanes;
  std::vector<PID> removedPlanes;
  std::map<PID, PlaneState> planes;
};

struct SkySnapshotPack: public tg::ClassPack<SkySnapshot> {
  SkySnapshotPack();
};

/*
 * A Sky is the basic core of the game state. It is semantically subordinate to
 * an Arena, and exposes a simple interface for clients and servers alike for
 * all kinds of circumstances, from server-side simulation to replaying
 * recordings.
 *
 * Planes are added and removed to a Sky "all willy-nilly", holding
 * persistent player records and such is the task of the Arena.
 */
class Sky {
 private:
  std::vector<Subsystem *> subsystems;

 public:
  Sky(const Map &map);
  Sky(const SkyInitializer &initializer);
  ~Sky();

  const Map map; // physics uses this at construction
  Physics physics;

  /**
   * Linking subsystems to the engine.
   */
  void linkSystem(Subsystem *);

  /**
   * Planes.
   */
  std::map<PID, Plane> planes;
  Plane *getPlane(const PID pid);
  Plane &addPlane(const PID pid,
                  const PlaneTuning &tuning,
                  const sf::Vector2f pos,
                  const float rot);
  void removePlane(const PID pid);
  void fireLaser(const PID pid);

  /**
   * Simulating.
   */
  void tick(float delta); // delta in seconds *everywhere*

  /**
   * Initializers and deltas.
   */
  SkyInitializer captureInitializer();

};

}

#endif //SOLEMNSKY_SKY_H
