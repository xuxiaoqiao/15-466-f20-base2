#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>



//Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
//	MeshBuffer const *ret = new MeshBuffer(data_path("level1.pnct"));
//	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
//	return ret;
//});
//
//Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
//	return new Scene(data_path("level1.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
//		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);
//
//		scene.drawables.emplace_back(transform);
//		Scene::Drawable &drawable = scene.drawables.back();
//
//		drawable.pipeline = lit_color_texture_program_pipeline;
//
//		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
//		drawable.pipeline.type = mesh.type;
//		drawable.pipeline.start = mesh.start;
//		drawable.pipeline.count = mesh.count;
//
//	});
//});

const std::array<std::string, 2> roller_scene_names = {"level1", "level2"};
std::vector<GLuint> roller_meshes_for_lit_color_texture_program_list;
Load<std::vector<MeshBuffer>> roller_mesh_list(LoadTagDefault, []()-> std::vector<MeshBuffer> const * {
	auto *result = new std::vector<MeshBuffer>();
	for (int scene_name_idx = 0; scene_name_idx < roller_scene_names.size(); scene_name_idx++) {
		const std::string &n = roller_scene_names.at(scene_name_idx);
		result->emplace_back(data_path(n + ".pnct"));
		GLuint roller_meshes_for_lit_color_texture_program = result->at(scene_name_idx).make_vao_for_program(
				lit_color_texture_program->program);
		roller_meshes_for_lit_color_texture_program_list.push_back(roller_meshes_for_lit_color_texture_program);
	}
	return result;
});

Load<std::vector<Scene>> roller_scene_list(LoadTagDefault, []() -> std::vector<Scene> const * {
	auto *result = new std::vector<Scene>();
	for (int name_idx = 0; name_idx < roller_scene_names.size(); name_idx++) {
		const std::string &n = roller_scene_names.at(name_idx);
		result->emplace_back(data_path(n + ".scene"),
							 [name_idx](Scene &scene, Scene::Transform *transform, std::string const &mesh_name) {
								 Mesh const &mesh = roller_mesh_list->at(name_idx).lookup(mesh_name);

								 scene.drawables.emplace_back(transform);
								 Scene::Drawable &drawable = scene.drawables.back();

								 drawable.pipeline = lit_color_texture_program_pipeline;

								 drawable.pipeline.vao = roller_meshes_for_lit_color_texture_program_list.at(name_idx);
								 drawable.pipeline.type = mesh.type;
								 drawable.pipeline.start = mesh.start;
								 drawable.pipeline.count = mesh.count;
							 });
	}
	return result;
});

PlayMode::PlayMode(int level_idx) : scene(roller_scene_list->at(level_idx)),
									level_map(roller_level_maps->at(level_idx)) {
	//get pointers to leg for convenience:
	coins_transforms.reserve(level_map.coins_pos.size());
	for (auto &transform : scene.transforms) {
		if (transform.name == "player") player = &transform;
		for(int i = 0; i < level_map.coins_pos.size();i++){
			if (transform.name == level_map.coins_pos[i].second) coins_transforms[i] = &transform;
			std::cout<<transform.name<<std::endl;
		}
	}

	if (player == nullptr) throw std::runtime_error("player not found.");

	player_base_rotation = player->rotation;
	player_base_position = player->position;


	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1)
		throw std::runtime_error(
				"Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	camera_base_position = camera->transform->position;

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (moving) return false;
	if (evt.type == SDL_KEYDOWN) {
		// if (evt.key.keysym.sym == SDLK_ESCAPE) {
		// 	SDL_SetRelativeMouseMode(SDL_FALSE);
		// 	return true;
		// } 
		
		if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			moving = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			moving = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			moving = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			moving = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_h) {
			if (wall) to_floor();
			else to_wall();
		}
	} 
	// else if (evt.type == SDL_KEYUP) {
	// 	if (evt.key.keysym.sym == SDLK_a) {
	// 		left.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_d) {
	// 		right.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_w) {
	// 		up.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_s) {
	// 		down.pressed = false;
	// 		return true;
	// 	}
	// } 
	// else if (evt.type == SDL_MOUSEBUTTONDOWN) {
	// 	if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
	// 		SDL_SetRelativeMouseMode(SDL_TRUE);
	// 		return true;
	// 	}
	// } else if (evt.type == SDL_MOUSEMOTION) {
	// 	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
	// 		glm::vec2 motion = glm::vec2(
	// 			evt.motion.xrel / float(window_size.y),
	// 			-evt.motion.yrel / float(window_size.y)
	// 		);
	// 		camera->transform->rotation = glm::normalize(
	// 			camera->transform->rotation
	// 			* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
	// 			* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
	// 		);
	// 		return true;
	// 	}
	// }

	return false;
}

