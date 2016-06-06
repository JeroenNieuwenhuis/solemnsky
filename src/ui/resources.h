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
/**
 * Hard-coded resource management for global assets.
 */
#pragma once

#include <SFML/Graphics.hpp>
#include "util/methods.h"
#include "util/printer.h"
#include "sheet.h"
#include "util/threads.h"

namespace ui {

/**
 * Metadata for a font.
 */
struct FontMetadata {
  // Constructors.
  FontMetadata(const std::string &url, const std::string &name);

  // Data.
  const std::string url, name;

};

/**
 * Metadata for a texture / spritesheet.
 */
struct TextureMetadata {
 private:
  TextureMetadata(
      const std::string &url,
      const std::string &name,
      const optional<SheetLayout> &spritesheetForm = {});

 public:
  // Constructors.
  static TextureMetadata TextureResource(
      const std::string &url,
      const std::string &name);
  static TextureMetadata SpritesheetResource(
      const std::string &url,
      const std::string &name,
      const sf::Vector2i &spriteDimensions,
      const sf::Vector2i &sheetTiling);

  // Data.
  const std::string url, name;
  optional<SheetLayout> spritesheetForm;

};

/**
 * Resource IDs.
 */
enum class FontID {
  Default // Font used for all text and titles
};

enum class TextureID {
  Title,
  MenuBackground,
  Credits,
  Lobby,
  Scoring,
  ScoreOverlay,
  PlayerSheet
};

namespace detail {

/**
 * Hard-coded metadata for resources.
 */
static const std::map<FontID, FontMetadata> fontMetadata;
static const std::map<TextureID, TextureMetadata> textureMetadata;

}

/**
 * An access to the data associated with a set of resources.
 */
class ResourceHolder {
  friend class ResourceLoder;
 private:
  const std::map<FontID, sf::Font> &fonts;
  const std::map<TextureID, sf::Texture> &textures;

  ResourceHolder(const std::map<FontID, sf::Font> &fonts,
                 const std::map<TextureID, sf::Texture> &textures);

 public:
  const FontMetadata &getFontData(const FontID id) const;
  const TextureMetadata &getTextureData(const TextureID id) const;
  const sf::Font &getFont(const FontID id) const;
  const sf::Texture &getTexture(const TextureID id) const;

};

/**
 * Manages the loading of a set of resources, resulting in a ResourceHolder.
 */
class ResourceLoader {
 private:
  std::map<FontID, sf::Font> fonts;
  std::map<TextureID, sf::Texture> textures;

  std::thread workingThread;
  std::vector<std::string> workerLog;
  std::mutex logMutex;

  float loadingProgress;
  bool loadingErrored;
  optional<ResourceHolder> holder;

  // Loading subroutines.
  void writeLog(const std::string &str);
  optional<std::string> loadFont(
      const FontID id, const FontMetadata &data);
  optional<std::string> loadTexture(
      const TextureID id, const TextureMetadata &data);

 public:
  ResourceLoader();

  void load(); // non-blocking
  float getProgress() const;
  void printNewLogs(Printer &p);
  bool getErrorStatus() const;

  ResourceHolder const *getHolder() const;

};

}
