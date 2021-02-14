#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "colors.h"

#define MAXLINE 100
#define FILESIZE 10000

void Search(char *line);
int Keyword(char *str);
int Operator(char *str);
int CheckSyntax(char *str);
int Comments(char* str);

int brace, brack, paren, double_quotes, single_quotes;

int main(int argc, char *argv[])
{
    extern int brace, brack, paren, double_quotes, single_quotes;
    FILE* fptr;
    char ext[3] = {'.', 'c', '\0'};
    char buf[FILESIZE], line[MAXLINE];

/* Open only files with ".c" extension */
    if(argc > 1) {
        if (NULL != strstr(argv[1], ext)) {
            fptr = fopen(argv[1], "r");
            printf("OPEN    : %s : " BOLDRED "Success" RESET "\n", argv[1]);
        }
        else {
            printf("Program accepts only the C source files.\n");
            return 0;
        }
    }
    else {
        printf("Error: ./a.out source_file.c\n");
        return 0;
    }
    
/* copy file contents to buffer */
    int ch, i = 0, j = 0;
    while ((ch = fgetc(fptr)) != EOF)
        buf[i++] = ch;
    buf[i++] = '\n';
    buf[i] = '\0';

/* Check for rudimentory syntax */
    Comments(buf);
//    return 0;
    CheckSyntax(buf);

    if (brace != 0)
        printf(BOLDRED "ERROR: " RESET "Unmatched Braces\n");
    if (brack != 0)
        printf(BOLDRED "ERROR: " RESET "Unmatched Brackets\n");
    if (paren != 0)
        printf(BOLDRED "ERROR: " RESET "Unmatched Parenthesis\n");
    if (double_quotes != 0)
        printf(BOLDRED "ERROR: " RESET "Unmatched Double Quotes\n");
    if (single_quotes != 0)
        printf(BOLDRED "ERROR: " RESET "Unmatched Single Quotes\n");

    if (brace != 0 || brack != 0 || paren != 0 || double_quotes != 0 || single_quotes != 0)
        return 0;

/* Start Parsing */
    printf("PARSING : %s : " BOLDRED "Started" RESET "\n\n", argv[1]);
    i = 0;
    while (*(buf + i) != '\0') {
        j = 0;
        while ((ch = *(buf + i++)) != '\n')
            line[j++] = ch;
        line[j] = '\0';
        Search(line);
    }
    printf("\nPARSING : %s : " BOLDRED "Done" RESET "\n", argv[1]);

    fclose(fptr);
    return 0;
}

/* CheckSyntax: Checks for {}, [], () */
int CheckSyntax(char *str)
{
    extern int brace, brack, paren, double_quotes, single_quotes;

    int dub = 0, sing = 0;
    while (*str++) {
        if (*str == '{')
            --brace;
        else if (*str == '}')
            ++brace;
        else if (*str == '[')
            --brack;
        else if (*str == ']')
            ++brack;
        else if (*str == '(')
            --paren;
        else if (*str == ')')
            ++paren;
        else if (*str == '\"') {
            if (!dub) {
                ++double_quotes;
                ++dub;
            } else
                double_quotes = dub = 0;
        }
        else if (*str == '\'') {
            if (!sing) {
                ++single_quotes;
                ++sing;
            } else
                single_quotes = sing = 0;
        }
    }
}

/* Comments: Checks for comments*/
int Comments(char *str)
{
    char buf[MAXLINE];
    int i = 0, j = 0;   
    
    while (*(str + i))
    {
        if (*(str + i) == '/')
        {
            buf[j++] = *(str + i);
            ++i;
            if (*(str + i) == '*') 
            {
                buf[j++] = *(str + i);
                ++i;
                while (*(str + i)) 
                {
                    if (*(str + i) == '*') 
                    {
                        buf[j++] = *(str + i);
                        ++i;
                        if (*(str + i) == '/') 
                        {
                            buf[j++] = *(str + i);
                            buf[j] = '\0';
                            j = 0;
   //                         printf("COMMENTS: %s\n", buf);
                            break;
                        }
                    }
                    buf[j++] = *(str + i);
                ++i;
                }
                if (*(str + i) == '\0')
                {
                    printf(BOLDRED "Error: " RESET  "Unmatched Comments\n");
                    exit(-1);
                }
            }
        }
        ++i;
    }
}