void PlayMode::next_pos(glm::uvec3 pos1, glm::uvec3 pos2, glm::vec3 op){
	// if (operation.x != 0) operation.x /= abs(operation.x);
	// if (operation.y != 0) operation.y /= abs(operation.y);

	// if (stance == 0){
	// 	pos1 -= operation.y * diry;
	// 	pos2 -= operation.y * diry;
	// 	pos1 += operation.x * dirx;
		
	// } else if (stance == 1){
	// 	pos1.x += operation.x * dirx;
	// 	pos2.x += operation.x * dirx;
	// 	if (operation.y < 0){
	// 		pos1.y = std::max(pos1.y, pos2.y) + 1;
	// 		pos2.y = pos1.y;
	// 	} else if (operation.y > 0){
	// 		pos1.y = std::min(pos1.y, pos2.y) - 1;
	// 		pos2.y = pos1.y;
	// 	}
	// } else if (stance == 2){
	// 	pos[0] += operation.x;
	// 	pos[1] -= operation.y;
	// 	pos[3] += operation.x*2;
	// 	pos[4] -= operation.y*2;
	// }

	return;
}

bool PlayMode::offmap(std::vector<int> pos){
	// if (map.floor.GetTileType(pos[1], pos[0]) == 0 || map.floor.GetTileType(pos[3], pos[2]) == 0) return true;
	return false;
}

void PlayMode::end_move(){
	left.pressed = false;
	right.pressed = false;
	down.pressed = false;
	up.pressed = false;
	moving = false;
}

void PlayMode::to_wall(){
	wall = true;
	dirx = glm::vec3(0.0f, 0.0f, 1.0f);
	dirz = glm::vec3(-1.0f, 0.0f, 0.0f);
	axisx = glm::vec3(0.0f, 0.0f, -1.0f);
	axisz = glm::vec3(1.0f, 0.0f, 0.0f);
	if (stance == 0) stance = 2;
	else if (stance == 2) stance = 0;
}

void PlayMode::to_floor(){
	wall = false;
	dirx = glm::vec3(1.0f, 0.0f, 0.0f);
	dirz = glm::vec3(0.0f, 0.0f, 1.0f);

	axisx = glm::vec3(-1.0f, 0.0f, 0.0f);
	axisz = glm::vec3(0.0f, 0.0f, -1.0f);

	if (stance == 0) stance = 2;
	else if (stance == 2) stance = 0;
}


