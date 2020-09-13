#include <stdio.h> 
#include <stdlib.h> 
#include "solkattu.h"

// ----------------------------------------------------------------------------
// prototypes

static sol *parse(char *text);
// ----------------------------------------------------------------------------

sol *parse(char *text) {
    int cur_pos = 0;
    char solkattu[100][4];			//max sol length is 4 (thom)
    int i = 0; 
    while(1){ 
        char r = text[cur_pos]; 
        int j = 0; 
        while(r != ' ' && text[cur_pos] != '\0') {
            solkattu[i][j++] = r;
            r = text[cur_pos]; 
        } 
        solkattu[i][j] = '\0';
        if(text[cur_pos] == '\0'){
            break; 
        } 
        i++; 
    }
}