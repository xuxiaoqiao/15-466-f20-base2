#include <vector>
#include <optional>
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

