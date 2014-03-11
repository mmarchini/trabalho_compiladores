int yywrap(){
    return 1;
}

void initMe(){
}

int main(){
    while(1){
        printf("%d\n", yylex());
    }
    return 0;
}

