#include "GameOver.hpp"
#include "PlayScene.hpp"


GameOver::GameOver() {
}

GameOver::~GameOver() {
}

cocos2d::Scene* GameOver::createScene() {
	auto s = cocos2d::Scene::create();
	auto l = GameOver::create();
	s->addChild(l);
	return s;
}

bool GameOver::init() {
	if(!Layer::init())
		return false;

	cocos2d::Label* l = cocos2d::Label::create("Game Over\n restart by pressing anything", "", 60);
	l->setAnchorPoint({ .5f, .5f });
	l->setPosition(General::size / 2);
	addChild(l);

	auto listener = cocos2d::EventListenerKeyboard::create();
	listener->onKeyPressed = [&](cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* e)->void {
		General::director->replaceScene(cocos2d::TransitionFade::create(0.25f, PlayScene::createScene()));
	};

	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}