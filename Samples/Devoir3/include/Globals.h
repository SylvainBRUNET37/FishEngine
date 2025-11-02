#ifndef GLOBALS_H
#define GLOBALS_H

class Globals
{
public:
	static int getNGamesWon();
	static void incrementNGamesWon();
	static const int getMaxGamesWon();
private:
	static int nGamesWon;
	static const int MAX_GAMES_WON = 3;
};

#endif // !GLOBALS_H

