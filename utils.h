
int passisempty(char *pass);

void set_password();

void set_censored_words();

int isunique(char *name);

int iscensored(char *word);

char* censor(char *message);    

int get_size(char** keys, char** values, int size);

char * json_stringify_single(char*keys,char* values, int size);
