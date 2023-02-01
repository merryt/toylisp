#include <stdio.h>

static char input[2048];

int main(int argc, char** argv){
    puts("=== Toylisp repl ====");
    puts("Press Ctrl+c to exit /n");
    for(;;){
        fputs("tlisp> ", stdout);
        fgets(input, 2048, stdin);
        printf("NO your a %s", input);

    }
    return 0;
}