//==================================================================================
// INCLUDES OF GENERAL LIBRARIES
//==================================================================================
#include <stdio.h>          // Standard-IO
#include <stdlib.h>         // Atexit function
#include <string.h>         // String functions
#include <fcntl.h>          // Nonblock input buffer
#include <errno.h>          // Error conditions



//==================================================================================
// INCLUDES OF MY OWN LIBRARIES
//==================================================================================
#include "TerminalInput.h"



//==================================================================================
// GLOBAL VARIABLES
//==================================================================================
int popUpActive = false;    // Status of the of popUp activity



//==================================================================================
// STRUCT DEFINITIONS
//==================================================================================
struct termios orig_termios; // Termios terminal settings



//==================================================================================
// TERMIOS TERMINAL SETTINGS FUNCTIONS
//==================================================================================

// The function resets the terminal settings
void resetTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

// The function enables the raw mode for input processing
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(resetTerminal);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// The function checks for a char in the input buffer
// without blocking the flow of the program
int nonblock_getchar()
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); //Set terminal not to block

    int ch = getchar();

    fcntl(STDIN_FILENO, F_SETFL, flags); // Set terminal to block again

    if (ch == EOF && errno == EAGAIN) {
        return -1; // No input in the buffer
    }
    return ch;
}

// The function changes the folderPath, to append the folder
// "word" to the path, making a new folderPath
char* appendWordToString(char* folderPath, char* word, bool withSlash)
{
    // If folderPath is NULL, return the word
    if (folderPath == NULL) {
        return strdup(word);
    }

    if(withSlash)
    {
        if(folderPath[1] != '\0')
        {
            // Calculate the new length: folderPath + '/' + word + null terminator
            size_t newLength = strlen(folderPath) + 1 + strlen(word) + 1;

            // Allocate memory for the new string
            char* newString = malloc(newLength);
            if (newString == NULL) {
                writeErrorToLogFile("AppendWordToString: Failed to allocate memory");
                return NULL;
            }

            // Copy folderPath, append '/', and append word
            strcpy(newString, folderPath);
            strcat(newString, "/");
            strcat(newString, word);

            return newString;
        }
        else
        {
            // Calculate the new length: '/' + word + null terminator
            size_t newLength = 1 + strlen(word) + 1;

            // Allocate memory for the new string
            char* newString = malloc(newLength);
            if (newString == NULL) {
                writeErrorToLogFile("AppendWordToString: Failed to allocate memory");
                return NULL;
            }

            // Copy '/', and append word
            strcpy(newString, "/");
            strcat(newString, word);

            return newString;
        }
    }
    else
    {
        // Calculate the new length: folderPath + word + null terminator
        size_t newLength = strlen(folderPath) + strlen(word) + 1;

        // Allocate memory for the new string
        char* newString = malloc(newLength);
        if (newString == NULL) {
            writeErrorToLogFile("AppendWordToString: Failed to allocate memory");
            return NULL;
        }

        // Copy folderPath, append '/', and append word
        strcpy(newString, folderPath);
        strcat(newString, word);

        return newString;
    }

}

// The function changes the folderPath, to remove the last
// folder from the path, and setting it as the new one
char* removeLastFolder(char* folderPath)
{
    if (folderPath == NULL || strlen(folderPath) == 0) {
        return strdup("/"); // oder NULL, je nach Anforderung
    }

    int i = strlen(folderPath) - 1;

    // Finde das letzte '/'
    while (i >= 0 && folderPath[i] != '/')
    {
        i--;
    }

    if (i < 0) {
        // Kein '/' gefunden, gib leeren String zurück
        return strdup("");
    }

    // Alloziere Speicher für den neuen String (+1 für '\0')
    char* newString = malloc(i + 1);
    if (newString == NULL) {
        perror("RemoveLastFolder: Failed to allocate memory");
        return NULL;
    }

    // Kopiere die Zeichen bis zum letzten '/'
    strncpy(newString, folderPath, i);
    newString[i] = '\0';

    if(newString[0] == '\0')
    {
        return "/";
    }

    return newString;
}



//==================================================================================
// GENERAL INPUT FOR THE WINDOW FUNCTIONS
//==================================================================================

// The function processes the users general game input to select the next thing that should happen
bool processGameInput(Window* window, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead)
{
    int ch = nonblock_getchar();
    char c = ch;

    if(ch == -1)
    {
        return false;
    }
    else if (c == '\n')
    {
        processEnterInput(&window->cursor, &(*folderPath), folderHead, fileHead);
    }
    else if (c == '\t')
    {
        processTabInput(window);
    }
    else if (c == 127 || c == 8)
    {
        //processBackInputForExplorer();
    }
    else if (c == '\x1b')
    {
        if(popUpActive == false)
        {
            processArrowKeyInputForExplorer(&window->cursor);
        }
        else if(popUpActive == true)
        {
            //processArrowKeyInputForMap();
        }
    }
    //else if (std::isalnum(c))
    //{
        //All normal things (numbers, characters)
        //std::string input(1, c);

        //if(popUpActive == false)
        //{
        //    processNormalInputForExplorer(input);
        //}
        //else if(popUpActive == true)
        //{
            //processNormalInputForMap(input);
        //}
    //}
    return true;
}

