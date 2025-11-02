#include "pch.h"
#include "Globals.h"

int Globals::nGamesWon = 0;

int Globals::getNGamesWon()
{
	return Globals::nGamesWon;
}

void Globals::incrementNGamesWon()
{
	Globals::nGamesWon++;
}

const int Globals::getMaxGamesWon()
{
	return Globals::MAX_GAMES_WON;
}


