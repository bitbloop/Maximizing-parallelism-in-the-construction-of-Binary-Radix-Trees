#include "renderer.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm\gtc\type_ptr.hpp>
#include "window.h"
#include "space.h"
#include "shader.h"

namespace
{
	const GLenum COMPONENT_TYPE = GL_FLOAT;
	const auto SIZE_OF_COMPONENT = sizeof(float);
	const int POSITION_OFFSET = 0;
	const int POSITION_SIZE = 3;
	const int VERTEX_COMPONENTS = POSITION_SIZE;

	struct GL3Pointcloud
	{
		unsigned int a_vertex_position;

		unsigned int vbo_id;
		unsigned int indexbo_id;

		unsigned int point_num;
	};

	void get_pointcloud_shader_attributes(const unsigned int& program_id, unsigned int& a_vertex_position)
	{
		a_vertex_position = glGetAttribLocation(program_id, "a_vertex_position_modelspace");
	}

	// will make [-1, 1] pointcloud
	void make_pointcloud_vbo(const unsigned int data_num, const float* data_xyz, unsigned int& vbo_id, unsigned int& indexbo_id)
	{
		glGenBuffers(1, &vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* VERTEX_COMPONENTS * data_num, data_xyz, GL_STATIC_DRAW);

		unsigned int* indices = new unsigned int[data_num];
		for (unsigned int i = 0; i < data_num; ++i) indices[i] = i;

		glGenBuffers(1, &indexbo_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbo_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * data_num, indices, GL_STATIC_DRAW);

		delete[] indices;
	}



	unsigned int _total_pointclouds_created = 0; // pointclouds created since the beginning of the simulation. will be used as tjhe key into the pointer map
	std::unordered_map<unsigned int, GL3Pointcloud*> _pointcloud_map;

	void create_pointcloud_instance(unsigned int& index_out, GL3Pointcloud*& pointcloud_ptr_out)
	{
		auto pointcloud_obj = new GL3Pointcloud();
		_pointcloud_map[++_total_pointclouds_created] = pointcloud_obj;
		index_out = _total_pointclouds_created;
		pointcloud_ptr_out = pointcloud_obj;
	}
};

unsigned int renderer::pointcloud::create(const unsigned int program_id, const unsigned int data_num, const float* data_xyz)
{
	GL3Pointcloud* pointcloud_ptr;
	unsigned int pointcloud_id;
	create_pointcloud_instance(pointcloud_id, pointcloud_ptr);
	get_pointcloud_shader_attributes(program_id, pointcloud_ptr->a_vertex_position);
	make_pointcloud_vbo(data_num, data_xyz, pointcloud_ptr->vbo_id, pointcloud_ptr->indexbo_id);
	pointcloud_ptr->point_num = data_num;
	return pointcloud_id;
}

void renderer::pointcloud::kill(const unsigned int pointcloud_id)
{
	assert(_pointcloud_map.find(pointcloud_id) != _pointcloud_map.end());
	auto pointcloud_ptr = _pointcloud_map[pointcloud_id];
	// TODO: Clean OpenGL resources.
	delete pointcloud_ptr;
}

void renderer::pointcloud::render(const unsigned int pointcloud_id)
{
	assert(_pointcloud_map.find(pointcloud_id) != _pointcloud_map.end());
	const auto pointcloud = _pointcloud_map[pointcloud_id];	

	glBindBuffer(GL_ARRAY_BUFFER, pointcloud->vbo_id);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(pointcloud->a_vertex_position);
	glVertexAttribPointer(
		pointcloud->a_vertex_position,                  // must match the layout in the shader.
		POSITION_SIZE,        // size
		COMPONENT_TYPE,           // type
		GL_FALSE,           // normalized?
		SIZE_OF_COMPONENT * VERTEX_COMPONENTS,    // stride
		(void*)(POSITION_OFFSET * SIZE_OF_COMPONENT)            // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointcloud->indexbo_id);

	glDrawArrays(GL_POINTS, 0, pointcloud->point_num); // Starting from vertex 0
	//glDrawElements(GL_TRIANGLE_STRIP, QUAD_INDEX_NUM, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(pointcloud->a_vertex_position);
}
