#include "radiobutton.h"

/**
 * RadioButton.
 */

namespace ui {

ui::RadioButton::RadioButton(const sf::Vector2f &pos,
                             const ui::RadioButton::Style &style) :
    style(style),
    pos(pos),
    heat(0, 1, 0),
    clicked(false) { }

void ui::RadioButton::tick(float delta) {
  heat += style.heatRate * (isHot ? 1 : -1);
}

void ui::RadioButton::render(ui::Frame &f) {

}

bool ui::RadioButton::handle(const sf::Event &event) {
  return false;
}

}