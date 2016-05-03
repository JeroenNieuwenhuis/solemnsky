#include "physics.h"
#include "util/methods.h"
#include "util/printer.h"

namespace sky {

/**
 * BodyTag.
 */

/**
 * Physics.
 */

Physics::Physics(const Map &map, PhysicsListener *const listener) :
    world({0, Settings().gravity / Settings().distanceScale}),
    listener(listener),
    dims(map.dimensions) {
  // world boundaries
  b2Body *body;
  body = createBody(rectShape({dims.x, 1}), BodyTag(), true);
  body->SetTransform(toPhysVec({dims.x / 2, dims.y}), 0);
  body = createBody(rectShape({dims.x, 1}), BodyTag(), true);
  body->SetTransform(toPhysVec({dims.x / 2, 0}), 0);
  body = createBody(rectShape({1, dims.y}), BodyTag(), true);
  body->SetTransform(toPhysVec({dims.x, dims.y / 2}), 0);
  body = createBody(rectShape({1, dims.y}), BodyTag(), true);
  body->SetTransform(toPhysVec({0, dims.y / 2}), 0);

  // obstacles
  for (const auto &obstacle : map.obstacles) {
    body = createBody(polygonShape(obstacle.localVerticies), BodyTag(), true);
    body->SetTransform(toPhysVec(obstacle.pos), 0);
  }
}

Physics::~Physics() {
  for (b2Body *body = world.GetBodyList();
       body != nullptr;
       body = body->GetNext()) {
    delete (BodyTag *) body->GetUserData();
  }
}

void Physics::tick(const float delta) {
  world.ClearForces();
  world.Step(delta, settings.velocityIterations, settings.positionIterations);
}

sf::Vector2f Physics::toGameVec(b2Vec2 vec) const {
  return {vec.x * settings.distanceScale,
          vec.y * settings.distanceScale};
}

b2Vec2 Physics::toPhysVec(sf::Vector2f vec) const {
  return {vec.x / settings.distanceScale,
          vec.y / settings.distanceScale};
}

float Physics::toGameDistance(float x) const {
  return x * settings.distanceScale;
}

float Physics::toPhysDistance(float x) const {
  return x / settings.distanceScale;
}

b2Body *Physics::createBody(const b2PolygonShape &shape,
                            const BodyTag &tag, bool isStatic) {
  b2BodyDef def;
  def.fixedRotation = false;
  def.type = isStatic ? b2_staticBody : b2_dynamicBody;

  b2Body *body = world.CreateBody(&def);
  body->SetUserData(new BodyTag(tag));
  body->CreateFixture(&shape, 10.0f);
  return body;
}

void Physics::deleteBody(b2Body *&body) {
  if (body) {
    delete (BodyTag *) body->GetUserData();
    world.DestroyBody(body);
    body = nullptr;
  }
}

b2PolygonShape Physics::rectShape(const sf::Vector2f &dims) {
  b2PolygonShape shape;
  const auto bdims = toPhysVec(dims);
  shape.SetAsBox(bdims.x / 2.0f, bdims.y / 2.0f);
  return shape;
}

b2PolygonShape Physics::polygonShape(const std::vector<sf::Vector2f> &verticies) {
  b2PolygonShape shape;
  b2Vec2 *points = new b2Vec2[verticies.size()];
  size_t i = 0;
  for (const auto &vertex : verticies) {
    points[i] = toPhysVec(vertex);
    i++;
  }
  shape.Set(points, (int32) verticies.size());
  delete[] points;
  return shape;
}

void Physics::approachRotVel(b2Body *body, float rotvel) const {
  body->ApplyAngularImpulse(
      body->GetInertia() * (toRad(rotvel) - body->GetAngularVelocity()),
      true);
}

void Physics::approachVel(b2Body *body, sf::Vector2f vel) const {
  b2MassData data;
  body->GetMassData(&data); // this is just the most horrifying API ever
  body->ApplyLinearImpulse(
      body->GetMass() * (toPhysVec(vel) - body->GetLinearVelocity()),
      data.center + body->GetWorldCenter(), true);
}

/**
 * PhysicalState.
 */

PhysicalState::PhysicalState(const sf::Vector2f &pos,
                             const sf::Vector2f &vel,
                             const Angle rot,
                             const float rotvel) :
    pos(pos), vel(vel), rot(rot), rotvel(rotvel) { }

void PhysicalState::hardWriteToBody(const Physics &physics,
                                    b2Body *const body) const {
  body->SetLinearVelocity(physics.toPhysVec(vel));
  body->SetAngularVelocity(toRad(rotvel));
  body->SetTransform(physics.toPhysVec(pos), toRad(rot));
}

void PhysicalState::writeToBody(const Physics &physics,
                                b2Body *const body) const {
  // TODO: use proper motors / motor joints for this instead of impulses
  physics.approachVel(body, vel);
  body->SetTransform(physics.toPhysVec(pos), toRad(rot));
  physics.approachRotVel(body, rotvel);
}

void PhysicalState::readFromBody(const Physics &physics,
                                 const b2Body *const body) {
  pos = physics.toGameVec(body->GetPosition());
  vel = physics.toGameVec(body->GetLinearVelocity());
  rot = toDeg(body->GetAngle());
  rotvel = toDeg(body->GetAngularVelocity());
}

}
