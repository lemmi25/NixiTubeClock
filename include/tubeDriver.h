#ifndef tubeDriver_h
#define tubeDriver_h

#include <Arduino.h>
#include <Shifty.h>

class tubeDriver
{
public:
    //define shift register pins
    tubeDriver(uint8_t DS, uint8_t SH, uint8_t ST, bool numitron);
    uint8_t writeSegment(uint8_t number, uint8_t segment);
    void bootUp();
    void off();

private:
    void writeNumber(uint8_t number, bool *truthTableNumber);
    Shifty tubeShift;
    boolean numitron;
};

#endif