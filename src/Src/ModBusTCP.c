#include "ModBusTCP.h"

ModBus_Client cl;
const uint8_t sock = 0;

void cs_sel() {
	HAL_GPIO_WritePin(cl.CS_port, cl.CS_pin, GPIO_PIN_RESET); //CS LOW
}

void cs_desel() {
	HAL_GPIO_WritePin(cl.CS_port, cl.CS_pin, GPIO_PIN_SET); //CS HIGH
}

uint8_t spi_rb(void) {
	uint8_t rbuf;
	HAL_SPI_Receive(cl.spi, &rbuf, 1, 0xFFFFFFFF);
	return rbuf;
}

void spi_wb(uint8_t b) {
	HAL_SPI_Transmit(cl.spi, &b, 1, 0xFFFFFFFF);
}

bool client_init(ModBus_Client* client)
{
	uint8_t bufSize[] = {2, 2, 2, 2};
	wiz_NetInfo buf_check;
	cl = *client;

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
	wizchip_init(bufSize, bufSize);
	wizchip_setnetinfo(&cl.wiznet);
	wizchip_getnetinfo(&buf_check);

    if (memcmp(cl.wiznet.mac, buf_check.mac, sizeof(buf_check.mac)) != 0) return false;
    if (memcmp(cl.wiznet.ip, buf_check.ip, sizeof(cl.wiznet.ip)) != 0) return false;
    if (memcmp(cl.wiznet.sn, buf_check.sn, sizeof(cl.wiznet.sn)) != 0) return false;
    if (memcmp(cl.wiznet.gw, buf_check.gw, sizeof(cl.wiznet.gw)) != 0) return false;
    if (memcmp(cl.wiznet.dns, buf_check.dns, sizeof(cl.wiznet.dns)) != 0) return false;

    if (cl.wiznet.dhcp != buf_check.dhcp) return false;

    return true;
}

uint8_t connect_server(ModBus_Server* server)
{
	if(socket(sock,Sn_MR_TCP,ModBusTCP_PORT,SF_TCP_NODELAY)!=0) { while(1) { return 1; } }
	for(int i=0;i<10;i++) { if(connect(sock,server->ip,ModBusTCP_PORT)==SOCK_OK) { break; } }
	return 0;
}

uint8_t write_HREG(ModBus_Server* server,uint16_t address,uint16_t data)
{
	uint16_t read_add = (address - 40000) & 0xFFFF;
	uint8_t time_out,
			response[30],
			received_len=0,
			RSR_len,
			hreg_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x00, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, WRITE_Hreg,
			hreg_address[0], hreg_address[1],
			(data >> 8) & 0xFF, data & 0xFF
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1]) {	return 0; }
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}

uint8_t write_COIL(ModBus_Server* server,uint16_t address,bool data)
{
	uint16_t read_add = (address) & 0xFFFF;
	uint8_t	time_out,
			response[12],
			received_len=0,
			RSR_len,
			coil_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x00, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, WRITE_COIL,
			coil_address[0], coil_address[1],
			(data) ? 0xFF : 0x00, 0x00
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1]) { return 0; }
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}

uint8_t read_HREGs(ModBus_Server* server,uint16_t *rx_Buf,uint16_t start,uint8_t num_regs)
{
	uint16_t read_add = (start - 40000) & 0xFFFF;
	uint8_t time_out,
			response[30],
			received_len=0,
			RSR_len,
			hreg_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x45, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, READ_Hregs,
			hreg_address[0], hreg_address[1],
			0x00, num_regs
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1])
			{
				for(int i=0;i<num_regs;i++) { *(rx_Buf+i)=(response[9+(i*2)]<<8)|response[10+(i*2)]; }
				return 0;
			}
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}

uint8_t read_IREGs(ModBus_Server* server,uint16_t *rx_Buf,uint16_t start,uint8_t num_regs)
{
	uint16_t read_add = (start - 30000) & 0xFFFF;
	uint8_t time_out,
			response[30],
			received_len=0,
			RSR_len,
			Ireg_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x45, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, READ_Iregs,
			Ireg_address[0], Ireg_address[1],
			0x00, num_regs
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1])
			{
				for(int i=0;i<num_regs;i++) { *(rx_Buf+i)=(response[9+(i*2)]<<8)|response[10+(i*2)]; }
				return 0;
			}
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}

uint8_t read_Coils(ModBus_Server* server,bool *rx_Buf,uint16_t start,uint8_t num_coils)
{
	uint16_t read_add = (start) & 0xFFFF;
	uint8_t time_out,
			response[11],
			received_len=0,
			RSR_len,
			Coil_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x45, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, READ_COILS,
			Coil_address[0], Coil_address[1],
			0x00, num_coils
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1])
			{
				for(int i=0;i<num_coils;i++) { rx_Buf[i] = ((response[9 + (i / 8)]) >> (i % 8)) & 0x01;}
				return 0;
			}
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}

uint8_t read_Inputs(ModBus_Server* server,bool *rx_Buf,uint16_t start,uint8_t num_inputs)
{
	uint16_t read_add = (start - 10000) & 0xFFFF;
	uint8_t time_out,
			response[11],
			received_len=0,
			RSR_len,
			Input_address[2] ={ (read_add >> 8) & 0xFF, read_add & 0xFF };

	uint8_t request[12] =
	{
			0x45, 0x14, 0x00, 0x00,
			0x00, 0x06, server->id, READ_INPUTS,
			Input_address[0], Input_address[1],
			0x00, num_inputs
	};

	send(sock,request,sizeof(request));
	while(received_len==0)
	{
		if ((RSR_len = getSn_RX_RSR(sock)) > 0)
		{
			received_len=recv(sock,response,RSR_len);
			if(response[0]==request[0]&&request[1]==response[1])
			{
				for(int i=0;i<num_inputs;i++) { rx_Buf[i] = ((response[9 + (i / 8)]) >> (i % 8)) & 0x01;}
				return 0;
			}
			else
			{
				disconnect(sock);
				return 2;
			}
		}
		else
		{
			time_out++;
			if(time_out>250)
			{
				disconnect(sock);
				return 4;
			}
		}
	}
	disconnect(sock);
	return 4;
}
