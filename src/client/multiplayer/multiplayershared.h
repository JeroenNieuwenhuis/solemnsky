/**
 * The shared core of the multiplayer client; the greatest common denominator
 * of all 3 client modes (the lobby, the game, and the scoring screen).
 */
#ifndef SOLEMNSKY_MULTIPLAYERSHARED_H
#define SOLEMNSKY_MULTIPLAYERSHARED_H

#include "client/ui/control.h"
#include "util/telegraph.h"
#include "sky/arena.h"
#include "sky/protocol.h"
#include "client/elements/elements.h"

/**
 * A connection to a server; manages the enet connection state, follows the
 * protocol to maintain a sky::Arena up-to-date, and maintains a message log.
 */
class MultiplayerConnection {
 private:
  ClientShared &shared;

  ENetEvent event;

  tg::Telegraph<sky::ServerPacket> telegraph;
  bool askedConnection;
  Cooldown disconnectTimeout;

  bool processPacket(const sky::ServerPacket &packet);

 public:
  MultiplayerConnection(
      ClientShared &shared,
      const std::string &serverHostname,
      const unsigned short serverPort);

  /**
   * Connection state.
   */
  tg::Host host;
  ENetPeer *server;
  bool disconnected, disconnecting;

  /**
   * Game state.
   */
  sky::Player *myPlayer;
  sky::Arena arena;
  std::vector<std::string> messageLog;

  /**
   * Methods.
   */
  void transmit(const sky::ClientPacket &packet);
  optional<sky::ServerPacket> poll(const float delta);
  void disconnect();
};

/**
 * The interface for the client in a certain Arena mode; given that the
 * interface is modal, we should write it in modes.
 */
class MultiplayerView: public ui::Control {
 protected:
  struct Style {
    // uniform style values
    sf::Vector2i scoreOverlayDims;
    sf::Vector2f chatPos,
        messageLogPos,
        readyButtonPos;
    float scoreOverlayTopMargin;
    float lobbyPlayersOffset,
        lobbyTopMargin,
        lobbyChatWidth,
        gameChatWidth;
    int lobbyFontSize;
    sf::Color playerSpecColor, playerJoinedColor;
    std::string readyButtonActiveDesc, readyButtonDeactiveDesc;
    Style();
  } style;

  // shared state
  ClientShared &shared;
  MultiplayerConnection &connection;

 public:
  MultiplayerView(ClientShared &shared, MultiplayerConnection &connection);
  virtual ~MultiplayerView() { }

  virtual void onBlur() = 0;
  virtual void onFocus() = 0;
  virtual void onPacket(const sky::ServerPacket &packet) = 0;
  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
};

#endif //SOLEMNSKY_MULTIPLAYERSHARED_H
