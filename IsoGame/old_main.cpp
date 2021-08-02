#include <stdio.h>

#include "glcontent.h"

#include "glslprogram.h"
#include "colors.h"
#include "fbo.h"
#include "mesh.h"
#include "primitive_generators.h"

#include "lerp.h"

#include <vector>;

#include "Engine.h"

#include "pathfinder.h"

Engine game_engine;

GLSLProgramManager program_manager;

FBOManager fbo_manager;

GLContent content;

Camera camera = Camera(glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(0,0,0), glm::vec3(0, 1, 0));

Mesh
screen_texture,
cube,
ground,
sky
;

FBOID
basic_fbo;

GLSLProgramID
RENDER_PROGRAM;

glm::vec3 ambient_color;

PlayerManager * player;
NPC * enemy1;
NPCManager npc_manager;

WalkMap map;


//Returns random float
inline float		randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void init()
{
	//// CREATE GLSL PROGAMS
	printf("\n");
	printf("Initialising GLSL programs...\n");
	RENDER_PROGRAM =
		program_manager.add_program("shaders/basic_texture.vert", "shaders/basic_texture.frag",
			content.get_model_mat(), content.get_view_mat(), content.get_proj_mat());

	//// ADDING HANDLES TO PROGRAMS
	printf("\n");
	printf("Adding handles to GLSL programs...\n");
	program_manager.get_program(RENDER_PROGRAM)
		->set_tex_handle();

	//// CREATE FBOS
	printf("\n");
	printf("Creating FBOs...\n");
	basic_fbo = fbo_manager.add_fbo(content.get_window_size(), &screen_texture);

	//// ADD OBJECTS TO FBOS
	fbo_manager.get_fbo(basic_fbo)
		//->add_mesh(&ground)
		//->add_mesh(&sphere)
		//->add_mesh(&sky)
		->add_mesh(&cube);

	//// CREATE OBJECTS
	printf("\n");
	printf("Initialising objects...\n");

	std::vector<glm::vec3> v;

	v = generate_square_mesh(1, 1);
	screen_texture = Mesh(
		"",
		pack_object(&v, GEN_UVS_RECTS, BLACK),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1), glm::radians(0.0f),
		glm::vec3(0, 0, 1), glm::radians(0.0f),
		glm::vec3(content.get_window_size().x, content.get_window_size().y, 1)
	);

	game_engine.add_entity(
		glm::vec2(),
		(new MeshAnimation(
			pack_object(&v, GEN_UVS_RECTS, BLUE),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), glm::radians(0.0f),
			glm::vec3(1, 0, 0), glm::radians(0.0f),
			glm::vec3(1, 1, 1) * 2000.0f
		))->add_frame_texture("textures/scene1.png"),
		0.0f
	);

	v = generate_centered_square_mesh(1, 1);

	enemy1 = npc_manager.add_npc(game_engine.add_entity(
		glm::vec2(200.0f, 0.0f),
		(new MeshAnimation(
			pack_object(&v, GEN_UVS_RECTS, BLUE),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), glm::radians(0.0f),
			glm::vec3(1, 0, 0), glm::radians(0.0f),
			glm::vec3(1, 1, 1)  * 100.0f
		))->add_frame_texture("textures/enemy.png"),
		0.0f
	),
		HealthBar(
			game_engine.add_entity(
				glm::vec2(),
				(new MeshAnimation(
					pack_object(&v, GEN_COLOR, GREEN),
					glm::vec3(0, 0, 0),
					glm::vec3(0, 1, 0), glm::radians(0.0f),
					glm::vec3(1, 0, 0), glm::radians(0.0f),
					glm::vec3(1, 10.0f, 1)
				)),
				0.0f
			),
			game_engine.add_entity(
				glm::vec2(),
				(new MeshAnimation(
					pack_object(&v, GEN_COLOR, RED),
					glm::vec3(0, 0, 0),
					glm::vec3(0, 1, 0), glm::radians(0.0f),
					glm::vec3(1, 0, 0), glm::radians(0.0f),
					glm::vec3(100.0f, 10.0f, 1)
				)),
				0.0f
			)
		)
	);
	enemy1->stance = AGGRESSIVE;

	player = new PlayerManager(
		game_engine.add_entity(
			glm::vec2(),
			(new MeshAnimation(
				pack_object(&v, GEN_UVS_RECTS, BLUE),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0), glm::radians(0.0f),
				glm::vec3(1, 0, 0), glm::radians(0.0f),
				glm::vec3(1, 1, 1)  * 100.0f
			))
				->add_frame_texture("textures/man_idle_1.png")
				->add_frame_texture("textures/man_idle_2.png")
				->add_frame_texture("textures/man_idle_3.png")
				->add_frame_texture("textures/man_idle_4.png")
				->add_frame_texture("textures/man_idle_5.png")
				->add_frame_texture("textures/man_idle_6.png")
				->add_frame_texture("textures/man_idle_7.png")
				->add_frame_texture("textures/man_idle_8.png")
				->add_frame_texture("textures/man_idle_9.png")
				->add_frame_texture("textures/man_idle_10.png")
				->add_frame_texture("textures/man_idle_11.png")
				->add_frame_texture("textures/man_idle_12.png")
				->add_frame_texture("textures/man_idle_13.png")
				->add_frame_texture("textures/man_idle_14.png")
				->add_frame_texture("textures/man_idle_15.png")
				->add_frame_texture("textures/man_idle_16.png"),
			0.0f
		)->add_mesh_animation(
			(new MeshAnimation(
				pack_object(&v, GEN_UVS_RECTS, BLUE),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0), glm::radians(0.0f),
				glm::vec3(1, 0, 0), glm::radians(0.0f),
				glm::vec3(1, 1, 1)  * 100.0f
			))
				->add_frame_texture("textures/man_run_right_1.png")
				->add_frame_texture("textures/man_run_right_2.png")
				->add_frame_texture("textures/man_run_right_3.png")
				->add_frame_texture("textures/man_run_right_4.png")
		)->add_mesh_animation(
			(new MeshAnimation(
				pack_object(&v, GEN_UVS_RECTS, BLUE),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0), glm::radians(0.0f),
				glm::vec3(1, 0, 0), glm::radians(0.0f),
				glm::vec3(1, 1, 1)  * 100.0f
			))
				->add_frame_texture("textures/man_run_left_1.png")
				->add_frame_texture("textures/man_run_left_2.png")
				->add_frame_texture("textures/man_run_left_3.png")
				->add_frame_texture("textures/man_run_left_4.png")
		)->add_mesh_animation(
			(new MeshAnimation(
				pack_object(&v, GEN_UVS_RECTS, BLUE),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0), glm::radians(0.0f),
				glm::vec3(1, 0, 0), glm::radians(0.0f),
				glm::vec3(1, 1, 1)  * 100.0f
			))
				->add_frame_texture("textures/man_run_up_1.png")
				->add_frame_texture("textures/man_run_up_2.png")
				->add_frame_texture("textures/man_run_up_3.png")
				->add_frame_texture("textures/man_run_up_4.png")
		)->add_mesh_animation(
			(new MeshAnimation(
				pack_object(&v, GEN_UVS_RECTS, BLUE),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0), glm::radians(0.0f),
				glm::vec3(1, 0, 0), glm::radians(0.0f),
				glm::vec3(1, 1, 1)  * 100.0f
			))
				->add_frame_texture("textures/man_run_up_4.png")
				->add_frame_texture("textures/man_run_up_3.png")
				->add_frame_texture("textures/man_run_up_2.png")
				->add_frame_texture("textures/man_run_up_1.png")
		),
			game_engine.add_entity(
				glm::vec2(),
				(new MeshAnimation(
					pack_object(&v, GEN_UVS_RECTS, BLUE),
					glm::vec3(0, 0, 0),
					glm::vec3(0, 1, 0), glm::radians(0.0f),
					glm::vec3(1, 0, 0), glm::radians(0.0f),
					glm::vec3(1, 1, 1) * 100.0f
				))
				->add_frame_texture("textures/ti_g_1.png")
				->add_frame_texture("textures/ti_g_2.png")
				->add_frame_texture("textures/ti_g_3.png")
				->add_frame_texture("textures/ti_g_4.png"),
				0.0f
			),
			game_engine.add_entity(
				glm::vec2(),
				(new MeshAnimation(
					pack_object(&v, GEN_UVS_RECTS, BLUE),
					glm::vec3(0, 0, 0),
					glm::vec3(0, 1, 0), glm::radians(0.0f),
					glm::vec3(1, 0, 0), glm::radians(0.0f),
					glm::vec3(1, 1, 1) * 100.0f
				))
				->add_frame_texture("textures/ti_r_1.png")
				->add_frame_texture("textures/ti_r_2.png")
				->add_frame_texture("textures/ti_r_3.png")
				->add_frame_texture("textures/ti_r_4.png")
				->add_frame_texture("textures/ti_r_5.png")
				->add_frame_texture("textures/ti_r_6.png")
				->add_frame_texture("textures/ti_r_7.png")
				->add_frame_texture("textures/ti_r_8.png"),
				0.0f
			),
				HealthBar(
					game_engine.add_entity(
						glm::vec2(),
						(new MeshAnimation(
							pack_object(&v, GEN_COLOR, GREEN),
							glm::vec3(0, 0, 0),
							glm::vec3(0, 1, 0), glm::radians(0.0f),
							glm::vec3(1, 0, 0), glm::radians(0.0f),
							glm::vec3(1, 10.0f, 1)
						)),
						0.0f
					),
					game_engine.add_entity(
						glm::vec2(),
						(new MeshAnimation(
							pack_object(&v, GEN_COLOR, RED),
							glm::vec3(0, 0, 0),
							glm::vec3(0, 1, 0), glm::radians(0.0f),
							glm::vec3(1, 0, 0), glm::radians(0.0f),
							glm::vec3(100.0f, 10.0f, 1)
						)),
						0.0f
					)
				)
		);

	v = generate_square_mesh(1, 1);
	game_engine.add_entity(
		glm::vec2(100.0f, 100.0f),
		new MeshAnimation(
			pack_object(&v, GEN_COLOR, GREEN),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), glm::radians(0.0f),
			glm::vec3(1, 0, 0), glm::radians(0.0f),
			glm::vec3(1, 1, 1) * 200.0f
		),
		0.0f
	);

	game_engine.add_entity(
		glm::vec2(250.0f, 200.0f),
		new MeshAnimation(
			pack_object(&v, GEN_COLOR, GREEN),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), glm::radians(0.0f),
			glm::vec3(1, 0, 0), glm::radians(0.0f),
			glm::vec3(1, 1, 1) * 200.0f
		), 0.0f
	);

	game_engine.add_entity(
		glm::vec2(400.0f, 100.0f),
		new MeshAnimation(
			pack_object(&v, GEN_COLOR, GREEN),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), glm::radians(0.0f),
			glm::vec3(1, 0, 0), glm::radians(0.0f),
			glm::vec3(1, 1, 1) * 200.0f
		), 0.0f
	);

	map.
		  add_node(glm::vec2(100.0f, 100.0f))
		->add_node(glm::vec2(100.0f, 300.0f))

		->add_node(glm::vec2(100.0f, 300.0f))
		->add_node(glm::vec2(300.0f, 300.0f))

		->add_node(glm::vec2(300.0f, 300.0f))
		->add_node(glm::vec2(300.0f, 100.0f))

		->add_node(glm::vec2(300.0f, 100.0f))
		->add_node(glm::vec2(100.0f, 100.0f))


		->add_node(glm::vec2(250.0f, 200.0f))
		->add_node(glm::vec2(250.0f, 400.0f))

		->add_node(glm::vec2(250.0f, 400.0f))
		->add_node(glm::vec2(450.0f, 400.0f))

		->add_node(glm::vec2(450.0f, 400.0f))
		->add_node(glm::vec2(450.0f, 200.0f))

		->add_node(glm::vec2(450.0f, 200.0f))
		->add_node(glm::vec2(250.0f, 200.0f))


		->add_node(glm::vec2(400.0f, 100.0f))
		->add_node(glm::vec2(400.0f, 300.0f))

		->add_node(glm::vec2(400.0f, 300.0f))
		->add_node(glm::vec2(600.0f, 300.0f))

		->add_node(glm::vec2(600.0f, 300.0f))
		->add_node(glm::vec2(600.0f, 100.0f))

		->add_node(glm::vec2(600.0f, 100.0f))
		->add_node(glm::vec2(400.0f, 100.0f))

		
		->extrapolate_nodes();

	map.
		add_tri_point(glm::vec2(100.0f, 100.0f))
		->add_tri_point(glm::vec2(100.0f, 300.0f))
		->add_tri_point(glm::vec2(300.0f, 300.0f))	

		->add_tri_point(glm::vec2(300.0f, 300.0f))
		->add_tri_point(glm::vec2(300.0f, 100.0f))
		->add_tri_point(glm::vec2(100.0f, 100.0f))


		->add_tri_point(glm::vec2(250.0f, 200.0f))
		->add_tri_point(glm::vec2(250.0f, 400.0f))
		->add_tri_point(glm::vec2(450.0f, 400.0f))

		->add_tri_point(glm::vec2(450.0f, 400.0f))
		->add_tri_point(glm::vec2(450.0f, 200.0f))
		->add_tri_point(glm::vec2(250.0f, 200.0f))


		->add_tri_point(glm::vec2(400.0f, 100.0f))
		->add_tri_point(glm::vec2(400.0f, 300.0f))
		->add_tri_point(glm::vec2(600.0f, 300.0f))

		->add_tri_point(glm::vec2(600.0f, 300.0f))
		->add_tri_point(glm::vec2(600.0f, 100.0f))
		->add_tri_point(glm::vec2(400.0f, 100.0f));

	printf("%i \n", map.is_outside(glm::vec2(50.0f, 150.0f)));
	printf("%i \n", map.is_outside(glm::vec2(150.0f, 350.0f)));
	printf("%i \n", map.is_outside(glm::vec2(170.0f, 140.0f)));
	printf("%i \n", map.is_outside(glm::vec2(110.0f, 280.0f)));

	player->set_walkmap(map);

	//std::vector<glm::vec2> loop;
	//loop.push_back(glm::vec2(0.0f, 0.0f));
	//loop.push_back(glm::vec2(0.0f, 1.0f));
	//loop.push_back(glm::vec2(1.0f, 1.0f));
	//loop.push_back(glm::vec2(1.0f, 2.0f));
	//loop.push_back(glm::vec2(1.5f, 2.0f));
	//loop.push_back(glm::vec2(1.5f, 0.0f));
	////loop = ptf(loop);

}

