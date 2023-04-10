#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    // open file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
		printf("Cannot open file.");
        return 1;
    }

    // read file
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (isupper(c)) {
            putchar(tolower(c));
        } else {
            putchar(c);
        }
    }

    // close file
    fclose(fp);

    return 0;
}