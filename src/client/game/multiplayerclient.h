/**
 * Multiplayer client.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "client/elements/elements.h"
#include "telegraph/telegraph.h"
#include "sky/protocol.h"
#include "sky/sky.h"
#include "sky/arena.h"
#include "client/subsystem/render.h"

class MultiplayerClient: public Game {
 private:
  ui::Button quitButton;
  ui::TextEntry chatEntry;
  ui::TextLog messageLog;

  /**
   * Connection state / network stuff.
   */
  tg::Host host;
  ENetPeer *server;
  tg::Telegraph<sky::prot::ServerPacket, sky::prot::ClientPacket> telegraph;
  Cooldown pingCooldown;

  bool triedConnection; // have we sent a verbs requesting connection yet?
  bool connected; // we've initialized the Arena

  ENetEvent event; // volatile

  /**
   * Local model of the Arena / game state.
   * Use with discretion until 'connected'.
   */
  sky::PlayerRecord *myRecord;
  sky::Arena arena;

  /**
   * Networking submethods.
   */
  void transmitServer(const sky::prot::ClientPacket &packet);
  // handle a network poll, assuming we're connected / initialized
  void handleNetwork(const ENetEvent &event);

 public:
  MultiplayerClient(ClientShared &state,
                    const std::string &serverHostname,
                    const unsigned short serverPort);

  /**
   * Game interface.
   */
  void onLooseFocus() override;
  void onFocus() override;
  void onExit() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_MULTICLIENT_H