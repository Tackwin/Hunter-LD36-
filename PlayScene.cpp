#include "PlayScene.hpp"
#include "GameOver.hpp"


PlayScene::PlayScene() {
}

PlayScene::~PlayScene() {
}


cocos2d::Scene* PlayScene::createScene() {
	auto s = cocos2d::Scene::create();
	auto l = PlayScene::create();
	s->addChild(l);
	return s;
}

bool PlayScene::init() {
	if(!Layer::init()) return false;

	draw = cocos2d::DrawNode::create();

	auto sprite = cocos2d::Sprite::create("res/map.png");
	sprite->setAnchorPoint({ .0f, .0f });
	sprite->setPosition(cocos2d::Vec2::ZERO);
	addChild(sprite, -10);

	auto fadeSprite = cocos2d::Sprite::create("res/fade.png");
	fadeSprite->setOpacity(0);
	fadeSprite->setColor(cocos2d::Color3B(50, 50, 50));
	fadeSprite->setAnchorPoint({ .0f, .0f });
	fadeSprite->setScale(General::size.width, General::size.height);
	fadeSprite->setPosition(0, 0);
	addChild(fadeSprite, 10, "fadeSprite");


	auto labelInfo = cocos2d::Label::create("", "", 40, cocos2d::Size::ZERO, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
	labelInfo->setMaxLineWidth(300);
	labelInfo->setAnchorPoint({ .5f, .5f });
	labelInfo->setPosition(General::size / 2);
	labelInfo->setOpacity(0);
	addChild(labelInfo, 20, "labelInfo");

	auto labelMessage = cocos2d::Label::create("", "", 25, cocos2d::Size::ZERO, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
	labelMessage->setAnchorPoint({ .5f, .5f });
	labelMessage->setPosition(General::size.width / 2.f, 4.f * General::size.height / 5.f);
	labelMessage->setVisible(false);
	addChild(labelMessage, 5, "messages");

	auto previewSprite = cocos2d::Sprite::create("res/guy0.png");
	previewSprite->setAnchorPoint({ 0.5f, 0.1f });
	previewSprite->setVisible(true);
	addChild(previewSprite, 2, "preview");

	{
		auto label = cocos2d::Label::create(std::string("Lancier: ") + std::to_string(dispo[Guy::GuyType::LANCIER]), "", 32);
		label->setAnchorPoint({ 0.f, 0.f });
		label->setPosition(10, 10);

		addChild(label, 1, std::string("label") + std::to_string(Guy::GuyType::LANCIER));
	}

	{
		auto label = cocos2d::Label::create(std::string("Heal: ") + std::to_string(dispo[Guy::GuyType::HEAL]), "", 32);
		label->setAnchorPoint({ 0.f, 0.f });
		label->setPosition(10, 50);

		addChild(label, 1, std::string("label") + std::to_string(Guy::GuyType::HEAL));
	}

	nextMonth();
	/*for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			guys.push_back(Guy::create({ 100.f + i * 100.f, 200.f + j * 100.f }, this, Guy::GuyType::LANCIER));
		}
	}*/

	//mamouths.push_back(Mamouth::create({ General::size.width - 100.f, General::size.height / 2.f }, this));


	auto listenerMouse = cocos2d::EventListenerMouse::create();
	listenerMouse->onMouseDown = [&](cocos2d::EventMouse* e)->void {
		if(!placement) {
			singleSelection = false;
			for(auto a : guys) {
				if(a->getBoundingBox().containsPoint(e->getLocationInView())) {
					for(auto b : guys)
						b->focused = false;

					a->focused = true;
					singleSelection = true;
					return;
				}
			}
			startSelection = true;
			rectangleSelection.origin = e->getLocationInView();
		} else {
			if(dispo[placeType] > 0) {
				dispo[placeType]--;
				auto guy = Guy::create(e->getLocationInView(), this, placeType);
				addChild(guy);
				guys.push_back(guy);
				auto l = (cocos2d::Label*)getChildByName(std::string("label") + std::to_string(placeType));
				l->setString(l->getString().substr(0, l->getString().find(" ")) + " " + std::to_string(dispo[placeType]));

				int s = 0;
				for(int i = 0; i < 2; i++) {
					s += dispo[i];
				}
				if(s == 0) {
					placement = false;
					getChildByName("preview")->setVisible(false);
					for(auto a : guys) {
						a->startRound();
					}
					nmamouth = month;
					for(int i = 0; i < (nmamouth < 4 ? nmamouth : 4); i++) {
						mamouths.push_back(Mamouth::create({ General::size.width - 150, (i + 1) * General::size.height / ((nmamouth < 4 ? nmamouth : 4) + 1) }, this));
					}
					nmamouth -= 4;
				}
			} else {
				auto m = (cocos2d::Label*)getChildByName("messages");
				m->setString("You have no more soldier of that type"); 
				m->setVisible(true);
				if(m->isScheduled("hide")) {
					m->unschedule("hide");
				}
				m->schedule([&](float)->void {
					getChildByName("messages")->setVisible(false);
				}, 5.f, "hide");
			}
		}
	};
	listenerMouse->onMouseMove = [&](cocos2d::EventMouse* e)->void {
		if(placement) {
			getChildByName("preview")->setPosition(e->getLocationInView());
		}
		if(startSelection) {
			rectangleSelection.size = e->getLocationInView() - rectangleSelection.origin;
		}
	};
	listenerMouse->onMouseUp = [&](cocos2d::EventMouse* e)->void {
		if(!placement) {
			if(rectangleSelection.size.width < 0) {
				rectangleSelection.origin.x += rectangleSelection.size.width;
				rectangleSelection.size.width *= -1;
			}
			if(rectangleSelection.size.height < 0) {
				rectangleSelection.origin.y += rectangleSelection.size.height;
				rectangleSelection.size.height *= -1;
			}

			if(startSelection && ((cocos2d::Vec2)rectangleSelection.size).getLengthSq() > 40 * 40) {
				for(auto a : guys) {
					a->focused = rectangleSelection.containsPoint(a->getPosition());
				}

				startSelection = false;
				rectangleSelection.size = cocos2d::Vec2::ZERO;
				rectangleSelection.origin = cocos2d::Vec2::ZERO;
			} else if(!singleSelection) {
				auto d = (e->getLocationInView() - getCeneterOfSwarm());
				for(auto a : guys) {
					if(a->focused) {
						a->goThere(a->getPosition() + d);
						a->focused = false;
					}
				}
				startSelection = false;
			}
		}
	};
	listenerMouse->onMouseScroll = [&](cocos2d::EventMouse* e)->void {
		int i = placeType;
		i += e->getScrollY();
		i %= 2;
		i = abs(i);
		placeType = static_cast<Guy::GuyType>(i);

		((cocos2d::Sprite*)getChildByName("preview"))->setTexture("res/guy" + std::to_string(i) + ".png");
	};
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerMouse, this);


	addChild(draw);
	scheduleUpdate();
	return true;
}

void PlayScene::update(float dt) {
	for(int i = 0; i < guys.size(); i++) {
		if(guys[i]->health <= 0) {
			if(std::find(std::begin(clusteringGuy), std::end(clusteringGuy), guys[i]) != std::end(clusteringGuy)) {
				clusteringGuy.erase(std::find(std::begin(clusteringGuy), std::end(clusteringGuy), guys[i]));
			}

			removeChild(guys[i]);
			guys.erase(guys.begin() + i);
			i--;
			pop--;

			if(guys.empty() && mamouths.size() > 0) {
				General::director->replaceScene(cocos2d::TransitionFade::create(0.25f, GameOver::createScene()));
			}
		}
	}
	for(int i = 0; i < mamouths.size(); i++) {
		if(mamouths[i]->health <= 0) {
			removeChild(mamouths[i]);
			mamouths.erase(mamouths.begin() + i);
			meat++;
			i--;
			if(nmamouth > 0) {
				mamouths.push_back(Mamouth::create({ General::size.width - 150, General::size.height / 2 }, this));
				nmamouth--;
			}
			if(nmamouth <= 0 && mamouths.empty()) {
				nextMonth();

				for(int j = 0; j < guys.size(); j++) {
					removeChild(guys[j]);
					guys.erase(guys.begin() + j);
					j--;
				}
			}
		}
	}
	
	if(!placement) {
		draw->clear();
		if(startSelection) {
			draw->drawRect(rectangleSelection.origin, (cocos2d::Vec2)rectangleSelection.size + rectangleSelection.origin, cocos2d::Color4F::GREEN);
		}
	}
	else {

	}
}

cocos2d::Vec2 PlayScene::separationVec(const Guy* p, float range) {
	if(guys.size() <= 1)
		return 0;
	cocos2d::Vec2 result = cocos2d::Vec2::ZERO;
	float ln = logf(0.01f);
	for(auto a : guys) {
		if(a == p || a->getPosition().getDistanceSq(p->getPosition()) > range * range || a->destination != p->destination)
			continue;

		cocos2d::Vec2 dt = p->getPosition() - a->getPosition();
		result += dt;
	}
	result.x /= guys.size() + 1;
	result.y /= guys.size() + 1;
	return result;
}

float PlayScene::alignSteer(Guy* p, float range) {
	if(guys.size() <= 1)
		return 0;
	float ln = log((0.1f) / 0.9f);
	float result = 0;
	for(auto a : guys) {
		if(a == p || a->getPosition().getDistanceSq(p->getPosition()) > range * range || a->destination != p->destination)
			continue;
		float dt = a->direction;
		result += dt;
	}
	result /= guys.size() - 1;
	//result *= 1 / (1 + expf(ln * result / range));
	return result;
}

cocos2d::Vec2 PlayScene::cohesionVec(const Guy* p, float range) {
	if(guys.size() <= 1)
		return 0;
	cocos2d::Vec2 avg = cocos2d::Vec2::ZERO;
	for(auto a : guys) {
		if(a == p || a->getPosition().getDistanceSq(p->getPosition()) > range * range || a->destination != p->destination)
			continue;
		cocos2d::Vec2 dt = a->getPosition() - p->getPosition();
		avg += dt;
	}
	avg.x /= guys.size() - 1;
	avg.y /= guys.size() - 1;
	//avg.scale(1 / (1 + expf(ln * avg.getLength() / range)));
	return avg;
}


cocos2d::Vec2 PlayScene::getCeneterOfSwarm() {
	cocos2d::Vec2 result = cocos2d::Vec2::ZERO;
	int n = 0;
	for(auto a : guys) {
		if(a->focused) {
			result += a->getPosition();
			n++;
		}
	}
	if(n == 0)
		return cocos2d::Vec2::ZERO;
	result.x /= n;
	result.y /= n;
	return result;
}

Mamouth* PlayScene::getTouched(const cocos2d::Rect &rect) {
	for(auto a : mamouths) {
		if(a->getBoundingBox().intersectsRect(rect))
			return a;
	}
	return nullptr;
}

Mamouth* PlayScene::getFirstMamouthInRange(const cocos2d::Vec2 &p, float range) {
	for(auto a : mamouths) {
		if(a->getPosition().getDistanceSq(p) < range * range)
			return a;
	}
	return nullptr;
}

cocos2d::Vec2 PlayScene::mostCrowdedCluster(float r) {
	long long bestI = 0;
	cocos2d::Vec2 best = cocos2d::Vec2();

	if(mamouthServed == 0 || clusteringGuy.empty()) {
		clusteringGuy = guys;
	}

	std::vector<Guy*> bestCluster;

	int w = 30;
	int h = 30;
	for(int i = 0; i < w; i++) {
		for(int j = 0; j < h; j++) {
			cocos2d::Vec2 current(i * General::size.width / (w + 1) + General::size.width / w,
				j * General::size.height / (h + 1) + General::size.height / h);

			std::vector<Guy*> temp;
			long long t = 0;
			for(int k = 0; k < clusteringGuy.size(); k++) {
				if(clusteringGuy[k]->type < 0) {
					clusteringGuy.erase(clusteringGuy.begin() + k);
					k--;
					continue;
				}
				if(clusteringGuy[k]->getPosition().getDistance(current) < r) {
					t++;
					temp.push_back(clusteringGuy[k]);
				}
			}
			if(t > bestI) {
				bestI = t;
				best = current;

				bestCluster = temp;
			}
		}
	}

	for(int k = 0; k < clusteringGuy.size();k++) {
		if(std::find(std::begin(bestCluster), std::end(bestCluster), clusteringGuy[k]) != std::end(bestCluster)) {
			cocos2d::log("erased, size %d", clusteringGuy.size());
			clusteringGuy.erase(clusteringGuy.begin() + k);
			k--;
		}
	}

	mamouthServed++;
	mamouthServed %= mamouths.size();
	return best;
}

void PlayScene::dealDamage(const cocos2d::Vec2 &p, float range, float damage) {
	for(auto a : guys) {
		if(a->getPosition().getDistanceSq(p) < range * range) {
			a->takeDamage(damage);
		}
	}

	for(int i = 0; i < 10; i++) {
		auto sprite = cocos2d::Sprite::create("res/choc.png");
		float a = cocos2d::RandomHelper::random_real<float>(0, 2 * 3.1415962);
		cocos2d::Vec2 o(cosf(a), sinf(a));
		o.scale(cocos2d::RandomHelper::random_real<float>(0, 60));
		o.x *= 150 / 60;
		sprite->setAnchorPoint({ 0.5f, 0.5f });
		sprite->setScale(cocos2d::RandomHelper::random_real<float>(0, 2));
		sprite->setRotation(a);
		sprite->setPosition(p + o);
		sprite->runAction(cocos2d::Sequence::createWithTwoActions(
			cocos2d::Spawn::createWithTwoActions(
				cocos2d::Sequence::createWithTwoActions(
					cocos2d::MoveBy::create(0.1f, { 0, 30 }),
					cocos2d::MoveBy::create(0.1f, { 0, -30 })
				),
				cocos2d::MoveBy::create(0.2f, { cosf(a) * 50, sinf(a) * 50 })
			),
			cocos2d::CallFunc::create([sprite]()->void {
				sprite->removeFromParent();
			})
		));
		addChild(sprite);
	}
}

void PlayScene::getDispo() {
	dispo[Guy::GuyType::LANCIER] = 35 * ( 2 / ( 1 + expf( -powf(pop, 1.05f) / 35 ) ) - 1);
	dispo[Guy::GuyType::HEAL] = 50 * (2 / ( 1 + expf(-pop / 500) ) - 1);
}


std::vector<Guy*> PlayScene::getGuyWithin(const cocos2d::Vec2 &p, float r) {
	std::vector<Guy*> g;
	for(auto a : guys) {
		if(a->getPosition().getDistanceSq(p) < r * r && a->getPosition() != p)
			g.push_back(a);
	}
	return g;
}


void PlayScene::nextMonth() {
	unscheduleUpdate();
	month++;
	pop += 10 * meat;
	meat = 0;

	getDispo();

	placement = true;
	getChildByName("preview")->setVisible(true);

	std::string str = "Month: " + std::to_string(month);
	str += "\nPopulation: " + std::to_string((int)pop);
	str += "\nLancer: " + std::to_string(dispo[Guy::GuyType::LANCIER]);
	str += "\nHealer: " + std::to_string(dispo[Guy::GuyType::HEAL]);
	auto l = (cocos2d::Label*)getChildByName("labelInfo");
	l->setString(str);
	getChildByName("fadeSprite")->runAction(cocos2d::Sequence::create(
		cocos2d::FadeIn::create(0.25f),
		cocos2d::DelayTime::create(1.f),
		cocos2d::FadeOut::create(0.25f),
		nullptr
	));
	l->runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(0.25f),
		cocos2d::FadeIn::create(0.25f),
		cocos2d::DelayTime::create(0.5f),
		cocos2d::FadeOut::create(0.25f),
		cocos2d::DelayTime::create(0.25f),
		nullptr
	));


	for(int i = 0; i < 2; i++) {
		auto l = (cocos2d::Label*)getChildByName(std::string("label") + std::to_string(i));
		l->setString(l->getString().substr(0, l->getString().find(" ")) + " " + std::to_string(dispo[i]));
	}
	scheduleOnce([&](float dt)->void {scheduleUpdate();}, 2.f, "restartupdate");
}
