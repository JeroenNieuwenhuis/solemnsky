/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
#include "telegraph/pack.h"
#include "arena.h"
#include <map>

namespace sky {

/**
 * Protocol verbs for the client.
 */
struct ClientPacket: public VerifyStructure {
  enum class Type {
    Ping, // request a pong, always available
    ReqJoin, // request joining in the arena, part of the connection protocol
    ReqDelta, // request a change to your player data
    ReqSpawn, // request to spawn
    ReqKill, // request to die
    ReqTeamChange, // request to change team / spectator mode
    NoteSkyDelta, // tell the server what you think is happening in the tutorial
    Chat // send a chat message
  };

  ClientPacket();
  ClientPacket(
      const Type type,
      const optional<std::string> &stringData = {},
      const optional<PlayerDelta> &playerDelta = {},
      const optional<SkyDelta> &skyDelta = {},
      const optional<Team> team = {});

  Type type;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;
  optional<SkyDelta> skyDelta;
  optional<Team> team;

  bool verifyStructure() const override;

  static ClientPacket Ping();
  static ClientPacket ReqJoin(const std::string &nickname);
  static ClientPacket ReqDelta(const PlayerDelta &playerDelta);
  static ClientPacket ReqSpawn();
  static ClientPacket ReqKill();
  static ClientPacket ReqTeamChange(const Team team);
  static ClientPacket NoteSkyDelta(const SkyDelta &skyDelta);
  static ClientPacket Chat(const std::string &message);
};

static const struct ClientPacketPack: public tg::ClassPack<ClientPacket> {
  ClientPacketPack();
} clientPacketPack;

/**
 * Messages from the server, of varied nature.
 */
struct ServerMessage: public VerifyStructure {
  enum class Type {
    Chat, // a client said something
    Broadcast // an important message to be displayed in big letters
  };

  ServerMessage();
  ServerMessage(
      const Type type,
      std::string contents,
      optional<std::string> from = {});

  Type type;
  optional<std::string> from; // who the message is from, potentially
  std::string contents;

  bool verifyStructure() const override;

  static ServerMessage Chat(const std::string &from,
                            const std::string &contents);
  static ServerMessage Broadcast(const std::string &contents);
};

static const struct ServerMessagePack: public tg::ClassPack<ServerMessage> {
  ServerMessagePack();
} serverMessagePack;

/**
 * Protocol verbs for the server.
 */
struct ServerPacket: public VerifyStructure {
  enum class Type {
    Pong,
    Message, // message for a client to log
    AckJoin, // acknowledge a ReqJoin, send ArenaInitializer
    NoteArenaDelta, // notify clients of a change in the arena
    NoteSkyDelta // transmit a client-bound snapshot
  };

  ServerPacket();
  ServerPacket(
      const Type type,
      const optional<ServerMessage> &message = {},
      const optional<PID> &pid = {},
      const optional<ArenaInitializer> &arenaInitializer = {},
      const optional<ArenaDelta> &arenaDelta = {},
      const optional<SkyDelta> &skyDelta = {});

  Type type;
  optional<ServerMessage> message;
  optional<PID> pid;
  optional<ArenaInitializer> arenaInitializer;
  optional<ArenaDelta> arenaDelta;
  optional<SkyDelta> skyDelta;

  bool verifyStructure() const override;

  static ServerPacket Pong();
  static ServerPacket Message(const ServerMessage &message);
  static ServerPacket AckJoin(const PID pid,
                              const ArenaInitializer &arenaInitializer);
  static ServerPacket NoteArenaDelta(const ArenaDelta &arenaDelta);
  static ServerPacket NoteSkyDelta(const SkyDelta &skyDelta);
};

static const struct ServerPacketPack: public tg::ClassPack<ServerPacket> {
  ServerPacketPack();
} serverPacketPack;

}

#endif //SOLEMNSKY_PROTOCOL_H
