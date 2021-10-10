#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

enum type; 

struct quad; 
struct vertice;
typedef unsigned int face[4];

int scale = 50;

int next_char_pointer(char *content, int current_offset, int lim, char delim);

char *out_file;
char *in_file;
int swap_endiness(int value);
int main(int argc, char *argv[]);
