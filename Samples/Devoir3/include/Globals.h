#ifndef GLOBALS_H
#define GLOBALS_H

class Globals
{
public:
	static int getNGamesWon();
	static void incrementNGamesWon();
	static int getMaxGamesWon();
private:
	static int nGamesWon;
	static constexpr int MAX_GAMES_WON = 3;
};

#endif // !GLOBALS_H

