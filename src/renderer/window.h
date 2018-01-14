#pragma once

namespace renderer
{
	namespace window
	{
		bool create();
		void kill();
		void pool_events_and_swap_buffers();
		bool should_window_close();
	}
} 