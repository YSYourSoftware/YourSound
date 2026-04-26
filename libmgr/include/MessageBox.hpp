#pragma once

#include <string>

enum MBTrustPublisherResult {
	YES_AND_TRUST = 0,
	YES = 1,
	NO = 2
};

void show_error(const std::string &message, const std::string &title="YourSound Library Manager");
void show_warning(const std::string &message, const std::string &title="YourSound Library Manager");
void show_info(const std::string &message, const std::string &title="YourSound Library Manager");

bool ask_yesno_question(const std::string &prompt, const std::string &message="", const std::string &title="YourSound Library Manager");
bool ask_yesno_warning(const std::string &prompt, const std::string &message="", const std::string &title="YourSound Library Manager");

MBTrustPublisherResult ask_yesno_trust_publisher(const std::string &prompt, const std::string &message, const std::string &publisher, const std::string &title="YourSound Library Manager");