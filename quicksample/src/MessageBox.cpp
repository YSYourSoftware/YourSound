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

#ifdef _WIN32 // task dialog specific
#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

std::wstring to_wstring(const std::string& str) {
	if (str.empty()) return {};

	const int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		nullptr,
		0
	);

	std::wstring result(size_needed, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		&result[0],
		size_needed
	);

	return result;
}

int yesno_handler(const std::string &prompt, const std::string &message, const std::string &title, HICON hicon, PCWSTR psz_icon, bool use_hicon) {
	const std::wstring wide_prompt = to_wstring(prompt);
	const std::wstring wide_message = to_wstring(message);
	const std::wstring wide_title = to_wstring(title);

	TASKDIALOGCONFIG config{};
	config.cbSize = sizeof(config);
	config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
	config.dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;

	config.pszWindowTitle = wide_title.c_str();
	config.pszContent = wide_message.c_str();
	config.pszMainInstruction = wide_prompt.c_str();

	if (use_hicon) {
		config.dwFlags |= TDF_USE_HICON_MAIN;
		config.hMainIcon = hicon;
	} else {
		config.pszMainIcon = psz_icon;
	}

	int button_pressed = 0;
	TaskDialogIndirect(&config, &button_pressed, nullptr, nullptr);

	return button_pressed;
}

bool ask_yesno_question(const std::string &prompt, const std::string &message, const std::string &title) {
	HICON hQuestionIcon = LoadIcon(nullptr, IDI_QUESTION);

	return yesno_handler(prompt, message, title, hQuestionIcon, nullptr, true) == 6;
}

bool ask_yesno_warning(const std::string &prompt, const std::string &message, const std::string &title) {
	return yesno_handler(prompt, message, title, nullptr, TD_WARNING_ICON, false) == 6;
}
#else
constexpr SDL_MessageBoxButtonData mb_yesno_buttons[] = {
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
};

bool ask_yesno_warning(const std::string &prompt, const std::string &message, const std::string &title) {
	return yesno_handler(prompt, message, title, mb_warning_flags, mb_yesno_buttons, 2) == 1;
};
#endif