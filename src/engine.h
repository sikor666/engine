#pragma once

#include <string>

class engine
{
public:
	engine() : version("1.0.0")
	{
	}

	int run();

	std::string version;
};
