#pragma once

#include <filesystem>
#include <span>

void write_sample(const std::span<const float> &samples, const std::filesystem::path &file);