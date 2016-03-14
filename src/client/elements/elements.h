/**
 * Abstract base classes for the structures that compose our interface -- pages
 * and games.
 */
#ifndef SOLEMNSKY_PAGE_H
#define SOLEMNSKY_PAGE_H

#include "clientshared.h"
#include "client/ui/widgets/widgets.h"

/**
 * Pages are the portals through which users interface with the tutorial
 * periphery: looking for servers, editing settings, loading tutorial records, etc.
 *
 * We have three pages in our menu interface. They are all persistent, in
 * that they always exist, with their state and tick() operations, whether they
 * are displayed or not.
 *
 * Pages may have multiple tabs: that is delegated to their particular
 * implementation.
 */
class Page: public ui::Control {
 protected:
  ClientShared &shared;

 public:
  Page(ClientShared &shared);
  // a page's lifetime is never handled by a reference to a Page
  // so we need no virtual dtor

  virtual void attachClientState() { } // when client state is initialized
  // (this is necessary because of the Control::attachState() thing)
  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
  virtual void onBlur() = 0;

  /**
   * General graphics.
   */
  void drawBackground(ui::Frame &f);
};

/**
 * An interface where the user plays a game of some sort; a tutorial, a
 * multiplayer match, etc.
 */
class Game: public ui::Control {
 protected:
  ClientShared &shared;

 public:
  Game(ClientShared &shared, const std::string &name);
  virtual ~Game() { }

  virtual void onChangeSettings(const SettingsDelta &settings) = 0;

  // try to exit, set Control::quitting flag eventually
  // (a multiplayer connection may want to do this at its leisure to
  // disconnect gracefully)
  virtual void doExit() = 0;

  std::string description; // description of the game
  std::string status; // brief status of the game
};

#endif //SOLEMNSKY_PAGE_H
