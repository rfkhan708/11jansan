#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED
#include <string>

#define SIZE_TOTAL_INSTNAME 6
#define SIZE_TOTAL_SYMBOL 10

using namespace std;

namespace Enums
{

 enum BUYSELL
	{
		BUY =1,
		SELL=2
	};

  enum _orderType
	{
		LIMIT=0,
		MARKET=1
	};

  enum  OrderStatus
{
	NONE=0,
	PENDING =1,
	REPLACED=2,
	OPEN = 3,
	CANCEL=4,
	REJECTED=5,
	TRADE=6
};

enum OrderType
{
	DAY,
	DAYMOD,
	DAYSL,
	DAYSLMOD,
	IOC

};

enum DepthBest
{
    Best=1,
    Depth=2,
    _IOC=3
};

/* enum MessageType
	{
		eORDER='A',
		eFOPAIR='F',
		eFOPAIRDIFF='G',
		eFOPAIRUNSUB='H',
		eIOCPAIR='I',
		eIOCPAIRUNSUB='J',
		eIOCPAIRDIFF='K',
		eCANCELALL='R',
		eSTOPALL='S'
	};*/


     struct FinalPrice
   	{
        int subToken;
    	 int Token;

 		 int MAXBID;
 		 int MINASK;
 		 int LTP;

   	};


     struct NFToken
	{
	public:
		int NEARTOKEN;
		int FARTOKEN;
		int TOKENTHREE;
		int PFNUMBER;
		int BLQ;
		bool IsSubscribe;

	};

# pragma pack(push, 1)
   struct strFOPAIRIOC
	{
         int INSTANCEGROUP;
		 int PORTFOLIONAME;
		 int TokenNear;
		 int TokenFar;
		 int TokenThree;
		 short TokenNearSide;
		 short TokenFarSide;
		 short TokenThreeSide;
		 int TokenNearRatio;
		 int TokenFarRatio;
		 int TokenThreeRatio;
		 int CreateQTY;
		 int ReverseQTY;
		 int TokenNearLtp;
		 int TokenFarLtp;
		 int TokenThreeLtp;
	};
	#pragma pack(pop)


 	struct OrderDetails
 		{
 			int Token;
 			int Token2;
 			BUYSELL side;
 			BUYSELL SIDE2;
 			long OrderNumber;
 			int Price;
 			int Price2;
 			int Qty;
 			int Qty2;
 			int ReplaceCount;
 			int OrderType;
 			int orderstat;
 			int TotalTraded;
 		//	string FarOrNear;
 		};

# pragma pack(push, 1)
 struct FOPAIRDIFFIOC
	{
		 int PORTFOLIONAME;
		 int TokenNear;
		 int TokenFar;
		 int TokenThree;
		 double BNSFDIFF;
		 double BFSNDIFF;
		 int BNSFMNQ;
		 int BFSNMNQ;
		 int BNSFMXQ;
		 int BFSNMXQ;
		 int Divisor;
		 int TickCount;
		 short Depth_Best;
		 strFOPAIRIOC _FOPair;
	};
#pragma pack(pop)

////================================ ExChange Struct
struct St_OrderFlag
{
    char AON:1;
    char IOC:1;
    char GTC:1;
    char DAY:1;
    char MIT:1;
    char SL:1;
    char MARKET:1;
    char ATO:1;
    char Reserved:3;
    char Frozen:1;
    char Modified:1;
    char Traded:1;
    char MatchedInd:1;
    char MF:1;
};


# pragma pack(push, 2)
 struct MS_OE_REQUEST_TR
        {

            short Length;
            int SequenceNumber;
            char ResrvSequenceNumber[2];
            char CheckSum[16];
            short MsgCount;

            short TransactionCode;
            int UserId;
            short ReasonCode;
            int TokenNo;
            char InstrumentName[SIZE_TOTAL_INSTNAME];
            char Symbol[SIZE_TOTAL_SYMBOL];
            int ExpiryDate;
            int StrikePrice;
            char OptionType[2];
            char AccountNumber[10];
            short BookType;
            short Buy_SellIndicator;
            int DisclosedVolume;
            int Volume;
            int Price;
            int GoodTillDate;
            char FlagIn;
            char FlagOut;

