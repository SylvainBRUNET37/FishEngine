#include "pch.h"
#include "Globals.h"

int Globals::nGamesWon = 0;

int Globals::getNGamesWon()
{
	return nGamesWon;
}

void Globals::incrementNGamesWon()
{
	nGamesWon++;
}

int Globals::getMaxGamesWon()
{
	return MAX_GAMES_WON;
}


