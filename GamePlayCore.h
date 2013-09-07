#pragma once
#include <vector>

class BaseGameEntity;
class DragonContact;
struct DragonicPair;

class GamePlayCore
{
private:

public:
	GamePlayCore();
	~GamePlayCore();
	void ApplyGamePlayEffects(std::vector<BaseGameEntity*>& entities,std::vector<DragonicPair*>& contactPairs);
};