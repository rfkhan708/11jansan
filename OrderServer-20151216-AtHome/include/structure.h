#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED

#pragma pack(2)
struct PacketFormat
{
	short Length;
	int SeqNumber;
	short ResrvSeqNumber;
	char Checksum[16];
	short Messae_Count;
};

#pragma pack(2)
struct MESSAGE_HEADER
{
	//Packet Length: 40 bytes
	short TransactionCode;
	int LogTime;
	char AlphaChar[2];
	int TraderId;
	short ErrorCode;
	char TimeStamp[8];
	char TimeStamp1[8];
	char TimeStamp2[8];
	short MessageLength;
};

struct MS_ERROR_RESPONSE
{
    MESSAGE_HEADER Header;
    char Key[14];
    char ErrorMessage[128];

};


#pragma pack(2)
struct ST_BROKER_ELIGIBILITY_PER_MKT
{
	/*char    Reserved : 4;
	char    AuctionMarket : 1;
	char    SpotMarket : 1;
	char    OddlotMarket : 1;
	char    NormalMarket : 1;
	char    Reserved1;
	*/

	char reserved_1;
	char reserved_2[1];

};

struct ST_ORDER_FLAGS
{
	char    AON : 1;
	char    IOC : 1;
	char    GTC : 1;
	char    Day : 1;
	char    MIT : 1;
	char    SL : 1;
	char    Market : 1;
	char    ATO : 1;
	char    Reserved : 3;
	char    Frozen : 1;
	char    Modified : 1;
	char    Traded : 1;
	char    MatchedInd : 1;
	char    MF : 1;
};

struct ADDITIONAL_ORDER_FLAGS
{
	char    Reserved : 1;
	char    COL : 1;
	char    Reserved1 : 6;
};


#pragma pack(2)
struct MS_SIGN_ON_2300
{
	//Packet Length: 214 bytes
	PacketFormat SendPF;
	struct MESSAGE_HEADER Header;
	int    UserId;
	char   Password[8];
	char   NewPassword[8];
	char   TraderName[26];
	int    LastPasswordChangeDate;
	char   BrokerId[5];
	char   Reserved1;
	short  BranchId;
	int    VersionNumber;
	int    Batch2StartTime;
	char   HostSwitchContext;
	char   Colour[50];
	char   Reserved2;
	short  UserType;
	double SequenceNumber;
	char   WsClassName[14];
	char   BrokerStatus;
	char   ShowIndex;
	ST_BROKER_ELIGIBILITY_PER_MKT StMkt;
	short  MemberType;
	char   ClearingStatus;
	char   BrokerName[25];
};
#pragma pack(2)
struct MS_SIGN_ON_OUT_2301
{
	//Packet Length: 214 bytes
	MESSAGE_HEADER Header;
	int     UserId;
	char    Password[8];
	char    NewPassword[8];
	char    TraderName[26];
	int     LastPasswordChangeDate;
	char    BrokerId[5];
	char    Reserved1;
	short   BranchId;
	int     VersionNumber;
	int     EndTime;
	char    Reserved2;
	//int     Batch2StartTime;
	//char    HostSwitchContext;
	char    Colour[50];
	char    Reserved3;
	short   UserType;
	double  SequenceNumber;
	char    WsClassName[14];//Reserved
	char    BrokerStatus;
	char    ShowIndex;

	ST_BROKER_ELIGIBILITY_PER_MKT StMkt;

	short   MemberType;
	char    ClearingStatus;
	char    BrokerName[25];
};

#pragma pack(2)
struct MS_SYSTEM_INFO_REQ_1600
{
	//Packet Length: 44 bytes
	PacketFormat SendPF;
	MESSAGE_HEADER Header;
	int LastUpdatePortfolioTime;

};

#pragma pack(2)
struct SignOutRequest_2320
{
	PacketFormat SendPF;
	MESSAGE_HEADER Header;
};

