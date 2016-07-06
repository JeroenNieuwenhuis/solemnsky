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
#include <boost/algorithm/string.hpp>
#include "arena.hpp"
#include "event.hpp"

namespace sky {

/**
 * PlayerDelta.
 */

PlayerDelta::PlayerDelta(const Player &player) :
    admin(player.isAdmin()),
    loadingEnv(player.isLoadingEnv()) {}

/**
 * Player.
 */

PlayerInitializer::PlayerInitializer(
    const PID pid, const std::string &nickname) :
    pid(pid), nickname(nickname), admin(false),
    loadingEnv(false), team(0) {}

Player::Player(Arena &arena, const PlayerInitializer &initializer) :
    Networked(initializer),
    nickname(initializer.nickname),
    admin(initializer.admin),
    team(initializer.team),
    loadingEnv(initializer.loadingEnv),

    latencyInitialized(bool(initializer.latencyStats)),
    latency(initializer.latencyStats
            ? initializer.latencyStats->first : 0),
    clockOffset(initializer.latencyStats
                ? initializer.latencyStats->second : 0),

    arena(arena),
    pid(initializer.pid) {}

void Player::applyDelta(const PlayerDelta &delta) {
  if (delta.nickname) nickname = *delta.nickname;
  admin = delta.admin;
  loadingEnv = delta.loadingEnv;
  if (delta.team) team = delta.team.get();
  if (delta.latencyStats) {
    latency = delta.latencyStats->first;
    clockOffset = delta.latencyStats->second;
    latencyInitialized = true;
  }
}

PlayerInitializer Player::captureInitializer() const {
  PlayerInitializer initializer;
  initializer.pid = pid;
  initializer.nickname = nickname;
  initializer.admin = admin;
  initializer.loadingEnv = loadingEnv;
  initializer.team = team;
  if (latencyInitialized) {
    initializer.latencyStats.emplace(latency, clockOffset);
  }
  return initializer;
}

std::string Player::getNickname() const {
  return nickname;
}

bool Player::isAdmin() const {
  return admin;
}

Team Player::getTeam() const {
  return team;
}

bool Player::isLoadingEnv() const {
  return loadingEnv;
}

bool Player::latencyIsCalculated() const {
  return latencyInitialized;
}

TimeDiff Player::getLatency() const {
  return latency;
}

Time Player::getClockOffset() const {
  return clockOffset;
}

void Player::doAction(const Action action, const bool state) {
  for (auto s : arena.subsystems) s.second->onAction(*this, action, state);
}

void Player::spawn(const PlaneTuning &tuning,
                   const sf::Vector2f &pos,
                   const float rot) {
  for (auto s : arena.subsystems) s.second->onSpawn(*this, tuning, pos, rot);
}

/**
 * Arena.
 */

ArenaDelta::ArenaDelta(const ArenaDelta::Type type) : type(type) {}

bool ArenaDelta::verifyStructure() const {
  switch (type) {
    case Type::Quit:
      return verifyRequiredOptionals(quit);
    case Type::Join:
      return verifyRequiredOptionals(join);
    case Type::Delta:
      return verifyRequiredOptionals(playerDeltas);
    case Type::EnvLoadState:
      return verifyRequiredOptionals(envLoadState);
    case Type::Motd:
      return verifyRequiredOptionals(motd);
    case Type::Mode:
      return verifyRequiredOptionals(mode);
    case Type::EnvChange:
      return verifyRequiredOptionals(environment);
  }
  return false; // enum out of bounds
}

ArenaDelta ArenaDelta::Quit(const PID pid) {
  ArenaDelta delta(Type::Quit);
  delta.quit = pid;
  return delta;
}

ArenaDelta ArenaDelta::Join(const PlayerInitializer &initializer) {
  ArenaDelta delta(Type::Join);
  delta.join = initializer;
  return delta;
}

ArenaDelta ArenaDelta::Delta(const PID pid, const PlayerDelta &playerDelta) {
  ArenaDelta delta(Type::Delta);
  delta.playerDeltas.emplace();
  delta.playerDeltas->emplace(pid, playerDelta);
  return delta;
}

ArenaDelta ArenaDelta::Delta(const std::map<PID, PlayerDelta> &playerDeltas) {
  ArenaDelta delta(Type::Delta);
  delta.playerDeltas = playerDeltas;
  return delta;
}

ArenaDelta ArenaDelta::EnvLoadState(const bool state) {
  ArenaDelta delta(Type::EnvLoadState);
  delta.envLoadState = state;
  return delta;
}

ArenaDelta ArenaDelta::Motd(const std::string &motd) {
  ArenaDelta delta(Type::Motd);
  delta.motd = motd;
  return delta;
}

ArenaDelta ArenaDelta::Mode(const ArenaMode arenaMode) {
  ArenaDelta delta(Type::Mode);
  delta.mode = arenaMode;
  return delta;
}

ArenaDelta ArenaDelta::EnvChange(const EnvironmentURL &map) {
  ArenaDelta delta(Type::EnvChange);
  delta.environment = map;
  return delta;
}

/**
 * SubsystemCaller.
 */

SubsystemCaller::SubsystemCaller(Arena &arena) :
    arena(arena) {}

void SubsystemCaller::doStartGame() {
  for (const auto &pair : arena.subsystems)
    pair.second->onStartGame();
}

void SubsystemCaller::doEndGame() {
  for (const auto &pair : arena.subsystems)
    pair.second->onEndGame();
}

/**
 * ArenaLogger.
 */

ArenaLogger::ArenaLogger(Arena &arena) : arena(arena) {
  arena.loggers.push_back(this);
}

/**
 * Arena.
 */

ArenaInit::ArenaInit(
    const std::string &name,
    const EnvironmentURL &environment,
    const ArenaMode mode) :
    name(name), environment(environment), mode(mode) {}

PID Arena::allocPid() const {
  return smallestUnused(players);
}

std::string Arena::allocNickname(const std::string &requestedNick,
                                 const optional<PID> ignorePid) const {
  std::string cleanReq(requestedNick);
  boost::algorithm::trim_right(cleanReq);

  std::stringstream readStream;
  PID nickNumber;
  const size_t rsize = cleanReq.size();
  std::vector<PID> usedNumbers;

  for (const auto &player : players) {
    if (ignorePid) {
      if (player.first == *ignorePid) continue;
    }

    const std::string &name = player.second.getNickname();
    if (name.size() < rsize) continue;
    if (name.substr(0, rsize) != cleanReq) continue;

    if (name == cleanReq) {
      usedNumbers.push_back(0);
      continue;
    }

    readStream.clear();
    readStream.str(name.substr(rsize));

    if (readStream.get() != '(') continue;
    readStream >> nickNumber;
    if (!readStream.good()) continue;
    if (readStream.get() != ')') continue;
    readStream.get();
    if (!readStream.eof()) continue;

    usedNumbers.push_back(nickNumber);
  }

  PID allocated = smallestUnused(usedNumbers);
  if (allocated == 0) return cleanReq;
  else
    return cleanReq + "("
        + std::to_string(smallestUnused(usedNumbers)) + ")";
}

void Arena::logEvent(const ArenaEvent &event) const {
  for (auto l : loggers) l->onEvent(event);
}

Player &Arena::joinPlayer(const PlayerInitializer &initializer) {
  if (Player *oldPlayer = getPlayer(initializer.pid)) quitPlayer(*oldPlayer);
  players.emplace(initializer.pid, Player(*this, initializer));
  Player &newPlayer = players.at(initializer.pid);
  for (auto s : subsystems) {
    s.second->registerPlayer(newPlayer);
    s.second->onJoin(newPlayer);
  }
  logEvent(ArenaEvent::Join(newPlayer.getNickname()));
  return newPlayer;
}

void Arena::quitPlayer(Player &player) {
  for (auto s : subsystems) {
    s.second->onQuit(player);
    s.second->unregisterPlayer(player);
  }
  logEvent(ArenaEvent::Quit(player.getNickname()));
  players.erase(player.pid);
}

void Arena::applyPlayerDelta(const PID pid, const PlayerDelta &playerDelta) {
  if (Player *player = getPlayer(pid)) {
    const auto oldNick = player->getNickname();
    const auto oldTeam = player->getTeam();

    player->applyDelta(playerDelta);

    const auto newNick = player->getNickname();
    const auto newTeam = player->getTeam();

    if (newNick != oldNick)
      logEvent(ArenaEvent::NickChange(oldNick, newNick));
    if (newTeam != oldTeam)
      logEvent(ArenaEvent::TeamChange(newNick, oldTeam, newTeam));

    for (auto &pair : subsystems) {
      pair.second->onDelta(*player, playerDelta);
    }
  }
}

Arena::Arena(const ArenaInit &initializer) :
    Networked(initializer),
    name(initializer.name),
    motd(initializer.motd),
    nextEnv(initializer.environment),
    mode(initializer.mode),
    uptime(0),
    subsystemCaller(*this) {
  for (auto const &player : initializer.players) {
    players.emplace(std::piecewise_construct,
                    std::forward_as_tuple(player.first),
                    std::forward_as_tuple(*this, player.second));
  }
}

void Arena::applyDelta(const ArenaDelta &delta) {
  switch (delta.type) {
    case ArenaDelta::Type::Quit: {
      if (Player *player = getPlayer(delta.quit.get())) {
        for (auto s : subsystems) s.second->onQuit(*player);
        quitPlayer(*player);
      }
      break;
    }

    case ArenaDelta::Type::Join: {
      Player &player = joinPlayer(delta.join.get());
      for (auto s : subsystems) s.second->onJoin(player);
      break;
    }

    case ArenaDelta::Type::Delta: {
      for (const auto &pair : delta.playerDeltas.get())
        applyPlayerDelta(pair.first, pair.second);
      break;
    }

    case ArenaDelta::Type::EnvLoadState: {
      bool newState = delta.envLoadState.get();
      forPlayers([&](Player &player) {
        player.loadingEnv = newState;
      });
      break;
    }

    case ArenaDelta::Type::Motd: {
      motd = *delta.motd;
      break;
    }

    case ArenaDelta::Type::Mode: {
      if (mode != *delta.mode) {
        mode = *delta.mode;
        logEvent(ArenaEvent::ModeChange(mode));
        for (auto s : subsystems) s.second->onMode(mode);
      }
      break;
    }

    case ArenaDelta::Type::EnvChange: {
      nextEnv = *delta.environment;
      logEvent(ArenaEvent::EnvChoose(nextEnv));
      for (auto s : subsystems) s.second->onMapChange();
    }

  }
}

ArenaInit Arena::captureInitializer() const {
  ArenaInit initializer(name, nextEnv);
  for (auto &player : players) {
    initializer.players.emplace(
        player.first, player.second.captureInitializer());
  }
  initializer.motd = motd;
  initializer.mode = mode;
  return initializer;
}

Player *Arena::getPlayer(const PID pid) {
  auto player = players.find(pid);
  if (player != players.end()) return &player->second;
  return nullptr;
}

void Arena::forPlayers(std::function<void(const Player &)> f) const {
  for (const auto &pair : players) f(pair.second);
}

void Arena::forPlayers(std::function<void(Player &)> f) {
  for (auto &pair : players) f(pair.second);
}

const std::map<PID, Player> &Arena::getPlayers() const {
  return players;
}

const std::string &Arena::getName() const {
  return name;
}

const std::string &Arena::getMotd() const {
  return motd;
}

const EnvironmentURL &Arena::getNextEnv() const {
  return nextEnv;
}

ArenaMode Arena::getMode() const {
  return mode;
}

Time Arena::getUptime() const {
  return uptime;
}

ArenaDelta Arena::connectPlayer(const std::string &requestedNick) {
  Player &player = joinPlayer(
      PlayerInitializer(allocPid(), allocNickname(requestedNick)));
  return ArenaDelta::Join(player.captureInitializer());
}

std::string Arena::allocNewNickname(const Player &player,
                                    const std::string &requestedNick) {
  return allocNickname(requestedNick, player.pid);
}

void Arena::tick(const TimeDiff delta) {
  uptime += Time(delta);
  for (auto s : subsystems) s.second->onTick(delta);
}

void Arena::poll(const TimeDiff delta) {
  for (auto s : subsystems) s.second->onPoll(delta);
}

}