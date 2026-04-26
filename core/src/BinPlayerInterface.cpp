#include "YourSound/BinPlayerInterface.hpp"

#include "YourSound/Rack.hpp"

#ifdef YS_IN_CORE_BUILD
#pragma message("Building core (dllexport)")
#else
#include <error-core-object-compiled-in-non-core-plugin>
#endif

YS_CORE_EXPORT void ysbp_rack_add_player(YS_RackHandle rack, YS_PlayerHandle player) {
	static_cast<YourSound::Rack*>(rack)->add_player(static_cast<YourSound::Player*>(player));
}