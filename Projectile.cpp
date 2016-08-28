#include "Projectile.hpp"
#include "PlayScene.hpp"
#include "Mamouth.hpp"

Projectile::Projectile() {
}

Projectile::~Projectile() {
}

Projectile* Projectile::create(const std::string &path, float x, float y, float damage, float angle, PlayScene* scene) {
	Projectile* p = new(std::nothrow)Projectile();
	if(p && p->initWithFile(path)) {
		p->scene = scene;
		p->autorelease();
		p->initOptions(x, y, damage, angle);
		return p;
	}
	CC_SAFE_DELETE(p);
	return p;
}

void Projectile::initOptions(float x, float y, float damage, float angle) {
	setAnchorPoint({ .5f, .5f });
	setRotation(-180 * angle / 3.1415962f );
	setPosition(x, y);
	this->damage = damage;
	this->angle = angle;
	scene->addChild(this);
	scheduleUpdate();
}

void Projectile::update(float dt) {
	setPosition(getPosition() + cocos2d::Vec2(cosf(angle), sinf(angle)) * dt * 600);

	auto m = scene->getTouched(getBoundingBox());
	if(m) {
		m->health -= damage;
		removeFromParent();
	}
}
