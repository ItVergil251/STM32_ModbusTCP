#include "main.h"

/*
	 			ModBus ERROR CODES
_____________________________________________________
| **Code** | **Meaning**                            |
|----------|----------------------------------------|
| `0xE0`   | Success                                |
| `0xE1`   | Illegal Function                       |
| `0xE2`   | Illegal Data Address                   |
| `0xE3`   | Illegal Data Value                     |
| `0xE4`   | Slave Device Failure (or Timeout in some systems) |
| `0xE5`   | Acknowledge                            |
| `0xE6`   | Slave Device Busy                      |
| `0xE8`   | Memory Parity Error                    |
| `0xEA`   | Gateway Path Unavailable               |
| `0xEB`   | Gateway Target Device Failed to Respond|
|---------------------------------------------------|

			ModBus FUNCTION CODES
__________________________________________________________________________________________________________________________
| **Function Code (Hex)** | **Name**                         | **Purpose**                                               |
|-------------------------|----------------------------------|-----------------------------------------------------------|
| `0x01`                  | Read Coils                       | Reads status of coils (binary outputs).                   |
| `0x02`                  | Read Discrete Inputs             | Reads status of discrete inputs (binary inputs).          |
| `0x03`                  | Read Holding Registers           | Reads holding registers (16-bit data).                    |
| `0x04`                  | Read Input Registers             | Reads input registers (16-bit data, read-only).           |
| `0x05`                  | Write Single Coil                | Writes a single coil (binary output) to ON or OFF.        |
| `0x06`                  | Write Single Register            | Writes a single holding register (16-bit data).           |
| `0x0F`                  | Write Multiple Coils             | Writes multiple coils (binary outputs).                   |
| `0x10`                  | Write Multiple Registers         | Writes multiple holding registers (16-bit data).          |
| `0x11`                  | Report Slave ID                  | Retrieves slave ID and diagnostic information.            |
| `0x17`                  | Read/Write Multiple Registers    | Reads and writes multiple registers in a single transaction. |
| `0x2B`                  | Read File Record                 | Reads file records from the slave.                        |
| `0x2C`                  | Write File Record                | Writes file records to the slave.                         |
| `0x43`                  | Read Exception Status            | Reads the exception status of the slave device.           |
| `0x46`                  | Predefined Communication Area    | Reads/writes predefined communication areas.              |
|------------------------------------------------------------------------------------------------------------------------|

*/

#define ModBusTCP_PORT (502)

#define READ_COILS (0x01)
#define READ_INPUTS (0x02)
#define READ_Hregs (0x03)
#define READ_Iregs (0x04)
#define WRITE_COIL (0x05)
#define WRITE_Hreg (0x06)

typedef struct
{
	uint8_t ip[4];
	uint8_t id;
}ModBus_Server;

typedef struct
{
	GPIO_TypeDef* CS_port;
	uint16_t 	  CS_pin;
	SPI_HandleTypeDef* spi;
	wiz_NetInfo  wiznet;
}ModBus_Client;

/*							INITILIAZE FUNCTIONS										*/
bool client_init(ModBus_Client* client);
uint8_t connect_server(ModBus_Server* server);

/*						WRITE SINGLE REGISTER & COIL FUNCTIONS							*/
uint8_t write_HREG(ModBus_Server* server,uint16_t address,uint16_t data);
uint8_t write_COIL(ModBus_Server* server,uint16_t address,bool data);
/*							READ REGISTERS & COILS FUNCTIONS							*/
uint8_t read_HREGs(ModBus_Server* server,uint16_t *rx_Buf,uint16_t start,uint8_t num_regs);
uint8_t read_IREGs(ModBus_Server* server,uint16_t *rx_Buf,uint16_t start,uint8_t num_regs);
uint8_t read_Coils(ModBus_Server* server,bool *rx_Buf,uint16_t start,uint8_t num_coils);
uint8_t read_Inputs(ModBus_Server* server,bool *rx_Buf,uint16_t start,uint8_t num_inputs);
