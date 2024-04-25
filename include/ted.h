#ifndef TED_H
#define TED_H

#include "buffer.h"

#define KEY_ENTER '\n'
#define KEY_INSERT 'i'
#define KEY_Q 'q'
#define KEY_ESC 27
#define BUFFER 1024
#define CTRL(x) ((x) & 0x1f)


typedef enum
{
	NORMAL,
	INSERT,
	REPLACE
} MODE;

typedef struct 
{
    buffer** newline;
    size_t linenumber;
    size_t total_size;

}Line;

char* Mode();
void EditorStart();
void NormalMode(int,int,int);

#endif
