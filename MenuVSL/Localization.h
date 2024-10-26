#pragma once

#include "pch.h"

class LocalizationLine {
public:
	std::string key;
	std::map<std::string, std::string> languages;

	LocalizationLine(std::string key) {
		this->key = key;
	}
};

class Localization {
public:
    static std::string CurrentLanguage;
    static std::vector<std::string> AvailableLanguages;

    static std::map<std::string, LocalizationLine*> Lines;

    static void RegisterLanguage(std::string language, std::string filePath);
    static bool HasLanguage(std::string language);

    static void RegisterLine(std::string key, std::string language, std::string text);

    static std::string GetLine(std::string key, std::string language);
    static std::string GetLineFormatted(std::string key, va_list args);
    static std::string GetLineFormatted(std::string key, ...);
};