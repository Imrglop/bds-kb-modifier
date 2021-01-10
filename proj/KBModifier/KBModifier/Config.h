#pragma once
#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using std::string;

class Config
{
public:
	bool load();
	std::string getString(string key);
	float getNumber(string key);
	bool getBool(string key);
	int findInList(std::vector<string> list, string item);
};
#endif
