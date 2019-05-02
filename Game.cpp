// File: Game.cpp
// Author: 
// Project: CS215 Project 3 Spring 2016
// Description of file contents:

#include "Game.h"
#include <mmsystem.h>
#include <ctime>
#include <cstdlib>


static CBitmap images [7];

Game::Die::Die ()
{
// This function will: implement the die

	pips = 0;
	selected = false;
	used = false;
}

void Game::Die::Display (CDC * deviceContextP) const
{
// This function will: display the background

	if (selected)
		deviceContextP->Rectangle (where);
	CDC memDC;
	int res = memDC.CreateCompatibleDC(deviceContextP);
	memDC.SelectObject(&images[pips]);
	deviceContextP->TransparentBlt(where.left+1, where.top+1, where.Width()-2, where.Height()-2, &memDC, 0, 0, 200, 200, SRCCOPY);
	DeleteDC(memDC);
}

Game::Game (CFrameWnd * windowP)
{
	prevTotal = 0;
	subTotal = 0;
	turnTotal = 0;
	whoseTurn = NONE;
	humanScore = 0;
	aiaScore = 0;
	bgImage.LoadBitmap(CString("BACKGROUND_BMP"));
	images[1].LoadBitmap(CString("Dice1_BMP"));
	images[2].LoadBitmap(CString("Dice2_BMP"));
	images[3].LoadBitmap(CString("Dice3_BMP"));
	images[4].LoadBitmap(CString("Dice4_BMP"));
	images[5].LoadBitmap(CString("Dice5_BMP"));
	images[6].LoadBitmap(CString("Dice6_BMP"));
	//BOOL soundPlayed = PlaySound(CString("SOUND_WAV"),GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
	srand (unsigned (time (NULL)));
	windowP->GetClientRect (&currentRect);
	SetUp (currentRect);
	rolledDice.resize(MAX_DICE, Die());
}

Game::~Game ()
{
// This function will: not do anything because destruct the game

}
void Game::Init (CFrameWnd * windowP)
{
// This function will: initializes the variables

	gameState = GO;
	whoseTurn = NONE;
	aiaState = 0;
	humanOnBoard = false;
	aiaOnBoard = false;
	humanScore = 0;
	aiaScore = 0;
	lastTurn = false;
	Enter ('t', windowP);
}

void Game::Instructions (CFrameWnd * windowP)
{
// This function will display the instructions

	char buffer [100];
	CString message = "Welcome to My Farkle Game\n\n";
	message += "You must get on board and if your turn total is\n\n";
	message += "less than 500 you keep rolling to stay on board\n\n";
	message += "the goal of the game is win the most points\n\n";
	sprintf_s (buffer, "The goal of this game is to accumulate %d\n", POINTS_TO_WIN);
	
	
	message += buffer;
	message += "points before your opponent Firelord Ozai.\n";
	CString title = "Instructions";
	windowP->MessageBox (message, title);
}

void Game::StartTurn (player_type who)
{
// This function will: initializes the variables

	if (gameState != GO)
		return;

	whoseTurn = who;
	turnDone = false;
	farkle = false;
	numToRoll = MAX_DICE;
	savedDice.clear ();
	prevTotal = 0;
	subTotal = 0;
	turnTotal = 0;
}

