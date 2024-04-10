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


typedef struct node
{
	char* text;
	int line;
	size_t size;
	struct node* next;
	struct node* prev;
}node;
