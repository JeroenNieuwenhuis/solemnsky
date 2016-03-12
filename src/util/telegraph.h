/**
 * Helpers for managing enet hosts (tg::Host) and sending / receiving
 * cereal-serialized objects over them (tg::Telegraph).
 */
#ifndef SOLEMNSKY_TELEGRAPH_H
#define SOLEMNSKY_TELEGRAPH_H

#include "enet/enet.h"
#include "util/types.h"
#include <sstream>
#include <cereal/archives/binary.hpp>
#include "util/methods.h"
#include <boost/range/iterator_range_core.hpp>

namespace tg {

namespace detail {
static int enetUsageCount = 0;
}

/**
 * ENet functionality can be used iff one of these is alive.
 * We need this because of enet's global state initialization /
 * deinitialization.
 */
struct UsageFlag {
  UsageFlag();
  ~UsageFlag();
};

/**
 * Host type: client or server.
 */
enum class HostType { Client, Server };

class Host {
 private:
  ENetHost *host;
  ENetEvent event;

  /**
   * Keeping track of peers.
   */
  void registerPeer(ENetPeer *peer);
  void unregisterPeer(ENetPeer *peer);

 public:
  Host(const HostType type,
       const unsigned short port = 0);
  ~Host();

  // uncopyable + unassignable
  Host(const Host &) = delete;
  Host &operator=(const Host &) = delete;

  /**
   * API.
   */
  std::vector<ENetPeer *> peers;
  ENetPeer *connect(const std::string &address, const unsigned short port);
  void disconnect(ENetPeer *);
  void transmit(ENetPeer *const peer,
                unsigned char *data, size_t size,
                const ENetPacketFlag flag);
  ENetEvent poll();
};

/**
 * Verifies structure of received packets with VerifyStructure.
 */
template<typename ReceiveType, typename TransmitType>
class Telegraph {
 private:
  ReceiveType receiveBuffer;
  std::stringstream dataBuffer;
  cereal::BinaryOutputArchive output;
  cereal::BinaryInputArchive input;

 public:

  Telegraph() : output(dataBuffer), input(dataBuffer) { }

  /**
   * Transmit a packet to one peer.
   */
  void transmit(
      Host &host, ENetPeer *const peer,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    output(value);
    std::string data = dataBuffer.str();
    host.transmit(peer, (unsigned char *) data.c_str(), data.size(), flag);
  }

  /**
   * Transmit same packet to an iterator range of peers.
   */
  void transmitMult(
      Host &host, const std::vector<ENetPeer *> &peers,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    output(value);
    std::string data = dataBuffer.str();
    for (ENetPeer *peer : peers)
      host.transmit(peer, (unsigned char *) data.c_str(), data.size(), flag);
  }

  /**
   * Transmit same packet to the peers in an iterator range of peers
   * that satisfy a predicate.
   */
  void transmitMultPred(
      Host &host, const std::vector<ENetPeer *> &peers,
      std::function<bool(ENetPeer *)> predicate,
      const TransmitType &value,
      ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE) {
    output(value);
    std::string data = dataBuffer.str();
    for (ENetPeer *peer : peers)
      if (predicate(peer))
        host.transmit(peer, (unsigned char *) data.c_str(), data.size(), flag);
  }

  optional<ReceiveType> receive(const ENetPacket *packet) {
    dataBuffer << packet->data;
    try {
      input(receiveBuffer);
      if (!verifyValue(receiveBuffer)) {
        appLog("Malformed packet: violated invariants!");
      } else return receiveBuffer;
    } catch (...) {
      appLog("Malformed packet: failed to decode!");
    }

    return {};
  }
};

}

#endif //SOLEMNSKY_TELEGRAPH_H
