#include "Guy.hpp"
#include "PlayScene.hpp"
#include "Projectile.hpp"

Guy::Guy() {
}

Guy::~Guy() {
}

Guy* Guy::create(const cocos2d::Vec2 &p, PlayScene* scene, GuyType type) {
	Guy* g = new(std::nothrow)Guy();
	if(g && g->initWithFile(std::string("res/guy") + std::to_string(type) +"Walking.png")) {
		g->scene = scene;
		g->autorelease();
		g->initOptions(p, type);
		return g;
	}
	CC_SAFE_DELETE(g);
	return g;
}

void Guy::initOptions(const cocos2d::Vec2 &p, GuyType type) {
	textureSize = getContentSize();
	setTextureRect({0, 0, textureSize.width / 3, textureSize.height});
	setAnchorPoint({ 0.5f, 0.1f });
	setPosition(p);
	selectedSprite = cocos2d::Sprite::create("res/selected.png");
	selectedSprite->setAnchorPoint({ .5f, .5f });
	selectedSprite->setPosition({ getBoundingBox().size.width * getAnchorPoint().x, getBoundingBox().size.height * getAnchorPoint().y });
	selectedSprite->setVisible(focused);
	addChild(selectedSprite, -1);

	this->type = type;

	draw = cocos2d::DrawNode::create();
	draw->setPosition(getAnchorPoint().x * getContentSize().width, getAnchorPoint().y * getContentSize().height);
	addChild(draw);

	scheduleUpdate();
}

void Guy::goThere(const cocos2d::Vec2 &p) {
	/*stopActionByTag(1);
	auto act = cocos2d::MoveTo::create(p.getLength() / speed, p);
	act->setTag(1);
	runAction(act);*/

	moving = true;
	destination = p;
	direction = (destination - getPosition()).getAngle();
}

void Guy::update(float dt) {
	selectedSprite->setVisible(focused);
	draw->clear();

	if(focused) {
		draw->drawSolidRect({ -getContentSize().width / 2 - 1, getContentSize().height - 1 },
		{ getContentSize().width * health / maxHealth - getContentSize().width / 2 + 1, getContentSize().height + 1 }, cocos2d::Color4F::RED);
		draw->drawSolidRect({ -getContentSize().width / 2, getContentSize().height },
		{ getContentSize().width * health / maxHealth - getContentSize().width / 2, getContentSize().height }, cocos2d::Color4F::GREEN);
	}

	if(moving) {
		timerFrame -= dt;
		if(timerFrame < 0) {
			nFrame++;
			nFrame %= 2;
			setTextureRect({ (1 + nFrame) * textureSize.width / 3.f, 0, textureSize.width / 3.f, textureSize.height });

			timerFrame = 1 / 10.f;
		}

		cocos2d::Vec2 old = getPosition();
		setPosition(getPosition() + (destination - getPosition()).getNormalized() * dt * speed);
		if(getPositionX() < 0 || getPositionX() >= General::size.width || getPositionY() < 0 || getPositionY() >= General::size.height) {
			setPosition(old);
		}
		// TODO FLOCKING/ BIRDS AI / SWARM
		/*draw->clear();
		cocos2d::Vec2 result = cocos2d::Vec2::ZERO;

		cocos2d::Vec2 t = scene->cohesionVec(this, 400);
		float factor = 2.0f / (1.0f + expf(logf(0.01f / 1.98f) * t.getLength() / 400.0f)) - 1.0f;
		t.scale(factor);
		//draw->drawLine(cocos2d::Vec2::ZERO, t, cocos2d::Color4F::GREEN);
		result += t;
		
		t = scene->separationVec(this, 400);
		factor = expf(log(0.1f) * t.getLength() / 400);
		t.scale(factor);
		//draw->drawLine(cocos2d::Vec2::ZERO, t, cocos2d::Color4F::YELLOW);
		result += t;

		/*float temp = scene->alignSteer(this, 400);
		draw->drawLine(cocos2d::Vec2::ZERO, cocos2d::Vec2(cosf(temp), sinf(temp)) * 50, cocos2d::Color4F::BLUE);
		
		result.rotateByAngle(cocos2d::Vec2::ZERO, (direction-temp) * 0.1f);
		*/
		//direction += (direction - result.getAngle()) * steeringSpeed * dt;

		//direction += (result.getAngle() - direction) * steeringSpeed * dt;
		/*float dtA = direction - (destination - getPosition()).getAngle();
		t = 2 / (1 + expf(log(0.01 / 0.99) * dtA / 0.785398163)) * dtA;
		draw->drawLine(cocos2d::Vec2::ZERO, t, cocos2d::Color4F::MAGENTA);
		direction -= t.getAngle() * steeringSpeed * dt;
		
		draw->drawLine(cocos2d::Vec2::ZERO, cocos2d::Vec2(cosf(direction), sinf(direction)) * 50, cocos2d::Color4F::RED);

		setPosition(getPosition() + (cocos2d::Vec2(cosf(direction), sinf(direction))) * speed * dt);
		setPosition(getPosition() + result * speed * dt / 50);

		*/
		if(getPosition().getDistanceSq(destination) < 25.f) {
			moving = false;
			destination = cocos2d::Vec2::ZERO;
		}
	} else {
		setTextureRect({ 0, 0, textureSize.width / 3, textureSize.height });
	}

}

void Guy::takeDamage(float d) {
	runAction(cocos2d::Sequence::createWithTwoActions(
		cocos2d::TintTo::create(0.1f, cocos2d::Color3B::RED),
		cocos2d::TintTo::create(0.1f, cocos2d::Color3B::WHITE)
	));

	health -= d;
}

void Guy::healBy(float d) {
	runAction(cocos2d::Sequence::createWithTwoActions(
		cocos2d::TintTo::create(0.1f, cocos2d::Color3B::GREEN),
		cocos2d::TintTo::create(0.1f, cocos2d::Color3B::WHITE)
	));

	health += d;
	health = health < maxHealth ? health : maxHealth;
}


void Guy::startRound() {
	schedule([&](float dt)->void {
		if(type == Guy::GuyType::LANCIER) {
			auto m = scene->getFirstMamouthInRange(getPosition(), 300);
			if(!m)
				return;
			Projectile::create("res/spear.png", getPositionX(), getPositionY(), 20, (m->getPosition() - getPosition()).getAngle(), scene);
		}
		else if(type == Guy::GuyType::HEAL) {
			auto g = scene->getGuyWithin(getPosition(), 100);
			for(auto a : g)
				a->healBy(30);
		}
	}, 2.f, "attack");
}