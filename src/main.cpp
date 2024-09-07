#include "astra_rocket.h"

void setup() {
    setCpuFrequencyMhz(240);
    astraCoreInit();
    //astraCoreTest();
}

void loop() {
    astraCoreUpdate();
}
