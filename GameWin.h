// File: GameWin.h
// Author: 
// Project: CS215 Project 3 Spring 2016
// Description of file contents:

#include <afxwin.h>
#include "Game.h"

class GameWin : public CFrameWnd
{
	public:
		GameWin ();
		~GameWin ();
		afx_msg void OnPaint ();
		afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
		afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point ); 
		afx_msg void OnTimer ( UINT nIDEvent );
	private:
		Game * myGameP;
		bool firstTime;
		DECLARE_MESSAGE_MAP ()
};
