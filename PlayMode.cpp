#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>


GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("level1.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("level1.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*hexapod_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "player") player = &transform;
	}

	if (player == nullptr) throw std::runtime_error("player not found.");

	player_base_rotation = player->rotation;
	player_base_position = player->position;
	

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
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
		moving = true;
		if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
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
			constexpr float PlayerSpeed = 100.0f;
			glm::vec2 move = glm::vec2(0.0f);
			if (left.pressed && !right.pressed) move.x = -1.0f;
			if (!left.pressed && right.pressed) move.x = 1.0f;
			if (down.pressed && !up.pressed) move.y = -1.0f;
			if (!down.pressed && up.pressed) move.y = 1.0f;
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
			}
			
			player->position = player_base_position + dmov * dirx * move.x + dmov * diry * move.y - abs(dmov) * dirz * stand;
			camera->transform->position = camera_base_position + dmov * dirx * move.x + dmov * diry * move.y - abs(dmov) * dirz * stand;
			// player->position += move.x * dirx * 0.15f + abs(move.x) * dirz * stand * 0.1f;
			// camera->transform->position += move.x * dirx * 0.15f + abs(move.x) * dirz * stand * 0.1f;
			// player->position = glm::vec3(0.0f, move.x, 0.0f);
			
			if (abs(dmov) == 1.0f){
				drot = 0.0f;
				// player_base_position = player_base_position + dmov * dirx * move.x - abs(dmov) * dirz * stand;
				// camera_base_position = camera_base_position + dmov * dirx * move.x - abs(dmov) * dirz * stand;
				player_base_position = player->position;
				camera_base_position = camera->transform->position;
				// for (int i=0; i<4; i++){
				// 	std::cout << (player->rotation)[i] << " ";
				// }
				// std::cout << std::endl;
				// std::cout << "reset rotation" << std::endl;
				player_base_rotation = player->rotation;
				dmov = 0.0f;
				left.pressed = false;
				right.pressed = false;
				down.pressed = false;
				up.pressed = false;
				
				move.x = 0.0f;
				move.y = 0.0f;
				stand *= -1.0f;
				moving = false;
			}
			
		}

		//reset button press counters:
		left.downs = 0;
		right.downs = 0;
		up.downs = 0;
		down.downs = 0;
	}
	
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
