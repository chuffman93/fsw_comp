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

namespace AllStar{
namespace Servers{

FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));

void TLM_STARTUP_ERROR(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x01);
  fmgServer->Log(DESTINATION_ERR, buf, 2);
}

void TLM_SERVERS_CREATED(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x02);
  fmgServer->Log(DESTINATION_ERR, buf, 2);
}

void TLM_BUS_LOAD_TEST(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x0100);
  fmgServer->Log(DESTINATION_DGN, buf, 2);
}

void TLM_SPI_STATS(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x0101);
  fmgServer->Log(DESTINATION_DGN, buf, 2);
}

void TLM_SAMPLE_TEST_2(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x0102);
  fmgServer->Log(DESTINATION_DGN, buf, 2);
}

void TLM_SAMPLE_TEST_3(void) {
  uint8_t * buf = (uint8_t *) malloc(2);
  Serialize ser(buf, 2);
  ser.serializeUInt16(0x0103);
  fmgServer->Log(DESTINATION_DGN, buf, 2);
}

void TLM_CREATING_ACPPACKET(int32 arg0) {
  uint8_t * buf = (uint8_t *) malloc(6);
  Serialize ser(buf, 6);
  ser.serializeUInt16(0x0400);
  ser.serializeInt32(arg0);
  fmgServer->Log(DESTINATION_ACP, buf, 6);
}

}
}

