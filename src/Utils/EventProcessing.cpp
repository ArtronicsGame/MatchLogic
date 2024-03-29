#include <EventProcessing.hpp>

using namespace std;
using namespace nlohmann;

void OnEvents::OnHeroMove(json& event, b2Body* body){
	b2Vec2 dir;
	float speed = event["_info"]["speed"];
	dir.x = event["_info"]["x"];
	dir.y = event["_info"]["y"];

	PlayerInfo* i = (PlayerInfo*) body->GetUserData();

	dir.x *= speed * i->speed;
	dir.y *= speed * i->speed;

	body->SetTransform(body->GetPosition(), atan2f(dir.y, dir.x));
	body->SetLinearVelocity(dir);
}

json OnEvents::OnHeroShoot(json& event, b2Body* body, b2World* world){
	float rotation = body->GetAngle();
	b2Vec2 pos = body->GetPosition();
	pos.x += 7 * cos(rotation);
	pos.y += 7 * sin(rotation);

	b2Body* bullet = spawn(BULLET, pos, rotation, world);
	Bullet* data = (Bullet*) bullet->GetUserData();
	data->ref = (PlayerInfo*) body->GetUserData();

	b2Vec2 force(FORCE_MAG * cos(rotation), FORCE_MAG * sin(rotation));
	bullet->ApplyForceToCenter(force, false);

	ObjectData *temp = (ObjectData *)bullet->GetUserData();
	json message = JSON::newItem(temp, pos, rotation);
	return message;
}

