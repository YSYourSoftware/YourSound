#include "MessageBox.hpp"

#include <SDL3/SDL_messagebox.h>

#include <format>

constexpr SDL_MessageBoxFlags mb_error_flags   = SDL_MESSAGEBOX_ERROR       | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
constexpr SDL_MessageBoxFlags mb_warning_flags = SDL_MESSAGEBOX_WARNING     | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
constexpr SDL_MessageBoxFlags mb_info_flags    = SDL_MESSAGEBOX_INFORMATION | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;

void show_error(const std::string &message, const std::string &title) {
	SDL_ShowSimpleMessageBox(mb_error_flags, title.c_str(), message.c_str(), nullptr);
};

void show_warning(const std::string &message, const std::string &title) {
	SDL_ShowSimpleMessageBox(mb_warning_flags, title.c_str(), message.c_str(), nullptr);
};

void show_info(const std::string &message, const std::string &title) {
	SDL_ShowSimpleMessageBox(mb_info_flags, title.c_str(), message.c_str(), nullptr);
};

constexpr SDL_MessageBoxButtonData mb_yesno_buttons[] = {
	{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"},
	{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No"}
};

constexpr SDL_MessageBoxButtonData mb_yesno_trust_buttons[] = {
	{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Trust Publisher"},
	{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"},
	{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No"}
};

int yesno_handler(const std::string &prompt, const std::string &message, const std::string &title, const SDL_MessageBoxFlags mb_flags, const SDL_MessageBoxButtonData *buttons, int nbuttons) {
	const std::string format_string = std::format("{}\n{}", prompt, message);

	const SDL_MessageBoxData mb_data{
		mb_flags,
		nullptr,
		title.c_str(),
		format_string.c_str(),
		nbuttons,
		buttons,
		nullptr
	};

	int user_btn = 0;
	SDL_ShowMessageBox(&mb_data, &user_btn);
	return user_btn;
}

bool ask_yesno_question(const std::string &prompt, const std::string &message, const std::string &title) {
	return yesno_handler(prompt, message, title, mb_info_flags, mb_yesno_buttons, 2) == 1;
}

bool ask_yesno_warning(const std::string &prompt, const std::string &message, const std::string &title) {
	return yesno_handler(prompt, message, title, mb_warning_flags, mb_yesno_buttons, 2) == 1;
}