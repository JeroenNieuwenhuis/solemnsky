/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 * Copyright (C) 2016  Glenn Smith
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
#include "inputaction.hpp"
#include "util/clientutil.hpp"

namespace ui {

/**
 * InputAction.
 */

InputAction::InputAction(const sf::Keyboard::Key &key) :
    key(key), joyAxis(), joyButton() {}

InputAction::InputAction(const sf::Joystick::Axis &axis, const AxisDirection &direction) :
    key(), joyAxis(std::make_pair(axis, direction)), joyButton() {}

InputAction::InputAction(const JoystickButton &button) :
    key(), joyAxis(), joyButton(button) {}

InputAction InputAction::actionForEvent(const sf::Event &event) {
  switch (event.type) {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
      return InputAction(event.key.code);
    case sf::Event::JoystickButtonPressed:
    case sf::Event::JoystickButtonReleased:
      return InputAction(event.joystickButton.button);
    case sf::Event::JoystickMoved:
      if (event.joystickMove.position > 0.0f)
        return InputAction(event.joystickMove.axis, InputAction::AxisDirection::Positive);
      else
        return InputAction(event.joystickMove.axis, InputAction::AxisDirection::Negative);
    default:
      //Nothing, this is an empty event
      return InputAction();
  }
}

std::vector<InputAction> InputAction::actionsForEvent(const sf::Event &event) {
  switch (event.type) {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
      return {InputAction(event.key.code)};
    case sf::Event::JoystickButtonPressed:
    case sf::Event::JoystickButtonReleased:
      return {InputAction(event.joystickButton.button)};
    case sf::Event::JoystickMoved:
      //Return both positive and negative directions because we'll miss some break events
      // due to controllers going from 100.0 to -5.0 in one event
      return {
          InputAction(event.joystickMove.axis, Positive),
          InputAction(event.joystickMove.axis, Negative)
      };
    default:
      //No actions, empty vector
      return {};
  }
}

bool InputAction::isMake(const sf::Event &e) const {
  //Keys/buttons are just a check if they were pressed
  if (key) return e.type == sf::Event::KeyPressed;
  if (joyButton) return e.type == sf::Event::JoystickButtonPressed;

  if (joyAxis) {
    switch (joyAxis->first) {
      //TODO: Investigate DirectInput on Windows where the Z axis does not actually behave like this
      case sf::Joystick::Axis::Z:
      case sf::Joystick::Axis::R:
        //Trigger axes are -1 (unpressed) to 1 (pressed)
        return e.joystickMove.position > 0.0f;
      default:
        //These only count if you're going in the right direction
        if (joyAxis->second == Positive)
          return e.joystickMove.position > 50.0f;
        else
          return e.joystickMove.position < -50.0f;
    }
  }

  return false;
}

std::string InputAction::getName() const {
  if (key) {
    return printKey(key.get());
  }
  if (joyButton) {
    return std::string("Button ") + std::to_string(joyButton.get());
  }
  if (joyAxis) {
    return std::string("Axis ") + std::to_string(joyAxis->first) + " " + (joyAxis->second == Negative ? "-" : "+");
  }
  return "<unknown>";
}

bool InputAction::operator==(const InputAction &other) const {
  //Optimize this so we don't have to do a bunch of optional<>::operator bool calls
  if (isKey() && other.isKey()) {
    return key.get() == other.key.get();
  }
  if (isJoyAxis() && other.isJoyAxis()) {
    return joyAxis.get() == other.joyAxis.get();
  }
  if (isJoyButton() && other.isJoyButton()) {
    return joyButton.get() == other.joyButton.get();
  }
  return false;
}

bool InputAction::operator<(const InputAction &other) const {
  //Optimize this so we don't have to do a bunch of optional<>::operator bool calls
  bool thisKey = isKey(), otherKey = other.isKey();
  bool thisJoy = isJoystick(), otherJoy = other.isJoystick();

  //Keys are lower than joystick actions
  if (thisKey && otherJoy) return true;
  if (otherKey && thisJoy) return false;

  //Lowest key wins
  if (thisKey && otherKey) return key.get() < other.key.get();

  //Last case, thisJoy && otherJoy
  bool thisAxis = isJoyAxis(), otherAxis = other.isJoyAxis();
  bool thisButton = isJoyButton(), otherButton = other.isJoyButton();

  //Axis is less than button
  if (thisAxis && otherButton) return true;
  if (thisButton && otherAxis) return false;

  //Lowest button wins
  if (thisButton && otherButton) return joyButton.get() < other.joyButton.get();

  //thisAxis && joyAxis: Whichever's direction is "less" than the other (in the enum)
  if (joyAxis->first == other.joyAxis->first) return joyAxis->second < other.joyAxis->second;
  return joyAxis->first < other.joyAxis->first;
}

}
