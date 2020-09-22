#include <vector>
#include <optional>
#include <utility>
#include <string>
#include <glm/glm.hpp>

#include "Load.hpp"


struct LevelMap;

extern Load<std::vector<LevelMap>> roller_level_maps;

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

    struct Player {
        glm::ivec3 pos1;
        glm::ivec3 pos2;
        int stance;
    };

    LevelMap(const Plane &floor, const std::optional<Plane> &rightWall, const std::vector<std::pair<glm::uvec3, std::string>> &coinsPos, const Player &p)
            : floor(floor), right_wall(rightWall), coins_pos(coinsPos), player(p) {}
    Plane floor;
    std::optional<Plane> right_wall;
    std::vector<std::pair<glm::uvec3, std::string>> coins_pos;
    Player player;

};