void Game::Display (CFrameWnd * windowP) 
{
// This function will: display what is in the boxes

	CPaintDC dc (windowP);
	CRect rect;
	windowP->GetClientRect (&rect);
	if (rect != currentRect)
	{	
		SetUp (rect);
		currentRect = rect;
	}
	dc.SetBkMode(TRANSPARENT);			
	CDC memDC;
	int res = memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&bgImage);
	dc.TransparentBlt (0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, 1899, 1145, SRCCOPY); 
	DeleteDC (memDC);

	CFont font;
	CString fontName = "Comic Sans MS";
	font.CreatePointFont(rect.Width() / 10, fontName, &dc);
	CFont* def_font = dc.SelectObject(&font);

	DisplayScoreRect (&dc);
	DisplayStatsRect (&dc);
	DisplayGameRect (&dc);

	dc.Rectangle(rollRect);
	dc.DrawText(CString ("Roll again"), rollRect, DT_CENTER);
	dc.Rectangle(saveRect);
	dc.DrawText(CString ("Save score"), saveRect, DT_CENTER);
	dc.Rectangle(instructionRect);
	dc.DrawText(CString ("Instructions"), instructionRect, DT_CENTER);
	dc.Rectangle(quitRect);
	dc.DrawText(CString("Quit"), quitRect, DT_CENTER);
	dc.Rectangle(restartRect);
	dc.DrawText(CString("Restart"), restartRect, DT_CENTER);
	//dc.Rectangle(scoreRect);
	//dc.DrawText(CString("Score"), scoreRect, DT_CENTER);
	//dc.Rectangle(statsRect);
	//dc.DrawText(CString("Statistics"), statsRect, DT_CENTER);
	dc.SelectObject(def_font);
}

void Game::SetUp (CRect rect)
{
	// This function will: set up the game and display where the boxes will be on the screen

	int width = rect.Width();
	int height =  rect.Height();
	gameRect = CRect (width/3, 5*height/12, 2*width/3, 11*height/12);
	CRect buttonRect = CRect (17*width/24, 3*height/4, 23*width/24, 11*height/12);
	rollRect = CRect (buttonRect.left, buttonRect.top, buttonRect.left + 2*buttonRect.Width()/5, buttonRect.bottom);
	saveRect = CRect (buttonRect.right - 2*buttonRect.Width()/5, buttonRect.top, buttonRect.right, buttonRect.bottom);
	instructionRect = CRect(0, 3 * height /4 , width / 6, 10 * height / 12);
	quitRect = CRect(0, 7 * height / 12, width / 6, 2 * height / 3);
	restartRect = CRect(0, 11 * height / 12, width / 6, height);
	scoreRect = CRect(3 * width / 4, height / 12, width, height / 3);
	statsRect = CRect(4, height / 12, width / 4, height / 3);

	int rows = 2, cols = 3;
	while (rows * cols < MAX_DICE)
	{
		rows++;
		cols++;
	}
	width = gameRect.Width() / (1 + cols * 4);
	height = gameRect.Height() / (1 + rows * 4);	
	int d = 0;
	for (int r = 0; d < MAX_DICE && r < rows; r++)
	{
		int top = gameRect.top + height * (1 + r * 4);
		for (int c = 0; d < MAX_DICE && c < cols; c++)
		{
			int left = gameRect.left + width * (1 + c * 4);
			locations[d] = CRect (left, top, left + 3 * width, top + 3 * height);
			d++;
		}
	}
}

void Game::DisplayGameRect (CDC * deviceContextP)
{
	// This function will: display whatever is in the rectangle

	//deviceContextP->Rectangle(gameRect);

	for (unsigned d = 0; d < rolledDice.size(); d++)
	{
		rolledDice[d].where = locations[d];
		rolledDice[d].Display (deviceContextP);
	}
	char msgString [100];
	//sprintf_s (msgString, "Change the value of MAX_DICE from %d to 6", MAX_DICE);
	CRect bottomHalf = CRect (gameRect.left, (gameRect.top + gameRect.bottom) / 2, gameRect.right, gameRect.bottom);
	//deviceContextP->DrawText(CString (msgString), bottomHalf, DT_CENTER);
}

void Game::DisplayScoreRect (CDC * deviceContextP)
{ 
	// This function will display the human and aia score
	deviceContextP->Rectangle(scoreRect);
	char user[100];
	char aia[100];
	
	
	//sprintf_s(user, "Your Score: %d", humanScore);
	sprintf_s(user, "Human Score: %d", humanScore);
	CRect user1 = CRect(scoreRect.left, (scoreRect.top + scoreRect.bottom) / 2, scoreRect.right, scoreRect.bottom);
	deviceContextP->DrawText(CString(user), user1, DT_CENTER);

	sprintf_s(aia, "AIA Score: %d", aiaScore);
	CRect aia1 = CRect(scoreRect.left, scoreRect.top, scoreRect.right, (scoreRect.bottom + scoreRect.top) / 2);
	deviceContextP->DrawText(CString(aia), aia1, DT_CENTER);

	//deviceContextP->Rectangle(scoreRect);
}

