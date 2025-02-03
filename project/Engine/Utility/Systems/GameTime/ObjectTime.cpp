#include "ObjectTime.h"

ObjectTime::ObjectTime() : time_(0.0f) {}

void ObjectTime::Update(float deltaTime) { time_ += deltaTime; }

