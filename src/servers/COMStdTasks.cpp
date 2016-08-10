/*
 * COMStdTasks.cpp
 * Written by: Brian Campuzano
 * Created: 10/20/2012
 */

#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/ConfigMessage.h"

#include "util/Logger.h"

//#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"
#include "POSIX.h"

//extern "C"
//{
//#include "util/eKermit.h"
//#include "util/kermit.h"
//#include "util/CircularBuffer.h"
//}

using namespace std;
using namespace AllStar::Core;


namespace AllStar
{
	namespace Servers
	{
		FSWPacket * COMHealthStatus(void)
		{
			FSWPacket * HSQuery = new FSWPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, COM_HS_CMD, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * HSRet = DispatchPacket(HSQuery);

			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("COMStdTasks: unfinished function entered!", LOGGER_LEVEL_FATAL);
			// Extract to beacon
			// UpdateCOMHS(uint16 signalStrength, uint8 transmitPower,uint8 TXPLLFlags,int8 PATemp, uint8 RXIFDetect,
			// uint8 RXGain, uint8 currentSense, uint8 voltageSense)
			
//			COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
//			void * outputArray[33] = {NULL};
//			uint32 enumArray[33] = {
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT
//				};
//
//			if(!ExtractParameters(*HSRet,enumArray,33,outputArray))
//			{
//				// Error
//			}
//			else
//			{
//				// Link Status
//				uint16 lwipPacketsTx	    = * (uint16 *) outputArray[0];
//				uint16 lwipPacketsRx	    = * (uint16 *) outputArray[1];
//				uint16 lwipPacketsDrop		= * (uint16 *) outputArray[2];
//				uint16 lwipPacketsErrors	= * (uint16 *) outputArray[3];
//
//				// ETHARP Status
//				uint16 lwipEtharpTx			= * (uint16 *) outputArray[4];
//				uint16 lwipEtharpRx			= * (uint16 *) outputArray[5];
//				uint16 lwipEtharpDrop		= * (uint16 *) outputArray[6];
//				uint16 lwipEtharpErrors		= * (uint16 *) outputArray[7];
//
//				// IP Status
//				uint16 lwipIPTx			= * (uint16 *) outputArray[8];
//				uint16 lwipIPRx			= * (uint16 *) outputArray[9];
//				uint16 lwipIPDrop		= * (uint16 *) outputArray[10];
//				uint16 lwipIPErrors		= * (uint16 *) outputArray[11];
//
//				// UDP Status
//				uint16 lwipUDPTx		= * (uint16 *) outputArray[12];
//				uint16 lwipUDPRx		= * (uint16 *) outputArray[13];
//				uint16 lwipUDPDrop		= * (uint16 *) outputArray[14];
//				uint16 lwipUDPErrors	= * (uint16 *) outputArray[15];
//
//				// TCP Status
//				uint16 lwipTCPTx		= * (uint16 *) outputArray[16];
//				uint16 lwipTCPRx		= * (uint16 *) outputArray[17];
//				uint16 lwipTCPDrop	    = * (uint16 *) outputArray[18];
//				uint16 lwipTCPErrors	= * (uint16 *) outputArray[19];
//
//				// Memory Status
//				uint16 lwipMemAvail	    = * (uint16 *) outputArray[20];
//				uint16 lwipMemUsed	    = * (uint16 *) outputArray[21];
//				uint16 lwipMemMax		= * (uint16 *) outputArray[22];
//				uint16 lwipMemErrors	= * (uint16 *) outputArray[23];
//				uint16 lwipMemIllegal	= * (uint16 *) outputArray[24];
//
//				uint16 comState		    = * (uint16 *) outputArray[25];
//				uint16 statBits		    = * (uint16 *) outputArray[26];
//
//				uint16 signalStrength	= * (uint16 *) outputArray[27];	// check
//				uint8 transmitPower	    = * (uint8 *) outputArray[28];	// check
//				uint8 RXIFDetect		= * (uint8 *) outputArray[29];	// check
//				uint8 RXGain			= * (uint8 *) outputArray[30];	// check
//				uint8 currentSensCOMHealthStatue	    = * (uint8 *) outputArray[31];	// check
//				uint8 voltageSense	    = * (uint8 *) outputArray[32];	// check
//
//				/*comServer->UpdateCOMHS(lwipPacketsTx,lwipPacketsRx,lwipPacketsDrop,lwipPacketsErrors,lwipEtharpTx, lwipEtharpRx,lwipEtharpDrop,lwipEtharpErrors,
//										lwipIPTx,lwipIPRx,lwipIPDrop,lwipIPErrors,lwipUDPTx,lwipUDPRx,lwipUDPDrop,lwipUDPErrors,
//										lwipTCPTx,lwipTCPRx,lwipTCPDrop,lwipTCPErrors,lwipMemAvail,lwipMemUsed,lwipMemMax,lwipMemErrors,lwipMemIllegal, comState, statBits,
//										signalStrength,transmitPower,RXIFDetect,RXGain,currentSense,voltageSense);*/
//			}
		
			return HSRet;
			//return(DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND, COM_HS_CMD));
		}

/*COMHealthStatu
		ReturnMessage * COMBeacon(void)
		{
			COMServer * comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
			uint8 * beaconArray = new uint8[79];
			beaconArray[0] = 'c';
			beaconArray[1] = 'o';
			beaconArray[2] = 's';
			beaconArray[3] = 'g';
			beaconArray[4] = 'c';
			beaconArray[5] = '1';
			beaconArray[6] = 'a';
			beaconArray[7] = 's';
			for(volatile int i = 8; i < 76; i++)
			{
				beaconArray[i] = i;
			}
			//insert end of line character
			beaconArray[76] = 'e';
			beaconArray[77] = 'o';
			beaconArray[78] = 'l';
			vector<uint8> beacon;
			beacon.assign(const_cast<uint8*>(beaconArray), const_cast<uint8*>(beaconArray) + 79);
			
			DEBUG_COUT("		In COMBeacon, calling COMDataSend...")
			ReturnMessage * retMsg2 = COMDataSend(beacon, SERVER_LOCATION_COM);
			
			delete beaconArray;
			
			DEBUG_COUT("		In COMBeacon, returned opcode: " << ((int) retMsg2->GetOpcode()));
			
			if(retMsg2->GetSuccess() && (retMsg2->GetOpcode() == COM_DATA_SEND_SUCCESS))
			{
				DataMessage dat(COM_BEACON_SUCCESS);
				ReturnMessage * ret = new ReturnMessage(&dat, true);
				delete retMsg2;
				return ret;
			}
			
			DEBUG_COUT("		In COMBeacon, Success bit was false...")
			ErrorMessage err(COM_BEACON_FAILURE);
			ReturnMessage * ret = new ReturnMessage(&err, false);
			delete retMsg2;
			return ret;
		}

		ReturnMessage * COMDataSend(const std::vector<uint8> & data, LocationIDType server)
		{
			VariableTypeData data_Holder(data);
			DEBUG_COUT("		In COMDataSend, calling DispatchPacket...")
#ifdef WIN32
			switch(server)
			{
				case SERVER_LOCATION_COM:
					return(DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND,
						COM_DATA_SEND_CMD, data_Holder));
				case SERVER_LOCATION_GND_COM:
					return(DispatchPacket(SERVER_LOCATION_GND_COM, HARDWARE_LOCATION_GND_COM, 1, 0, MESSAGE_TYPE_COMMAND,
						COM_DATA_SEND_CMD, data_Holder));
				default:
					return(DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND,
						COM_DATA_SEND_CMD, data_Holder));
			}
#else
			return(DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND, COM_DATA_SEND_CMD, data_Holder));
#endif
		}

		ReturnMessage * COMReset(void)
		{
			return(DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 1, 0, MESSAGE_TYPE_COMMAND, COM_RESET_CMD));
		}

		ReturnMessage * COMDataReceive(std::vector<uint8> & data, LocationIDType server)
		{
			//DEBUG_COUT("		COMDataReceive: called...")
			
			COMServer * comServer;
#ifdef WIN32
			switch(server)
			{
				case SERVER_LOCATION_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
					//DEBUG_COUT("			Space COM: received " << data.size() << " bytes.")
					break;
				case SERVER_LOCATION_GND_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(GROUND_COM_SERVER_SINGLETON));
					//DEBUG_COUT("			Ground COM: received " << data.size() << " bytes.")
					break;
				default:
					ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					// Write to event log
					return eRet;
			}
#else
			comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
#endif
			
			switch(comServer->GetState())
			{
				case BEACON_STATE:
				{
					//how did we get data while we are beaconing!
					DEBUG_COUT("		In COMDataReceive, received beacon...")
					ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					return eRet;
				}
				case COMMAND_STATE:
				{
					DEBUG_COUT("		COMDataReceive: In CommandState...")
					comServer->SetParseFlag();
					break;
				}
				case SEND_STATE:
				case RECEIVE_STATE:
				{
					//grab the pointer to the input buffer
					//CircularBuffer * inBuf = comServer->GetInputBuffer();
					if(inBuf == NULL)
					{
						DEBUG_COUT("		COMDataReceive: input buffer is NULL...")
						ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
						ReturnMessage * eRet = new ReturnMessage(&err, false);
						return eRet;
					}
					//add data to the buffer.
					//check if there is room in the buffer.
					if(data.size() >= (inBuf->capacity - inBuf->used - 1))
					{
						DEBUG_COUT("		COMDataReceive: input buffer is full...")
						//not enough room in buffer
						ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
						ReturnMessage * eRet = new ReturnMessage(&err, false);
						return eRet;
					}

					std::vector<uint8>::iterator it = data.begin();
					//extract data from vector and put in input buffer.
					for (; it != data.end(); ++it, inBuf->used++)
					{
						inBuf->buffer[inBuf->head] = *it;
						inBuf->head++;
						if(inBuf->head == IBUFLEN)
						{
							inBuf->head = 0;
						}
					}
					//DEBUG_COUT("		COMDataReceive: added data to buffer...")
					DataMessage dat(COM_DATA_RECEIVE_SUCCESS);
					ReturnMessage * ret = new ReturnMessage(&dat, true);
					return ret;
				}
				default:
					ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					return eRet;
			}
		}

		ReturnMessage * COMLogin(LocationIDType server)
		{
			debug_led_set_led(1, LED_ON);
			DEBUG_COUT(		"COMLogin: Called...")
			COMServer * comServer;
#ifdef WIN32
			switch(server)
			{
				case SERVER_LOCATION_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
					break;
				case SERVER_LOCATION_GND_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(GROUND_COM_SERVER_SINGLETON));
					break;
				default:
					ErrorMessage err(COM_LOGIN_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, true);
					// Write to event log
					return eRet;
			}
#else
			comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
#endif
			
			if (comServer->SetState(COMMAND_STATE))
			{
				DataMessage dat(COM_LOGIN_SUCCESS);
				ReturnMessage * ret = new ReturnMessage(&dat, true);
				// Write to event log
				return ret;
			}
			else
			{
				ErrorMessage err(COM_LOGIN_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				// Write to event log
				return eRet;
			}
		}

		ReturnMessage * COMLogout(LocationIDType server, MessageCodeType opcode)
		{
			//debug_led_set_led(7, LED_TOGGLE);
			COMServer * comServer;
#ifdef WIN32
			switch(server)
			{
				case SERVER_LOCATION_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
					break;
				case SERVER_LOCATION_GND_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(GROUND_COM_SERVER_SINGLETON));
					break;
				default:
					ErrorMessage err(COM_LOGOUT_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					// Write to event log
					return eRet;
			}
#else
			comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
#endif
			ReturnMessage * ret = NULL;
			switch(opcode)
			{
				case COM_LOGOUT_CMD:
					ret = DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 0, 0, MESSAGE_TYPE_COMMAND, COM_LOGOUT_CMD);
					break;
				case COM_GND_LOGGED_OUT_CMD:
				case COM_CONNECTION_LOST_CMD:
					break;
				default:
					ErrorMessage err(COM_LOGOUT_FAILURE);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					// Write to event log
					return eRet;
			}
			
			//TODO: Check if eKermit is running and delete the task if it is.
			if (comServer->SetState(BEACON_STATE))
			{
				if(ret != NULL)
				{
					if(ret->GetSuccess())
					{
						DataMessage dat(COM_LOGOUT_SUCCESS);
						ReturnMessage * ret = new ReturnMessage(&dat, true);
						// Write to event log
						return ret;
					}
					else
					{
						ErrorMessage err(COM_LOGOUT_FAILURE);
						ReturnMessage * eRet = new ReturnMessage(&err, false);
						// Write to event log
						return eRet;
					}
				}
				DataMessage dat(COM_LOGOUT_SUCCESS);
				ReturnMessage * ret = new ReturnMessage(&dat, true);
				// Write to event log
				return ret;
			}
			else
			{
				ErrorMessage err(COM_LOGOUT_FAILURE);
				ReturnMessage * eRet = new ReturnMessage(&err, false);
				// Write to event log
				return eRet;
			}
		}

		//function to send a message back to ground, packetception
		void DispatchGndPacket(MessageTypeEnum type, MessageCodeType opCode, std::list<VariableTypeData*> parameters)
		{
			//message holder
			MultiDataMessage * msg;
			//build message based on type and opCode.
			switch (type)
			{
				case MESSAGE_TYPE_COMMAND:
					msg = new CommandMessage(opCode);
					break;
				case MESSAGE_TYPE_DATA:
					msg = new DataMessage(opCode);
					break;
				case MESSAGE_TYPE_ERROR:
					msg = new ErrorMessage(opCode);
					break;
				case MESSAGE_TYPE_CONFIG:
					msg = new ConfigMessage(opCode);
					break;
				default:
					return;
			}
			
			msg->SetParameters(parameters);
			
			VariableTypeData packet_hold(FSWPacket(SERVER_LOCATION_COM, SERVER_LOCATION_GND, 0, 1, msg));
			delete msg;
			ReturnMessage * retMsg =  DispatchPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, 0, 1, MESSAGE_TYPE_COMMAND, 
					COM_DATA_SEND_CMD, packet_hold);

			MessageProcess(SERVER_LOCATION_COM, retMsg);
			return;
		}
*/


