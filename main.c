int running;
int nline;

int yywrap(){
    return 1;
}

void initMe(){
    running = 1;
    nline=0;
}

int getLineNumber(){ return nline; }


int main(){
    initMe();
    while(running)
        printf("%d\n", yylex());
    printf("\nlinhas: %d\n", getLineNumber());
    return 0;
}

