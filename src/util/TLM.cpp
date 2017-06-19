/*******************************************************
 *
 * THIS FILE IS AUTO-GENERATED, DO NOT EDIT THIS FILE!
 * To edit this file, edit the auto-coder located
 * in TLM/auto_code/py
 *
 ******************************************************/

#include <stdint.h>
#include "util/serialize.h"
#include "servers/FMGServer.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));

void TLM_STARTUP_ERROR(void) {
  size_t size = 6;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0001);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_SERVERS_CREATED(void) {
  size_t size = 6;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0002);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_MAIN_EXIT(void) {
  size_t size = 6;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0003);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_SPI_ACS_TX_FAIL(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0004);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_SPI_COM_TX_FAIL(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0005);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_SPI_EPS_TX_FAIL(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0006);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_SPI_PLD_TX_FAIL(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0007);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ERR, buf, size);
}

void TLM_MODE_SWITCH(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0100);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_MOD, buf, size);
}

void TLM_SPI_ACS_RX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0401);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_COM_RX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0402);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_EPS_RX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0403);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_PLD_RX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0404);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_ACS_TX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0405);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_COM_TX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0406);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_EPS_TX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0407);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

void TLM_SPI_PLD_TX(uint8 arg0) {
  size_t size = 7;
  uint8_t * buf = (uint8_t *) malloc(size);
  Serialize ser(buf, size);
  ser.serialize_int32(getTimeInSec());
  ser.serialize_uint16(0x0408);
  ser.serialize_uint8(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, size);
}

