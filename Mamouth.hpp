#pragma once
#include "General.hpp"
class PlayScene;
class Mamouth : public cocos2d::Sprite{
public:
	Mamouth();
	~Mamouth();

	PlayScene* scene = nullptr;
	cocos2d::DrawNode* draw = nullptr;

	float health = 1000;
	float maxHealth = 1000;
	float speed = 100;

	static Mamouth* create(const cocos2d::Vec2 &p, PlayScene* scene);
	void initOptions(const cocos2d::Vec2 &p, PlayScene* scene);

	void update(float dt);

	void charge(float);
};

