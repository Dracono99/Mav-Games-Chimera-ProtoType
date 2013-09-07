#pragma once
#include <cassert>
#include <Windows.h>
template <class entity_type>
class State
{
public:
	virtual void Enter(entity_type*)=0;
	virtual void Update(entity_type*, float dt)=0;
	virtual void Render(entity_type*)=0;
	virtual void GetInput(entity_type*,RAWINPUT* raw)=0;
	virtual void OnResetDevice(entity_type*)=0;
	virtual void OnLostDevice(entity_type*)=0;
	virtual void Exit(entity_type*)=0;
	virtual ~State(){}
};

class state
{
public:
	int mStateID;
	virtual void Enter()=0;
	virtual void Update(float dt)=0;
	virtual void Render()=0;
	virtual void OnResetDevice()=0;
	virtual void OnLostDevice()=0;
	virtual void Exit()=0;
	virtual ~state(){}
};

template <class entity_type>
class StateMachine
{
private:
	//a pointer to the agent that owns this instance
	entity_type* m_pOwner;
	State<entity_type>* m_pCurrentState;
	//a record of the last state the agent was in
	State<entity_type>* m_pPreviousState;
	//this state logic is called every time the FSM is updated
	State<entity_type>* m_pGlobalState;
public:
	StateMachine(entity_type* owner):m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}
	//use these methods to initialize the FSM
	void SetCurrentState(State<entity_type>* s){m_pCurrentState = s;}
	void SetGlobalState(State<entity_type>* s) {m_pGlobalState = s;}
	void SetPreviousState(State<entity_type>* s){m_pPreviousState = s;}
	//call this to update the FSM
	void Update(float dt)const
	{
		//if a global state exists, call its execute method
		if (m_pGlobalState) m_pGlobalState->Update(m_pOwner,dt);
		//same for the current state
		if (m_pCurrentState) m_pCurrentState->Update(m_pOwner,dt);
	}
	//change to a new state
	void ChangeState(State<entity_type>* pNewState)
	{
		assert(pNewState &&
			"<StateMachine::ChangeState>: trying to change to a null state");
		//keep a record of the previous state
		m_pPreviousState = m_pCurrentState;
		//call the exit method of the existing state
		if(m_pCurrentState!=NULL)
		{
			m_pCurrentState->Exit(m_pOwner);
		}
		//change state to the new state
		m_pCurrentState = pNewState;
		//call the entry method of the new state
		m_pCurrentState->Enter(m_pOwner);
	}
	//change state back to the previous state
	void RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}
	//accessors
	State<entity_type>* CurrentState() const{return m_pCurrentState;}
	State<entity_type>* GlobalState() const{return m_pGlobalState;}
	State<entity_type>* PreviousState() const{return m_pPreviousState;}
	//returns true if the current state’s type is equal to the type of the
	//class passed as a parameter.
	bool isInState(const State<entity_type>& st)const;
};

template <class entity_type>
class stateMachine
{
private:
	//a pointer to the agent that owns this instance
	entity_type* m_pOwner;
	state* m_pCurrentState;
	//a record of the last state the agent was in
	state* m_pPreviousState;
	//this state logic is called every time the FSM is updated
	state* m_pGlobalState;
public:
	stateMachine(entity_type* owner):m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}
	//use these methods to initialize the FSM
	void SetCurrentState(state* s){m_pCurrentState = s;}
	void SetGlobalState(state* s) {m_pGlobalState = s;}
	void SetPreviousState(state* s){m_pPreviousState = s;}
	//call this to update the FSM
	void Update(float dt)const
	{
		//if a global state exists, call its execute method
		if (m_pGlobalState) m_pGlobalState->Update(dt);
		//same for the current state
		if (m_pCurrentState) m_pCurrentState->Update(dt);
	}
	//change to a new state
	void ChangeState(state* pNewState)
	{
		assert(pNewState &&
			"<StateMachine::ChangeState>: trying to change to a null state");
		//keep a record of the previous state
		m_pPreviousState = m_pCurrentState;
		//call the exit method of the existing state
		if(m_pCurrentState!=NULL)
		{
			m_pCurrentState->Exit();
		}
		//change state to the new state
		m_pCurrentState = pNewState;
		//call the entry method of the new state
		m_pCurrentState->Enter();
	}
	//change state back to the previous state
	void RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}
	//accessors
	state* CurrentState() const{return m_pCurrentState;}
	state* GlobalState() const{return m_pGlobalState;}
	state* PreviousState() const{return m_pPreviousState;}
	//returns true if the current state’s type is equal to the type of the
	//class passed as a parameter.
	bool isInState(const state& st)const;
};
