#include "Packets.h"
#include "include/structure.h"
#include "include/Socket.h"
#include <string.h>
#include <netinet/in.h>
//#include<Winsock2.h>

#include<iostream>

md5_state_t state;

Packets::Packets()
{

}

Packets::~Packets()
{
}


void Packets:: md5CheckSum(const unsigned char* Buffer, int BufferSize, unsigned char* dstBuffer)
{
    md5_init(&state);
	md5_append(&state, Buffer, BufferSize);
	md5_finish(&state, dstBuffer);
}




MS_SIGN_ON_2300 Packets::LoginPacket_2300(int UserId, char pwd1[8], char NewPwd[8], char BrokerId[], short BranchId,
	int VersionNumber, short UserType, char WsClassName[14], char BrokerName[25])
{
   /* static char pwd[]="Sq@020;1";
    static char bid[]="12468";
	static char wcn[14]="1234567";
	static char bname[25]="DIVYA PORTFOLIO PVT LTD";
*/
    MS_SIGN_ON_2300  obj;

    memset(&obj,0,sizeof(MS_SIGN_ON_2300));

	obj.Header.TransactionCode=ntohs(2300);
	obj.Header.LogTime=0;
	memset(obj.Header.AlphaChar ,' ',2);
	obj.Header.TraderId=ntohl(UserId);

	obj.Header.ErrorCode=0;
	memset(obj.Header.TimeStamp ,0,8);
	memset(obj.Header.TimeStamp1 ,0,8);
	memset(obj.Header.TimeStamp2 ,0,8);
	obj.Header.MessageLength=ntohs(214);
	obj.UserId=ntohl(UserId);
	memcpy(obj.Password,pwd1,8);
	memset(obj.TraderName ,' ',26);
	obj.LastPasswordChangeDate=ntohl(0);
	memcpy(obj.BrokerId  ,BrokerId,5);

	if(NewPwd=="")
        memcpy(obj.NewPassword,NewPwd,8);
    else
        memset(obj.NewPassword,' ',8);

    obj.BranchId=ntohs(BranchId);
    obj.VersionNumber=ntohl(VersionNumber);
	obj.Batch2StartTime=0;
	obj.HostSwitchContext=' ';
	memset(obj.Colour,' ',50);
	obj.UserType=ntohs(UserType);
	obj.SequenceNumber=ntohs(0);
	memcpy(obj.WsClassName,WsClassName,14);
    obj.BrokerStatus=' ';
	obj.ShowIndex=' ';
	obj.MemberType=0;
	obj.ClearingStatus=' ';
	memcpy(obj.BrokerName,BrokerName,sizeof(obj.BrokerName));
	obj.StMkt.reserved_1=' ';

    obj.SendPF.Length=ntohs(240);
    obj.SendPF.Messae_Count=ntohs(1);
    obj.SendPF.SeqNumber=ntohl(1);

    md5_byte_t check[16];
    md5CheckSum((const unsigned char*)&obj+24,sizeof( MS_SIGN_ON_2300 )-24,check);
    memcpy(&obj.SendPF.Checksum,&check,16);

return obj;
}

MS_SYSTEM_INFO_REQ_1600 Packets::SystemInfoRequestPacket_1600(int UserId)
{
    MS_SYSTEM_INFO_REQ_1600 obj;
    memset(&obj,0,70);

    obj.Header.TransactionCode=ntohs(1600);

	obj.Header.LogTime=0;
	memset(obj.Header.TimeStamp ,0,8);
	memset(obj.Header.TimeStamp1 ,0,8);
	memset(obj.Header.TimeStamp2 ,0,8);
	obj.Header.MessageLength=ntohs(44);
	obj.Header.TraderId=ntohl(UserId);
	obj.LastUpdatePortfolioTime=0;

    obj.SendPF.Length=ntohs(70);
    obj.SendPF.Messae_Count=ntohs(1);
    obj.SendPF.SeqNumber=ntohl(2);

    md5_byte_t check1[16];

    md5CheckSum((const unsigned char*)&obj+24,46,check1);
    memcpy(&obj.SendPF.Checksum,&check1,16);

    return obj;

}

SignOutRequest_2320 Packets:: Fun_SignOut_2320(short UserId, int sequanceNumber)
{
	SignOutRequest_2320 SignOut;
	memset(&SignOut, 0, sizeof(SignOutRequest_2320));

	SignOut.Header.TransactionCode = ntohs((short)2320);
	SignOut.Header.LogTime = ntohl(0);
	memset(SignOut.Header.AlphaChar, ' ', 2);
	SignOut.Header.TraderId = ntohl(UserId);
	SignOut.Header.ErrorCode = ntohs((short)0);
	memset(SignOut.Header.TimeStamp, ' ', 8);
	memset(SignOut.Header.TimeStamp1, ' ', 8);
	memset(SignOut.Header.TimeStamp2, ' ', 8);
	SignOut.Header.MessageLength = ntohs(40);

	SignOut.SendPF.Length = ntohs(sizeof(SignOutRequest_2320));
	SignOut.SendPF.Messae_Count = ntohs(1);
	SignOut.SendPF.SeqNumber = ntohl(3);
	md5_byte_t check[16];
	md5CheckSum((const unsigned char*)&SignOut + 24, sizeof(SignOutRequest_2320)-24, check);
	memcpy(&SignOut.SendPF.Checksum, &check, 16);
	return SignOut;
}