void physics()
{
	game_engine.update_entities();
	npc_manager.update_npcs();
	content.set_eye_pos(glm::vec3(player->e->pos,0));
	player->update();
	//enemy1->e->pos.x += 0.5f;
}

void draw_loop()
{
	content.set_camera(&camera);

	physics();

	FBO::unbind();

	VarHandle
		*model_mat_handle,
		*texture_handle,
		*view_mat_handle,
		*proj_mat_handle;

	content.clearAll();
	content.loadOrtho();
	program_manager.load_program(RENDER_PROGRAM);
	model_mat_handle = program_manager.get_current_program()->get_model_mat4_handle();
	texture_handle = program_manager.get_current_program()->get_tex_handle();
	//fbo_manager.get_fbo(basic_fbo)->binding_draw_meshes(model_mat_handle, texture_handle);
	//fbo_manager.get_fbo(basic_fbo)->draw_render_mesh(model_mat_handle, texture_handle);
	//screen_texture.draw(0, model_mat_handle, texture_handle);
	//cube.draw(0, model_mat_handle, texture_handle);
	ground.draw(0, model_mat_handle, texture_handle);

	game_engine.draw_entity_meshes(model_mat_handle, texture_handle);
}

//unsigned char * image_data_pixels = new unsigned char[content.get_window_size().x * content.get_window_size().y * 3];
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glm::vec2 target = glm::vec3(xpos, -ypos,0) 
				+ glm::vec3(0, content.get_window_size().y,0) 
				- glm::vec3(content.get_window_size(),0) / 2.0f 
				+ glm::vec3(player->e->pos, 0);		
			player->on_click(&npc_manager, target);
		
			break;
		}
	}
}

