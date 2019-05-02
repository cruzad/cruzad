// File: GameApp.cpp
// Author: 
// Project: CS215 Project 3 Spring 2016
// Description of file contents:

#include <afxwin.h>
#include "GameApp.h"


BOOL GameApp::InitInstance ()
{
	// This function will: update the game window

	m_pMainWnd = new GameWin();
	m_pMainWnd->ShowWindow (m_nCmdShow);
	m_pMainWnd->UpdateWindow ();

	return TRUE;
}

GameApp GameApp;
