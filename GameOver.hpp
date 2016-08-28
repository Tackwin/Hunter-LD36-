#pragma once
#include "General.hpp"
class GameOver : public cocos2d::Layer {
public:
	GameOver();
	~GameOver();

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameOver);
};

