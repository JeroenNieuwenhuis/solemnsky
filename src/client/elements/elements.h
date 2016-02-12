/**
 * Abstract base classes for the structures that compose our interface -- pages
 * and games.
 */
#ifndef SOLEMNSKY_PAGE_H
#define SOLEMNSKY_PAGE_H

#include "clientshared.h"

/**
 * Pages are the portals through which users interface with the game
 * periphery: looking for servers, editing settings, loading game records, etc.
 *
 * We have three pages in our menu interface. They are all persistent, in
 * that they always exist, with their state and tick() operations, whether they
 * are displayed or not.
 *
 * Pages may have multiple tabs: that is delegated to their particular
 * implementation.
 */
class Page : public ui::Control {
protected:
  ClientShared &shared;

public:
  Page(ClientShared &state) : shared(state) { }

  virtual void attachClientState() { } // when client state is initialized
  // (this is necessary because of the Control::attachState() thing)

  virtual void onLooseFocus() = 0;
  virtual void onFocus() = 0;
};

/**
 * There is exactly one place for a Game -- in the unique_ptr<Game> of the
 * global ClientState. At any moment the game may or may not exist -- if it
 * exists, it
 */
class Game : public ui::Control {
protected:
  ClientShared &shared;

public:
  Game(ClientShared &shared, const std::string &name);

  bool concluded;

  // when the UI gains focus over the game
  virtual void onLooseFocus() = 0;

  // when the UI gives focus to the game
  virtual void onFocus() = 0;

  // try to exit, set Control::conclude flag eventually
  virtual void onExit() = 0;

  std::string name; // description of the game
  std::string status; // brief status of the game
};

#endif //SOLEMNSKY_PAGE_H