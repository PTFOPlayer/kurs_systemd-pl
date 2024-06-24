#include <stdio.h>
#include <time.h>
#include <string.h>
int main() {
    
    time_t now;
    time(&now);

    FILE * file = fopen("/var/exfile", "w");;
    fprintf(file, "%s", ctime(&now) );
}