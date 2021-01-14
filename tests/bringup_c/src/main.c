#include <stdio.h>
#include <stdint.h>

// #include "micro_features_generator.h"

extern void setup(void);
extern void loop(void);


int main(void){
    printf("Hello\n");

    setup();

    printf("Setup complete\n");

    while(1){
        loop();
        printf("Loop complete\n");
    }
}