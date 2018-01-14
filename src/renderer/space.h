#pragma once

#include <glm\gtc\type_ptr.hpp>

namespace renderer
{
	namespace space
	{
		glm::vec3 compute_2d_mouse_world_coords(const glm::vec2& mouse_position); // returns coordinate at z == 0

		void update_viewport_matrix(const int window_width, const int window_height);
		void update_view_matrix(const glm::vec3& camera_pos, const glm::vec3& camera_target);
		void update_projection_matrix(const int window_width, const int window_height);

		const glm::mat4x4& get_view_matrix();
		const glm::mat4x4& get_projection_matrix();
		const glm::mat4x4& get_inv_view_matrix();
		const glm::mat4x4& get_inv_projection_matrix();
	}
} 