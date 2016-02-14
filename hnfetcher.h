void FetchArticles(struct FetchResult *result);
void SortList(struct article **head);

int init_articles(struct article *head);

int  ArticleSize(struct article *head);

void DumpArticles(struct article *head);
void xDumpArticles(char *prefix,struct article *head);

struct article* ArticleGet(struct article *head, int index);

void add_article(struct article *list, char *text, char *link, char *hash);
void add_article_by_hash(struct article **list, char *text, char *link, char *hash, char *saved);

void ClearList(struct article *list);
void applySearch(struct article **list, char* searchText);
void ResetSearch(struct article **list);
char *int2bin(int a, char *buffer, int buf_size);
