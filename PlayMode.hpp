#include "Mode.hpp"

#include "Scene.hpp"

#include "LevelMap.hpp"

#include <glm/glm.hpp>

#include <array>

#include <vector>
#include <deque>
#include <set>

struct PlayMode : Mode {
	PlayMode(int level_idx);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual bool update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	bool offmap(std::pair<glm::ivec3, glm::ivec3> pos);
	void end_move();
	void to_wall();
	void to_floor();
	std::pair<glm::ivec3, glm::ivec3> next_pos(glm::ivec3 pos1, glm::ivec3 pos2, glm::vec3 op);


	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	LevelMap level_map;

	//hexapod leg to wobble:
	Scene::Transform *player = nullptr;
	std::vector<Scene::Transform*> coins_transforms;

	glm::quat player_base_rotation;
	glm::vec3 player_base_position;
	glm::quat camera_base_rotation;
	glm::vec3 camera_base_position;

	glm::ivec3 pos1 = level_map.player.pos1;
	glm::ivec3 pos2 = level_map.player.pos2;
	std::pair<glm::ivec3, glm::ivec3> newpos;

	// float wobble = 0.0f;
	float drot = 0.0f;
	float dmov = 0.0f;
	// float stand = -1.0f; // 1 if standing
	bool moving = false;
	bool portaling = false;
	float portaldir = 0.0f;
	int stance = level_map.player.stance; // 0 is along x axis, 1 is along y axis, 2 is along z axis
	glm::ivec2 portalto;
	
	int coinFound = -1;
	std::set<int> collected_coins;

	glm::vec3 dirx = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 diry = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 dirz = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 axisx = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 axisy = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 axisz = glm::vec3(0.0f, 0.0f, -1.0f);

	// glm::vec3 axis = axisx;
	bool wall = false;
	//camera:
	Scene::Camera *camera = nullptr;



};

