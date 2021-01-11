#include <tubeDriver.h>
#include <Shifty.h>

tubeDriver::tubeDriver(uint8_t DS, uint8_t SH, uint8_t ST, bool numitron)
{
    //will always be 15 because clock has 2 shift registers
    this->numitron = numitron;

    if (this->numitron == true)
    {
        this->tubeShift.setBitCount(32);
    }else
    {
        this->tubeShift.setBitCount(16);
    }
        
    this->tubeShift.setPins(DS, SH, ST);
    
    
}

uint8_t tubeDriver::writeSegment(uint8_t number, uint8_t segment)
{
    if (this->numitron == false)
    {
        bool truthTableNumber[4];
        tubeDriver::writeNumber(number, truthTableNumber);
        switch (segment)
        {
        case 1:
            this->tubeShift.writeBit(4, truthTableNumber[0]);
            this->tubeShift.writeBit(5, truthTableNumber[1]);
            this->tubeShift.writeBit(6, truthTableNumber[2]);
            this->tubeShift.writeBit(7, truthTableNumber[3]);
            break;
        case 2:
            this->tubeShift.writeBit(0, truthTableNumber[0]);
            this->tubeShift.writeBit(1, truthTableNumber[1]);
            this->tubeShift.writeBit(2, truthTableNumber[2]);
            this->tubeShift.writeBit(3, truthTableNumber[3]);
            break;
        case 3:
            this->tubeShift.writeBit(8, truthTableNumber[0]);
            this->tubeShift.writeBit(9, truthTableNumber[1]);
            this->tubeShift.writeBit(10, truthTableNumber[2]);
            this->tubeShift.writeBit(11, truthTableNumber[3]);
            break;
        case 4:
            this->tubeShift.writeBit(12, truthTableNumber[0]);
            this->tubeShift.writeBit(13, truthTableNumber[1]);
            this->tubeShift.writeBit(14, truthTableNumber[2]);
            this->tubeShift.writeBit(15, truthTableNumber[3]);
            break;

        default:
            return 0;
            break;
        }
    }else
    {
        bool truthTableNumber[8];
        tubeDriver::writeNumber(number, truthTableNumber);
        switch (segment)
        {
        case 1:
            this->tubeShift.writeBit(0, truthTableNumber[0]);
            this->tubeShift.writeBit(1, truthTableNumber[1]);
            this->tubeShift.writeBit(2, truthTableNumber[2]);
            this->tubeShift.writeBit(3, truthTableNumber[3]);
            this->tubeShift.writeBit(4, truthTableNumber[4]);
            this->tubeShift.writeBit(5, truthTableNumber[5]);
            this->tubeShift.writeBit(6, truthTableNumber[6]);
            this->tubeShift.writeBit(7, truthTableNumber[7]);
            break;
        case 2:
            this->tubeShift.writeBit(8, truthTableNumber[0]);
            this->tubeShift.writeBit(9, truthTableNumber[1]);
            this->tubeShift.writeBit(10, truthTableNumber[2]);
            this->tubeShift.writeBit(11, truthTableNumber[3]);
            this->tubeShift.writeBit(12, truthTableNumber[4]);
            this->tubeShift.writeBit(13, truthTableNumber[5]);
            this->tubeShift.writeBit(14, truthTableNumber[6]);
            this->tubeShift.writeBit(15, truthTableNumber[7]);
            break;
        case 3:
            this->tubeShift.writeBit(16, truthTableNumber[0]);
            this->tubeShift.writeBit(17, truthTableNumber[1]);
            this->tubeShift.writeBit(18, truthTableNumber[2]);
            this->tubeShift.writeBit(19, truthTableNumber[3]);
            this->tubeShift.writeBit(20, truthTableNumber[4]);
            this->tubeShift.writeBit(21, truthTableNumber[5]);
            this->tubeShift.writeBit(22, truthTableNumber[6]);
            this->tubeShift.writeBit(23, truthTableNumber[7]);
            break;
        case 4:
            this->tubeShift.writeBit(24, truthTableNumber[0]);
            this->tubeShift.writeBit(25, truthTableNumber[1]);
            this->tubeShift.writeBit(26, truthTableNumber[2]);
            this->tubeShift.writeBit(27, truthTableNumber[3]);
            this->tubeShift.writeBit(28, truthTableNumber[4]);
            this->tubeShift.writeBit(29, truthTableNumber[5]);
            this->tubeShift.writeBit(30, truthTableNumber[6]);
            this->tubeShift.writeBit(31, truthTableNumber[7]);
            break;

        default:
            return 0;
            break;
        }
    }
    
    
    
    return 1;
}

void tubeDriver::writeNumber(uint8_t number, bool *truthTableNumber)
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
    }else
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

void tubeDriver::off()
{
    tubeDriver::writeSegment(10, 1);
    tubeDriver::writeSegment(10, 2);
    tubeDriver::writeSegment(10, 3);
    tubeDriver::writeSegment(10, 4);
}

void tubeDriver::bootUp()
{
    for (uint8_t i = 0; i < 10; i++)
    {
        //Serial.println("Simple Test");
        tubeDriver::writeSegment(i, 1);
        tubeDriver::writeSegment(i, 2);
        tubeDriver::writeSegment(i, 3);
        tubeDriver::writeSegment(i, 4);
        delay(250);
    }

    delay(1000);

    tubeDriver::writeSegment(10, 1);
    tubeDriver::writeSegment(10, 2);
    tubeDriver::writeSegment(10, 3);
    tubeDriver::writeSegment(10, 4);

    delay(1000);
}