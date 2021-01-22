#include <xs1.h>
#include <stdint.h>

void get_noise_array(int32_t *array, unsigned n){
    static const unsigned random_poly = 0xEDB88320;
    static unsigned g;
    for(int i=0; i<n; i++){
        crc32(g, -1, random_poly);
        array[i] = g;
    }
}