#pragma pack(2)
struct SignOutResponse_2321
{
	MESSAGE_HEADER Header;
	int UserId;
	char reserved[145];
};

#pragma pack(2)
struct Invitation_Msg
{
	short TransactionCode;
	short Invitation_count;
};

#pragma pack(2)
struct RecvPacket
{
	int Sequence_number;
	short Resrv_Sequencenumber;
	char Checksum[16];
	short Messae_Count;
	char Data[512];

};

#pragma pack(2)
struct ST_MARKET_STATUS
{
	short  Normal;
	short  Oddlot;
	short  Spot;
	short  Auction;
};

#pragma pack(2)
struct  ST_STOCK_ELIGIBLE_INDICATORS
{
	char Reserved1;
	char Reserved2;
};

#pragma pack(2)
struct MS_SYSTEM_INFO_DATA
{
	PacketFormat pf;
	MESSAGE_HEADER obj;
	ST_MARKET_STATUS _st_obj;
	ST_MARKET_STATUS _st_EX_obj;
	ST_MARKET_STATUS _st_PL_obj;
	char    UpdatePortfolio;
	int    MarketIndex;
	short   DefaultSettlementPeriod_N;
	short   DefaultSettlementPeriod_S;
	short   DefaultSettlementPeriod_A;
	short   CompetitorPeriod;
	short   SolicitorPeriod;
	short   WarningPercent;
	short   VolumeFreezePercent;
	short     SnapQuoteTime;
	char      Reserved[2];
	int      BoardLotQuantity;
	int      TickSize;
	short     MaximumGtcDays;
	ST_STOCK_ELIGIBLE_INDICATORS _obj_ST_Stock;
	short     DisclosedQuantityPercentAllowed;
	int      RiskFreeInterestRate;
};

struct CONTRACT_DESC_TR
{
	char    InstrumentName[6];
	char    Symbol[10];
	int     ExpiryDate;
	int     StrikePrice;
	char    OptionType[2];
};

#pragma pack(2)
struct MS_OE_REQUEST_TR_20000
{
	//Packet Length: 110 bytes
	short   TransactionCode;
	int UserId;
	short   ReasonCode;
	int     TokenNo;

	CONTRACT_DESC_TR    Contract;

	char    AccountNumber[10];
	short   BookType;
	short   BuySellIndicator;
	int     DisclosedVolume;
	int     Volume;
	int     Price;
	int     GoodTillDate;

	ST_ORDER_FLAGS StOrderFlag;

	short   BranchId;
	int     TraderId;
	char    BrokerId[5];
	char    OpenClose;
	char    Settlor[12];
	short   ProClientIndicator;

	ADDITIONAL_ORDER_FLAGS AdditionalOrdFlag;
	int     filler;
	double  NnfField;

};
struct MS_OM_REQUEST_TR1
{
	/*
	Packet Length: 138 bytes
	Transaction Code:
	ORDER_MOD_IN_TR (20040)
	ORDER_CANCEL_IN_TR (20070)
	ORDER_QUICK_CANCEL_IN_TR (20060)
	*/

	short   TransactionCode;
	int     UserId;
	char    ModifiedCancelledBy;
	int     TokenNo;
	CONTRACT_DESC_TR Contract;

	double  OrderNumber;
	char    AccountNumber[10];
	short   BookType;
	short   BuySellIndicator;
	int     DisclosedVolume;
	int     DisclosedVolumeRemaining;
	int     TotalVolumeRemaining;
	int     Volume;
	int     VolumeFilledToday;
	int     Price;
	int     GoodTillDate;
	int     EntryDateTime;
	int     LastModified;
	ST_ORDER_FLAGS StOrderFlag;

	short   BranchId;
	int     TraderId;
	char    BrokerId[5];
	char    OpenClose;
	char    Settlor[12];
	short   ProClientIndicator;

	ADDITIONAL_ORDER_FLAGS AdditionalOrdFlag;
	int filler;
	double NnfField;
};



#endif // STRUCTURE_H_INCLUDED
