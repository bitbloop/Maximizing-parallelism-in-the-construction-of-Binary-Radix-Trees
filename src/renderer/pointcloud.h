#pragma once


namespace renderer
{
	namespace pointcloud
	{
		unsigned int create(const unsigned int program_id, const unsigned int data_num, const float* data_xyz);
		void kill(const unsigned int pointcloud_id);
		void render(const unsigned int pointcloud_id);
	};
};
