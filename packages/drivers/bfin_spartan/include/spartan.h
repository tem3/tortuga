/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Steve Moskovchenko <stevenm@umd.edu>
 * All rights reserved.
 *
 * Author: Steve Moskovchenko <stevenm@umd.edu>
 * File:  packages/drivers/bfin_spartan/include/spartan.h
 */

#ifndef RAM_DRIVER_BFIN_SPARTAN_07_19_2008
#define RAM_DRIVER_BFIN_SPARTAN_07_19_2008

#ifdef __cplusplus
extern "C" {
#endif

struct dataset;
    
#define SRAM_BASE 0x20300000
#define SRAM_SIZE 0x100000

/*

// System timer config register:
//  Bit 15 - Clock reset.  When high, resets clock to 0 and prevents clock increments.
//                          When low, clock functions normally
#define ADDR_TIMER_CONFIG   0x202C0100

// System timer raw output
#define ADDR_TIMER_WORD0    0x202C0000
#define ADDR_TIMER_WORD1    0x202C0002
#define ADDR_TIMER_WORD2    0x202C0004
#define ADDR_TIMER_WORD3    0x202C0006

// System timer latched output:
//   When TIMER_LAT0 is read, the upper 48 bits of the system timer are latched
//   and can be read from TIMER_LAT1-3
#define ADDR_TIMER_LAT0     0x202C0010
#define ADDR_TIMER_LAT1     0x202C0012
#define ADDR_TIMER_LAT2     0x202C0014
#define ADDR_TIMER_LAT3     0x202C0016

// All four registers are connected the TIMER_WORD0
//   Use this to determine word read timing in multi-word instructions
#define ADDR_TIMER_TEST0    0x202C0020
#define ADDR_TIMER_TEST1    0x202C0022
#define ADDR_TIMER_TEST2    0x202C0024
#define ADDR_TIMER_TEST3    0x202C0026

// LED output register:
//   Bit 0: LED 1 (on when bit is high)
//   Bit 1: LED 2 (on when bit is high)
#define ADDR_LED            0x202E0000

// Test/Loopback registers:
#define ADDR_TESTREG0       0x202E0010
#define ADDR_TESTREG1       0x202E0012
#define ADDR_TESTREG2       0x202E0014
#define ADDR_TESTREG3       0x202E0016
#define ADDR_LOOPBACK0      0x202E0020
#define ADDR_LOOPBACK1      0x202E0022
#define ADDR_LOOPBACK2      0x202E0024
#define ADDR_LOOPBACK3      0x202E0026

// FPGA Version
//   Most significant byte is major reversion number
//   Least significant byte is minor reversion number
#define ADDR_VERSION        0x202E0080


// Raw ADC data latches
#define ADDR_ADC0               0x202F0000
#define ADDR_ADC1               0x202F0002
#define ADDR_ADC2               0x202F0004
#define ADDR_ADC3               0x202F0006
#define ADDR_ADC4               0x202F0008
#define ADDR_ADC5               0x202F000A
#define ADDR_ADC6               0x202F000C
#define ADDR_ADC7               0x202F000E

// ADC Data FIFO outputs
#define ADDR_FIFO_OUT1A         0x202F0020
#define ADDR_FIFO_OUT1B         0x202F0022
#define ADDR_FIFO_OUT2A         0x202F0024
#define ADDR_FIFO_OUT2B         0x202F0026

// ADC Data FIFO size: Gives the number of entries remaining to be read from the FIFO
#define ADDR_FIFO_COUNT1A       0x202F00A0
#define ADDR_FIFO_COUNT1B       0x202F00A2
#define ADDR_FIFO_COUNT2A       0x202F00A4
#define ADDR_FIFO_COUNT2B       0x202F00A6

// ADC Data FIFO Empty flag: 0xFFFF when empty, 0x0000 otherwise.
#define ADDR_FIFO_EMPTY1A       0x202F00B0
#define ADDR_FIFO_EMPTY1B       0x202F00B2
#define ADDR_FIFO_EMPTY2A       0x202F00B4
#define ADDR_FIFO_EMPTY2B       0x202F00B6

// ADC Data FIFO Half-full flag: 0xFFFF when at least half full, 0x0000 otherwise
#define ADDR_FIFO_HALF1A        0x202F00C0
#define ADDR_FIFO_HALF1B        0x202F00C2
#define ADDR_FIFO_HALF2A        0x202F00C4
#define ADDR_FIFO_HALF2B        0x202F00C6

// ADC Data FIFO Status register:
// Bits 0,4,8,12 = FIFO Empty
// Bits 1,5,9,13 = FIFO Half full
// Bits 2,6,10,14 = FIFO Full
// Bits 3,7,11,15 = FIFO Overflow occurred
#define ADDR_FIFO_STATUS1       0x202F00D0

// ADC Config register:
//  0 - AD1_M0 = set to 0 to sample 2 channels and select the channel with A0
//  1 - AD1_M1 (always 0)
//  2 - AD1_A0
//  3 - AD1_CS
//  4 - AD2_M0
//  5 - AD2_M1 (always 0)
//  6 - AD2_A0
//  7 - AD2_CS
//  12:8 - x
//  13 - FIFO Reset.  When high, clears the FIFO without affecting any other ADC operations.
//              NOTE:  When bringing the FIFO out of reset from either a FIFO reset or ADC reset,
//              the first data point read from each FIFO will be garbage.  A dummy read should be performed
//              after the first data point has arrived in the FIFO to clear the intermediate buffer.
//              Also, when the FIFO Empty flag goes high, there is actually one data point remaining to be read
//              (this is the point that gets stuck in the intermediate buffer during reset).
//  14 - Sample Enable.  When high, sampes are taken.  When low, last sample is completed and no new samples are initiated.
//  15 - ADC module reset.  When high, stops ADC clock, loads new prescaler value, resets prescaler,
//          bit counter, data latches, and sample count.
#define ADDR_ADCONFIG           0x202F0100

// ADC Prescaler register:
//   On reset, this value is latched into the ADC module and used to determine the sample frequency.
//   Sample frequency = 500ksps / (ADPRESCALER+1)
#define ADDR_ADPRESCALER        0x202F0104

// ADC Timestamp:
//   When the Sample Enable bit goes high, the current 64-bit system timer is latched into these registers
#define ADDR_TIMESTAMP0         0x202F0180
#define ADDR_TIMESTAMP1         0x202F0182
#define ADDR_TIMESTAMP2         0x202F0184
#define ADDR_TIMESTAMP3         0x202F0186

// ADC Sample Counts:
//   Number of samples taken since last reset.  Overflows at 65535
#define ADDR_ADSAMPLE_COUNT1    0x202F0200
#define ADDR_ADSAMPLE_COUNT2    0x202F0202

// ADC internal state machine output: indicates what bit/clock edge the ADC is currently executing
#define ADDR_ADSTATE1           0x202F0210
#define ADDR_ADSTATE2           0x202F0212

// SampleCount1 FIFO output, size, empty flag, half full flag, and status register
#define ADDR_FIFO_OUT1S         0x202F0220
#define ADDR_FIFO_COUNT1S       0x202F02A0
#define ADDR_FIFO_EMPTY1S       0x202F02B0
#define ADDR_FIFO_HALF1S        0x202F02C0
#define ADDR_FIFO_STATUS2       0x202F02D0

*/

// Bus addresses
#define ADDR_ADC0           0x202F0000  //0x202F 0000
#define ADDR_ADC1           0x202F0004
#define ADDR_ADC2           0x202F0008
#define ADDR_ADC3           0x202F000C
#define ADDR_ADC4           0x202F0010
#define ADDR_ADC5           0x202F0014
#define ADDR_ADC6           0x202F0018
#define ADDR_ADC7           0x202F001C

#define ADDR_ADConfig1          0x202F0100
#define ADDR_ADConfig2          0x202F0104

#define ADDR_SampleCount1       0x202F0110
#define ADDR_SampleCount2       0x202F0114
#define ADDR_SampleCount1Swap   0x202F0118
#define ADDR_SampleCount2Swap   0x202F011C
#define ADDR_SampleCount1Latch  0x202F0120
#define ADDR_SampleCount2Latch  0x202F0124

#define ADDR_BitCounter1        0x202F0130
#define ADDR_BitCounter2        0x202F0134

#define ADDR_FIFO_OUT1A         0x202F0020
#define ADDR_FIFO_OUT1B         0x202F0024
#define ADDR_FIFO_OUT2A         0x202F0028
#define ADDR_FIFO_OUT2B         0x202F002C
#define ADDR_FIFO_OUT1S         0x202F0128
#define ADDR_FIFO_OUT2S         0x202F012C

#define ADDR_FIFO_COUNT1A       0x202F00A0
#define ADDR_FIFO_COUNT1B       0x202F00A4
#define ADDR_FIFO_COUNT2A       0x202F00A8
#define ADDR_FIFO_COUNT2B       0x202F00AC
#define ADDR_FIFO_COUNT1S       0x202F01A8
#define ADDR_FIFO_COUNT2S       0x202F01AC

#define ADDR_FIFO_EMPTY1A       0x202F00B0
#define ADDR_FIFO_EMPTY1B       0x202F00B4
#define ADDR_FIFO_EMPTY2A       0x202F00B8
#define ADDR_FIFO_EMPTY2B       0x202F00BC
#define ADDR_FIFO_EMPTY1S       0x202F01B8
#define ADDR_FIFO_EMPTY2S       0x202F01BC

#define ADDR_FIFO_HALF1A        0x202F00C0
#define ADDR_FIFO_HALF1B        0x202F00C4
#define ADDR_FIFO_HALF2A        0x202F00C8
#define ADDR_FIFO_HALF2B        0x202F00CC
#define ADDR_FIFO_HALF1S        0x202F01C8
#define ADDR_FIFO_HALF2S        0x202F01CC

#define ADDR_FIFO_STATUS1       0x202F0108
#define ADDR_FIFO_STATUS2       0x202F010C

#define ADDR_ADStep             0x202F0130

#define ADDR_LED                0x202E0000
#define ADDR_Testreg0           0x202E0010
#define ADDR_Testreg1           0x202E0014
#define ADDR_Testreg2           0x202E0018
#define ADDR_Testreg3           0x202E001C
#define ADDR_Loopback0          0x202E0020
#define ADDR_Loopback1          0x202E0024
#define ADDR_Loopback2          0x202E0028
#define ADDR_Loopback3          0x202E002C

#define REG(a) (*(volatile unsigned short *) a)

int initADC();
int captureSamples(struct dataset * s);

void greenLightOn();
void greenLightOff();

void yellowLightOn();
void yellowLightOff();
    
#ifdef __cplusplus
} // extern "C" {
#endif

#endif // RAM_DRIVER_BFIN_SPARTAN_07_19_2008
