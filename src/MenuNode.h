#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3ddebug.h>
#include <vector>
#include "Node.h"



namespace Node
{
	enum menuState { MAIN, LEVELSELECT, SETTINGS };
	//class CollisionShape;

	class MenuNode : public NodeBase
	{
	private:

		void changeState();
		void returnState();

	protected:

	public:
		menuState menuLevel;
		int menuSelect;

		int playGame;

		MenuNode();
		~MenuNode();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		void drawMenu();
	};
}