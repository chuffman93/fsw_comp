/*******************************************************
 *
 * THIS FILE IS AUTO-GENERATED, DO NOT EDIT THIS FILE!
 * To edit this file, edit the auto-coder located
 * in TLM/auto_code/py
 *
 ******************************************************/

#ifndef TLM_H_
#define TLM_H_

#include <stdint.h>
#include "util/serialize.h"
#include "servers/FMGServer.h"

void TLM_STARTUP_ERROR(void);
void TLM_SERVERS_CREATED(void);
void TLM_MAIN_EXIT(void);
void TLM_RESTART_THREAD(uint8 arg0);
void TLM_MODE_SWITCH(uint8 arg0);
void TLM_SYS_CALL(int16 arg0);
void TLM_RESET_COMMANDED(void);
void TLM_TX_SILENCE_ENTERED(void);
void TLM_TX_SILENCE_EXITED(void);
void TLM_DWLK_CLEARED(int16 arg0);
void TLM_UPLK_CLEARED(int16 arg0);
void TLM_IMMED_CLEARED(int16 arg0);
void TLM_LOAD_NEW_SCHEDULE(void);
void TLM_LOAD_DEFAULT_SCHEDULE(void);
void TLM_SCHEDULE_BAD_SIZE(void);
void TLM_SCHEDULE_BAD_ITEM(void);
void TLM_SCHEDULE_BAD_READ(void);
void TLM_SCHEDULE_BAD_OPEN(void);
void TLM_SPI_ACS_RX(uint8 arg0);
void TLM_SPI_COM_RX(uint8 arg0);
void TLM_SPI_EPS_RX(uint8 arg0);
void TLM_SPI_PLD_RX(uint8 arg0);
void TLM_SPI_ACS_TX(uint8 arg0);
void TLM_SPI_COM_TX(uint8 arg0);
void TLM_SPI_EPS_TX(uint8 arg0);
void TLM_SPI_PLD_TX(uint8 arg0);
void TLM_SPI_ACS_TX_FAIL(uint8 arg0);
void TLM_SPI_COM_TX_FAIL(uint8 arg0);
void TLM_SPI_EPS_TX_FAIL(uint8 arg0);
void TLM_SPI_PLD_TX_FAIL(uint8 arg0);
void TLM_ACS_SERVER_STARTED(void);
void TLM_CDH_SERVER_STARTED(void);
void TLM_CMD_SERVER_STARTED(void);
void TLM_COM_SERVER_STARTED(void);
void TLM_EPS_SERVER_STARTED(void);
void TLM_ERR_SERVER_STARTED(void);
void TLM_FMG_SERVER_STARTED(void);
void TLM_GPS_SERVER_STARTED(void);
void TLM_PLD_SERVER_STARTED(void);
void TLM_SCH_SERVER_STARTED(void);
#endif
