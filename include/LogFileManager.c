//==================================================================================
// INCLUDES OF GENERAL LIBRARIES
//==================================================================================
#include <stdio.h>          // Standard-IO
#include <time.h>           // Current time



//==================================================================================
// INCLUDES OF MY OWN LIBRARIES
//==================================================================================
#include "LogFileManager.h"



//==================================================================================
// FUNCTION DEFINITIONS
//==================================================================================

// The function writes the given error message into the
// standard log file for errors
void writeErrorToLogFile(char* errorMessage)
{
    char* logFileName = "LogFile.log";  // The name of the log file to write errors into

    time_t now;
    time(&now);

    // Convert the time to local time
    struct tm *lokale_zeit = localtime(&now);

    // Open the log file
    FILE *file = fopen(logFileName, "a");
    
    // Check if file got opened
    if (file == NULL) {
        printf("Fehler beim Öffnen der Datei!\n");
        return;
    }

    // Print the current time into the log file
    fprintf(file, "%02d:%02d:%02d: ",
           lokale_zeit->tm_hour, lokale_zeit->tm_min, lokale_zeit->tm_sec);

    // Print the error message into the log file
    fprintf(file, "%s\n", errorMessage);
    
    // Close the log file
    fclose(file);
}



