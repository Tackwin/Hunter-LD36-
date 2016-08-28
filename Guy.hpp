#pragma once
#include "General.hpp"
class PlayScene;
class Guy : public cocos2d::Sprite {
public:
	Guy();
	~Guy();

	enum GuyType {
		LANCIER,
		HEAL
	} type;

	bool focused = false;
	float speed = 10 * 10.f;
	PlayScene* scene = nullptr;

	cocos2d::Sprite* selectedSprite = nullptr;
	cocos2d::DrawNode* draw = nullptr;

	cocos2d::Size textureSize;

	bool moving = false;
	cocos2d::Vec2 destination = cocos2d::Vec2::ZERO;

	float direction;
	float steeringSpeed = 0.314159f * 20.f;

	float health = 100;
	float maxHealth = 100;

	int nFrame = 0;

	float timerFrame = 1 / 10.f;

	static Guy* create(const cocos2d::Vec2 &p, PlayScene *scene, GuyType type = GuyType::LANCIER);
	void initOptions(const cocos2d::Vec2 &p, GuyType type = GuyType::LANCIER);
	void goThere(const cocos2d::Vec2 &p);

	void update(float dt);

	void takeDamage(float d);
	void healBy(float d);

	void startRound();
};

