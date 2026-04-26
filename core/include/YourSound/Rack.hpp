#pragma once

#include "YourSound/Player.hpp"

#include <vector>

typedef void *YS_RackHandle;

namespace YourSound {
	class Rack {
		public:
			Rack() = default;
			~Rack() = default;

			void add_player(Player *player);
			void remove_player(uint8_t index);
		private:
			std::vector<Player*> m_players;
	};
}