void Game::DisplayStatsRect (CDC * deviceContextP)
{
	// This function will: whose turn it is and score turn
	char whoseturn[100];
	char total[100];
	deviceContextP->Rectangle(statsRect);
	CRect total1 = CRect(statsRect.left, (statsRect.top + statsRect.bottom) / 2, statsRect.right, statsRect.bottom);
	CRect user1 = CRect(statsRect.left, statsRect.top, statsRect.right, (statsRect.bottom + statsRect.top) / 2);
	if (whoseTurn == HUMAN)
	{
		sprintf_s(whoseturn, "Your Turn");
	}
	else if (whoseTurn == AIA)
	{
		sprintf_s(whoseturn, "Firelord Ozai Turn");
	}
	else
	{
		sprintf_s(whoseturn, "");
	}
	sprintf_s(total, "Turn Score: %d", turnTotal);
	deviceContextP->DrawText(CString(total), user1, DT_CENTER);
	deviceContextP->DrawText(CString(whoseturn),total1, DT_CENTER);
	//deviceContextP->Rectangle(statsRect);
	//deviceContextP->Rectangle(statsRect);
}

void Game::Click (CPoint point, CFrameWnd * windowP)
{
// This function will: set the clicks for the boxes and allow you to click on it
	if (whoseTurn != HUMAN)
		return;
	char selection = ' ';
	if (rollRect.PtInRect(point))
		selection = 'r';
	else if (saveRect.PtInRect(point))
		selection = 's';
	else if (restartRect.PtInRect(point))
		selection = 'n';
	else if (quitRect.PtInRect(point))
		selection = 'q';
	else if (instructionRect.PtInRect(point))
		selection = 'i';
	else
		for (unsigned d = 0; d < rolledDice.size(); d++)
			if (rolledDice[d].where.PtInRect (point))
				selection = 'a' + d;
	Enter (selection, windowP);
}

bool Game::Enter (char selection, CFrameWnd * windowP)
{
// This function will handle all the cases for each turn

	if (whoseTurn == AIA)
		return false;
	if (selection == 't') // Start Turn
	{
		StartTurn (HUMAN);
		Roll ();
		InvalidateRects(3, windowP);
		if (farkle)
			selection = 's';
	}
	if (selection == 'r') // Roll Dice
	{
		SaveSelected();
		if (!farkle)
			Roll ();
		InvalidateRects(3, windowP);
		if (farkle)
			selection = 's';
	}
	if (selection == 's') // Save Dice and End Turn
	{
		SaveSelected();
		InvalidateRects(5, windowP);
		SaveScore();
		CheckForWinner(windowP);
		if (gameState == GO)
		{
				aiaState = 1;
				AIAPlayer(windowP);
		}
	}
	if (selection >= 'a' && selection <= MAX_DICE+'a')
	{
		int which = selection - 'a';
		if (which < 0 || which >= int (rolledDice.size()))
			return false;
		rolledDice[which].selected = !rolledDice[which].selected;
		InvalidateRects(1, windowP);
	}
	if (selection == 'i') // Show Instructions
	{
		Instructions (windowP);
	}
	if (selection == 'n') // Start New Game
	{
		Init (windowP);
		InvalidateRects(7, windowP);
	}
	if (selection == 'q') // Exit From Game
	{
		gameState = QUITTER;
		Message (windowP);
		exit (1);
	}
	
	return true;
}

bool Game::TurnDone ()
{
	// This function will tell when the game is done.

	return turnDone;
}