// The function processes the enter input of the user for selecting something or opening it
bool processEnterInput(Cursor* cursorPos, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead)
{
    // Interacting with the explorer
    if (popUpActive == false)
    {
        // Interaction with the locations
        if(cursorPos->menu == 'L')
        {
            if(cursorPos->y == 4)
            {
                *folderPath = strdup("/home");
            }
            else if(cursorPos->y == 6)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Desktop", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 8)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Documents", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 10)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Downloads", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 12)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Music", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 14)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Pictures", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 16)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/Videos", getenv("HOME"));
                *folderPath = strdup(path);
            }
            else if(cursorPos->y == 18)
            {
                char path[1024];
                snprintf(path, sizeof(path), "%s/.local/share/Trash/files", getenv("HOME"));
                *folderPath = strdup(path);
            }
        }

        // Interaction with the files
        if(cursorPos->menu == 'F')
        {
            if (!cursorPos || !*folderPath) {
                return false;
            }

            DynamicObject* current = *folderHead;

            if (cursorPos->y == 6)
            {
                *folderPath = removeLastFolder(*folderPath);
                cursorPos->y = 6;
            }
            else
            {
                while (current != NULL)
                {
                    if (cursorPos->x == current->x && cursorPos->y == current->y)
                    {
                        *folderPath = appendWordToString(*folderPath, current->path, true);
                        cursorPos->y = 6;
                        return true;
                    }
                    current = current->nextObject;
                }
            }

            current = *fileHead;

            while (current != NULL)
            {
                if (cursorPos->x == current->x && cursorPos->y == current->y)
                {
                    char* openCommand = "xdg-open ";

                    openCommand = appendWordToString(openCommand, *folderPath, false);
                    openCommand = appendWordToString(openCommand, current->path, true);

                    system(openCommand);

                    return true;
                }
                current = current->nextObject;
            }

        }
    }
    // Interacting with a popUp
    else if (popUpActive == true)
    {
    }

    return true;
}

// The function processes the general inputs of the user
// such as numbers, characters, and special characters
// specifically for the Explorer
void processNormalInputForExplorer(char input){}

// The function processes the tab input of the user
// switchting the panel from left to middle to right
void processTabInput(Window* window)
{
    // List of panels:
    // L = Locations
    // F = Files
    // I = Interactions

    if(window->cursor.menu == 'L')
    {
        window->cursor.menu = 'F';
        window->cursor.x = 26;
        window->cursor.y = 6;
    }
    else if(window->cursor.menu == 'F')
    {
        window->cursor.menu = 'I';
        window->cursor.x = window->terminal.x - 22;
        window->cursor.y = 4;
    }
    else if(window->cursor.menu == 'I')
    {
        window->cursor.menu = 'L';
        window->cursor.x = 4;
        window->cursor.y = 4;
    }
}



//==================================================================================
// ARROW KEY INPUT FOR THE WINDOW FUNCTIONS
//==================================================================================

// Processes if there is an option over the current one
// and switches to it, if possible
void processUpInputForExplorer(Cursor* cursorPos)
{
    if(cursorPos->menu == 'L')
    {
        if(cursorPos->y > 4)
        {
            cursorPos->y = cursorPos->y - 2;
        }
    }
    else if(cursorPos->menu == 'F')
    {
        if(cursorPos->y > 6)
        {
            cursorPos->y = cursorPos->y - 1;
        }
    }
    else if(cursorPos->menu == 'I')
    {
        if(cursorPos->y > 4)
        {
            cursorPos->y = cursorPos->y - 2;
        }
    }
}

// Processes if there is an option under the current one
// and switches to it, if possible
void processDownInputForExplorer(Cursor* cursorPos)
{
    if(cursorPos->menu == 'L')
    {
        if(cursorPos->y < 18)
        {
            cursorPos->y = cursorPos->y + 2;
        }
    }
    else if(cursorPos->menu == 'F')
    {
        cursorPos->y = cursorPos->y + 1;
    }
    else if(cursorPos->menu == 'I')
    {
        if(cursorPos->y < 6)
        {
            cursorPos->y = cursorPos->y + 2;
        }
    }
}

// The function processes the arrow keys input for the Explorer
// to choose the corresponding menu item (ChatGPT)
void processArrowKeyInputForExplorer(Cursor* cursorPos)
{
    char seq[2];
    seq[0] = getchar();
    seq[1] = getchar();
    if (seq[0] == '[')
    {
        switch (seq[1])
        {
            case 'A':
                processUpInputForExplorer(cursorPos);
                break;
            case 'B':
                processDownInputForExplorer(cursorPos);
                break;
        }
    }
}



