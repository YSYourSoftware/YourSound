#pragma once

#include <string>

enum class SaveWorkResponse : uint8_t { SAVE = 0, DISCARD = 1, CANCEL = 2 };

void show_error(const std::string &message, const std::string &title = "YourSound QuickSample");
void show_warning(const std::string &message, const std::string &title = "YourSound QuickSample");
void show_info(const std::string &message, const std::string &title = "YourSound QuickSample");

bool ask_yesno_question(const std::string &prompt, const std::string &message = "",
						const std::string &title = "YourSound QuickSample");
bool ask_yesno_warning(const std::string &prompt, const std::string &message = "",
					   const std::string &title = "YourSound QuickSample");

SaveWorkResponse warn_unsaved_work(const std::string &prompt, const std::string &message = "",
								   const std::string &title = "YourSound QuickSample");

bool ask_abort_retry_warning(const std::string &prompt, const std::string &message,
							 const std::string &title = "YourSound QuickSample");
