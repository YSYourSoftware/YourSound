add_subdirectory(sol2)

target_link_libraries(YourSound PRIVATE sol2)
target_sources(YourSound PRIVATE toml.hpp)