void Game::AIAPlayer(CFrameWnd * windowP)
{
	//This function is the artifical intelligent computer who will play against you
	if (whoseTurn == HUMAN)
		return;
	switch (aiaState)
	{
	case 0:
		break;
	case 1:
		//Start timer
		windowP->SetTimer(1, 1500, NULL);
		aiaState = 2;
		InvalidateRects(7, windowP);
		break;
	case 2:
		StartTurn(AIA);
		aiaState = 3;
		InvalidateRects(7, windowP);
		break;
	case 3:
		Roll();
		if (farkle)
			aiaState = 6;
		else		
			aiaState = 4;
		InvalidateRects(7, windowP);
		break;
	case 4:
		SelectDice();
		aiaState = 5;
		InvalidateRects(1, windowP);
	case 5:
		SaveSelected();
		if (RollAgain() == true)
			aiaState = 3;
		else
			aiaState = 6;
		InvalidateRects(7, windowP);
		break;
	case 6:
		SaveScore();
		aiaState = 7;
		InvalidateRects(7, windowP);
		break;
	case 7:
		aiaState = 0;
		//stop timer
		windowP->KillTimer(1);
		CheckForWinner(windowP);
		if (gameState == GO)
			Enter('t', windowP);
		break;
	}






	// This function will:
		/*
		//if (whoseTurn == HUMAN)
			//return;
		StartTurn();
		while (!turnDone)
		{
			wait();
			//Roll();
			outs << *this;
			if (aiaOnBoard == false)
			{
				int score = CalculateScore(rolledDice);
				for (int i = 0; i < rolledDice.size(); i++)
				{

					if (rolledDice[i].used == true)
					{
						rolledDice[i].selected = true;
						//SaveSelected();
					}

				}
				SaveSelected();
				wait();
				if (turnTotal < 500)
					Roll();
				else
					SaveScore();
			}
			else if (lastTurn == false && aiaScore < POINTS_TO_WIN)
			{
				int score = CalculateScore(rolledDice);
				for (int i = 0; i < rolledDice.size(); i++)
				{
					if (rolledDice[i].used)
						rolledDice[i].selected = true;
				}
				SaveSelected();
				//wait();
				if (turnTotal < 500)
					Roll();
				else
					SaveScore();



			}
		}
		return turnTotal;
	}


		return;
	}*/
}
bool Game::Done ()
{
	return gameState != GO;
}

void Game::Message (CFrameWnd * windowP)
{
// This function will display the messages when you click on the buttons.

	CString message;
	switch (gameState)
	{
		case HUMANWINS:
			message = "Avatar win.s"; break;
		case AIAWINS: 
			message = "Firelord Ozai wins."; break;
		case QUITTER:
			message = "Good-bye."; break;
	}
	CString title = "Game Over";
	windowP->MessageBox (message, title);
	//Init (windowP);
}

void Game::Roll ()
{
	// This function will roll the dice.

	farkle = false;
	rolledDice.clear ();
	selectedDice.clear ();
	remainingDice.clear ();
	for (int i = 0; i < numToRoll; i++)
	{
		Die one;
		one.pips = 1 + rand() % 6;
		rolledDice.push_back (one);
	}
	rolledScore = CalculateScore (rolledDice);
	if (rolledScore == 0)
		farkle = true;
}

int Game::CalculateScore (vector <Die> & dice)
{
	//This function will calculate the score for each every roll
	static int points [7][7] = 
	   {{0,   0,   0,    0,    0,    0,    0}, 
		{0, 100, 200, 1000, 2000, 4000, 8000},
		{0,   0,   0,  200,  400,  800, 1600},
		{0,   0,   0,  300,  600, 1200, 2400},
		{0,   0,   0,  400,  800, 1600, 3200},
		{0,  50, 100,  500, 1000, 2000, 4000},
		{0,   0,   0,  600, 1200, 2400, 4800}};

	int counts [7] = {0, 0, 0, 0, 0, 0, 0};
	for (unsigned i = 0; i < dice.size(); i++)
	{
		counts[dice[i].pips]++;
		dice[i].used = false;
	}
	int ones = 0, twos = 0;
	int score = 0;
	for (int i = 1; i <= 6; i++)
	{
		if (counts[i] == 1)
			ones++;
		if (counts[i] == 2)
			twos++;
		int pts = points[i][counts[i]];
		if (pts > 0)
			for (unsigned j = 0; j < dice.size(); j++)
				if (dice[j].pips == i)
					dice[j].used = true; 
		score += pts;
	}
	if (ones == 6 || twos == 3)
		for (unsigned j = 0; j < dice.size(); j++)
			dice[j].used = true; 
	if (ones == 6)
		score = 1000;
	else if (twos == 3)
		score = 500;
	return score;
}


