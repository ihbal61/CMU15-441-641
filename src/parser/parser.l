%{
#include <stdio.h>
#include "y.tab.h"
#undef YY_INPUT
extern char *parsing_buf;	/* The buffer to read the data from */
extern size_t parsing_buf_size;	/* Size of the buffer */
extern int parsing_offset;	/* Current offset in the buffer */

#define MIN(__a, __b) (((__a) < (__b)) ? (__a) : (__b))

/* Redefine YY_INPUT to read from a buffer instead of stdin! */
#define YY_INPUT(__b, __r, __s) do {					\
		__r = MIN(__s, parsing_buf_size - parsing_offset);	\
		memcpy(__b, parsing_buf + parsing_offset, __r);		\
		parsing_offset += __r;					\
	} while(0)
%}
%option noyywrap

char            [\x0-\x7f]

/* 大写字母 */
upalpha         [A-Z]

/* 小写字母 */
loalpha         [a-z]

/* 字母 */
alpha           [A-Za-z]

/* 控制字符 */
ctl             [\x0-\x1f\x7f]

/* 回车键 */
cr              \x0d

/* 换行符 */
lf              \x0a

/* 空格 */
sp              \x20

/* 水平制表符 */
ht              \x09

/* 双引号 */
quote           \x22

/* 行尾 */
crlf            \x0d\x0a

/* 线性空格 */
lws             [\x20\x09]1*(\x20|\x09)

/* 数字 */
digit           [0-9]

/* 冒号 */
colon           :

/* 空格 */
ws              [\x20\x09]*

/* 分隔符 */
separators      [\(\)\<\>@\,;:\\\"/\[\]?=\{\}]

/**/
token_char      [\x0-\x7f]{-}[\x0-\x1f\x7f]{-}[\{\}\(\)\<\>@\,;:\\\"/\[\]?=\x20\x09]

%%

"/" {
    yylval.i = yytext[0];
    return t_backslash;
}

{crlf} {
    return t_crlf;
}

{sp} {
    yylval.i = yytext[0];
    return t_sp;
}

{ws} {
    strcpy(yylval.str, yytext);
    return t_ws;
}

{digit} {
    yylval.i = atoi(yytext);
    return t_digit;
}

"." {
    yylval.i = '.';
    return t_dot;
}

{colon} {
    yylval.i = ':';
    return t_colon;
}

{separators} {
    yylval.i = yytext[0];
    return t_separators;
}

{token_char} {
    yylval.i = yytext[0];
    return t_token_char;
}

{lws} {
    return t_lws;
}

{ctl} {
    return t_ctl;
}

