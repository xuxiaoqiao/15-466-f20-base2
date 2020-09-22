#include "LevelMap.hpp"
#include "data_path.hpp"
#include <json.hpp>
#include <fstream>
#include <cassert>

using nlohmann::json;

extern const std::array<std::string, 2> roller_scene_names;

static LevelMap::Plane parse_plane(const json &j);
static LevelMap parse_level_map(const json &j);
static glm::uvec3 parse_uvec3(const json &j);

Load<std::vector<LevelMap>> roller_level_maps(LoadTagDefault, []() -> std::vector<LevelMap> const * {
    std::vector<LevelMap> *result = new std::vector<LevelMap>;
    std::ifstream ifs(data_path("levels.json"));
    json j = json::parse(ifs);
    assert(j.size() == roller_scene_names.size());
    for (const auto &lj : j) {
        LevelMap l = parse_level_map(lj);
        result->push_back(l);
    }
    return result;
});

static LevelMap parse_level_map(const json &j) {
    LevelMap::Plane floor = parse_plane(j.at("floor"));
    std::optional<LevelMap::Plane> right_wall = std::nullopt;
    if (j.contains("right_wall")) {
        right_wall = std::make_optional(parse_plane(j.at("right_wall")));
    }
    std::vector<glm::uvec3> coins_pos;
    assert(j.at("coins_position").is_array());
    for (const auto &c : j.at("coins_position")) {
        coins_pos.push_back(parse_uvec3(c));
    }
    return LevelMap(floor, right_wall, coins_pos);
}


static LevelMap::Plane parse_plane(const json &j) {
    int depth = j.at("depth");
    int width = j.at("width");
    std::vector<int> content = j.at("map");
    LevelMap::Plane p(depth, width, content);
    return p;
}

static glm::uvec3 parse_uvec3(const json &j) {
    glm::uvec3 result;
    assert(j.is_array());
    result[0] = j.at(0);
    result[1] = j.at(1);
    result[2] = j.at(2);
    return result;
}