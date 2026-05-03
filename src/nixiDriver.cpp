#include <nixiDriver.h>
#include <Shifty.h>

nixiDriver::nixiDriver(uint8_t DS, uint8_t SH, uint8_t ST, bool numitron)
{
    this->numitron = numitron;

    if (this->numitron == true)
    {
        this->nixiShift.setBitCount(32);
    }
    else
    {
        // clock has 2 shift registers for nixie mode
        this->nixiShift.setBitCount(16);
    }

    this->nixiShift.setPins(DS, SH, ST);
}

uint8_t nixiDriver::writeSegment(uint8_t number, uint8_t segment)
{
    if (this->numitron == false)
    {
        bool truthTableNumber[4];
        nixiDriver::writeNumber(number, truthTableNumber);
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
    }
    else
    {
        bool truthTableNumber[8];
        nixiDriver::writeNumber(number, truthTableNumber);
        switch (segment)
        {
        case 1:
            this->nixiShift.writeBit(0, truthTableNumber[0]);
            this->nixiShift.writeBit(1, truthTableNumber[1]);
            this->nixiShift.writeBit(2, truthTableNumber[2]);
            this->nixiShift.writeBit(3, truthTableNumber[3]);
            this->nixiShift.writeBit(4, truthTableNumber[4]);
            this->nixiShift.writeBit(5, truthTableNumber[5]);
            this->nixiShift.writeBit(6, truthTableNumber[6]);
            this->nixiShift.writeBit(7, truthTableNumber[7]);
            break;
        case 2:
            this->nixiShift.writeBit(8, truthTableNumber[0]);
            this->nixiShift.writeBit(9, truthTableNumber[1]);
            this->nixiShift.writeBit(10, truthTableNumber[2]);
            this->nixiShift.writeBit(11, truthTableNumber[3]);
            this->nixiShift.writeBit(12, truthTableNumber[4]);
            this->nixiShift.writeBit(13, truthTableNumber[5]);
            this->nixiShift.writeBit(14, truthTableNumber[6]);
            this->nixiShift.writeBit(15, truthTableNumber[7]);
            break;
        case 3:
            this->nixiShift.writeBit(16, truthTableNumber[0]);
            this->nixiShift.writeBit(17, truthTableNumber[1]);
            this->nixiShift.writeBit(18, truthTableNumber[2]);
            this->nixiShift.writeBit(19, truthTableNumber[3]);
            this->nixiShift.writeBit(20, truthTableNumber[4]);
            this->nixiShift.writeBit(21, truthTableNumber[5]);
            this->nixiShift.writeBit(22, truthTableNumber[6]);
            this->nixiShift.writeBit(23, truthTableNumber[7]);
            break;
        case 4:
            this->nixiShift.writeBit(24, truthTableNumber[0]);
            this->nixiShift.writeBit(25, truthTableNumber[1]);
            this->nixiShift.writeBit(26, truthTableNumber[2]);
            this->nixiShift.writeBit(27, truthTableNumber[3]);
            this->nixiShift.writeBit(28, truthTableNumber[4]);
            this->nixiShift.writeBit(29, truthTableNumber[5]);
            this->nixiShift.writeBit(30, truthTableNumber[6]);
            this->nixiShift.writeBit(31, truthTableNumber[7]);
            break;

        default:
            return 0;
            break;
        }
    }

    return 1;
}

void nixiDriver::writeNumber(uint8_t number, bool *truthTableNumber)
{
    if (this->numitron == false)
    {
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
    }
    else
    {
        switch (number)
        {
        case 0:
            truthTableNumber[0] = HIGH;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = LOW;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 1:
            truthTableNumber[0] = HIGH;
            truthTableNumber[1] = LOW;
            truthTableNumber[2] = LOW;
            truthTableNumber[3] = LOW;
            truthTableNumber[4] = LOW;
            truthTableNumber[5] = LOW;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 2:
            truthTableNumber[0] = HIGH;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = LOW;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = LOW;
            truthTableNumber[7] = LOW;
            break;
        case 3:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = LOW;
            truthTableNumber[7] = LOW;
            break;
        case 4:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = LOW;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = LOW;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 5:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = LOW;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 6:
            truthTableNumber[0] = HIGH;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = LOW;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 7:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = LOW;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = LOW;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = LOW;
            truthTableNumber[7] = LOW;
            break;
        case 8:
            truthTableNumber[0] = HIGH;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;
        case 9:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = HIGH;
            truthTableNumber[2] = HIGH;
            truthTableNumber[3] = HIGH;
            truthTableNumber[4] = HIGH;
            truthTableNumber[5] = HIGH;
            truthTableNumber[6] = HIGH;
            truthTableNumber[7] = LOW;
            break;

        default:
            truthTableNumber[0] = LOW;
            truthTableNumber[1] = LOW;
            truthTableNumber[2] = LOW;
            truthTableNumber[3] = LOW;
            truthTableNumber[4] = LOW;
            truthTableNumber[5] = LOW;
            truthTableNumber[6] = LOW;
            truthTableNumber[7] = LOW;
            break;
        }
    }
}

void nixiDriver::bootUp()
{
    for (uint8_t i = 0; i < 10; i++)
    {
        //Serial.println("Simple Test");
        nixiDriver::writeSegment(i, 1);
        delay(500);
        nixiDriver::writeSegment(i, 2);
        delay(500);
        nixiDriver::writeSegment(i, 3);
        delay(500);
        nixiDriver::writeSegment(i, 4);
        delay(500);
    }

    delay(1000);

    nixiDriver::writeSegment(10, 1);
    nixiDriver::writeSegment(10, 2);
    nixiDriver::writeSegment(10, 3);
    nixiDriver::writeSegment(10, 4);

    delay(1000);
}

void nixiDriver::off()
{
    nixiDriver::writeSegment(10, 1);
    nixiDriver::writeSegment(10, 2);
    nixiDriver::writeSegment(10, 3);
    nixiDriver::writeSegment(10, 4);
}