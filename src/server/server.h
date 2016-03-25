#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "util/telegraph.h"
#include "sky/protocol.h"
#include "sky/event.h"
#include "sky/arena.h"
#include <iostream>

/**
 * Arena subsystem
 */
class ServerLogger: sky::Subsystem {

};

class Server {
 private:
  tg::UsageFlag flag; // for enet global state

  double uptime;

  sky::ArenaInitializer start;
  sky::Arena arena;

  /**
   * Connection state.
   */
  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  std::vector<ENetPeer *> connectingPeers;

  /**
   * Events.
   */
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const ArenaEvent &event);

  /**
   * Network logic.
   */
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);
  sky::Player *playerFromPeer(ENetPeer *peer) const;
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);

 public:
  Server(const Port port, const sky::ArenaInitializer &initializer);

  void tick(float delta);
  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
