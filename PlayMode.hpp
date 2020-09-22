#include "Mode.hpp"

#include "Scene.hpp"

#include "LevelMap.hpp"

#include <glm/glm.hpp>
#include <json.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode(int level_idx);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

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

	glm::quat player_base_rotation;
	glm::vec3 player_base_position;
	glm::vec3 camera_base_position;

	// float wobble = 0.0f;
	float drot = 0.0f;
	float dmov = 0.0f;
	// float stand = -1.0f; // 1 if standing
	bool moving = false;
	int stance = 0; // 0 is along x axis, 1 is along y axis, 2 is along z axis

	glm::vec3 dirx = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 diry = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 dirz = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 axisx = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 axisy = glm::vec3(0.0f, 1.0f, 0.0f);
	// glm::vec3 axisz = glm::vec3(0.0f, 0.0f, 1.0f);

	// glm::vec3 axis = axisx;
	
	//camera:
	Scene::Camera *camera = nullptr;

};

