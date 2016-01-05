/**
 * Splash screen, a cover for loading our resources (resources.h).
 */
#ifndef SOLEMNSKY_SPLASH_H
#define SOLEMNSKY_SPLASH_H

#include "ui/control.h"
#include <memory>

namespace ui {

// TODO: make this threaded so the window doesn't freeze up
class SplashScreen : public Control {
private:
  bool screenDrawn{false};
  std::shared_ptr<Control> afterLoading;

public:
  SplashScreen(std::shared_ptr<Control> afterLoading) :
      afterLoading(afterLoading) { }

  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual void handle(sf::Event event) override;
};

}

#endif //SOLEMNSKY_SPLASH_H
