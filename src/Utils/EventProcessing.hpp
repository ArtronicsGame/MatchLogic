#ifndef _FRAME_PRE_PROCESS
#define _FRAME_PRE_PROCESS

#include <BasicHeaders.hpp>


using namespace std;
using namespace nlohmann;

namespace OnEvents{
	const float FORCE_MAG = 100000.0f;

	void OnHeroMove(json& event, b2Body* body);
	json OnHeroShoot(json& event, b2Body* body, b2World* world);
}

#endif