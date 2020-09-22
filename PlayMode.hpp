#include "Mode.hpp"

#include "Scene.hpp"

#include "LevelMap.hpp"

#include <glm/glm.hpp>

#include <array>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode(int level_idx);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	bool offmap(std::vector<int> pos);
	void end_move();
	std::vector<int> next_pos(std::vector<int> pos, glm::vec3 move);

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
	std::vector<Scene::Transform*> coins_transforms;

	glm::quat player_base_rotation;
	glm::vec3 player_base_position;
	glm::vec3 camera_base_position;

	std::vector<int> pos{1,2,2,2};
	std::vector<int> newpos{1,2,2,2};

	struct LevelMap {
		struct Plane {
			// width-major
			Plane(int depth, int width, const std::vector<int> &content) : depth(depth), width(width),
																		content(content) {
				assert(content.size() == depth * width);
			}
			int depth;
			int width;
			// GetTileType: 0 if there is no tile here. 1 if there is tile here
			int GetTileType(int depth_idx, int width_idx) const {
				int idx = depth_idx * width + width_idx;
				if (depth_idx >= depth || depth_idx < 0 ||
					width_idx >= width || width_idx < 0) {
					return 0;
				}
				return content.at(idx);
			}
		private:
			std::vector<int> content;
		};

		LevelMap(const Plane &floor, const std::optional<Plane> &rightWall, const std::vector<glm::uvec3> &coinsPos)
				: floor(floor), right_wall(rightWall), coins_pos(coinsPos) {}
		Plane floor;
		std::optional<Plane> right_wall;
		std::vector<glm::uvec3> coins_pos;
	};

	inline struct LevelMap generate_mock_level_map() {
		auto floor = LevelMap::Plane(4, 8,
									{0, 0, 0, 0, 1, 1, 1, 1,
									1, 1, 1, 1, 1, 0, 0, 0,
									1, 1, 1, 1, 1, 0, 0, 0,
									1, 1, 1, 0, 0, 0, 0, 0});
		auto coins_pos = std::vector<glm::uvec3>{{3, 0, 0}};

		LevelMap m(floor, std::nullopt, coins_pos);
		return m;
	}

	// float wobble = 0.0f;
	float drot = 0.0f;
	float dmov = 0.0f;
	// float stand = -1.0f; // 1 if standing
	bool moving = false;
	int stance = 0; // 0 is along x axis, 1 is along y axis, 2 is along z axis

	int coinFound = 0;

	LevelMap map = generate_mock_level_map();

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

