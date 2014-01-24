/*
 * Handles the different timings
 * in order to communicate properly with the
 * Maxim DS18B2
 * 
 * Mattijs Korpershoek
 * <mattijs.korpershoek@gmail.com>
 */
#ifndef __INC_PROTOCOLOPERATIONS__
#define __INC_PROTOCOLOPERATIONS__

#include "bitOperations.h"
#include "protocolOperations.h" 
#include "thermOperations.h"

void sendInitializationSequence( void );

void writeBitToBus( Bit bitToWrite );
Bit readBitFromBus( void );

void writeByteToBus(u8 byteToWrite);
u8 readByteFromBus(u8 byteToWrite);

void writeFunctionCommand(FunctionCommand command);
void writeROMCommand(ROMCommand romcommand);

#endif /* end of include guard: __INC_PROTOCOLOPERATIONS__ */
