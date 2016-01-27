#include "server.h"
#include "util/packer.h"
#include "util/methods.h"
#include "sky/flight.h"
#include "sky/delta.h"

struct MyStruct {
  MyStruct() = default;

  MyStruct(float x, float y) : x(x), y(y) { };

  float x;
  optional<float> y;

  std::string show() {
    if (y)
      return "x: " + std::to_string(x) + " y: " + std::to_string(*y);
    return "x: " + std::to_string(x);
  }
};

int main() {
  const PackRule<float> floatPack = ValueRule<float>();
  const PackRule<optional<float>> optionalPack =
      OptionalRule<float>(floatPack);
  const PackRule<MyStruct> classPack =
      ClassRule<MyStruct>(
          MemberRule<MyStruct, float>(floatPack, &MyStruct::x),
          MemberRule<MyStruct, optional<float>>(optionalPack, &MyStruct::y)
      );

  Packet packet;

  appLog(LogType::Debug, "** floatPack **");
  float fl = 999999.0f;
  appLog(LogType::Debug, "input: " + std::to_string(fl));
  packet = pack(floatPack, fl);
  packet.dump();
  appLog(LogType::Debug, "output: " +
                         std::to_string(
                             unpack(floatPack, packet)));

  appLog(LogType::Debug, "** optionalPack **");
  optional<float> optFl = {88888.0f};
  appLog(LogType::Debug, "input: " + std::to_string(*optFl));
  packet = pack(optionalPack, optFl);
  packet.dump();
  appLog(LogType::Debug, "output: " +
                         std::to_string(*unpack(optionalPack, packet)));

  appLog(LogType::Debug, "** classPack **");
  MyStruct myStruct{42, 53};
  appLog(LogType::Debug, "input: " + myStruct.show());
  packet = pack(classPack, myStruct);
  packet.dump();
  appLog(LogType::Debug, "output: " + unpack(classPack, packet).show());

  appLog(LogType::Debug, "** classPack **");
  sky::PlaneTuning tuning;
  tuning.hitbox.x = 10;
  appLog(LogType::Debug, "input: has hitbox x dim " +
                         std::to_string(tuning.hitbox.x));
  packet = pack(sky::pk::planeTuningPack, tuning);
  packet.dump();
  tuning = unpack(sky::pk::planeTuningPack, packet);
  appLog(LogType::Debug, "output: has hitbox x dim " +
                         std::to_string(tuning.hitbox.x));
}
