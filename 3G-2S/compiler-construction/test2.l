%{
void print(){
    printf("%s\n", yytext);

}
%}
%%
.   {print();}
\n  {print();}  // ASCII를 포함한 모든 문자를 인식
%%
// subroutines
int yywrap(){
    return 1;   
}

int main(int argc, char *argv[]){
    yyin = fopen(argv[1], "r"); //read only mode
    yylex();
    return 0;   
}

// USE: ./test.exe source.txt
