#ifndef STRUCTURE_H_INCLUDED
#define STRUCTURE_H_INCLUDED
#include <string>

using namespace std;

#ifndef STRUCTURE_H
#define STRUCTURE_H
#define MAXBUF 1024
 enum MessageType
	{
		eORDER='A',
		eFOPAIR='F',
		eFOPAIRDIFF='G',
		eFOPAIRUNSUB='H',
		eIOCPAIR='I',
		eIOCPAIRUNSUB='J',
		eIOCPAIRDIFF='K',
		eCANCELALL='R'
	};
 struct strFOPAIR
	{
		 int PORTFOLIONAME;
		 int TokenNear;
		 int TokenFar;
	};

class Structure
{
    public:
        Structure();
        virtual ~Structure();
    protected:
    private:
};

struct config
{
   char multicastAddress[MAXBUF];
   char LanIp[MAXBUF];
   char port[MAXBUF];

};
 struct ST_SEC_ELIGIBILITY_PER_MARKET2// [4]
        {
            int Security_Status;
            char  res1;
            char Eligibility;
            char Reserved[2];// [2]
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
#pragma pack(1)
		struct Dep
		{
		int Price;
		int Quantity;
		};

/*#pragma pack(1)
	typedef	struct FinalPrice
		{
            int MAXBID;
            int MINASK;
            int BIDQ;
            int ASKQ;
		};*/


 struct FinalPrice
   	{
        int sub_token;
    	 int Token;

 		 int MAXBID;
 		 int MINASK;
 		 int LTP;

   	};

		#pragma pack(1)
 		struct STREAM_HEADER
		{
			 short msg_len;
			 short stream_id;
			 int  seq_no;
                         //char Message_Type;
		};

		#pragma pack(1)
		 struct Master_Data_Header
		{
			 STREAM_HEADER Global_Header;
			 char	Message_Type;
			 int Token_Count;

		};

		#pragma pack(1)
		 struct Contract_Information_Message
		{
			 STREAM_HEADER Global_Header;
			 char Message_Type;
			 short Stream_ID;
			 int Token_Number;
			 char Instrument[6];
			 char Symbol[10];
			 int Expiry_Date;
			 int Strike_Price;
             		char Option_Type[2];

		};

		#pragma pack(1)
		 struct Spread_Contract_Information
		{
			 STREAM_HEADER Global_Header;
			 char	Message_Type;
			 short stream_id;
			 int Tokken1;
			 int Tokken2;

		};

		#pragma pack(1)
		 struct Trailer_Message
		{
			 STREAM_HEADER Global_Header;
			 char	Message_Type;
			 int Token_Count;
       		 };

		#pragma pack(1)
		  struct Order_Message
		{
			 STREAM_HEADER Global_Header;
			 char Message_Type;
			 double Timestamp;
			 double Order_Id;
			 int Token;
			 char Order_Type;
			 int Price;
			 int Quantity;
		};

		#pragma pack(1)
			typedef	struct PriceArrray
		{
            int sub_token;
            int Token;
            int LTP;
            FinalPrice _FP[4];
		};

		#pragma pack(1)
		 struct Trade_Message
		{
			 STREAM_HEADER Global_Header;
			 char Message_Type;
			 double Timestamp;
			 double Buyorder_Id;
			 double Selloder_Id;
			 int Token;
			 int Trade_Price;
			 int Trade_Quantity;
		};
		#pragma pack(1)
        	struct Spread_Trade_Message
		{
			 STREAM_HEADER Global_Header;
			 char Message_Type;
			 double Timestamp;
			 double Buy_Order_ID;
			 double Sell_Order_ID;
			 int Token;
			 int Trade_Price;
			 int Quantity;

		};

		# pragma pack(push, 1)
struct FOPAIRLEG2
    {
         int InstanceGroup;
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
    };

#pragma pack(pop)

#endif // STRUCTURE_H



#endif // STRUCTURE_H_INCLUDED
