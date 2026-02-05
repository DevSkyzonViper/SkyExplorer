#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

//==================================================================================
// INCLUDES OF GENERAL LIBRARIES
//==================================================================================
#include <stdio.h>              // Standard-IO
#include <stdlib.h>             // Malloc and free
#include <dirent.h>             // Directory Management
#include <stdbool.h>            // Bool variable
#include <string.h>             // String functions
#include <sys/stat.h>           // File/Folder Serial ID
#include <errno.h>              // Error conditions
#include <sys/ioctl.h>          // For ioctl and struct winsize
#include <unistd.h>             // For STDOUT_FILENO
#include <signal.h>             // Terminal interrupts



//==================================================================================
// INCLUDES OF MY OWN LIBRARIES
//==================================================================================
#include "SkyExplorerStructs.h"
#include "LogFileManager.h"



//==================================================================================
// GLOBAL VARIABLES
//==================================================================================
bool terminalSizeChange = false;    // Interrupt signal for terminal size
bool terminateTerminal = false;     // Interrupt signal for terminating the terminal


//==================================================================================
// FUNCTION DEFINITIONS
//==================================================================================

// The function creates a new node and sorts it into the list
// so that the linked list is sorted by filename (A < Z)
void createNewNodeAndSort(DynamicObject** head, char* path)
{
    DynamicObject* newNode = (DynamicObject*)malloc(sizeof(DynamicObject));

    // If the allocation failed
    if (!newNode)
    {
        writeErrorToLogFile("CreateNewNodeAndSort: Memory allocation.");
        return;
    }

    // Fills the object with the data
    newNode->x = 0;
    newNode->y = 0;
    newNode->nextObject = NULL;
    newNode->path = strdup(path);

    // If the copy failed
    if (!newNode->path)
    {
        writeErrorToLogFile("CreateNewNodeAndSort: Strdup");
        free(newNode);
        return;
    }

    // If the first element has a smaller filename (A < Z)
    if (*head == NULL || strcmp(newNode->path, (*head)->path) < 0)
    {
        // The node is the first element
        newNode->nextObject = *head;
        *head = newNode;
        return;
    }

    DynamicObject* current = *head;

    // Checks the linked list, until a bigger filename (Z > A) is found
    while (current->nextObject != NULL && strcmp(current->nextObject->path, newNode->path) < 0)
    {
        current = current->nextObject;
    }

    // Puts the object into the list
    newNode->nextObject = current->nextObject;
    current->nextObject = newNode;
}

// The function clears the given linked list until the head is alone
void clearLinkedList(DynamicObject** head)
{
    DynamicObject* current = *head;
    DynamicObject* next;

    while (current != NULL)
    {
        next = current->nextObject; // Save the next object in the list

        // Free the space of the object
        free(current->path);
        free(current);

        //Change to the next object
        current = next;
    }

    *head = NULL; //Sets the head to show no objects
}

// The function gets the current terminal size
void getTerminalSize(Window* window)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        writeErrorToLogFile("GetTerminalSize: ioctl");
        return;
    }
    window->terminal.x = w.ws_col;
    window->terminal.y = w.ws_row;
}

// The function clears the window by filling it with blank spaces
void clearWindow(Window* window)
{
    for (int i = 0; i < window->size.x * window->size.y; i++)
    {
        window->c[i] = ' ';
    }
}

// The function creates a box onto the window given the position and size of it
void makeBoxWindow(Window* window, int x, int y, int width, int height)
{
    // Checks the bounds of the box to print
    if (x < 0 || y < 0 || x + width > window->size.x || y + height > window->size.y)
    {
        writeErrorToLogFile("MakeBoxWindow: Box is out of bounds");
        return;
    }

    //Creates the box for the window
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int pos = (y + j) * window->size.x + (x + i);
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1)
            {
                window->c[pos] = '#';
            }
            else
            {
                window->c[pos] = ' ';
            }
        }
    }
}

// The function writes the given text onto the window buffer
void writeTextWindow(Window* window, char* text, int x, int y)
{
    int i = 0; 

    while (text[i] != '\0')
    {
        int pos = y * window->size.x + (x + i);
        window->c[pos] = text[i];
        i++;
    }
}

