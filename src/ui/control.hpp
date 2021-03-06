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
 * The core of our UI abstraction.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <memory>
#include "util/telegraph.hpp"
#include "frame.hpp"
#include "settings.hpp"
#include "resources.hpp"

namespace ui {

/**
 * Manager for performance profile data collected by the game loop.
 */
struct Profiler {
  Profiler(const unsigned int size);

  RollingSampler<TimeDiff> cycleTime, logicTime, renderTime;
  RollingSampler<size_t> primCount;

};

struct ProfilerSnapshot {
  ProfilerSnapshot() = default; // 'null' defaults, all zeroes
  ProfilerSnapshot(const Profiler &profiler);

  TimeStats cycleTime, logicTime, renderTime;

};

/**
 * All the references passed to a Control; some accessible directly, some
 * through protected Control aliases.
 *
 * The `resources` reference is undefined in SplashScreen; everywhere else
 * it is defined.
 */
struct AppRefs {
  friend class Control;
  AppRefs(Settings &settings,
          const AppResources &resources,
          const Time &time,
          const sf::RenderWindow &window,
          const Profiler &profiler);

  // References accessible from Control member variables.
  Settings &settings;
  const AppResources &resources;

  // References only accessible from this struct.
  const Time &uptime;
  const sf::RenderWindow &window;
  const Profiler &profiler;

  double timeSince(const Time event) const;

};

/**
 * The control abstraction: a tangible GUI entity.
 */
class Control {
 protected:
  // Children
  std::vector<Control *> children;
  void areChildren(std::initializer_list<Control *> controls);

 public:
  Control(const AppRefs &references);
  virtual ~Control() { }

  // Ref to reference struct, and aliases.
  const AppRefs &references;
  const AppResources &resources;
  Settings &settings;

  // Quitting flag.
  bool quitting;

  // Callbacks.
  virtual void poll(); // Called once every update cycle.
  virtual void tick(const TimeDiff delta); // Called in regular intervals.
  virtual void render(Frame &f);
  virtual bool handle(const sf::Event &event);
  virtual void reset();

  // Signal callbacks.
  virtual void signalRead();
  virtual void signalClear();

};

/**
 * Runs a top-level ui::Control, resulting in a full app.
 */
class ControlExec {
 private:
  tg::UsageFlag flag;

  // Settings -- read on startup, written on close, propogated to Controls.
  std::string settingsFile;
  Settings settings;

  // SFML / graphics state.
  sf::RenderWindow window;
  Frame frame;
  Cooldown resizeCooldown;

  // Timing.
  sf::Clock cycleClock;
  Time uptime;
  const TimeDiff tickStep;
  TimeDiff rollingTickTime;

  // Profiling.
  sf::Clock profileClock;
  ui::Profiler profiler;

  // AppRefs and Control.
  AppRefs appState;
  std::unique_ptr<Control> ctrl;

  // App loop submethods.
  void tick();
  void handle();
  void renderAndSleep();

 public:
  ControlExec();
  ~ControlExec();

  void run(std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp);

};

void runSFML(std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp);

}

