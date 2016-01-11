#pragma once
#include "include/structure.h"
#include "include/md5.h"

class Packets
{
public:
	int SeqNo, InvitationCount;
public:
	Packets();
	virtual ~Packets();
void md5CheckSum(const unsigned char* Buffer, int BufferSize, unsigned char* dstBuffer);

	//MS_SYSTEM_INFO_REQ

	SignOutRequest_2320  Fun_SignOut_2320(short UserId, int sequanceNumber);

	MS_SIGN_ON_2300 LoginPacket_2300(int UserId, char pwd1[8], char NewPwd[8], char BrokerId[], short BranchId,
	int VersionNumber, short UserType, char WsClassName[14], char BrokerName[25]);


	MS_SYSTEM_INFO_REQ_1600 SystemInfoRequestPacket_1600(int UserId);


};

