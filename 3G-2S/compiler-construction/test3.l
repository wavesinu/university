%{
void print(){
    printf("%s\n", yytext);
}
%}

%%
if               {print();}
[a-z][a-z0-9]*   {print();}
[0-9]+           {print();}
[   \t\n]        {}
.       {printf("ERROR\n");}
%%

int yywrap(){
    return 1;
}

int main(int argc, char *argv[]){
    yyin = fopen(argv[1], "r"); //read only mode
    yylex();
    return 0;   
}
