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
 * Rendering the sky.
 */
#pragma once
#include "util/client/resources.h"
#include <list>
#include "ui/control.h"
#include "ui/sheet.h"
#include "sky/sky.h"
#include <SFML/Graphics.hpp>

namespace sky {

/**
 * Graphics state associated with a Participation.
 */
struct PlaneGraphics {
  PlaneGraphics(const Participation &plane);
  const Participation &participation;

  bool orientation;
  float flipState, rollState; // these two values contribute to the roll
  Angle roll() const;

  void tick(const float delta);
  void kill();
  void spawn();
};

/**
 * Subsystem, attaching additional graphics-related state to Players and
 * exposing top-level methods for rendering the game.
 *
 * Invariant: `sky` remains alive.
 */
class SkyRender: public Subsystem<PlaneGraphics> {
 private:
  // Parameters.
  const SkyManager &skyManager;
  const Sky &sky;

  // State.
  std::map<PID, PlaneGraphics> graphics;
  const ui::SpriteSheet planeSheet;

  // Render submethods.
  float findView(const float viewWidth,
                 const float totalWidth,
                 const float viewTarget) const;
  void renderBars(ui::Frame &f,
                  std::vector<std::pair<float, const sf::Color &>> bars,
                  sf::FloatRect area);
  void renderProps(ui::Frame &f, const Participation &participation);
  void renderPlaneGraphics(ui::Frame &f, const PlaneGraphics &graphics);
  void renderMap(ui::Frame &f);

 protected:
  // Subsystem impl.
  void registerPlayer(Player &player);
  void unregisterPlayer(Player &player);
  void onTick(const float delta) override;

 public:
  SkyRender(Arena &arena,
            const SkyManager &skyManager,
            const Sky &sky);
  ~SkyRender();

  // User API.
  void render(ui::Frame &f, const sf::Vector2f &pos);
  bool enableDebug;
};

}
