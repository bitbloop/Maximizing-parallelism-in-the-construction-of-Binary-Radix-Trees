


#include "test_render_data.h"
#include "util\timer.h"
#include "renderer\renderer.h"


/** */
void maximizing_tree_construction_parallelism::test_render_data(const char* obj_mesh_filename)
{
	std::cout << "Visualizing data." << std::endl;
	// ________________________________________________________________
	// load the data

	loader_obj::obj *mesh{ loader_obj::obj_create(obj_mesh_filename) };
	assert(mesh);
	const auto data_num{ obj_num_vert(mesh) };

	auto* data_xyz{ new float[data_num * 3] };
	for (auto vi = 0; vi < obj_num_vert(mesh); ++vi)
		loader_obj::obj_get_vert_v(mesh, vi, &data_xyz[vi * 3]);

	// Print the number of loaded vertices
	std::cout << "Loaded " << obj_mesh_filename << "." << std::endl << "Number of vertices " << data_num << "." << std::endl;

	// ________________________________________________________________
	// Rendering part

	// Create OpenGL window and context
	::renderer::window::create();

	// Change the view transformation matrix
	::renderer::space::update_view_matrix({ -7, 9, 9 }, { 0, 4, 0 });

	// Load the shader to be used for rendering
	const auto shader_id{ ::renderer::shader::load_program({ "../src/maximizing_tree_construction_parallelism/basic.glsl" }) };
	glUseProgram(shader_id);

	// get shader uniform locations
	const auto u_projection_matrix{ glGetUniformLocation(shader_id, "u_projection_matrix") };
	const auto u_view_matrix{ glGetUniformLocation(shader_id, "u_view_matrix") };

	// update the shader uniforms
	if (u_projection_matrix != -1) glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, glm::value_ptr(::renderer::space::get_projection_matrix()));
	if (u_view_matrix != -1) glUniformMatrix4fv(u_view_matrix, 1, GL_FALSE, glm::value_ptr(::renderer::space::get_view_matrix()));

	// create the visual representation of loaded data
	const auto pointcloud_id{ ::renderer::pointcloud::create(shader_id, data_num, data_xyz) };

	// loop until we close the window
	while (!::renderer::window::should_window_close()) {
		glClear(GL_DEPTH_BUFFER_BIT);
		::renderer::pointcloud::render(pointcloud_id);
		::renderer::window::pool_events_and_swap_buffers();
	}

	// ________________________________________________________________
	// Uninitialize the data

	delete[] data_xyz;

	loader_obj::obj_delete(mesh);
	::renderer::pointcloud::kill(pointcloud_id);
	::renderer::window::kill();
}


