typedef struct ArticleNode {
   struct ArticleNode *next;
   char* ArticleText;
} ArticleNode;

typedef enum PrintStyle{
    JUSTCONTENTS,
    CONTENTS_AND_POINTER
}PrintStyle;

ArticleNode* NewArticleNodeWithText( const char* newText);
void print(ArticleNode* head, PrintStyle style);
void append(ArticleNode* head, char* _new);
void AppendAll(ArticleNode* head,  char* CommaSepString);