		//TODO: uncomment and deal with this
/*		//function used to try to build a packet out of the input buffer and parse it.
		bool ParseGndCommand(LocationIDType server)
		{
			DEBUG_COUT("ParseGndCommand called...")
			
			COMServer * comServer;
#ifdef WIN32
			switch(server)
			{
				case SERVER_LOCATION_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
					break;
				case SERVER_LOCATION_GND_COM:
					comServer = dynamic_cast<COMServer *>(Factory::GetInstance(GROUND_COM_SERVER_SINGLETON));
					break;
				default:
					return false;
			}
#else
			comServer = dynamic_cast<COMServer *>(Factory::GetInstance(COM_SERVER_SINGLETON));
#endif

			FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			//empty list for dispatching ground packets with no var
			std::list<VariableTypeData*> empty;
			
			//grab the pointer to the input buffer
			CircularBuffer * inBuf = comServer->GetInputBuffer();
			char * fileIn = comServer->GetFileInputBuffer();
			char * fileOut = comServer->GetFileOuputBuffer();
			if(inBuf == NULL || fileIn == NULL || fileOut == NULL)
			{
				DEBUG_COUT("ParseGndCommand: Buffer failure...")
				DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_NULL_BUFFER_FAILURE, empty);
				return false;
			}
			
			uint8 * buffer = GetCurrentBuffer(inBuf);
			
			// Parse data, look for ground command.
			FSWPacket gndCmd(buffer, inBuf->used);
			free(buffer);
			
			if((SERVER_LOCATION_GND == gndCmd.GetSource()) && (SERVER_LOCATION_COM == gndCmd.GetDestination()) 
					&& (gndCmd.GetMessagePtr() != NULL))
			{
				DEBUG_COUT("ParseGndCommand: FSWPacket format correct...")
				MultiDataMessage * gndMsg = dynamic_cast<MultiDataMessage *> (gndCmd.GetMessagePtr());
				if(gndMsg == NULL)
				{
					DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_MSG_CAST_FAILURE, empty);
					return false;
				}
				MessageCodeType opcode = gndMsg->GetOpcode();
				if(opcode == EKERMIT_SEND_CMD)
				{
					DEBUG_COUT("ParseGndCommand: kermit downlink mode...")
					//extract file list
					std::list<VariableTypeData*> filesNotFound;
					std::list<VariableTypeData*> params = gndMsg->GetParameters();
					size_t size = params.size();
					if(size > MAX_DOWNLINK_FILES)
					{
						//too many files
						DEBUG_COUT("ParseGndCommand: too many files...")
						DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_SEND_TOO_MANY_FILES_FAILURE, empty);
						return false;
					}
					
					string data;
					UCHAR ** fileList = new UCHAR*[MAX_DOWNLINK_FILES];
					uint32 index = 0;
					
					for(size_t i = 0; i < size; i++)
					{
						VariableTypeData temp = * params.front();
						params.pop_front();
						if(VAR_TYPE_ENUM_STRING != temp.GetType())
						{
							//incorrect variable type data
							
							DEBUG_COUT("ParseGndCommand: parameter not a string...")
							DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_SEND_WRONG_VAR_TYPE_FAILURE, empty);
							return false;
						}

						data = *((string *)temp.GetData());

						if(data.length() > FN_MAX)
						{
							DEBUG_COUT("ParseGndCommand: filename too long...")
							DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_SEND_FILENAME_TOO_LONG_FAILURE, empty);
							return false;
						}
						
						//check file list
						if(0 < fileHandler->FileExists(data.c_str()))
						{
							//create array
							fileList[index] = new UCHAR[FN_MAX];
							//set file list
							DEBUG_COUT("ParseGndCommand: found file: " << data.c_str())
							uint8 length = data.copy((char *) fileList[index], data.length(), 0);
							fileList[index][length] = NULL;
							DEBUG_COUT("	ParseGndCommand: copied file name: " << fileList[index])
							index++;
						}
						else
						{
							//add to list of files not found
							DEBUG_COUT("ParseGndCommand: couldn't find file: " << data.c_str())
							VariableTypeData fileName(data);
							filesNotFound.push_back(&fileName);
							//Failed to find file keep going
						}
					}

					//switch modes
					if(!comServer->SetState(SEND_STATE))
					{
						//send failure return message back
						DEBUG_COUT("ParseGndCommand: failed to set state...")
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SEND_SET_STATE_FAILURE, empty);
						return false;
					}
					
					fileList[index] = NULL;					
					
					xTaskHandle * invokeTaskHandle = new xTaskHandle();
					KermitSendParameter * parameters = new KermitSendParameter();
					
					parameters->server = server;
					parameters->fileList = (UCHAR **) fileList;
					parameters->numFiles = index;
					vSemaphoreCreateBinary(parameters->syncSem);
					parameters->doneSem = xSemaphoreCreateMutex( );
					parameters->inBuf = inBuf;
					parameters->fileIn = fileIn;
					parameters->fileOut = fileOut;
					
					if((!comServer->SetSendTaskHandle(invokeTaskHandle)) || (!comServer->SetSendParameters(parameters)))
					{
						DEBUG_COUT("ParseGndCommand: failed to set task handle or task params...")
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SET_PARAMS_FAILURE, empty);
						return false;
					}
					
					//send success return message back to ground station
					if(filesNotFound.size() == 0)
					{
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SEND_SUCCESS, empty);
					}
					else if(index == 0)
					{
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SEND_FAILURE, filesNotFound);
					}
					else
					{
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SEND_PARTIAL_SUCCESS, filesNotFound);
					}
					
					DEBUG_COUT("ParseGndCommand: starting kermit in downlink mode...")
					
					comServer->SetKermitRunFlag(true);
					
					xSemaphoreTake(&(parameters->syncSem), SEM_WAIT_DELAY);

					CREATE_TASK(KermitSendTask, (const signed char* const)"KermitSendTask", 5000, parameters, configMAX_PRIORITIES - 3,
						invokeTaskHandle);

					xSemaphoreTake(&(parameters->syncSem), SEM_WAIT_DELAY);
					return false;
				}
				else if(opcode == EKERMIT_RECEIVE_CMD)
				{
					
					DEBUG_COUT("ParseGndCommand: kermit receive mode...")
					//extract number of files
					std::list<VariableTypeData*> params = gndMsg->GetParameters();
					size_t size = params.size();
					if(size != 0)
					{
						DEBUG_COUT("ParseGndCommand: wrong number params...")
						DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_RECEIVE_NUM_PARAMS_FAILURE, empty);
						return false;
					}

					//switch modes
					if(!comServer->SetState(RECEIVE_STATE))
					{
						DEBUG_COUT("ParseGndCommand: failed to set state...")
						//send failure return message back
						DispatchGndPacket(MESSAGE_TYPE_ERROR, EKERMIT_RECEIVE_SET_STATE_FAILURE, empty);
						return false;
					}

					xTaskHandle * invokeTaskHandle = new xTaskHandle();
					KermitReceiveParameter * parameters = new KermitReceiveParameter();

					parameters->server = server;
					vSemaphoreCreateBinary(parameters->syncSem);
					parameters->doneSem = xSemaphoreCreateMutex( );
					parameters->inBuf = inBuf;
					parameters->fileIn = fileIn;
					parameters->fileOut = fileOut;
					
					if((!comServer->SetReceiveTaskHandle(invokeTaskHandle)) || (!comServer->SetReceiveParameters(parameters)))
					{
						DEBUG_COUT("ParseGndCommand: failed to set task handle or task params...")
						DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_SET_PARAMS_FAILURE, empty);
						return false;
					}
					
					DEBUG_COUT("ParseGndCommand: starting kermit in receive mode...")
					//send success return message back
					DispatchGndPacket(MESSAGE_TYPE_DATA, EKERMIT_RECEIVE_SUCCESS, empty);
					
					comServer->SetKermitRunFlag(true);
					
					xSemaphoreTake(&(parameters->syncSem), SEM_WAIT_DELAY);

					CREATE_TASK(KermitReceiveTask, (const signed char* const)"KermitReceiveTask", 5000, parameters, configMAX_PRIORITIES - 3,
						invokeTaskHandle);

					xSemaphoreTake(&(parameters->syncSem), SEM_WAIT_DELAY);
					return false;
				}
				
				//wrong opcode can't parse packet
				return false;
			}
			//incorrectly constructed packet most likely not enough data to make a packet don't flush
			return true;
		}

		void KermitSendTask(void * params)
		{
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			KermitSendParameter * parameters = (KermitSendParameter *) params;
			
			DEBUG_COUT("KermitSendTask: First File Name: " << parameters->fileList[0])

			xSemaphoreGive(&(parameters->syncSem));
			xSemaphoreTake(&(parameters->doneSem), SEM_WAIT_DELAY);
			
			for(int i = 0; i < 20; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}

			if(RunKermit(A_SEND, parameters->numFiles, parameters->fileList, (UCHAR *) parameters->fileIn, 
				(UCHAR *) parameters->fileOut, parameters->inBuf, parameters->server) == 0)
			{
				//Kermit exited with a failure end send task
				//error stuff
			}
			
			xSemaphoreGive(&(parameters->doneSem));

			while (1)
			{
				usleep(10000);
			}
		}

		void KermitReceiveTask(void * params)
		{
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			debug_led_set_led(4, LED_ON);
			KermitReceiveParameter * parameters = (KermitReceiveParameter *) params;

			xSemaphoreGive(&(parameters->syncSem));
			xSemaphoreTake(&(parameters->doneSem), SEM_WAIT_DELAY);
			
			for(int i = 0; i < 45; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}
			
			//while(parameters->inBuf->used == 0);
			
			if(RunKermit(A_RECV, 0, NULL, (UCHAR *) parameters->fileIn, (UCHAR *) parameters->fileOut, 
				parameters->inBuf, parameters->server) == 0)
			{
				//Kermit exited with a failure end receive task
			}

			xSemaphoreGive(&(parameters->doneSem));

			while (1)
			{
				debug_led_set_led(7, LED_TOGGLE);
				usleep(1000000);
			}
		}*/
	}
}
