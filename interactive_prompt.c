#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv){
    puts("=== Toylisp repl ====");
    puts("Press Ctrl+c to exit /n");
    for(;;){
        char* input = readline("tlisp> ");

        add_history(input);

        printf("NO your a %s", input);
        free(input);
    }
    return 0;
}