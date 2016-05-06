/**
 * Common abstraction for our multiplayer servers.
 */
#pragma once
#include "sky/sky.h"
#include "sky/arena.h"
#include "util/telegraph.h"
#include "sky/protocol.h"
#include "sky/event.h"
#include "sky/arena.h"
#include "sky/event.h"
#include <iostream>

/**
 * Methods that the ServerExec exposes / shares with Server.
 */
struct ServerShared {
 private:
  class ServerExec &exec;

 public:
  ServerShared(tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
               class ServerExec &exec);

  tg::Host &host;
  tg::Telegraph<sky::ClientPacket> &telegraph;

  sky::Player *playerFromPeer(ENetPeer *peer) const;

  // applyAndSendDelta
  void applyAndSendDelta(const sky::ArenaDelta &arenaDelta);

  // Transmission.
  void sendToClients(const sky::ServerPacket &packet);
  void sendToClientsExcept(const PID pid,
                           const sky::ServerPacket &packet);
  void sendToClient(ENetPeer *const client,
                    const sky::ServerPacket &packet);
  void rconResponse(ENetPeer *const client, const std::string &response);

  // Logging.
  void logEvent(const ServerEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);
};

/**
 * Type-erasure for Server, representing the uniform API.
 */
class ServerListener: public sky::SubsystemListener {
  friend class ServerExec;
 protected:
  virtual void onPacket(ENetPeer *const client,
                        sky::Player &player,
                        const sky::ClientPacket &packet) = 0;

};

/**
 * The Server abstraction: a sky::Arena subsystem with additional callbacks
 * for networking and access to a ServerShared object.
 */
template<typename PlayerData>
class Server: public ServerListener, public sky::Subsystem<PlayerData> {
 protected:
  ServerShared &shared;
  sky::SkyManager &sky;

 public:
  Server(ServerShared &shared,
         sky::Arena &arena, sky::SkyManager &sky) :
      sky::Subsystem<PlayerData>(arena),
      shared(shared),
      sky(sky) { }

};

/**
 * Subsystem that just logs arena events through the onEvent callback.
 */
class ServerLogger: public sky::ArenaLogger {
 private:
  ServerShared &shared;

 protected:
  void onEvent(const sky::ArenaEvent &event) override;

 public:
  ServerLogger(ServerShared &shared, sky::Arena &arena);

};

/**
 * State and logic associated with the execution of a Server.
 * We manage the basics here.
 */
class ServerExec {
  friend class ServerShared;

 private:
  tg::UsageFlag flag; // for enet global state
  double uptime;

  tg::Host host;
  tg::Telegraph<sky::ClientPacket> telegraph;
  ServerShared shared;

  sky::Arena arena;
  sky::SkyManager sky;
  std::unique_ptr<ServerListener> server;
  Cooldown packetBroadcastTimer;

  ServerLogger logger;

  // Server loop subroutines.
  void processPacket(ENetPeer *client, const sky::ClientPacket &packet);
  void tick(float delta);

 public:
  ServerExec(const Port port,
             const sky::ArenaInit &arena,
             const sky::SkyInitializer &sky,
             std::function<std::unique_ptr<ServerListener>(
                 ServerShared &, sky::Arena &, sky::SkyManager &)> server);

  void run();

  bool running;
};
