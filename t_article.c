#include "Articles.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>


int main(void){
    printf("Testing Articles\n");
    ArticleNode* node = NewArticleNodeWithText("Wibble");
    append(node, "foo..");
    printf("Appending a list of articles\n");
    AppendAll(node, "one,two,three,four");
    print(node, JUSTCONTENTS);
}