            short BranchId;
            int TraderId;
            char BrokerId[5];
            char Open_Close;
            char Settlor[12];
            short Pro_ClientIndicator;
            char Reserved1;
            int filler;
            double nnffield;

        };
#pragma pack(pop)

# pragma pack(push, 2)

  struct MS_OM_REQUEST_TR
{

       short Length:16;
       long SequenceNumber:32;
       char ResrvSequenceNumber[2];
       char CheckSum[16];
       short MsgCount:16;
       short TransactionCode;

       int UserId;
       char Modified_CancelledBy;
       int TokenNo;
       char InstrumentName[6];
       char Symbol[10];
       int ExpiryDate;
       int StrikePrice;
       char OptionType[2];
       double OrderNumber;

      char AccountNumber[10];
      short BookType;
      short Buy_SellIndicator;
      int DisclosedVolume;
      int DisclosedVolumeRemaining;
      int TotalVolumeRemaining;
      int Volume;
      int VolumeFilledToday;
      int Price;
      int GoodTillDate;
      int EntryDateTime;
      int LastModified;

    /*   char AON:1;
        char IOC:1;
        char GTC:1;
        char DAY:1;
        char MIT:1;
        char SL:1;
        char MARKET:1;
        char ATO:1;
        char Reserved:3;
        char Frozen:1;
        char Modified:1;
        char Traded:1;
        char MatchedInd:1;
        char MF:1;  */

     char FlagIn;
     char FlagOut;
     short BranchId;
     int TraderId;
     char BrokerId[5];
     char Open_Close;
     char Settlor[12];
     short Pro_ClientIndicator;
     char Reservedadd;
     int filler;
     double nnffield;
};


#pragma pack(pop)


# pragma pack(push, 1)
 struct MS_TRADE_CONFIRM_TR
	{
		 short TransactionCode;
		 int LogTime;
		 int TraderId;
		 double	Timestamp;
         double Timestamp1;
		 double	Timestamp2;
		 double ResponseOrderNumber;
		 char BrokerId[5];
		 char Reserved;
		 char AccountNumber[10];
		 short Buy_SellIndicator;
		 int OriginalVolume;
		 int DisclosedVolume;
		 int RemainingVolume;
		 int DisclosedVolumeRemaining;
		 int Price;
		 char AON:1;
        char IOC:1;
        char GTC:1;
        char DAY:1;
        char MIT:1;
        char SL:1;
        char MARKET:1;
        char ATO:1;
        char Reservedj:3;
        char Frozen:1;
        char Modified:1;
        char Traded:1;
        char MatchedInd:1;
        char MF:1;


		 int GoodTillDate;
		 int FillNumber;
		 int FillQuantity;
		 int FillPrice;
		 int VolumeFilledToday;
		 char ActivityType[2];
		 int ActivityTime;
		 int Token;

		 char InstrumentName[6];
        char Symbol[10];
        int ExpiryDate;
        int StrikePrice;
        char OptionType[2];

		 char OpenClose;
		 char BookType;
		 char Participant[12];
		 char Reserved1;
        };
#pragma pack(pop)


# pragma pack(push, 2)
struct MS_OE_RESPONSE_TR
{
     short TransactionCode;
     int LogTime;
     int UserId;
     short ErrorCode;
     double TimeStamp1;//20
     char TimeStamp2;
     char Modified_CancelledBy;//22
     short ReasonCode;
     int TokenNo;//28

	 char InstrumentName[6];
	 char Symbol[10];
	 int ExpiryDate;
	 int StrikePrice;
	 char OptionType[2];

     char CloseoutFlag;
     double OrderNumber;
     char AccountNumber[10];
     short BookType;
     short Buy_SellIndicator;
     int DisclosedVolume;
     int DisclosedVolumeRemaining;
     int TotalVolumeRemaining;
     int Volume;
     int VolumeFilledToday;
     int Price;
     int GoodTillDate;
     int EntryDateTime;
     int LastModified;