// The function opens the given folderpath, to read the folders and files out of the folderpath,
// sorts them into the corresponding linked list and prints them onto the window
void searchAndWriteDirectory(Window* window, char* folderPath, DynamicObject** folderHead, DynamicObject** fileHead)
{
    // Cleares the folders and files list
    clearLinkedList(folderHead);
    clearLinkedList(fileHead);

    // Prepares the directory walker
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];

    createNewNodeAndSort(folderHead, "..");

    // Opens the folder of the path
    dir = opendir(folderPath);

    // If the folder doesnt exist
    if (dir == NULL)
    {
        writeErrorToLogFile("SearchAndWriteDirectory: opendir.");
        return;
    }

    // Checks the current directory until there are no things
    while ((entry = readdir(dir)) != NULL)
    {
        // Skips hidden files
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        // Updates the object path to the current object
        snprintf(path, sizeof(path), "%s/%s", folderPath, entry->d_name);

        // If the stats of the object got found
        if (stat(path, &statbuf) == 0)
        {
            // Checks the stat and adds the object to the list
            if (S_ISDIR(statbuf.st_mode))
            {
                createNewNodeAndSort(folderHead, entry->d_name);
            }
            else if (S_ISREG(statbuf.st_mode))
            {
                createNewNodeAndSort(fileHead, entry->d_name);
            }
        }
    }

    // Closes the current folder
    closedir(dir);

    // Temporary variables
    DynamicObject* folderNode = *folderHead;
    DynamicObject* fileNode = *fileHead;
    char buffer[257];

    int Anzahl = 0;

    // If not in the root directory "/"
    //if(folderPath[1] != '\0')
    //{
    //    Anzahl = 1;
        // Prints the checkbox for the folder
    //    writeTextWindow(window, "[ ] -", 25, 6);

        // Prints the foldername onto the window
    //    writeTextWindow(window, "[..]", 31, 6);
    //}


    if(folderNode != NULL && fileNode != NULL)
    {
        // Prints the folders onto the middle panel
        while (folderNode != NULL)
        {
            // Stops if the window space isnt enough
            if(window->terminal.y < 13 + Anzahl)
            {
                break;
            }

            // Prints the checkbox for the folder
            writeTextWindow(window, "[ ] -", 25, 6 + Anzahl);

            // Fills the buffer with foldername
            snprintf(buffer, sizeof(buffer), "[%s]", folderNode->path);

            // Prints the foldername onto the window
            writeTextWindow(window, buffer, 31, 6 + Anzahl);

            // Updates the position of the folder on the window
            folderNode->x = 26;
            folderNode->y = 6 + Anzahl;

            // Goes to check the next object
            folderNode = folderNode->nextObject;
            Anzahl++;
        }

        // Makes the seperation line for folders and files
        for(int i = 0; i < window->terminal.x - 50; i++)
        {
            writeTextWindow(window, "-", 23 + i, 7 + (Anzahl * 1));
        }

        // Prints the files onto the middle panel
        while (fileNode != NULL)
        {
            // Stops if the window space isnt enough
            if(window->terminal.y < 13 + Anzahl)
            {
                break;
            }

            // Prints the checkbox for the file
            writeTextWindow(window, "[ ] -", 25, 9 + Anzahl);

            // Fills the buffer with filename
            snprintf(buffer, sizeof(buffer), "%s", fileNode->path);

            // Prints the filename onto the window
            writeTextWindow(window, buffer, 31, 9 + Anzahl);

            // Updates the position of the file on the window
            fileNode->x = 26;
            fileNode->y = 9 + Anzahl;

            // Goes to check the next object
            fileNode = fileNode->nextObject;
            Anzahl++;
        }
    }
    else if(folderNode == NULL && fileNode != NULL)
    {
        // Makes the seperation line for folders and files
        for(int i = 0; i < window->terminal.x - 50; i++)
        {
            writeTextWindow(window, "-", 23 + i, 7 + (Anzahl * 1));
        }

        // Prints the files onto the middle panel
        while (fileNode != NULL)
        {
            // Stops if the window space isnt enough
            if(window->terminal.y < 13 + Anzahl)
            {
                break;
            }

            // Prints the checkbox for the file
            writeTextWindow(window, "[ ] -", 25, 9 + Anzahl);

            // Fills the buffer with filename
            snprintf(buffer, sizeof(buffer), "%s", fileNode->path);

            // Prints the filename onto the window
            writeTextWindow(window, buffer, 31, 9 + Anzahl);

            // Updates the position of the file on the window
            fileNode->x = 26;
            fileNode->y =  + Anzahl;

            // Goes to check the next object
            fileNode = fileNode->nextObject;
            Anzahl++;
        }
    }
    else if(fileNode == NULL && folderNode != NULL)
    {
        // Prints the folders onto the middle panel
        while (folderNode != NULL)
        {
            // Stops if the window space isnt enough
            if(window->terminal.y < 13 + Anzahl)
            {
                break;
            }

            // Prints the checkbox for the folder
            writeTextWindow(window, "[ ] -", 25, 6 + Anzahl);

            // Fills the buffer with foldername
            snprintf(buffer, sizeof(buffer), "[%s]", folderNode->path);

            // Prints the foldername onto the window
            writeTextWindow(window, buffer, 31, 6 + Anzahl);

            // Updates the position of the folder on the window
            folderNode->x = 26;
            folderNode->y = 6 + Anzahl;

            // Goes to check the next object
            folderNode = folderNode->nextObject;
            Anzahl++;
        }
    }

}

