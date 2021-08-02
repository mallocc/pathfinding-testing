#pragma once

#include "glm.h"
#include "mesh.h"
#include <stdio.h>
#include <map>
#include <vector>
#include "pathfinder.h"



class WorldObject
{
private:
	v2 pos;
	AnimatedMesh * mesh;

public:
	WorldObject(v2 pos, AnimatedMesh * mesh, float draw_depth)
	{
		this->pos = pos;
		this->mesh = mesh;
		this->mesh->pos.z = draw_depth;
	}

	AnimatedMesh * get_mesh()
	{
		return mesh;
	}

	v2 get_pos()
	{
		return pos;
	}

	void set_pos(v2 pos)
	{
		mesh->pos = glm::vec3(pos, mesh->pos.z);
	}

};

#define SPELL_TYPE_MELEE 0
#define SPELL_TYPE_RANGE 1

class Spell
{
private:
	char * name = "N/A";
	int damage = 0;
	float hit_rate = 0.0f;
	int type = SPELL_TYPE_MELEE;
	WorldObject * animation;

public:
	Spell(char * name, int damage, float hit_rate, int type, WorldObject * animation)
	{
		this->name = name;
		this->damage = damage;
		this->hit_rate = hit_rate;
		this->type = type;
		this->animation = animation;
	}

};
class SpellBook
{
private:
	std::vector<Spell> learnt_spells;
	Spell current_spell;

public:
	
	void learn_spell(Spell spell)
	{
		learnt_spells.push_back(spell);
	}

	Spell select_spell(int i)
	{
		return learnt_spells[i];
	}

	Spell get_current_spell()
	{
		return current_spell;
	}
};

class Item
{
private:
	char * name = "N/A";
	int weight = 0;
	int value = 0;
	WorldObject * on_character;
	WorldObject * on_floor;

public:
	Item(char * name, int weight, int value, WorldObject * on_character, WorldObject * on_floor)
	{
		this->name = name;
		this->weight = weight;
		this->value = value;
		this->on_character = on_character;
		this->on_floor = on_floor;
	}
};
class Consumable : Item
{
	Consumable(char * name, int weight, int value, WorldObject * on_character, WorldObject * on_floor)
		: Item(name, weight, value, on_character, on_floor)
	{
		
	}
};
class Wearable : Item
{
	Wearable(char * name, int weight, int value, WorldObject * on_character, WorldObject * on_floor)
		: Item(name, weight, value, on_character, on_floor)
	{

	}
};

class Inventory
{
private:
	std::vector<Item*> items;
	int max_size = 1;
public:
	Inventory(int max_size)
	{
		this->max_size = max_size;
	}
	
	void deposit_item(Item * item)
	{
		items.push_back(item);
	}

	Item * withdraw_item(int i)
	{
		return items[i];
	}
};

class CoinPouch
{
private:
	int amount = 0;
public:
	CoinPouch(int amount)
	{
		this->amount = amount;
	}

	void deposit_money(int amount)
	{
		this->amount += amount;
	}

	bool withdraw_amount(int amount)
	{
		if (amount < this->amount)
			return false;
		this->amount -= amount;
		return true;
	}
};


class Dialogue
{
public:
	char * statement = "null statement";
	char * response = "null response";
	bool terminator = true;
	std::vector<Dialogue*> sub_dialogues;

	Dialogue(char * statement, char * response, bool terminator)
	{
		this->statement = statement;
		this->response = response;
		this->terminator = terminator;
	}

	Dialogue * add_dialogue(char * statement, char * response, bool terminator)
	{
		Dialogue * new_dialogue = new Dialogue(statement, response, terminator);
		sub_dialogues.push_back(new_dialogue);
		return this;
	}

	Dialogue * add_dialogue(Dialogue * sub_dialogue)
	{
		sub_dialogues.push_back(sub_dialogue);
		return this;
	}
};


#define NPC_IDLE 0
#define NPC_ATTACK 1
#define NPC_WALK 2
#define NPC_DEAD 3
#define NPC_NUM 4

class NPC
{
private:
	AnimatedMesh animations[4];
	SpellBook spell_book;
	Inventory inventory;
	CoinPouch coin_pouch;
	Dialogue dialogue;

public:

};