        char AON:1;
        char IOC:1;
        char GTC:1;
        char DAY:1;
        char MIT:1;
        char SL:1;
        char MARKET:1;
        char ATO:1;
        char Reserved:3;
        char Frozen:1;
        char Modified:1;
        char Traded:1;
        char MatchedInd:1;
        char MF:1;

     short BranchId;
     int TraderId;
     char BrokerId[5];
     char Open_Close;
     char Settlor[12];
     short Pro_ClientIndicator;
	 char Reserved1l;
     int filler;
     double nnffield;
};
#pragma pack(pop)


# pragma pack(push, 2)
 struct MS_OE_REQUEST
	{
    short TransactionCode;
	 int LogTime;
	 char AlphaChar[2];
	 int TraderId;
	 short ErrorCode;
     char Timestamp[8];
	 char TimeStamp1[8];
	 char TimeStamp2[8];
	 short MessageLength;

		 char	ParticipantType;
		 char Reserved1;
		 short CompetitorPeriod;
		 short SolicitorPeriod;
		 char	Modified_CancelledBy;
		 char Reserved2;
		 short ReasonCode;
		 int Reserved3;
		 int TokenNo;

        char InstrumentName[6];
        char Symbol[10];
        int ExpiryDate;
        int StrikePrice;
        char OptionType[2];
        short CALEVEL;
		 char CounterPartyBrokerId[5];
		 char Reserved4;
		 short Reserved5;
		 char	CloseoutFlag;
		 char Reserved6;
		 short OrderType;
		 double OrderNumber;

		 char AccountNumber[10];
		 short BookType;
		 short Buy_SellIndicator;
		 int DisclosedVolume;
		 int DisclosedVolumeRemaining;
		 int TotalVolumeRemaining;
		 int Volume;
		 int VolumeFilledToday;
		 int Price;
		 int TriggerPrice;
		 int GoodTillDate;
		 int EntryDateTime;
		 int MinimumFill_AONVolumel;
		 int LastModified;

		  char AON:1;
        char IOC:1;
        char GTC:1;
        char DAY:1;
        char MIT:1;
        char SL:1;
        char MARKET:1;
        char ATO:1;
        char Reserved:3;
        char Frozen:1;
        char Modified:1;
        char Traded:1;
        char MatchedInd:1;
        char MF:1;

		 short BranchId;
		 int Traderid;
		char BrokerId[5];

		 char cOrdFiller[24];
		 char	Open_Close;

		 char Settlor[12];
		 short Pro_ClientIndicator;
		 short SettlementPeriod;
		  char Reservedadd;
		 char GiveupFlag1;//
		 short filler;
		 char filler1;
		 char filler2;

		 double nnffield;
		 double mkt_replay;

	};
#pragma pack(pop)

//=============================================================

struct NearMonthPacket
 	 			{
 		MS_OE_REQUEST_TR NEARMONTHMARKET;
 		MS_OE_REQUEST_TR NEARMONTHMARKET2;
 		//MS_OE_REQUEST_TR NEARMONTHSELLMARKET;

        //MS_OE_REQUEST_TR THIRDMONTHYMARKET;
 		//MS_OE_REQUEST_TR THIRDMONTHSELLMARKET;

 		MS_OE_REQUEST_TR FARMONTH;
 		//MS_OE_REQUEST_TR FARMONTHSELL;
 		MS_OM_REQUEST_TR FARMONTHMODBUY;
 		MS_OM_REQUEST_TR FARMONTHMODSELL;
 	 	};


    struct ST_SEC_ELIGIBILITY_PER_MARKET2// [4]
	{
		int Security_Status;
		char  res1;
		char Eligibility;
		char Reserved[2];// [2]
	};

# pragma pack(push, 2)
    struct ContractDetails
    {
        int Token;
        int AssetToken;
        char InstrumentName[6];
        char Symbol[10];
        char Series[2];
        /*string InstrumentName;
        string Symbol;
        string Series;*/
        int ExpiryDate;
        int StrikePrice;
        char OptionType[2];
        //string OptionType;
        int BoardLotQuantity;

