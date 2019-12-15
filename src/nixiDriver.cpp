#include <nixiDriver.h>
#include <Shifty.h>

nixiDriver::nixiDriver(uint8_t DS, uint8_t SH, uint8_t ST)
{
    //will always be 15 because clock has 2 shift registers
    this->nixiShift.setBitCount(16);
    this->nixiShift.setPins(DS, SH, ST);
}

uint8_t nixiDriver::writeSegment(uint8_t number, uint8_t segment)
{
    bool *truthTableNumber = nixiDriver::writeNumber(number);
    switch (segment)
    {
    case 1:
        this->nixiShift.writeBit(4, truthTableNumber[0]);
        this->nixiShift.writeBit(5, truthTableNumber[1]);
        this->nixiShift.writeBit(6, truthTableNumber[2]);
        this->nixiShift.writeBit(7, truthTableNumber[3]);
        break;
    case 2:
        this->nixiShift.writeBit(0, truthTableNumber[0]);
        this->nixiShift.writeBit(1, truthTableNumber[1]);
        this->nixiShift.writeBit(2, truthTableNumber[2]);
        this->nixiShift.writeBit(3, truthTableNumber[3]);
        break;
    case 3:
        this->nixiShift.writeBit(8, truthTableNumber[0]);
        this->nixiShift.writeBit(9, truthTableNumber[1]);
        this->nixiShift.writeBit(10, truthTableNumber[2]);
        this->nixiShift.writeBit(11, truthTableNumber[3]);
        break;
    case 4:
        this->nixiShift.writeBit(12, truthTableNumber[0]);
        this->nixiShift.writeBit(13, truthTableNumber[1]);
        this->nixiShift.writeBit(14, truthTableNumber[2]);
        this->nixiShift.writeBit(15, truthTableNumber[3]);

        break;

    default:
        return 0;
        break;
    }

    return 1;
}

bool *nixiDriver::writeNumber(uint8_t number)
{

    bool *truthTableNumber = (bool *)malloc(sizeof(bool) * 4);

    switch (number)
    {

    case 0:
        truthTableNumber[0] = LOW;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = LOW;
        break;
    case 1:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = LOW;
        break;
    case 2:
        truthTableNumber[0] = LOW;
        truthTableNumber[1] = HIGH;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = LOW;
        break;
    case 3:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = HIGH;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = LOW;
        break;
    case 4:
        truthTableNumber[0] = LOW;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = HIGH;
        truthTableNumber[3] = LOW;
        break;
    case 5:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = HIGH;
        truthTableNumber[3] = LOW;
        break;
    case 6:
        truthTableNumber[0] = LOW;
        truthTableNumber[1] = HIGH;
        truthTableNumber[2] = HIGH;
        truthTableNumber[3] = LOW;
        break;
    case 7:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = HIGH;
        truthTableNumber[2] = HIGH;
        truthTableNumber[3] = LOW;
        break;
    case 8:
        truthTableNumber[0] = LOW;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = HIGH;
        break;
    case 9:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = LOW;
        truthTableNumber[2] = LOW;
        truthTableNumber[3] = HIGH;
        break;

    default:
        truthTableNumber[0] = HIGH;
        truthTableNumber[1] = HIGH;
        truthTableNumber[2] = HIGH;
        truthTableNumber[3] = HIGH;
        break;
    }
    return truthTableNumber;
}