#pragma once
#include "General.hpp"
#include "Mamouth.hpp"
#include "Guy.hpp"
class PlayScene : public cocos2d::Layer{
public:
	PlayScene();
	~PlayScene();

	std::vector<Guy*> guys;
	std::vector<Mamouth*> mamouths;

	cocos2d::DrawNode* draw = nullptr;

	cocos2d::Rect rectangleSelection;
	bool startSelection = false;
	bool singleSelection = false;

	int month = 0;

	float meat = 0;
	float pop = 25;

	bool placement = true;

	int dispo[2];
	int nmamouth = 0;

	std::vector<Guy*> clusteringGuy;
	int mamouthServed = 0;

	Guy::GuyType placeType = Guy::GuyType::LANCIER;

	virtual bool init();
	static cocos2d::Scene* createScene();
	CREATE_FUNC(PlayScene);

	void update(float dt);

	cocos2d::Vec2 separationVec(const Guy* p, float range);
	float alignSteer(Guy* p, float range);
	cocos2d::Vec2 cohesionVec(const Guy* p, float range);

	cocos2d::Vec2 getCeneterOfSwarm();

	// Otherwise return nullptr
	Mamouth* getTouched(const cocos2d::Rect &rect);
	Mamouth* getFirstMamouthInRange(const cocos2d::Vec2 &p, float range);

	cocos2d::Vec2 mostCrowdedCluster(float r);

	void dealDamage(const cocos2d::Vec2 &p, float range, float damage);

	std::vector<Guy*> getGuyWithin(const cocos2d::Vec2 &p, float r);

	void getDispo();

	void nextMonth();
};

