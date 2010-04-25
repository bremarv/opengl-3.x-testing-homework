#include "GameManager.h"
#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		GameManager game;
		game.init();
		game.play();
	} catch (std::exception &e)
	{
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
	}
	return 0;
}