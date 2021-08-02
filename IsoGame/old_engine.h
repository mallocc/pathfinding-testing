#pragma once

#include "glm.h"
#include "mesh.h"
#include <stdio.h>
#include <map>
#include <vector>
#include "pathfinder.h"

#define PASSIVE 0
#define DEFENSIVE 1
#define AGGRESSIVE 2

#define IDLE 0
#define WALK_RIGHT 1
#define WALK_LEFT 2
#define WALK_UP 3
#define WALK_DOWN 4
#define ATTACK_MELEE 5
#define ATTACK_SPELL 6
#define ATTACK_BOW 7

typedef int EntityID;

struct Entity
{
	glm::vec2 pos;
	float depth;
	EntityID id = -1;
	std::vector<Mesh*> meshs;
	int current_mesh = 0;
	float fr = 0.7f;
	bool visible = true;
	
	Entity(EntityID id, glm::vec2 pos, Mesh* mesh, float depth)
	{
		this->id = id;
		this->pos = pos;
		add_mesh_animation(mesh);
		this->depth = depth;
	}

	void update()
	{				
		for(Mesh* m : meshs)
			m->pos = glm::vec3(pos.x, pos.y,0);
	}

	bool is_within(glm::vec2 p)
	{
		glm::vec2 t = p - pos + glm::vec2(meshs[current_mesh]->scale / 2.0f);
		return t.x >= 0 && t.x <= meshs[current_mesh]->scale.x && t.y >= 0 && t.y <= meshs[current_mesh]->scale.y;
	}

	Entity * add_mesh_animation(Mesh* mesh)
	{
		meshs.push_back(mesh);
		return this;
	}

	void set_current_mesh(int current_mesh)
	{
		//this->current_mesh = current_mesh;
	}

	Mesh* get_current_mesh()
	{
		return meshs[current_mesh];
	}
};

struct Attack
{
	std::string name = "N/A";
	int damage = 0;
	int cooldown = 100;
	int current_cooldown = cooldown;

	int attack_type = ATTACK_MELEE;

	Attack() {}
	Attack(int damage, std::string name, int attack_type)
	{
		this->damage = damage;
		this->name = name;
		this->attack_type = attack_type;
	}

	int update_cooldown()
	{
		if(current_cooldown > 0)
			current_cooldown--;
		return current_cooldown;
	}

	void reset_cooldown()
	{
		current_cooldown = cooldown;
	}

	bool on_cooldown()
	{
		return current_cooldown > 0;
	}
};

struct HealthBar
{
	Entity * back;
	Entity * front;

	HealthBar() {}
	HealthBar(Entity * front, Entity * back)
	{
		this->back = back;
		this->front = front;
	}

	void update(float value)
	{
		front->meshs.back()->scale.x = back->meshs.back()->scale.x * value;
	}

	void set_pos(glm::vec2 pos)
	{
		back->pos = pos;
		front->pos = pos;
	}

};

struct NPC
{
	Entity * e;
	HealthBar health_bar;
	glm::vec2 target;	
	std::vector<glm::vec2> path;
	int current_path_node = -1;

	std::string name = "Unnamed";
	Attack current_attack = Attack(1, "punch", ATTACK_MELEE);

	std::string dialogue = "i do not wish to speak with you";

	int stance = PASSIVE;
	int max_health = 10, health = max_health;

	NPC(Entity * e, HealthBar health_bar)
	{
		this->e = e;
		this->health_bar = health_bar;
	}

	void update()
	{
		current_attack.update_cooldown();
		if (current_path_node >= 0)
		{
			glm::vec2 to_target = target - e->pos;
			if (glm::length(to_target) > 2.0f)
			{
				to_target = glm::normalize(to_target) + 1.0f / (float)INT_MAX;
				e->pos += to_target * glm::vec2(1, 1) * 2.0f;
			}
			else
			{
				current_path_node++;
				if (current_path_node == path.size())
				{
					current_path_node = -1;
					return;
				}
				target = path[current_path_node];
			}
		}
		health_bar.set_pos(e->pos + glm::vec2(0.0f,e->meshs.back()->scale.y));
		health_bar.update(health / (float)max_health);
	}

	void set_path(std::vector<glm::vec2> path)
	{
		if (path.size() > 1)
		{
			this->path = path;
			current_path_node = 0;
			target = path[current_path_node];
		}
	}

