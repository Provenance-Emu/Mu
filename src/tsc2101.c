#include <stdint.h>
#include <stdbool.h>

#include "emulator.h"


void tsc2101Reset(void){

}

uint32_t tsc2101StateSize(void){

}

void tsc2101SaveState(uint8_t* data){

}

void tsc2101LoadState(uint8_t* data){

}

bool tsc2101ExchangeBit(bool bit){
   debugLog("Unimplemented TSC2101 transfer:%d\n", bit);
}
