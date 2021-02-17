/*** Definition section ***/

%{
/* C code to be copied verbatim */
#include <stdio.h>
#include <string.h>
%}
%%
    /*** Rules section ***/

[()+*/-]                    {
                                fprintf(yyout,"%s\n",yytext);
                            }

([a-zA-Z]+[0-9]*)+          {
                                fprintf(yyout,"id\n");
                            }

[0-9]+                      {
                                fprintf(yyout,"num\n");
                            }

[ \t]+                      {}

.                           {
                                printf("invalid character\n");
                            }
%%
    /*** C Code section ***/


int main() {
    extern FILE *yyout;
    yyout = fopen("token.txt", "w+");
    yylex();
    return 0;
}