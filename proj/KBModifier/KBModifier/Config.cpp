// Simpler .properties config

#include "Config.h"
#include <fstream>
#include <iostream>
#include "dllmain.h"

using std::string;
using std::fstream;

std::vector<string> keys;
std::vector<string> values;

const char* filepath = "kb_config.properties";
char splitChar = '=';
const char* fileDefault = "!! Knockback Mod for BDS 1.16.200.2 (1.1.0)\n!! by Imrglop\n\n# Anything starting with '#' or '!' is a comment\n\nformat_version=1.2.0\n\n############################################################\n\n# Sprint knockback: The amount of extra knockback you deal while sprinting\n# default doesn't include W-Tapping, if you set it anything more than \n# default, you will have w-tapping on your server\n\nsprint_knockback=double\n\n# Allowed values:\n! decreased: 0.25\n! default-normal: 0.40 (for non-sprint knockback)\n! default: 0.50\n! extended: 0.75\n! double: 1.0\n! quad: 2.0\n! far: 5.0\n! extreme: 10.0\n\n# Knockback: Non-sprint knockback, normal knockback\n# for this one, set to default-normal for normal\n\nknockback=default-normal\n\n# Allowed values : true / false\n# Will disable knockback.\n\nno_knockback=false\n\n# Description: will disable mob collision\n\nmob_collision_off=false\n\n# Description: whether or not Y knockback changing is enabled\n\nedit_y_knockback=false\n\n# Allowed values: any number\n# Description: The Y knockback, only if edit_y_knockback is enabled\n# Default: 0.40\n\ny_knockback=0.50\n\n# Description: the time (in ticks) in between each hit for mobs/players\n# default: 10\n# allowed values: Any number without decimal\nhurt_time=8\n\n###########################################################";

bool Config::load()
{
	fstream file;
	file.open(filepath, std::ios_base::in);
	string finalCollection = "";
	if (file.fail())
	{
		// file failed
		if (errno == ENOENT) {
			log << filepath << " doesn't exist, trying to make new one...\n";
			std::ofstream newFile(filepath);
			newFile << fileDefault;
			newFile.close();
			std::flush(newFile);
			this->load();
			return true;
		}
		else {
			log << "Couldn't load config! errno: " << errno;
			return false;
		}
		return false;
	}
	// file is good

	string line = "";
	for (string currentLine; std::getline(file, line);)
	{
		std::istringstream toSplit(line);
		if (line[0] == '#' || line[0] == '!') { // comment
			continue;
		}
		std::vector<string> sl;
		for (std::string each; std::getline(toSplit, each, splitChar);) {
			sl.push_back(each);
		};
		if (sl.size() < 2) continue; // empty line
		keys.push_back(sl[0]);
		values.push_back(sl[1]);
	}
	file.close();
	return true;
}

int Config::findInList(std::vector<string> list, string item)
{
	unsigned int i = 0;
	for (; i < list.size(); i++)
	{
		if (list[i].compare(item) == 0) 
			break;
	};
	return i;
}

bool Config::getBool(string key)
{
	return values[findInList(keys, key)].compare("true") == 0 ? true : false;
}

int Config::getInteger(string key)
{
	try {
		string toint = values[findInList(keys, key)];
		return std::stoi(toint);
	}
	catch (std::exception e)
	{
		log << "Config Error: cannot set " << key << " to integer\n";
		return 0;
	}
}

float Config::getNumber(string key)
{
	try {
		string tofloat = values[findInList(keys, key)];
		return std::stof(tofloat);
	}
	catch (std::exception e) {
		log << "Config Error: cannot set " << key << " to number\n";
		return 0.0;
	}
}

unsigned char Config::getByte(string key)
{
	try {
		string tobyte = values[findInList(keys, key)];
		return (unsigned char)std::stoi(tobyte);
	}
	catch (std::exception e)
	{
		log << "Config Error: cannot set " << key << " to byte\n";
		return (unsigned char)0;
	}
}

string Config::getString(string key)
{
	return values[findInList(keys, key)];
}