        bool operator<(const ContractDetails& x)const
        {
            if(Token<x.Token)
                true;
            else
                false;
        }

    };
#pragma pack(pop)

struct CheckStruct
{
    int Token;
    int BoardLotQuantity;
    };



    struct Contract_File
    {

    string NEATFO;
	char res1;
	string VersionNumber;
	char res2;

	int Token;
	char res_1;
	int AssetToken;
	char  res_2;

	string InstrumentName;// [6]
	char res3;
	string Symbol;// [10]
	char res4;
	string Series;// [2]
	char resl[2]; //[2]
	int ExpiryDate;// (in seconds from January 1, 1980)
	char res5;
	int StrikePrice;
	char res6;
	string OptionType;// [2]
	char res7;
	string Category;// [1]
	char res8;
	int CALevel;
	char res9;
	char ReservedIdentifier[1];//[1]
	char res10;
	int PermittedToTrade;
	int res11;
	int IssueRate;
	ST_SEC_ELIGIBILITY_PER_MARKET2 obj_st_sec[4];
	char res12;
	int IssueStartDate;
	char res13;
	int InterestPaymentDate;
	char res14;
	int Issue_Maturity_Date;
	char res15;
	int  MarginPercentage;
	char res16;
	int MinimumLotQuantity;
	char res17;
	int BoardLotQuantity;
	char res18;
	int TickSize;
	char res19;
	double IssuedCapital;
	char res20;
	int FreezeQuantity;
	char res21;
	int WarningQuantity;
	char res22;
	int ListingDate;
	char res23;
	int ExpulsionDate;
	char res24;
	int ReadmissionDate;
	char res25;
	int RecordDate;
	char res26;
	int NoDeliveryStartDate;
	char res27;
	int NoDeliveryEndDate;
	char res28;
	int LowPriceRange;
	char res29;
	int HighPriceRange;
	char res30;
	int ExDate;
	char res31;
	int BookClosureStartDate;
	char res32;
	int BookClosureEndDate;
	char res33;
	int LocalLDBUpdateDateTime;
	char res34;
	int ExerciseStartDate;
	char res35;
	int ExerciseEndDate;
	char res36;
	short TickerSelection;
	char res37;
	long OldTokenNumber;
	char res38;
	string CreditRating;// [12]
	char res39;
	string Name;// [25]
	char res40;
	string EGMAGM;
	char res41;
	char InterestDivident;
	char res42;
	char RightsBonus;
	char res43;
	char MFAON;
	char res44;
	string Remarks;// [24]
	char res45;
	char ExStyle;
	char res46;
	char ExAllowed;
	char res47;
	char ExRejectionAllowed;
	char res48;
	char PlAllowed;
	char res49;
	char CheckSum;
	char  res50;
	char IsCOrporateAdjusted;
	char  res51;
	string SymbolForAsset;// [10]
	char res52;
	string InstrumentOfAsset;// [6]
	char res53;
	char BasePrice;
	char res54;
	char DeleteFlag;
	};
struct TokenPartnerDetails
	{
		 Contract_File CF;
		 FinalPrice FP;
		 int PartnerLeg:32;
		 int PortfolioName:32;
		 bool PrimeToken;//
		void *STGDetail;

	};

enum Exchange
{
    FO=2,
    EQ=4,
    CURR=6,
    MCX=8,
    MCX_SX=9
};
///===============


    enum STGTYPE
	{
        FOFO =1,
		TWOLEGOPT =2
	};

