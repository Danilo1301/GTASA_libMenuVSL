#include "Localization.h"
#include "ModConfig.h"

#include <cstdarg> // Para va_list, va_start, va_arg, va_end
#include <cstdio>  // Para sprintf
#include <string>
#include "Log.h"

std::string Localization::CurrentLanguage = "";
std::vector<std::string> Localization::AvailableLanguages;

std::map<std::string, LocalizationLine*> Localization::Lines;

void Localization::RegisterLanguage(std::string language, std::string filePath)
{
	if(HasLanguage(language)) return;

    AvailableLanguages.push_back(language);

	Json::Value value = ModConfig::ReadFile(filePath);

	for (auto member : value.getMemberNames())
	{
		RegisterLine(member, language, value[member].asString());
	}
}

bool Localization::HasLanguage(std::string language)
{
	return stringExistsInVector(AvailableLanguages, language);
}

void Localization::RegisterLine(std::string key, std::string language, std::string text)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Localization: RegisterLine " << key << " (" << language << ") " << "'" << text << "'" << std::endl;

	if (Lines.count(key) == 0)
	{
		Lines[key] = new LocalizationLine(key);
	}

	auto line = Lines[key];
	line->languages[language] = text;
}

std::string Localization::GetLine(std::string key, std::string language)
{
	if (Lines.count(key) > 0)
	{
		auto line = Lines[key];

		if (line->languages.count(language) > 0)
		{
			//return "[" + language + "]" + line->languages[language];
			return line->languages[language];
		}
	}

	return "[" + language + "] " + key;
}

std::string Localization::GetLineFormatted(std::string key, va_list args)
{
    char buffer[256];  // Buffer para armazenar a string formatada

    std::string line = GetLine(key, CurrentLanguage);  // Obtém a linha como formato

    // Formata a string com os argumentos variáveis usando a linha como formato
    vsnprintf(buffer, sizeof(buffer), line.c_str(), args);

    return std::string(buffer);
}

std::string Localization::GetLineFormatted(std::string key, ...)
{
    va_list args;
    va_start(args, key);

    // Chama a versão que usa va_list e a linha como formato
    std::string result = GetLineFormatted(key, args);

    va_end(args);

    return result;
}