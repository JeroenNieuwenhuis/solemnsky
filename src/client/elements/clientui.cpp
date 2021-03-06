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
#include "clientui.hpp"
#include "style.hpp"
#include "ui/settings.hpp"

MessageInteraction::MessageInteraction(const ui::AppRefs &references) :
    Control(references),
    messageEntry(references,
                 style.game.messageEntry,
                 style.game.messagePos,
                 "[enter to type]"),
    messageLog(references,
               style.game.messageLog,
               style.game.messagePos + sf::Vector2f(0, -style.game.chatYPadding)) {
  areChildren({&messageEntry, &messageLog});
}

void MessageInteraction::tick(const TimeDiff delta) {
  ui::Control::tick(delta);
}

void MessageInteraction::render(ui::Frame &f) {
  ui::Control::render(f);
}

bool MessageInteraction::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MessageInteraction::reset() {
  ui::Control::reset();
}

void MessageInteraction::signalRead() {
  ui::Control::signalRead();
  if (messageEntry.inputSignal) {
    if (!messageEntry.inputSignal.get().empty())
      inputSignal = messageEntry.inputSignal;
  }

  messageLog.collapsed = !messageEntry.isFocused();
}

void MessageInteraction::signalClear() {
  ui::Control::signalClear();
  inputSignal.reset();
}

bool MessageInteraction::handleClientAction(const ui::ClientAction action, const bool state) {
  switch (action) {
    case ui::ClientAction::Chat: {
      if (state) messageEntry.focus();
      return true;
    }
    default:
      return false;
  }
}

/**
 * EnginePrinter.
 */
EnginePrinter::EnginePrinter(MessageInteraction &messageInteraction) :
    consolePrinter(LogOrigin::Engine) {
  areChildPrinters({&messageInteraction.messageLog, &consolePrinter});
}

/**
 * ClientPrinter.
 */

ClientPrinter::ClientPrinter(MessageInteraction &messageInteraction) :
    consolePrinter(LogOrigin::Client) {
  areChildPrinters({&consolePrinter, &messageInteraction.messageLog});
}

/**
 * GameConsolePrinter.
 */

void printConsolePrefix(Printer &p) {
  p.print("[console] ");
}

GameConsolePrinter::GameConsolePrinter(MessageInteraction &messageInteraction) :
    consolePrinterBase(LogOrigin::Client),
    consolePrinter(consolePrinterBase, printConsolePrefix) {
  areChildPrinters({&consolePrinter, &messageInteraction.messageLog});
}

void GameConsolePrinter::input(const std::string &string) {
  setColor(255, 0, 0);
  print(">> ");
  setColor(255, 255, 255);
  print(string);
  breakLine();
}

void GameConsolePrinter::output(const std::string &string) {
  setColor(255, 0, 0);
  print("<< ");
  print(string);
  breakLine();
}
