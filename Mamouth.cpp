#include "Mamouth.hpp"
#include "PlayScene.hpp"


Mamouth::Mamouth() {
}

Mamouth::~Mamouth() {
}

Mamouth* Mamouth::create(const cocos2d::Vec2 &p, PlayScene *scene) {
	Mamouth* m = new(std::nothrow)Mamouth();
	if(m && m->initWithFile("res/mamouth.png")) {
		m->autorelease();
		m->initOptions(p, scene);
		return m;
	}
	CC_SAFE_DELETE(m);
	return m;
}

void Mamouth::initOptions(const cocos2d::Vec2 &p, PlayScene* scene) {
	setAnchorPoint({ 0.5f, 0.1f });
	setPosition(p);
	scene->addChild(this);
	this->scene = scene;
	scheduleUpdate();

	scheduleOnce([&](float dt)->void {
		charge(dt);
	}, 5.f, "charge");

	draw = cocos2d::DrawNode::create();
	draw->setPosition({getAnchorPoint().x * getContentSize().width, getAnchorPoint().y * getContentSize().height});
	addChild(draw, 1);
}

void Mamouth::update(float dt) {
	draw->clear();
	cocos2d::Vec2 offset = getAnchorPoint();
	offset.x *= getContentSize().width;
	offset.y *= getContentSize().height;
	draw->drawSolidRect( cocos2d::Vec2(-getContentSize().width / 2 - 1, getContentSize().height - 1),
					cocos2d::Vec2(getContentSize().width / 2 + 2, 32 + getContentSize().height - 1), cocos2d::Color4F::RED);
	draw->drawSolidRect( cocos2d::Vec2(- getContentSize().width / 2, getContentSize().height),
					cocos2d::Vec2((getContentSize().width) * health / maxHealth - getContentSize().width / 2, 30 + getContentSize().height), cocos2d::Color4F::GREEN);
}

void Mamouth::charge(float dt) {
	cocos2d::Vec2 crowdedCluster = this->scene->mostCrowdedCluster(150);
	if(crowdedCluster == cocos2d::Vec2::ZERO) {
		scheduleOnce([&](float dt)->void {
			charge(dt);
		}, 3.f, "charge");
		return;
	}
	runAction(
		cocos2d::Sequence::createWithTwoActions(
		cocos2d::MoveTo::create((crowdedCluster - getPosition()).getLength() / 300.f, crowdedCluster),
		cocos2d::CallFunc::create([&]()->void {
			scene->dealDamage(getPosition(), 150, 50);
			scheduleOnce([&](float dt)->void {
				charge(dt);
			}, 3.f, "charge");
		})
	));
}