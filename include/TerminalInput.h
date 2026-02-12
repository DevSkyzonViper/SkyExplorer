#ifndef TERMINAL_INPUT_H
#define TERMINAL_INPUT_H

//==================================================================================
// INCLUDES OF GENERAL LIBRARIES
//==================================================================================
#include <stdbool.h>        // Bool variable
#include <termios.h>        // Terminal Raw-Mode
#include <unistd.h>         // For STDOUT_FILENO



//==================================================================================
// GLOBAL VARIABLES
//==================================================================================
extern int popUpActive;     // Status of the of popUp activity



//==================================================================================
// INCLUDES OF MY OWN LIBRARIES
//==================================================================================
#include "SkyExplorerStructs.h"
#include "LogFileManager.h"



//==================================================================================
// TERMIOS TERMINAL SETTINGS FUNCTIONS
//==================================================================================

// The function resets the terminal settings
void resetTerminal();

// The function enables the raw mode for input processing
void enableRawMode();

// The function checks for a char in the input buffer
// without blocking the flow of the program
int nonblock_getchar();



//==================================================================================
// GENERAL INPUT FOR THE WINDOW FUNCTIONS
//==================================================================================

// The function processes the users general game input to select the next thing that should happen
bool processGameInput(Window* window, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);

// The function processes the enter input of the user for selecting something or opening it
bool processEnterInput(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);

// The function processes the general inputs of the user
// such as numbers, characters, and special characters
// specifically for the menu
void processNormalInputForExplorer(char input);

// The function processes the tab input of the user
// switchting the panel from left to middle to right
void processTabInput(Window* window);

bool processBackInputForExplorer(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);




//==================================================================================
// ARROW KEY INPUT FOR THE WINDOW FUNCTIONS
//==================================================================================

// Processes if there is an option over the current one
// and switches to it, if possible
void processUpInputForExplorer(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);

// Processes if there is an option under the current one
// and switches to it, if possible
void processDownInputForExplorer(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);

bool processRightInputForExplorer(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);

// The function processes the arrow keys input for the menu
// to choose the corresponding menu item (ChatGPT)
bool processArrowKeyInputForExplorer(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead);




#endif //TERMINAL_INPUT_H