	bool move_to(glm::vec2 target, WalkMap map)
	{
		if (!map.is_outside(target))
		{
			set_path(get_walkmap_path(e->pos, target, map));
			return true;
		}
		return false;
	}

	bool is_dead()
	{
		return health <= 0;
	}

	void cast_on(Attack enemy_attack)
	{
		switch (enemy_attack.attack_type)
		{
		case ATTACK_MELEE:
			health -= enemy_attack.damage;
			break;
		case ATTACK_SPELL:
			break;
		case ATTACK_BOW:
			break;
		}
	
		if (is_dead())
			printf("%s has been killed\n", name.c_str());
	}

	void talk_to()
	{
		printf("%s: \'%s\'\n", name.c_str(), dialogue.c_str());
	}
};

struct NPCManager
{
	std::map<EntityID, NPC*> npcs;

	NPC * add_npc(Entity * e, HealthBar health_bar)
	{
		NPC * npc = new NPC(e, health_bar);
		npcs.insert({ npc->e->id, npc });
		return npc;
	}

	void update_npcs()
	{
		for (auto& sm_pair : npcs)
			sm_pair.second->update();
	}

	NPC * get_npc_click(glm::vec2 p)
	{
		for (auto& sm_pair : npcs)
		{
			if (sm_pair.second->e->is_within(p))
			{
				return sm_pair.second;
			}
		}
		return nullptr;
	}

};

struct TargetIcon
{
	Entity * red_icon;
	Entity * green_icon;
	glm::vec2 pos;
	bool green_on = true;

	TargetIcon(Entity * red_icon, Entity * green_icon)
	{
		this->red_icon = red_icon;
		this->green_icon = green_icon;
		set_pos(pos);
		set_visible(false);
	}

	void set_pos(glm::vec2 pos)
	{
		red_icon->pos = pos;
		green_icon->pos = pos;
		this->pos = pos;
	}

	void set_visible(bool visible)
	{
		if (!visible)
		{
			red_icon->visible = false;
			green_icon->visible = false;
		}
		else
		{
			green_icon->visible = false;
			red_icon->visible = false;
			if (green_on)
				green_icon->visible = true;
			else
				red_icon->visible = true;
		}
	}

	void set_green_on()
	{
		green_on = true;
	}
	void set_red_on()
	{
		green_on = false;
	}
};

struct PlayerManager
{
	Entity * e;
	Entity * target_icon_move;
	Entity * target_icon_attack;
	HealthBar health_bar;
	glm::vec2 target;
	std::vector<glm::vec2> path;
	int current_path_node = -1;

	WalkMap current_walkmap;

	std::string name = "Unnamed";
	Attack current_attack = Attack(1, "punch", ATTACK_MELEE);

	int interact_range = 100;
	int combat_range = 100;
	
	NPC * targeted_npc = nullptr;
	bool attacking = false;

	float move_speed = 1.0f;

	int max_health = 10, health = max_health;

	PlayerManager() {}
	PlayerManager(Entity * e, Entity * target_icon_move, Entity * target_icon_attack, HealthBar health_bar)
	{
		this->e = e;
		this->target_icon_move = target_icon_move;
		this->target_icon_attack = target_icon_attack;
		this->health_bar = health_bar;
	}

	void update()
	{
		current_attack.update_cooldown();

		if (current_path_node >= 0)
		{
			glm::vec2 to_target = target - e->pos;
			if (glm::length(to_target) > 2.0f)
			{
				to_target = glm::normalize(to_target) + 1.0f / (float)INT_MAX;
				e->pos += to_target * move_speed;
				if (fabs(to_target.x) > fabs(to_target.y))
				{
					if (to_target.x > 0)
						e->set_current_mesh(WALK_RIGHT);
					else
						e->set_current_mesh(WALK_LEFT);
				}
				else
				{
					if (to_target.y > 0)
						e->set_current_mesh(WALK_UP);
					else
						e->set_current_mesh(WALK_DOWN);
				}
			}
			else
			{				
				current_path_node++;
				if (current_path_node == path.size())
				{
					e->set_current_mesh(IDLE);
					target_icon_move->visible = false;					
					current_path_node = -1;
					return;
				}
				target = path[current_path_node];
			}
		}	

		if(false)
		if (targeted_npc != nullptr)
		{
			//if (current_path_node >= 0 && targeted_npc->e->pos != path.back())
			//	move_to(targeted_npc->e->pos);

			if (attacking)
			{
				if (glm::length(targeted_npc->e->pos - e->pos) <= combat_range)
					attack();
				target_icon_attack->pos = targeted_npc->e->pos;
			}
			else
			{
				if (glm::length(targeted_npc->e->pos - e->pos) <= interact_range)
					talk_to();
			}
		}

		health_bar.set_pos(e->pos);
		health_bar.update(health / (float)max_health);
	}

