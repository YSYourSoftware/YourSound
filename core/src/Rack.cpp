#include "YourSound/Rack.hpp"

using namespace YourSound;

void Rack::add_player(Player *player) {
	m_players.push_back(player);
}

void Rack::remove_player(const uint8_t index) {
	m_players.erase(m_players.begin() + index);
}