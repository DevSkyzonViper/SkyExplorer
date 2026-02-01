#ifndef SKY_EXPLORER_STRUCTS_H
#define SKY_EXPLORER_STRUCTS_H


//==================================================================================
// STRUCT DEFINITIONS
//==================================================================================
extern struct termios orig_termios; // Termios terminal settings

typedef struct
{
    char* name;
    char menu;
    int x;
    int y;
} Cursor;           // Cursor that is on the window

typedef struct
{
    char* path;
    int x;
    int y;
} StaticObject;     // A static file / folder object

typedef struct DynamicObject
{
    char* path;
    int x;
    int y;
    struct DynamicObject* nextObject;
} DynamicObject;    // A linked list file / folder object

typedef struct
{
    int x;
    int y;
} Size;     // Size of an GUI component

typedef struct
{
    Cursor cursor;
    Size size;
    Size terminal;
    char* c;
    bool popUpActive;
} Window;   // Terminal Window




#endif //SKY_EXPLORER_STRUCTS_H