	void set_path(std::vector<glm::vec2> path)
	{
		if (path.size() > 1)
		{
			this->path = path;
			current_path_node = 0;
			target = path[current_path_node];
			target_icon_move->pos = path[path.size() - 1];
			target_icon_move->visible = true;
		}
	}

	void set_walkmap(WalkMap map)
	{
		this->current_walkmap = map;
	}

	bool move_to(glm::vec2 target)
	{
		if (!current_walkmap.is_outside(target))
		{
			set_path(get_walkmap_path(e->pos, target, current_walkmap));
			return true;
		}
		return false;
	}

	void attack()
	{
		targeted_npc = targeted_npc;
		attacking = true;
		target_icon_attack->visible = true;
		if (glm::length(targeted_npc->e->pos - e->pos) <= combat_range)
		{
			if (!targeted_npc->is_dead())
			{
				if (!current_attack.on_cooldown())
				{
					current_attack.reset_cooldown();
					targeted_npc->cast_on(current_attack);
				}
			}
		}
		else
		{
			move_to(targeted_npc->e->pos);
		}
	}
	void attack(NPC * npc)
	{
		targeted_npc = npc;
		attacking = true;
		target_icon_attack->visible = true;
		if (glm::length(targeted_npc->e->pos - e->pos) <= combat_range)
		{
			if (!targeted_npc->is_dead())
			{
				if (!current_attack.on_cooldown())
				{
					current_attack.reset_cooldown();
					targeted_npc->cast_on(current_attack);
				}
				else
					printf("%s is on cooldown\n", current_attack.name.c_str());
			}
			else
				printf("target is already dead!\n");
		}
		else
		{
			move_to(targeted_npc->e->pos);
		}
	}

	void talk_to()
	{
		targeted_npc = targeted_npc;
		attacking = false;		
		if (glm::length(targeted_npc->e->pos - e->pos) <= interact_range)
		{
			if (!targeted_npc->is_dead())
				targeted_npc->talk_to();
		}
		else
		{
			move_to(targeted_npc->e->pos);
		}
	}
	void talk_to(NPC * npc)
	{
		targeted_npc = npc;
		attacking = false;
		if (glm::length(targeted_npc->e->pos - e->pos) <= interact_range)
		{
			if (!targeted_npc->is_dead())
				targeted_npc->talk_to();
			else
				printf("target looks dead...\n");
		}
		else
		{
			move_to(targeted_npc->e->pos);
		}
	}

	void on_click(NPCManager * npc_manager, glm::vec2 p)
	{
		target_icon_attack->visible = false;
		NPC * npc = npc_manager->get_npc_click(p);
		if (npc != nullptr)
		{
			switch (npc->stance)
			{
			case PASSIVE:
				talk_to(npc);
				break;
			case DEFENSIVE:
				break;
			case AGGRESSIVE:
				attack(npc);
				break;
			default:
				printf("%s doesnt have a stance\n", npc->name.c_str());
			}
		}
		else
		{
			targeted_npc = nullptr;
			move_to(p);
		}
	}
};


struct Engine
{
	float dt = 1.0f;

	int entities_created = 0;
	std::map<EntityID, Entity * > entities;

	Engine()
	{
	}

	Entity * add_entity(glm::vec2 pos, Mesh * mesh, float depth)
	{
		entities_created++;
		Entity * e = new Entity(entities_created, pos, mesh, depth);
		entities.insert({ e->id, e });
		return e;
	}

	Entity * get_entity(EntityID id)
	{
		return entities[id];
	}

	void update_entities()
	{
		for (auto& sm_pair : entities)
			sm_pair.second->update();
	}

	void draw_entity_meshes(VarHandle * model, VarHandle * tex)
	{
		for (auto& sm_pair : entities)
			if (sm_pair.second != nullptr)
			if(sm_pair.second->visible)
				if (sm_pair.second->get_current_mesh() != nullptr) 
					sm_pair.second->get_current_mesh()->draw(0, model,tex);
	}

};