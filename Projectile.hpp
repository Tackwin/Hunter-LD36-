#pragma once
#include "General.hpp"

class PlayScene;
class Projectile : public cocos2d::Sprite{
public:
	Projectile();
	~Projectile();

	float damage = 0;
	float angle = 0;

	PlayScene* scene = nullptr;

	static Projectile* create(const std::string &path, float x, float y, float damage, float angle, PlayScene* scene);

	void initOptions(float x, float y, float damage, float angle);

	void update(float dt);
};

