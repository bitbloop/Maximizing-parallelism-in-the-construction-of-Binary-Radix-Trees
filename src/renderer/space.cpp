#include "renderer.h"
#include <iostream>


#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "space.h"



namespace
{
	glm::mat4x4 _view_mat;
	glm::mat4x4 _projection_mat;
	glm::mat4x4 _inv_projection_mat;
	glm::mat4x4 _inv_view_mat;

	glm::vec4 _viewport;
	//glm::vec3 _camera_pos;

	float _camera_z_min{ 0.5 };
	float _camera_z_max{ 100 };
};


void renderer::space::update_viewport_matrix(const int window_width, const int window_height)
{
	_viewport[0] = 0;
	_viewport[1] = 0;
	_viewport[2] = (float)window_width;
	_viewport[3] = (float)window_height;

	glViewport(0, 0, window_width, window_height);
}


void renderer::space::update_view_matrix(const glm::vec3& camera_pos, const glm::vec3& camera_target)
{
	const glm::vec3 up = { 0, 1, 0 };
	_view_mat = glm::lookAt(camera_pos, camera_target, up);
	_inv_view_mat = glm::inverse(_view_mat);
}

void renderer::space::update_projection_matrix(const int window_width, const int window_height)
{
	_projection_mat = glm::perspective(45.0f, (GLfloat)window_width / (GLfloat)window_height, _camera_z_min, _camera_z_max);
	_inv_projection_mat = glm::inverse(_projection_mat);
}

const glm::mat4x4& renderer::space::get_view_matrix()
{
	return _view_mat;
}

const glm::mat4x4& renderer::space::get_projection_matrix()
{
	return _projection_mat;
}

const glm::mat4x4 & renderer::space::get_inv_view_matrix()
{
	return _inv_projection_mat;
}

const glm::mat4x4 & renderer::space::get_inv_projection_matrix()
{
	return _inv_view_mat;
}

/** */
glm::vec3 renderer::space::compute_2d_mouse_world_coords(const glm::vec2& mouse_position)
{
	glm::vec3 mouse_n{ mouse_position.x, mouse_position.y, 0.0 };
	glm::vec3 mouse_f{ mouse_position.x, mouse_position.y, 1.0 };
	mouse_n = glm::unProject(mouse_n, _view_mat, _projection_mat, _viewport);
	mouse_f = glm::unProject(mouse_f, _view_mat, _projection_mat, _viewport);
	glm::vec3 d{ mouse_f - mouse_n };
	float t{ -mouse_n.z / d.z };
	return { mouse_n.x + d.x * t, mouse_n.y + d.y * t, 0 };
}