// The function clears the terminal screen,
// and the window buffer for rerendering
void clearUI(Window* window)
{
    system("clear");
    clearWindow(window);
}

// The function renders the terminal window by clearing it, showing the default locations
// on the left panel, showing the files and folders in the current folder on the middle
// panel, and interaction options on the left side of the terminal
void render(Window* window, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead)
{
    // Cleares the terminal and window
    clearUI(window);

    // Make a border for the whole window
    makeBoxWindow(window, 0, 0, window->terminal.x - 1, window->terminal.y - 1);

    // Making the titlebar box on top and showing the name in the middle of it
    makeBoxWindow(window, 0, 0, window->terminal.x - 1, 3);
    writeTextWindow(window, "SkyExplorer", (int)(window->terminal.x / 2) - 6, 1);

    // Making and filling the location panel on the left
    makeBoxWindow(window, 0, 2, 22, window->terminal.y - 3);
    makeBoxWindow(window, 0, 20, 22, window->terminal.y - 21);
    writeTextWindow(window, "[ ] Home",     3, 4);
    writeTextWindow(window, "[ ] Desktop",  3, 6);
    writeTextWindow(window, "[ ] Documents",3, 8);
    writeTextWindow(window, "[ ] Downloads",3, 10);
    writeTextWindow(window, "[ ] Music",    3, 12);
    writeTextWindow(window, "[ ] Pictures", 3, 14);
    writeTextWindow(window, "[ ] Videos",   3, 16);
    writeTextWindow(window, "[ ] Trash",    3, 18);


    // Makes a search box and writes the current folder into there
    makeBoxWindow(window, 22, 2, window->terminal.x - 22 - 26, 3);
    writeTextWindow(window, *folderPath, 24, 3);

    // Searches the files and folders in the current folder and creates
    // and fills the middle panel with the sorted list of folders and files
    searchAndWriteDirectory(window, *folderPath, folderHead, fileHead);

    // Making and filling the right interaction panel for the terminal
    makeBoxWindow(window, window->terminal.x - 26, 2, 25, window->terminal.y - 3);
    writeTextWindow(window, "[ ] Create Folder", window->terminal.x - 23, 4);
    writeTextWindow(window, "[ ] Create File", window->terminal.x - 23, 6);

    //Showing the cursor on the screen
    writeTextWindow(window, "*", window->cursor.x, window->cursor.y);

    //Printing out the window onto the terminal
    for (int j = 0; j < window->terminal.y - 1; j++)
    {
        for (int i = 0; i < window->terminal.x - 1; i++)
        {
            printf("%c", window->c[j * window->size.x + i]);
        }
        printf("\n");
    }
}

// The function handles terminal window interrupts
void handle_sigwinch(int sig)
{
    switch (sig)
    {
        // If the terminal size was changed
        case SIGWINCH:
            terminalSizeChange = true;
            break;

        // If the termination key got pressed
        case SIGINT:
            terminateTerminal = true;
            break;
    }
}

// The function initializes the window for the terminal, for a max size of 236x66, and handling the
// window resize interrupt and making the default position of the cursor being the "home"-location
// and enabling raw-mode for the terminal input
void initWindow(Window* window, char** folderPath, DynamicObject** folderHead, DynamicObject** fileHead)
{
    signal(SIGWINCH, handle_sigwinch); // Enabling SIGWINCH interrupt signals
    signal(SIGINT, handle_sigwinch);   // Enabling SIGINT interrupt signals

    getTerminalSize(window); // Getting the terminal size

    //Creating the window itself
    window->size.x = 236;
    window->size.y = 66;
    window->c = (char*)malloc(window->size.x * window->size.y * sizeof(char));

    // Setting standard cursor location
    window->cursor.name = "home";
    window->cursor.menu = 'L';
    window->cursor.x = 4;
    window->cursor.y = 4;

    // Error handling for allocation
    if (window->c == NULL)
    {
        writeErrorToLogFile("InitWindow: Memory allocation.");
        exit(EXIT_FAILURE);
    }

    // Clearing the window
    for (int i = 0; i < window->size.x * window->size.y; i++)
    {
        *(window->c + i) = ' ';
    }

    // Rendering the window for the first time
    render(window, folderPath, folderHead, fileHead);
}




#endif // TERMINAL_UI_H