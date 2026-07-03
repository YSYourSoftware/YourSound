target_sources(YourSound PRIVATE
	IntegratedPlayers/BasicOSCPlayer.cpp
	IntegratedPlayers/LuaPlayers.cpp
	IntegratedPlayers/ModLFO.cpp

	UI/Contols.cpp

	BinPlayerInterface.cpp
	Config.cpp
	Player.cpp
	Rack.cpp
PUBLIC
	External/adpcm.c
	External/stb_vorbis.c

	ResourceTypes/BinaryResource.cpp
	ResourceTypes/SampleResource.cpp
)