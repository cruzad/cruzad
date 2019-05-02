// File: Game.h
// Author: 
// Project: CS215 Project 3 Spring 2016
// Description of file contents: implements teh functions in the class

#ifndef GAME_H
#define GAME_H

#include <afxwin.h>
#include <vector>
#include <string>
#include <mmsystem.h>

using namespace std;

#define MAX_DICE 6
#define POINTS_TO_WIN 10000
#define MIN_FIRST_SCORE 100

enum game_states { GO, HUMANWINS, AIAWINS, QUITTER };
enum player_type { NONE, HUMAN, AIA };

class Game
{
    public:
	Game (CFrameWnd * windowP);
        ~Game ();
	void Init (CFrameWnd * windowP);
        void Instructions (CFrameWnd * windowP);
        void Display (CFrameWnd * windowP);
	void Click (CPoint point, CFrameWnd * windowP);
	void AIAPlayer (CFrameWnd * windowP);
	bool TurnDone ();
	bool Done ();

    private:
	struct Die
	{
		Die (); 
		void Display (CDC * deviceContextP) const;
		int pips;
		bool selected;
		bool used;			
		CRect where;
	};

	bool Enter (char selection, CFrameWnd * windowP);
	void StartTurn (player_type who);
	void Roll ();
	int CalculateScore (vector <Die> & dice);
	void SaveSelected ();
	void Game::SaveScore ();
	void CheckForWinner (CFrameWnd * windowP);
	void SetUp (CRect window);
	void DisplayGameRect (CDC * deviceContextP);
	void DisplayScoreRect (CDC * deviceContextP);
	void DisplayStatsRect (CDC * deviceContextP);
	void InvalidateRects(int which, CFrameWnd * windowP);
	void Message (CFrameWnd * windowP);
	int SelectDice ();
	bool RollAgain ();

	string title;
	vector <Die> rolledDice;
	vector <Die> remainingDice;
	vector <Die> selectedDice;
	vector <Die> savedDice;

	game_states gameState;
	player_type whoseTurn;
	bool turnDone;
	bool farkle;
	int numToRoll;
	int rolledScore;
	int prevTotal;
	int subTotal;
	int turnTotal;
	bool humanOnBoard;
	bool aiaOnBoard;
	int humanScore;
	int aiaScore;
	int aiaState;
	bool lastTurn;

	CRect locations [MAX_DICE];
	int size;
	int pos;
	CRect currentRect;		
	CRect gameRect;
	CRect scoreRect;		
	CRect statsRect;
	CRect restartRect;
	CRect quitRect;
	CRect rollRect;
	CRect saveRect;
	CRect instructionRect;
	CBitmap bgImage;
};

#endif