void Game::SaveScore ()
{
	//This function will save the score of the AIA and the HUMAN
	if (whoseTurn == HUMAN)
	{
		if (turnTotal >= MIN_FIRST_SCORE)
		{
			humanOnBoard = true;
			turnDone = true;
		}
		if (humanOnBoard == true)
			humanScore = humanScore + turnTotal;
	}

	else
	{
		if (turnTotal >= MIN_FIRST_SCORE)
		{
			aiaOnBoard = true;
			turnDone = true;
		}
		if (aiaOnBoard == true)
			aiaScore = aiaScore + turnTotal;

	}
		//aiaScore += turnTotal;
	
	
	/*if (CalculateScore(selectedDice) == 0)
	{
		turnTotal = 0;
		turnDone = true;
	}
	else
	{
		turnTotal = turnTotal + CalculateScore(selectedDice);
	}*/
	

	
	
	/*
	if (aiaScore >= POINTS_TO_WIN || humanScore >= POINTS_TO_WIN)
	{
		lastTurn = true;
		turnDone = true;
	}
	if (lastTurn == true && aiaScore > humanScore)
	{
		gameState = AIAWINS;
		turnDone = true;
	}
	else if (lastTurn == true && humanScore > aiaScore)
	{
		gameState = HUMANWINS;
		turnDone = true;
	}

*/
	turnDone = true;
	
}

void Game::CheckForWinner (CFrameWnd * windowP)
{	
	//This function will check to see who the winner is of the game.
	whoseTurn = NONE;	
	
	if (lastTurn == true && aiaScore > humanScore)
	{
		gameState = AIAWINS;
		turnDone = true;
	}
	else if (lastTurn == true && humanScore > aiaScore)
	{
		gameState = HUMANWINS;
		turnDone = true;
	}

	if (aiaScore >= POINTS_TO_WIN || humanScore >= POINTS_TO_WIN)
	{
		lastTurn = true;
		turnDone = true;
	}

	turnDone = true;


	if (gameState != GO)
		Message (windowP);
}
void Game::InvalidateRects(int which, CFrameWnd * windowP)
{
	//This function will make it so your screen does not flicker
	if (which & 1)
		windowP->InvalidateRect(gameRect);
	if (which & 2)
		windowP->InvalidateRect(statsRect);
	if (which & 4)
		windowP->InvalidateRect(scoreRect);
}

void Game::SaveSelected ()
{
	//This function will select what type of dice you save.
	selectedDice.clear();
	remainingDice.clear();
	for (int i = 0; i < rolledDice.size(); i++)
	{
		if (rolledDice[i].selected)
		{
			selectedDice.push_back(rolledDice[i]);
			//savedDice.push_back(rolledDice[i]);
		}
		else
		{
			remainingDice.push_back(rolledDice[i]);
		}
	}

	subTotal = CalculateScore(selectedDice);


	int j = 0;
	while (j < selectedDice.size())
		//for(int j = 0; j < selectedDice.size(); j++);
	{
		//cout << selectedDice[j];
		if (selectedDice[j].used)
		{
			savedDice.push_back(selectedDice[j]);
		}
		j++;
	}
	numToRoll = remainingDice.size();
	if (numToRoll == 0)
	{
		numToRoll = MAX_DICE;
	}

	if (subTotal == 0 || selectedDice.size() == 0)
	{
		turnDone = true;
		farkle = true;
		//SaveScore();
	}
	turnTotal += subTotal;
	if (farkle)
		turnTotal = 0;


}
int Game::SelectDice()
{
	//This function will select certain dice to caluclate the score
	int score = CalculateScore(rolledDice);
	for (int i = 0; i < rolledDice.size(); i++)
	{
		if (rolledDice[i].used == true)
			rolledDice[i].selected = true;
	}
	return 0;
}


bool Game:: RollAgain()
{
	//This function will roll again until turnTotal if it is less than 500
	if (turnTotal < 500)
		return true;
	else
		return false;
}