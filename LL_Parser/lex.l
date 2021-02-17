%{
# include <stdio.h>
# include <stdlib.h>
%}
FILE *fp;
fp = fopen("tokens.txt", "w+");
%%
[a-zA-Z]+([0-9]*[a-zA-Z]*)* {fprintf(fp, "Token:id Lexeme:%s", yytext);}
