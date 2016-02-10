#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include "sky/protocol.h"
#include "sky/arena.h"
#include <iostream>

struct PlayerClient {
  PlayerClient(const sf::IpAddress &address, const unsigned short port);

  const sf::IpAddress address;
  const unsigned short port;

  double lastPing;
};

class Server {
 private:
  double uptime;

  optional<sky::Sky> sky;
  sky::Arena arena;
  std::map<sky::PID, PlayerClient> clients;

  tg::Telegraph<sky::prot::ServerPacket, sky::prot::ClientPacket> telegraph;

  /**
   * Client management helpers.
   */
  sky::PID getFreePID() const; // get a PID not currently used in the arena
  optional<sky::PID> clientFromIP(const sf::IpAddress &address) const;

  /**
   * Transmission synonyms.
   */
  void sendToClient(const sky::prot::ServerPacket &packet,
                    const PlayerClient &client);
  void sendToAll(const sky::prot::ServerPacket &packet);

  /**
   * Packet processing subroutines.
   */
  void processFromClient(sky::PID pid,
                         const sky::prot::ClientPacket &packet);
  void processConnection(
      tg::Reception<sky::prot::ClientPacket> &&reception);

 public:
  Server();

  void tick(float delta);

  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