void Search(char *line)
{
#if 1
    char parse[MAXLINE];

    int i = 0, j = 0;
    while (*(line + i)) 
    {
        /* find the pre-processor directives */
        if (*(line + i) == '#') 
        {
            parse[j++] = *(line + i++);
            while (*(line + i) != '\n')
                parse[j++] = *(line + i++);
            parse[j] = '\0';
            printf("PPDirective: %s\n", parse);
            j = 0;
            ++i;
        }
       
        if (*(line + i) == '/' && *(line + i + 1) == '*') {
            parse[j++] = *(line + i); parse[j++] = *(line + i + 1);
            ++i;
            ++i;
            while (*(line + i)) {
                parse[j++] = *(line + i);
                if (*(line + i) == '*') {
                    parse[j++] = *(line + i);
                    ++i;
                    if (*(line + i) == '/') {
                        parse[j++] = *(line + i);
                        parse[j] = '\0';
                        printf("COMMENT     : %s\n", parse);
                    }
                }
                ++i;
            }
            ++i;
            j = 0;
        }

        /* find the character and string literals */
        if (*(line + i) == '\"' || *(line + i) == '\'')
        {
            ++i;
            while (*(line + i) != '\"' && *(line + i) != '\'')
                parse[j++] = *(line + i++);
  //          parse[j++] = *(line + i++);
            parse[j] = '\0';
            printf("Literal    : %s\n", parse);
             ++i;
            j = 0;
        }
        
        /* find the operator symbols */
        else if (!isalnum(*(line + i)) && Operator(line + i) && *(line + i) != ' ' && *(line + i) != '\n')
        {
            printf("Operator   : %c\n", *(line + i));
            ++i;
        }

        /* find the keywords and identifiers */ 
        else if (isalpha(*(line + i))) 
        {
            while (*(line + i) != ' ' && isalpha(*(line + i))) 
                parse[j++] = *(line + i++);
            parse[j] = '\0';
            if (Keyword(parse))
                printf("Keyword    : %s\n", parse);
            else
                printf("Identifier : %s\n", parse);
//            ++i;
            j = 0;
        }
        
        /* find the lonely integer literals */
        else if (isdigit(*(line + i)))
            printf("Literal    : %c\n", *(line + i++));

        /* skip newline character */
        else
            ++i;
    }
#else
    printf("%s\n", line);
#endif
}


int Keyword(char *str)
{
    char *keyword[] = {"auto", "break", "case", "char", "const", "continue", "default", "do", 
                        "double",	"else", "enum", "extern", "float", "for", "goto", "if", 
                        "int", "long", "register", "return", "short", "signed",	"sizeof", "static", 
                        "struct", "switch", "typedef", "union", "unsigned", "void",	"volatile", "while", NULL,
    };

    int i = 0;
    while (*(keyword + i) != NULL) {
        if (!strcmp(*(keyword + i), str))
            return 1;
        ++i;
    }
    return 0;
}


int Operator(char *str)
{
    char *operator[] = {"()", "[]", "->", ".",
        "!", "~", "++", "--", "+", "-", "(type)", "*", "&", "sizeof",
        "*", "/", "%",
        "+", "-",
        "<<", ">>",
        "<", "<=", ">", ">=",
        "==", "!=",
        "&",
        "^",
        "|",
        "&&",
        "||",
        "?:",
        "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=",
        ",", NULL,
    };

    int i = 0;
    while (*(operator + i) != NULL) {
        if ((strcmp(*(operator + i), str)))
            return 1;
        ++i;
    }
    return 0;
}