    enum MessageType
	{
		eORDER=65,
		eORDERRej=66,
		eLOGIN=67,
		eMESSAGE=68,
		eHEARTBEAT=69,
		eFOPAIR=70,
		eFOPAIRDIFF=71,
		eFOPAIRUNSUB=72,
		eIOCPAIR=73,
		eIOCPAIRUNSUB=74,
		eIOCPAIRDIFF=75,
		eA_LOGIN=76,	//LogIn of Admin
		eA_UPDATE=77,
		eA_UPDATEClient=78,
		eA_UPDATEOrder=79,
		eA_UPDATEComplete=80,
		eA_MESSAGE=81,
		eCANCELALL=82,
		eSTOP_ALL=83,
        eDelete = 84,
        eManualOrd = 85


	};

    enum LogInStatus
	{
		LogIn=201,
		PwdError=202,
		UserAlreadyLogIn=203,
		PwdExpire=204,
		LogOutStatus=205,
		LogOutbyAdmin=206,
		LogOutNoheartbeat=207,
		LogOut=208,
		UserAlreadyLogOut=209,
	};

	enum BP
	{
		PROD = 1,
		BASE =2
	};
//#pragma pack(2)
# pragma pack(push, 2)
struct PFHolder
{
    short PF;
    int CID;
    long HashKey;
    int _size;
    double OrderNo;
    long SubscriptionTag;
    short side;
    //char* buff;
    MS_OE_REQUEST_TR _oetr;
    MS_OM_REQUEST_TR _omtr;

    bool operator<(const PFHolder& x)const
    {
        if(PF<x.PF)
            true;
        else
            false;
    }

};
#pragma pack(pop)


 struct FindByToken {
    const int Token;
    const int Price;
    const int Qty;
    const short Side;
    FindByToken(const int& Token, const int& Price, const int& Qty, const short& Side) :
    Token(Token),Price(Price),Qty(Qty),Side(Side) {}
    bool operator()(const PFHolder& j) const {
        return j._oetr.TokenNo == Token && j._oetr.Price== Price && j._oetr.Volume == Qty && j._oetr.Buy_SellIndicator == Side;


    }
    };

#pragma pack(2)
struct StructHash
{
    int Token1;
    int Token2;
    int Token3;
    int Price1;
    int Price2;
    int Price3;
    int Qty1;
    int Qty2;
    int Qty3;
    short side1;
    short side2;
    short side3;
};

#pragma pack(2)
    struct InHeader
    {
        short TransectionCode;
		int ClintId;
    };
#pragma pack(2)
    struct  C_SignIn
	{

        short TransectionCode;
		int ClintId;
		char Password[9];
		short Status;
		STGTYPE StgType;
	};


struct ClientUpdateMsg
    {
         short TransectionCode;
		 int ClintId;
		 short PF;
         char buffer[1024];
    };

#pragma pack(2)
struct PFUpdateMsg
    {
         long TransectionCode;
		 //int ClintId;
         char buffer[1024];
    };

struct SubscriptionTagDets
{
    short ecode;
    int CID;
    short PFNumber;
};
	struct C_Contract_Desc
	{

        char InstrumentName[7];
		char	Symbol[11];
        int ExpiryDate;
		int StrikePrice;
		char OptionType[3];
		short CALevel;
	};

    struct  C_LotIN
	{
		 short TransectionCode;
		 double OrderNo;
		 int ClintId;
		 int TokenNo;
		 C_Contract_Desc contract_obj;
		 char AccountNumber[11];
		 short Buy_SellIndicator;
		 int DisclosedVolume;
		 int Volume;
		 int Price;
		 int TriggerPrice;
		 char Open_Close;
		 short Reasoncode;
	};

    struct  HeartBeatInfo//size not match
	{
		 int tapStatus;
		 int dataStatus;
		 short tapQueue;
		 short ClientQueue;
	};

    struct  C_OrderReject
	{
		 double OrderNo;
		 short Reasoncode;
	};


    struct IniInputs
    {
        int SECTION;
        string CONTRACTFILEPATH;
        string DATANANOPATH;
        int BROKERID;
        int BRANCHID;
        int USERID;
        int VERSION;
        string SERVERIP;
        int SERVERPORT;
    };

}


#endif // ENUMS_H_INCLUDED
