/**
 * Metadata and caching for our resources.
 */
#ifndef SOLEMNSKY_RESOURCES_H
#define SOLEMNSKY_RESOURCES_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

/****
 * Resource metadata.
 */

enum class Res {
  Font,
  PlayerSheet,
  TitleScreen,
  LAST // for checking number of resources defined
};

enum class ResType {
  Font, Texture
};

struct ResRecord {
  std::string path;
  ResType type;

  // if it's a spritesheet, we have some metadata about how it's laid out
  bool isSheet;
  int tileX, tileY, countX, countY;

  ResRecord(std::string path,
            ResType type,
            bool isSheet,
            int tileX = 0, int tileY = 0, int countX = 0, int countY = 0) :
      path(path),
      type(type),
      isSheet(isSheet),
      tileX(tileX), tileY(tileY), countX(countX), countY(countY) { }
};

std::string filepathTo(Res res);
ResRecord recordOf(Res res);

/****
 * Resource caching.
 */

namespace detail {

class ResMan {
private:
  std::map<int, sf::Texture> textures;
  std::map<int, sf::Font> fonts;
  bool initialized{false};

public:
  void loadRes();

  const sf::Texture &recallTexture(Res res);
  const sf::Font &recallFont(Res res);
};

static ResMan resMan{};

}

/**
 * Access the resource manager object.
 */
void loadResources();
const sf::Texture &textureFrom(Res res);
const sf::Font &fontFrom(Res res);

#endif //SOLEMNSKY_RESOURCES_H
