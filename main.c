int yywrap(){
    return 1;
}

void initMe(){
}

int main(){
    yylex();
    return 0;
}