static void	key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{

		case GLFW_KEY_B:
			printf("%i\n", game_engine.entities_created);
			break;
		case GLFW_KEY_G:					
			
			break;

		case GLFW_KEY_O:
			content.set_isometric_depth(content.get_isometric_depth() + 0.25f);
			break;
		case GLFW_KEY_L:
			content.set_isometric_depth(content.get_isometric_depth() - 0.25f);
			break;

		case GLFW_KEY_UP:
			break;
		case GLFW_KEY_DOWN:
			break;
		case GLFW_KEY_LEFT:
			break;
		case GLFW_KEY_RIGHT:
			break;



		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}

int main()
{
	content.set_clear_color(GREY);
	//content.set_eye_pos(glm::vec3(0,0,-3));
	content.run(draw_loop, init, key_callback, mouse_button_callback);



	return 0;
}

/*GLint viewport[4]; //var to hold the viewport info
			GLdouble modelview[16]; //var to hold the modelview info
			GLdouble projection[16]; //var to hold the projection matrix info
			GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
			GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

			glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
			glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
			glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

			double xpos, ypos;
			//getting cursor position
			glfwGetCursorPos(window, &xpos, &ypos);

			winX = (float)xpos;
			winY = 0;
			winZ = (float)viewport[3] - (float)ypos;

			player->pos = glm::vec3(xpos, 0, ypos);*/