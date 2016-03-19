#include "multiplayerui.h"
#include "client/elements/style.h"

/**
 * MultiplayerLobby.
 */

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Lobby, shared, connection),

    specButton(style.base.normalButton, style.multi.lobbyButtonPos, "spectate"),
    redButton(style.base.normalButton,
              style.multi.lobbyButtonPos + style.multi.lobbyButtonSep,
              "join red"),
    blueButton(style.base.normalButton,
               style.multi.lobbyButtonPos + 2.0f * style.multi.lobbyButtonSep,
               "join blue"),
    chatInput(style.base.normalTextEntry,
              style.multi.chatPos, "[enter to chat]") {
  areChildren({&specButton, &redButton, &blueButton, &chatInput});
  connection.eventLog.push_back(sky::ClientEvent::LobbyStart());
}

void MultiplayerLobby::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});

  float offset = 0;
  for (auto iter = connection.eventLog.rbegin();
       iter < connection.eventLog.rend(); iter++) {
    auto p = ui::TextProperties::normal;
    p.size = style.base.normalFontSize;
    p.maxWidth = 500;
    p.alignBottom = true;
    offset -= f.drawText(style.multi.messageLogPos + sf::Vector2f(0, offset),
               (sky::ClientEvent(*iter)).print(), p);
  }

  offset = 0;
  sf::Color teamColor;
  for (const sky::Player &player : connection.arena.players) {
    offset += style.base.normalFontSize;
    teamColor = sf::Color::Black;
    if (player.team == 1) teamColor = sf::Color::Red;
    if (player.team == 2) teamColor = sf::Color::Blue;
    f.drawText(style.multi.playerListPos + sf::Vector2f(0, offset),
               player.nickname, style.base.normalFontSize,
               teamColor);
  }

  ui::Control::render(f);
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatInput.focus();
      return true;
    }
  }
  return false;
}

void MultiplayerLobby::reset() {
  ui::Control::reset();
}

void MultiplayerLobby::signalRead() {
  ui::Control::signalRead();

  sky::PlayerDelta delta(*connection.myPlayer);

  if (specButton.clickSignal)
    connection.requestTeamChange(0);
  if (redButton.clickSignal)
    connection.requestTeamChange(1);
  if (blueButton.clickSignal)
    connection.requestTeamChange(2);
  if (chatInput.inputSignal)
    connection.transmit(sky::ClientPacket::Chat(*chatInput.inputSignal));
}

void MultiplayerLobby::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerLobby::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerLobby::onChangeSettings(const SettingsDelta &settings) {

}

/**
 * MultiplayerGame.
 */

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    sky(*connection.arena.sky),
    renderSystem(&sky),
    skyDeltaUpdate(0.1) {
  sky.linkSystem(&renderSystem);
}

bool MultiplayerGame::skyDied() const {
  return !connection.arena.sky;
}

void MultiplayerGame::tick(float delta) {
  if (skyDeltaUpdate.cool(delta)) {
    skyDeltaUpdate.reset();
    connection.transmit(sky::ClientPacket::ReqSkyDelta(
        sky.collectDelta()
    ));
  }
}

void MultiplayerGame::render(ui::Frame &f) {
  if (sky::Plane *plane =
      sky.getPlane(connection.myPlayer->pid)) {
    renderSystem.render(f, plane->state.pos);
  } else renderSystem.render(f, {0, 0});

  f.drawText({300, 300}, "need to implement this");
}

bool MultiplayerGame::handle(const sf::Event &event) {
  return false;
}

void MultiplayerGame::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerGame::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerGame::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerGame::onChangeSettings(const SettingsDelta &settings) {

}

/**
 * MultiplayerScoring.
 */

MultiplayerScoring::MultiplayerScoring(
    ClientShared &shared, MultiplayerConnection &connection) :
    MultiplayerView(sky::ArenaMode::Scoring, shared, connection) {
  connection.eventLog.push_back(sky::ClientEvent::ScoringStart());
}

void MultiplayerScoring::tick(float delta) {

}

void MultiplayerScoring::render(ui::Frame &f) {

}

bool MultiplayerScoring::handle(const sf::Event &event) {
  return false;
}

void MultiplayerScoring::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerScoring::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerScoring::onPacket(const sky::ServerPacket &packet) {

}

void MultiplayerScoring::onChangeSettings(const SettingsDelta &settings) {

}
