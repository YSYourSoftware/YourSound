#pragma once

#include <string>

void show_error(const std::string &message, const std::string &title="YourSound");
void show_warning(const std::string &message, const std::string &title="YourSound");
void show_info(const std::string &message, const std::string &title="YourSound");

bool ask_yesno_question(const std::string &prompt, const std::string &message="", const std::string &title="YourSound");
bool ask_yesno_warning(const std::string &prompt, const std::string &message="", const std::string &title="YourSound");