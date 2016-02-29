#include <stdlib.h>

char* makeRepeatedString(char repeatingChar, int count){
    char* tmp = malloc(count);
    int x = 0 ;
    for(x = 0;x< count;x++){
        *(tmp + x) = repeatingChar;
    }
    *(tmp + count) = '\0';
    return tmp;
}

