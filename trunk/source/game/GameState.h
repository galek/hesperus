/***
 * hesperus: GameState.h
 * Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#ifndef H_HESP_GAMESTATE
#define H_HESP_GAMESTATE

#include <source/gui/GUIComponent.h>
#include <source/util/UserInput.h>

namespace hesp {

//#################### TYPEDEFS ####################
typedef shared_ptr<class GameState> GameState_Ptr;

class GameState
{
	//#################### DESTRUCTOR ####################
public:
	virtual ~GameState();

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	virtual void enter() = 0;
	virtual void leave() = 0;
	virtual GameState_Ptr update(int milliseconds, UserInput& input) = 0;

	//#################### PROTECTED METHODS ####################
protected:
	void set_display(const GUIComponent_Ptr& display);
};

}

#endif
