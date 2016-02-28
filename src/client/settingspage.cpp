#include "settingspage.h"

ui::TextEntry::Style SettingsPage::Style::textEntryStyle(
    const SettingsPage::Style &style) {
  ui::TextEntry::Style textStyle;
  textStyle.fontSize = style.fontSize;
  textStyle.dimensions = style.textEntryDimensions;
  return textStyle;
}

SettingsPage::SettingsPage(ClientShared &state) :
    Page(state),
    style(),
    newSettings(shared.settings),
    nicknameChooser(style.nicknameEntryPos, "", true,
                    style.textEntryStyle(style)) {
  nicknameChooser.contents = shared.settings.nickname;
}

void SettingsPage::writeToSettings() {
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

/**
 * Page interface.
 */

void SettingsPage::onBlur() {
  newSettings.nickname = nicknameChooser.contents;

  writeToSettings();
  nicknameChooser.reset();
}

void SettingsPage::onFocus() {

}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  if (delta.nickname) nicknameChooser.contents = *delta.nickname;
  newSettings = shared.settings;
}

/**
 * Game interface.
 */

void SettingsPage::tick(float delta) {
  nicknameChooser.tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);

  f.drawText(style.nicknameDescPos, {style.nicknameDesc}, style.fontSize,
             style.descColor);
  nicknameChooser.render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  return nicknameChooser.handle(event);
}

void SettingsPage::signalRead() {
  if (nicknameChooser.inputSignal) {
    newSettings.nickname = nicknameChooser.contents;
    writeToSettings();
  }
}

void SettingsPage::signalClear() {
  nicknameChooser.signalClear();
}