void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// wobble += elapsed / 10.0f;
	// wobble -= std::floor(wobble);
	// if (moving){
	// 	glm::vec2 move = glm::vec2(0.0f);
	// 	if (left.pressed && !right.pressed) move.x = -1.0f;
	// 	if (!left.pressed && right.pressed) move.x = 1.0f;
	// 	if (down.pressed && !up.pressed) move.y = -1.0f;
	// 	if (!down.pressed && up.pressed) move.y = 1.0f;
		
	// 	player->position += move.x * elapsed * dirx + move.y * elapsed * diry;
	// 	camera->transform->position += move.x * elapsed * dirx + move.y * elapsed * diry;
	// 	moving = false;
	// }


	if (moving){

		
		//move camera:
		{

			//combine inputs into a move:
			constexpr float PlayerSpeed = 300.0f;
			glm::vec3 move = glm::vec3(0.0f);
			if (left.pressed) move.x = -1.0f;
			if (right.pressed) move.x = 1.0f;
			if (down.pressed) move.y = -1.0f;
			if (up.pressed) move.y = 1.0f;
		
			// newpos = next_pos(pos, move);
			// if (offmap(newpos)){ //check map
			// 	end_move();
			// 	return;
			// }
			//make it so that moving diagonally doesn't go faster:
			// if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

			// glm::mat4x3 frame = player->make_local_to_parent();
			// glm::vec3 movx = frame[0];
			// glm::vec3 dirx = -frame[1];
			// glm::vec3 dirz = frame[2];
			// player->rotation *= glm::angleAxis(
			// 	glm::radians(1.57f*move.x),
			// 	glm::vec3(0.0f, 1.0f, 0.0f)
			// );
			// player->rotation *= toQuat(0.0f, 45.0f*PI/180.0f*move.x, 0.0f);
			drot += elapsed*PlayerSpeed;
			
			if (abs(drot) >= 90.0f){
				drot = drot/abs(drot)*90.0f;
			}
			dmov = drot/90.0f;
				
			
			// if (drotx == 0 && stand == 1.0f){
			// 	for (int i=0; i<3; i++){
			// 		std::cout << (move.x * dirz * stand)[i] << " ";
			// 	}
			// 	std::cout << std::endl;
			// 	player->position += move.x * dirx+ move.x * dirz * stand;
			// 	camera->transform->position += move.x * dirx + move.x * dirz * stand;
			// 	move.x = 0;
			// 	stand = -1.0f;
			// }
			if (move.x != 0){
				player->rotation = glm::angleAxis(
					glm::radians(drot*move.x),
					axisy
				) * player_base_rotation;
			} else if (move.y != 0){
				player->rotation = glm::angleAxis(
					glm::radians(drot*move.y),
					axisx
				) * player_base_rotation;

			} else if (move.z != 0){
				player->rotation = glm::angleAxis(
					glm::radians(drot*move.z),
					axisz
				) * player_base_rotation;
			}
			if (stance == 0){
				move.z = abs(move.x)*0.5;
				move.x *= 1.5;
			} else if (stance == 1){
				move.z = abs(move.y)*0.5;
				move.y *= 1.5;
			} else if (stance == 2){
				move.z = -0.5;
				move.x *= 1.5;
				move.y *= 1.5;
			}
			
			
			player->position = player_base_position + dmov * (dirx * move.x + diry * move.y + dirz * move.z);
			camera->transform->position = camera_base_position + dmov * (dirx * move.x + diry * move.y + dirz * move.z);
			
			if (abs(dmov) == 1.0f){
				drot = 0.0f;
			
				player_base_position = player->position;
				camera_base_position = camera->transform->position;
				player_base_rotation = player->rotation;
				dmov = 0.0f;

				if (stance == 0){
					if (move.x != 0) stance = 2;
				} else if (stance == 1){
					if (move.y != 0) stance = 2;
				} else if (stance == 2){
					if (move.x != 0) stance = 0;
					else if (move.y != 0) stance = 1;
				}
				// pos = newpos;
				end_move();
			}
			
		}

		//reset button press counters:
		left.downs = 0;
		right.downs = 0;
		up.downs = 0;
		down.downs = 0;
	}
	
	// //check if we collect coins
	// for(int i = 0; i<level_map.coins_pos.size();i++){
	// 	if (level_map.coins_pos[i].first ==pos1 || level_map.coins_pos[i].first == pos2){
	// 		coinFound = i;
	// 		break;
	// 	}
	// }
	// //transit coin
	// if (coinFound>=0){
	// 	constexpr float CoinSpeed = 100.0f;
	// 	float z_move = CoinSpeed*elapsed;
	// 	coins_transforms[coinFound]->position.z += z_move;
	// 	if(coins_transforms[coinFound]->position.z >= 40){
	// 		coinFound = -1;
	// 	}
	// }
	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	GL_ERRORS();
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
