#include <stdio.h>
#include <math.h>

typedef struct{
	float x;
	float y;
} point;

int add_togeather(int x, int y){
	return x + y;
}


int main(int argc, char** argv){
	point p;
	p.x = 0.1;
	p.y = 10.0;

	float length = sqrt(p.x * p.x + p.y * p.y);
	printf("%f", length);
	puts("hello, world");
	printf("count: %d ?", add_togeather(3,4));

	puts("/n");
	for(int i = 0; i < 5; i++){
		puts("hello worlds");
	}
	int i=0;
	while(i < 5){
		puts("while...");
		i++;
	}

	return 0;
}
