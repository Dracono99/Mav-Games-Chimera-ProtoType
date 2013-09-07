#include <vector>
#include "FSMnStates.h"

class Menu
{
private:
	std::vector<state*> m_vMenuSates;
	stateMachine<Menu>* m_pStateMachine;
	// returns true if state exists and the integer i passed in tells the idx in the state vector its located
	// if it doesn't exist it returns false;
	bool doesMenuStateExist(int menuState,int& i);
public:
	Menu();
	~Menu();
	void SwitchMenuState(int menuStateId);
	void Enter();
	void Update( float dt);
	void Render();
	void OnResetDevice();
	void OnLostDevice();
	void Exit();
};
