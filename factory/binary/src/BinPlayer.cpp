#include "BinPlayerHexwave.hpp"
#include "BinPlayerSF2.hpp"
#include "BinPlayerSampler.hpp"

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Player.hpp>

YS_START_BP_REGISTRY
	YS_REGISTER_BP("org.yoursoftware.sound.hexwave", BinPlayerHexwave);
	YS_REGISTER_BP("org.yoursoftware.sound.sf2", BinPlayerSF2);
	YS_REGISTER_BP("org.yoursoftware.sound.sampler", BinPlayerSampler);
YS_END_BP_REGISTRY