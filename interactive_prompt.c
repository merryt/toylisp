#include "mpc.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

/* Fake add_history function */
void add_history(char* unused) { }

/* Otherwise include the editline headers */
#else
#include <editline/history.h>
#include <editline/readline.h>
#endif

typedef struct lval {
    int type;
    long num;
    char* err;
    char* sym;
    int count;
    struct lval** cell;
} lval; // lval stands for Lisp VALue

/* Create Enumeration of Possible lval Types */
enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR
};

/* Create Enumeration of Possible Error Types */
enum {
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

/* Create a new number type lval */
lval lval_num(long x)
{
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

/* Create a new error type lval */
lval lval_err(int x)
{
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

// print an 'lval'
void lval_print(lval v)
{
    switch (v.type) {
        // In the case the type is a number print it
        // In the case the type is an error
    case LVAL_NUM:
        printf("%li", v.num);
        break;

        // In the case the type is an error
    case LVAL_ERR:
        if (v.err == LERR_DIV_ZERO) {
            printf("Error: Division by zero!");
        }
        if (v.err == LERR_BAD_OP) {
            printf("Error: Invalid Operator!");
        }
        if (v.err == LERR_BAD_NUM) {
            printf("Error: Invalid Number!");
        }

        break;
    }
}

// Print an 'lval' followed by a newline
void lval_println(lval v)
{
    lval_print(v);
    putchar('\n');
}

lval eval_op(lval x, char* op, lval y)
{
    if (x.type == LVAL_ERR) {
        return x;
    }

    if (x.type == LVAL_ERR) {
        return y;
    }

    if (strcmp(op, "+") == 0) {
        return lval_num(x.num + y.num);
    }
    if (strcmp(op, "-") == 0) {
        return lval_num(x.num - y.num);
    }
    if (strcmp(op, "*") == 0) {
        return lval_num(x.num * y.num);
    }
    if (strcmp(op, "/") == 0) {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    }
    if (strcmp(op, "%") == 0) {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num % y.num);
    }
    if (strcmp(op, "^") == 0) {
        return lval_num(pow(x.num, y.num));
    }
    if (strcmp(op, "min") == 0) {
        if (x.num < y.num) {
            return lval_num(x.num);
        } else {
            return lval_num(y.num);
        }
    }
    if (strcmp(op, "max") == 0) {
        if (x.num > y.num) {
            return lval_num(x.num);
        } else {
            return lval_num(y.num);
        }
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t)
{
    // If tagged as a number return it directly
    if (strstr(t->tag, "number")) // strstr compares two strings, it finds the second paramater in the first
    {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_num(LERR_BAD_NUM);
    }

    // the operator is always the second child
    char* op = t->children[1]->contents;

    // we store the third child in 'x'
    lval x = eval(t->children[2]);

    // we store the third child in 'x'
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

int main(int argc, char** argv)
{
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("Sexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* TLISP = mpc_new("tlisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                 \
    number    : /-?[0-9IO]+/;                                             \
    symbol  : '+' | '-' | '*' | '/' | '%' | '^' | /(min)/ | /(max)/;      \
    sexpr  : '(' <expr>* ')' ;                                            \
    expr      : <number> | '(' <operator> <expr>+ ')';                    \
    tlisp     : /^/ <operator> <expr>+ /$/ ;                              \
    ",
        Number, Symbol, Sexpr, Expr, TLISP);

    puts("Toy testing lisp repl");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("tlisp > ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, TLISP, &r)) {
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
    mpc_cleanup(5, Number, Symbol, Sexpr, Expr, TLISP);
    return 0;
}