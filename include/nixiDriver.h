#ifndef nixiDriver_h
#define nixiDriver_h

#include <Arduino.h>
#include <Shifty.h>

class nixiDriver
{
public:
    //define shift register pins
    nixiDriver(uint8_t DS, uint8_t SH, uint8_t ST);
    uint8_t writeSegment(uint8_t number, uint8_t segment);
    void bootUp();
    void off();

private:
    void writeNumber(uint8_t number, bool *truthTableNumber);
    Shifty nixiShift;
};

#endif