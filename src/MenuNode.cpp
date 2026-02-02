#include "MenuNode.h"
namespace
{
	int mainLimit = 2;
	int LevelSelectLimit = 6;
	int SettingsLimit = 5;
}

namespace Node
{

	MenuNode::MenuNode()
	{
		playGame = -1;
		menuLevel = MAIN;
		menuSelect = 0;
	}
	MenuNode::~MenuNode()
	{

	}

	void MenuNode::update(float deltaTime)
	{

	}
	void MenuNode::fixedUpdate(float deltaTime)
	{
		auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
		auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
		auto stick = joypad_get_inputs(JOYPAD_PORT_1);

		if (btn.d_down) menuSelect++;
		if (btn.d_up) menuSelect--;
		switch (menuLevel)
		{
		case Node::MAIN:
			if (menuSelect >= mainLimit) menuSelect = mainLimit - 1;
			if (menuSelect < 0) menuSelect = 0;
			break;
		case Node::LEVELSELECT:
			if (menuSelect >= LevelSelectLimit) menuSelect = LevelSelectLimit - 1;
			if (menuSelect < 0) menuSelect = 0;
			break;
		case Node::SETTINGS:
			if (menuSelect >= SettingsLimit) menuSelect = SettingsLimit - 1;
			if (menuSelect < 0) menuSelect = 0;
			break;
		default:
			break;
		}

		if (btn.z)
		{
			changeState();
		}
		else if ((btn.r || btn.l) && menuLevel != MAIN)
		{
			returnState();
		}

	}

	void MenuNode::drawMenu()
	{
		t3d_debug_printf(20, 30 + 10*menuSelect, "O");
		switch (menuLevel)
		{
		case Node::MAIN:
			t3d_debug_printf(40, 20, "Game Menu");
			t3d_debug_printf(40, 30, "Start Game");
			t3d_debug_printf(40, 40, "Level Select");
			//t3d_debug_printf(40, 50, "Settings");
			break;
		case Node::LEVELSELECT:
			t3d_debug_printf(40, 20, "Level Select Menu");
			t3d_debug_printf(40, 30, "Level 0: Tutorial");
			t3d_debug_printf(40, 40, "Level 1: Jump!");
			t3d_debug_printf(40, 50, "Level 2: Ramp");
			t3d_debug_printf(40, 60, "Level 3: Half-pipe");
			t3d_debug_printf(40, 70, "Level 4: Bowl");
			t3d_debug_printf(40, 80, "Level 5: Finale");
			break;
		case Node::SETTINGS:
			t3d_debug_printf(40, 20, "Settings Menu");
			break;
		default:
			break;
		}

		t3d_debug_printf(40, 170, "Use D-pad to move cursor");
		t3d_debug_printf(40, 180, "Use Z to select");
		t3d_debug_printf(40, 190, "Use L to return");
		
	}

	void MenuNode::changeState()
	{
		switch (menuLevel)
		{
		case Node::MAIN:
			switch (menuSelect)
			{
			case 0:
				playGame = 0;
				//menuLevel = MAIN;
				//menuSelect = 0;
				break;
			case 1:
				menuLevel = LEVELSELECT;
				menuSelect = 0;
				break;
			case 2:
				menuLevel = SETTINGS;
				menuSelect = 0;
				break;
			default:
				break;
			}
			break;
		case Node::LEVELSELECT:
			playGame = menuSelect;
			break;
		case Node::SETTINGS:
			
			break;
		default:
			break;
		}
	}
	void MenuNode::returnState()
	{
		menuLevel = MAIN;
		menuSelect = 0;
	}


}