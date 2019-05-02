// File: GameWin.cpp
// Author: 
// Project: CS215 Project 3 Spring 2016
// Description of file contents:

#include <afxwin.h>
#include "GameWin.h"

GameWin::GameWin ()
{
	// This function will: show the window title

	CString WindowTitle = "The Last Farkle Bender.";
	Create (NULL, WindowTitle);
	myGameP = new Game (this);
	firstTime = true;
}

GameWin::~GameWin()
{
	delete myGameP;
}

afx_msg void GameWin::OnPaint ()
{
	// This function will: 

	myGameP->Display (this);
	if (firstTime)
	{
		myGameP->Instructions(this);
		myGameP->Init(this);		
		Invalidate (TRUE);
		firstTime = false;
	}
}

afx_msg void GameWin::OnLButtonDown( UINT nFlags, CPoint point )
{
	// This function will: set the down button

	myGameP->Click (point, this);
	if (myGameP->Done())
	{
		myGameP->Init(this);
		Invalidate (TRUE);
	}
}

afx_msg void GameWin::OnLButtonDblClk( UINT nFlags, CPoint point )
{	
	// This function will: set the double click 

	OnLButtonDown (nFlags, point);
}

afx_msg void GameWin::OnTimer (UINT nIDEvent)
{	
	// This function will: Handle timer events

	if (nIDEvent == 1)
	{
		myGameP->AIAPlayer (this); 
		if (myGameP->TurnDone())
		{
			if (myGameP->Done())
			{
				myGameP->Init(this);
				Invalidate (TRUE);
			}
		}
	}
}

BEGIN_MESSAGE_MAP (GameWin, CFrameWnd)
	ON_WM_PAINT ()
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONDBLCLK ()
	ON_WM_TIMER ()
END_MESSAGE_MAP ()
