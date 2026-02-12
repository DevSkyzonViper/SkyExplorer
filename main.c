//==================================================================================
// INCLUDES OF GENERAL LIBRARIES
//==================================================================================
#include <stdio.h>          // Standard-IO
#include <stdbool.h>        // Bool variable



//==================================================================================
// INCLUDES OF MY OWN LIBRARIES
//==================================================================================
#include "./include/TerminalInput.h"    // Process terminal input
#include "./include/TerminalUI.h"       // Terminal User Interface



//==================================================================================
// MAIN FUNCTION
//==================================================================================

int main(void)
{
    // Program running bool
    bool running = true;

    // Current Folder and Files, as linked lists
    DynamicObject* folderHead = NULL;
    DynamicObject* fileHead = NULL;

    // The window and path of the current directory
    Window window;
    char* folderPath = "/home/sky/Documents";

    // Initializing the window for the terminal
    initWindow(&window, &folderPath, &folderHead, &fileHead);
    enableRawMode();
    
    while (running)
    {
        // Interrupt that the terminal was resized
        if(terminalSizeChange)
        {
            // Updating the size
            getTerminalSize(&window);
            render(&window, &folderPath, &folderHead, &fileHead);
            terminalSizeChange = false;
        }

        // Interrupt that the terminal should get closed
        if(terminateTerminal)
        {
            running = false;
        }

        // Processing game input from keyboard buffer
        if(processGameInput(&window, &folderPath, &folderHead, &fileHead))
        {
            // Rerenders if an input was made
            render(&window, &folderPath, &folderHead, &fileHead);
        }
    }

    return 0;
}

