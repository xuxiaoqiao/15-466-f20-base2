#include "LevelMap.hpp"
#include "data_path.hpp"
#include <json.hpp>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>

using nlohmann::json;

static LevelMap::Plane parse_plane(const json &j);
static LevelMap parse_level_map(const json &j);
static glm::ivec3 parse_ivec3(const json &j);
static glm::ivec2 parse_ivec2(const json &j);

Load<std::vector<LevelMap>> roller_level_maps(LoadTagDefault, []() -> std::vector<LevelMap> const * {
    std::vector<LevelMap> *result = new std::vector<LevelMap>;
    std::ifstream ifs(data_path("levels.json"));
    json j = json::parse(ifs);
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
    std::vector<std::pair<glm::ivec3, std::string>> coins_pos;
    assert(j.at("coins").is_array());
    for (const auto &c : j.at("coins")) {
        coins_pos.emplace_back(parse_ivec3(c.at("position")), c.at("id"));
    }
    LevelMap::Player p;
    const json &player_json = j.at("player");
    p.pos1 = parse_ivec3(player_json.at("pos1"));
    p.pos2 = parse_ivec3(player_json.at("pos2"));
    p.stance = player_json.at("stance");
    std::vector<std::pair<glm::ivec2, glm::ivec2>> portals;
    if (j.contains("portals")) {
        assert(j.at("portals").is_array());
        const json &portals_json = j.at("portals");
        for (const auto &p_json : portals_json) {
            glm::ivec2 a = parse_ivec2(p_json.at(0));
            glm::ivec2 b = parse_ivec2(p_json.at(1));
            portals.emplace_back(a, b);
        }
    }
    return LevelMap(floor, right_wall, coins_pos, p, portals);
}


static LevelMap::Plane parse_plane(const json &j) {
    int depth = j.at("depth");
    int width = j.at("width");
    std::vector<int> content = j.at("map");
    LevelMap::Plane p(depth, width, content);
    return p;
}

static glm::ivec3 parse_ivec3(const json &j) {
    glm::ivec3 result;
    assert(j.is_array());
    result[0] = j.at(0);
    result[1] = j.at(1);
    result[2] = j.at(2);
    return result;
}

static glm::ivec2 parse_ivec2(const json &j) {
    glm::ivec2 result;
    assert(j.is_array());
    result[0] = j.at(0);
    result[1] = j.at(1);
    return result;
}
