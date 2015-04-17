/* Copyright 2014, ACSE & CADIEEL
 *    ACSE   : http://www.sase.com.ar/asociacion-civil-sistemas-embebidos/ciaa/
 *    CADIEEL: http://www.cadieel.org.ar
 * All rights reserved.
 *
 *    or
 *
 * Copyright 2014, Your Name <youremail@domain.com>
 * All rights reserved.
 *
 *    or
 *
 * Copyright 2014, ACSE & CADIEEL & Your Name <youremail@domain.com
 *    ACSE   : http://www.sase.com.ar/asociacion-civil-sistemas-embebidos/ciaa/
 *    CADIEEL: http://www.cadieel.org.ar
 * All rights reserved.
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \brief Short description of this file
 **
 ** Long description of this file
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Template Template to start a new module
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 *
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * yyyymmdd v0.0.1 initials initial version
 */

/*==================[inclusions]=============================================*/
#include "os.h"
#include "ciaak.h"
#include "leds.h"
#include "ciaaModbus.h"

/*==================[macros and definitions]=================================*/
#define CIAA_BLINKING_MODBUS_ID        2

#define MODBUS_ADDRESS_TECLADO_REMOTO  0X0000
#define MODBUS_ADDRESS_ESTADO_LEDS     0X0001

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
static uint16_t cmd0x03ReadHoldingReg(
       uint16_t start,
       uint16_t quantity,
       uint8_t * exceptioncode,
       uint8_t * buf
       );

static void cmd0x10WriteMultipleReg(
      uint16_t start,
      uint16_t quantity,
      uint8_t bytecount,
      uint8_t * exceptioncode,
      uint8_t * buf
      );

/*==================[internal data definition]===============================*/
static int32_t hModbusSlave;
static int32_t hModbusAscii;
static int32_t hModbusGateway;

static const ciaaModbus_slaveCmd_type callbacksStruct =
{
   NULL,
   NULL,
   cmd0x03ReadHoldingReg,
   NULL,
   NULL,
   NULL,
   NULL,
   cmd0x10WriteMultipleReg,
};

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
static uint16_t cmd0x03ReadHoldingReg(
       uint16_t start,
       uint16_t quantity,
       uint8_t * exceptioncode,
       uint8_t * buf
       )
{
   /* used to indicate total of registers reads */
   int8_t ret = 0;
   /* used to indicate quantity of registers processed */
   uint16_t quantityRegProcessed;
   uint16_t temp16u;

   /* loop to read all registers indicated */
   do
   {
      /* select register address to be read */
      switch (start)
      {
         /* estado de los leds */
         case MODBUS_ADDRESS_ESTADO_LEDS:
            temp16u = leds_get();
            ciaaModbus_writeInt(buf, temp16u);
            quantityRegProcessed = 1;
            break;

         /* wrong address */
         default:
            *exceptioncode = CIAA_MODBUS_E_WRONG_STR_ADDR;
            quantityRegProcessed = -1;
            break;
      }

      /* if quantityRegProcessed > 0, successful operation */
      if (quantityRegProcessed > 0)
      {
         /* update buffer pointer to next register */
         buf += (quantityRegProcessed*2);

         /* next address to be read */
         start += quantityRegProcessed;

         /* increment count of registers */
         ret += quantityRegProcessed;
      }
      else
      {
         /* an error occurred in reading */
         ret = -1;
      }
      /* repeat until:
      * - read total registers or
      * - error occurs
      */
   }while ((ret > 0) && (ret < quantity));

   return ret;
}

static void cmd0x10WriteMultipleReg(
      uint16_t start,
      uint16_t quantity,
      uint8_t bytecount,
      uint8_t * exceptioncode,
      uint8_t * buf
      )
{
   /* used to indicate quantity of registers processed */
   uint16_t quantityRegProcessed;

   uint16_t temp16u;

   /* loop to write all registers indicated */
   do
   {
      /* select register address to be write */
      switch (start)
      {
         /* escritura de estado de teclas */
         case MODBUS_ADDRESS_TECLADO_REMOTO:
            temp16u = ciaaModbus_readInt(buf);
            //procesarTeclas(temp16u);
            quantityRegProcessed = 1;
            break;

         /* escritura de leds */
         case MODBUS_ADDRESS_ESTADO_LEDS:
            temp16u = ciaaModbus_readInt(buf);
            leds_set(temp16u);
            quantityRegProcessed = 1;
            break;

         /* wrong address */
         default:
            *exceptioncode = CIAA_MODBUS_E_WRONG_STR_ADDR;
            quantityRegProcessed = -1;
            break;
      }

      /* if quantityRegProcessed > 0, successful operation */
      if (quantityRegProcessed > 0)
      {
         /* update buffer pointer to next register */
         buf += (quantityRegProcessed*2);

         /* next address to be write */
         start += quantityRegProcessed;

         quantity -= quantityRegProcessed;
      }
      else
      {
         quantity = 0;
      }

      /* repeat until:
      * - read total registers or
      * - error occurs
      */
   }while (0 < quantity);
}

/*==================[external functions definition]==========================*/
extern void modbusSlave_init(void)
{
   int32_t fdSerialPort;

   fdSerialPort = ciaaPOSIX_open("/dev/serial/uart/0", O_RDWR | O_NONBLOCK);

   /* Open Modbus Slave */
   hModbusSlave = ciaaModbus_slaveOpen(
         &callbacksStruct,
         CIAA_BLINKING_MODBUS_ID);

   /* Open Transport Modbus Ascii */
   hModbusAscii = ciaaModbus_transportOpen(
         fdSerialPort,
         CIAAMODBUS_TRANSPORT_MODE_ASCII_SLAVE);

   /* Open Gateway Modbus */
   hModbusGateway = ciaaModbus_gatewayOpen();

   /* Add Modbus Slave to gateway */
   ciaaModbus_gatewayAddSlave(
         hModbusGateway,
         hModbusSlave);

   /* Add Modbus Transport to gateway */
   ciaaModbus_gatewayAddTransport(
         hModbusGateway,
         hModbusAscii);
}

/** \brief Modbus Task
 *
 * esta funcion debe ser llamada por una tarea con un período
 * de 5 ms.
 */
extern void modbusSlave_task(void)
{
   ciaaModbus_gatewayMainTask(hModbusGateway);
}


/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
