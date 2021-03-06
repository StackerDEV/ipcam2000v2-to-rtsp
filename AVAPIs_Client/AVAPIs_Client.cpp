﻿// AVAPIs_Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <sys/timeb.h>
#include <wchar.h>
#include <io.h>
#pragma comment(lib, "ws2_32")
#define WSA_VERSION MAKEWORD(2, 2) // using winsock 2.2
#include "IOTCAPIs.h"
#include "AVAPIs.h"
#include "AVFRAMEINFO.h"
#include "AVIOCTRLDEFs.h"
#include "picosha2.h"
#include <sstream>

#define MAX_ARRAY_SIZE 1

using namespace std;
int gSID;

void loading(std::string msg, unsigned int ms)
{
	for (char& c : msg) {
		printf("%c", c);
		Sleep(ms);
	}
}

void *recv_io_ctrl(void* arg) {
	int avIndex = *(int*)arg;
	unsigned int ioType;
	SMsgAVIoctrlDeviceInfoResp ioMsg;
	memset(&ioMsg, 0, sizeof(SMsgAVIoctrlDeviceInfoResp));
	int ret = avRecvIOCtrl(avIndex, &ioType, (char*)&ioMsg, sizeof(SMsgAVIoctrlDeviceInfoResp), 10000);

	char firmware[256];
	unsigned long version;
	version = ioMsg.version;
	printf("[ Device Information\n");
	printf("[ Model: %s\n", ioMsg.model);
	printf("[ Vendor: %s\n", ioMsg.vendor);
	sprintf_s(firmware, "%d.%d.%d.%d", (version & 0xFF000000) >> 24, (version & 0xFF0000) >> 16, (version & 0xFF00) >> 8, (version & 0xFF) >> 0);
	printf("[ Firmware: %s\n", firmware);
	return NULL;
}

int main(int argc, char *argv[])
{
	int ret;
	char *passWd = "";

	if (argc < 2)
	{
		printf("Usage: AVAPIs_Client <password>\n");
		exit(0);
	}

	DWORD t1 = GetTickCount();
	printf("Generating cryptographic password..\n");
	std::string pwarg = argv[1];
	std::string hash_hex_str;
	picosha2::hash256_hex_string(pwarg + "a%2f@L1.nT6$)aP!", hash_hex_str);
	for (int i = 0; i < 15; i++) {
		hash_hex_str[i] = toupper(hash_hex_str[i]);
	}
	hash_hex_str.resize(15, -1);
	passWd = _strdup(hash_hex_str.c_str());

	printf("StreamClient start...\n");
	ret = IOTC_Initialize2(0);
	printf("IOTC_Initialize() ret = %d\n", ret);
	if (ret != IOTC_ER_NoERROR)
	{
		printf("IOTCAPIs_Device exit...!!\n");
		return 0;
	}
	avInitialize(8);

	st_LanSearchInfo2 psLanSearchInfo[MAX_ARRAY_SIZE];
	memset(psLanSearchInfo, 0, sizeof(st_LanSearchInfo2)*MAX_ARRAY_SIZE);
	IOTC_Lan_Search2(psLanSearchInfo, MAX_ARRAY_SIZE, 5000);
	printf("Searching lan for devices");
	loading("......\n", 400);

	for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
		if (psLanSearchInfo->UID[0]) {
			printf("UID:%s IP:%s Port:%d\n", psLanSearchInfo->UID, psLanSearchInfo->IP, \
				psLanSearchInfo->port);
			printf("Found, connecting to device..\n");
		}
		else {
			printf("No local devices found.\nExiting..\n");
			exit(0);
		}
	}

	gSID = IOTC_Connect_ByUID(psLanSearchInfo->UID);
	printf("Call IOTC_Connect_ByUID(%s) ret(%d).......\n", psLanSearchInfo->UID, gSID);
	struct st_SInfo Sinfo;
	IOTC_Session_Check(gSID, &Sinfo);
	if (Sinfo.Mode == 0)
		printf("Device is from %s:%d[%s] Mode=P2P\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID);
	else if (Sinfo.Mode == 1)
		printf("Device is from %s:%d[%s] Mode=RLY\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID);
	else if (Sinfo.Mode == 2)
		printf("Device is from %s:%d[%s] Mode=LAN\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID);
	printf("Device info VER[%X] NAT[%d]\n", Sinfo.IOTCVersion, Sinfo.NatType);
	printf("Cost time = %ld ms\n", GetTickCount() - t1);

	char buf[256];
	unsigned long version;
	IOTC_Get_Version(&version);
	sprintf_s(buf, "%d.%d.%d.%d", (version & 0xFF000000) >> 24, (version & 0xFF0000) >> 16, (version & 0xFF00) >> 8, (version & 0xFF) >> 0);
	printf("IOTC Version: %s\n", buf);
	unsigned long srvType;
	int avIndex = avClientStart(gSID, "admin", passWd, 20000, &srvType, 0); // 
	printf("Call avClientStart OK(%d).......\n", avIndex);
	if (avIndex < 0)
	{
		printf("avClientStart failed[%d]\n", avIndex);
		return 0;
	}
	
	stringstream rtspServ;
	SMsgAVIoctrlDeviceInfoReq ioMsg;
	memset(&ioMsg, 0, sizeof(SMsgAVIoctrlDeviceInfoReq));
	avSendIOCtrl(avIndex, IOTYPE_USER_IPCAM_DEVINFO_REQ, (char*)&ioMsg, sizeof(SMsgAVIoctrlDeviceInfoReq));
	printf("Call avSendIOCtrl => IOTYPE_USER_IPCAM_DEVINFO_REQ\n");
	printf("[--------------------------------------------------------------------------]\n");
	recv_io_ctrl(&avIndex);
	rtspServ << "rtsp://admin:" << std::string(passWd) << "@" + std::string(psLanSearchInfo->IP) << "/live0.264";
	printf("[ RTSP streaming URL: %s\n", rtspServ.str().c_str());
	printf("[--------------------------------------------------------------------------]\n");
	printf("avClientStop calling....\n");
	avClientStop(avIndex);
	printf("avClientStop OK\n");
	IOTC_Session_Close(gSID);
	printf("IOTC_Session_Close OK\n");
	avDeInitialize();
	IOTC_DeInitialize();
	printf("StreamClient exit...\n");

	return 0;
}

