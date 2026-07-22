#include "BinPlayer2A03.hpp"
#include "BinPlayerHexwave.hpp"
//#include "BinPlayerSampler.hpp"
#include "BinPlayerWavetable.hpp"

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Player.hpp>

YS_START_BP_REGISTRY
	YS_REGISTER_BP("org.yoursoftware.sound.2a03", BinPlayer2A03);
	YS_REGISTER_BP("org.yoursoftware.sound.hexwave", BinPlayerHexwave);
	//YS_REGISTER_BP("org.yoursoftware.sound.sampler", BinPlayerSampler);
	YS_REGISTER_BP("org.yoursoftware.sound.wavetable", BinPlayerWavetable);
YS_END_BP_REGISTRY