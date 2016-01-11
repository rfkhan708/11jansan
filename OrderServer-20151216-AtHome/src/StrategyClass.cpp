#include "StrategyClass.h"
#include "string.h"

#include "StaticClass.h"
#include <arpa/inet.h>
#include "../PFMap.h"
//#include <boost/thread/mutex.hpp>
//#include<boost/thread/condition_variable.hpp>

#include <pthread.h>
#include <math.h>

using namespace std;




StrategyClass::StrategyClass()
{
    //ctor
            BuyFarStatus= (OrderStatus)NONE;
            SellFarStatus = (OrderStatus)NONE;

            SellTradeCounter=0;
            BuyTradeCounter=0;

            SellTradeCounterleg2 =0;
            SellTradeCounterleg3 =0;

            BuyTradeCounterleg2 =0;
            BuyTradeCounterleg3 =0;




            memset(&FarFP,0,sizeof(FinalPrice));
            memset(&NearFP,0,sizeof(FinalPrice));
            memset(&ThirdFP,0,sizeof(FinalPrice));

            CancelCode=ntohs(20070);
            ModificationCode=ntohs(20040);

            memset(&_pfBuy,0,sizeof(PFHolder));
            memset(&_pfSell,0,sizeof(PFHolder));


            memset(&_pfMKTBuy,0,sizeof(PFHolder));
            memset(&_pfMKTSell,0,sizeof(PFHolder));


            memset(&_pfMKTBuy2,0,sizeof(PFHolder));
            memset(&_pfMKTSell2,0,sizeof(PFHolder));


            _BNSFMNQ=0;
            _BFSNMNQ =0;
            _BNSFMXQ =0;
            _BFSNMXQ =0;
            BFSNDIFF =0;
            BNSFDIFF =0;
            InstanceGroup=0;

            FarToken =0;
            NearToken =0;
            TokenThree =0;

        count_mutex = PTHREAD_MUTEX_INITIALIZER;
        initbuy_mutex = PTHREAD_MUTEX_INITIALIZER;
        initsell_mutex = PTHREAD_MUTEX_INITIALIZER;

        memset(&FarFP,0,sizeof(FinalPrice));
        memset(&NearFP,0,sizeof(FinalPrice));
        memset(&ThirdFP,0,sizeof(FinalPrice));
        BLQ=0;

        Token1MinCreateRatio = 0;
                Token2MinCreateRatio = 0;
                Token3MinCreateRatio = 0;

                Token1MinReverseRatio = 0;
                Token2MinReverseRatio = 0;
                Token3MinReverseRatio = 0;

                Token1MaxCreateRatio = 0;
                Token2MaxCreateRatio = 0;
                Token3MaxCreateRatio = 0;

                Token1MaxReverseRatio = 0;
                Token2MaxReverseRatio = 0;
                Token3MaxReverseRatio = 0;
                _CreateRemainingQty=0;
                _ReverseRemainingQty =0 ;
                LoopCount=0;

 iLeg1BestCreatePrice =0;
 iLeg1BestReversePrice=0;
 iLeg2MktCreatePrice=0;
 iLeg2MktReversePrice=0;
 iLeg3MktCreatePrice=0;
 iLeg3MktReversePrice=0;


}

StrategyClass::~StrategyClass()
{
    cout << " Destructor called PF "<< PFNumber<<endl;
    //dtor
}

long long StrategyClass::concat(long long x, long long y)
{
    long long temp = y;
    while (y != 0) {
        x *= 10;
        y /= 10;
    }
    return x + temp;
}

 void StrategyClass::OnSubscriptionEventHandler(strFOPAIRIOC* _FOpairObj)
        {
            FOpairObj = *_FOpairObj;
           // strFOPAIR _FOpairObj;
           // memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));
            FarFP.Token = _FOpairObj->TokenFar;
            NearFP.Token = _FOpairObj->TokenNear;
            ThirdFP.Token = _FOpairObj->TokenThree;
            InstanceGroup = _FOpairObj->INSTANCEGROUP;

            Token1Ratio = _FOpairObj->TokenFarRatio;
            Token2Ratio = _FOpairObj->TokenNearRatio;
            Token3Ratio = _FOpairObj->TokenThreeRatio;

            Leg2HitRatio = (float)Token2Ratio / (float)Token1Ratio;
            Leg3HitRatio =(float) Token3Ratio / (float) Token1Ratio;

            FarToken =FarFP.Token;
            NearToken = NearFP.Token;
            TokenThree = ThirdFP.Token;

            PFNumber = _FOpairObj->PORTFOLIONAME;


           // cout << "PFNumber "<< PFNumber <<" New class OnSub PF " << PFNumber << " Far " << FarFP.Token << " Near "<< NearFP.Token<<endl<<endl;



             _pfBuy.PF = PFNumber;
            _pfBuy.CID = ClientID;

             _pfSell.PF = PFNumber;
            _pfSell.CID = ClientID;



             _pfMKTBuy.PF = PFNumber;
            _pfMKTBuy.CID = ClientID;

             _pfMKTSell.PF = PFNumber;
            _pfMKTSell.CID = ClientID;




             _pfMKTBuy2.PF = PFNumber;
            _pfMKTBuy2.CID = ClientID;

             _pfMKTSell2.PF = PFNumber;
            _pfMKTSell2.CID = ClientID;


            long lng=concat((short)(MessageType)eORDER,_pfSell.CID);
            lng = concat(lng,(short)_pfSell.PF);

            _pfBuy.SubscriptionTag = lng;
            _pfSell.SubscriptionTag = lng;

            _pfMKTBuy.SubscriptionTag = lng;
            _pfMKTSell.SubscriptionTag = lng;

            _pfMKTBuy2.SubscriptionTag = lng;
            _pfMKTSell2.SubscriptionTag = lng;

            _BNSFMNQ=0;
            _BFSNMNQ =0;
            _BNSFMXQ =0;
            _BFSNMXQ =0;
            BFSNDIFF =0;
            BNSFDIFF =0;

            CancelBuyOrder = false;
	 	 	CancelSellOrder= false;

        }

        void StrategyClass::OnUnSubscriptionEventHandler(strFOPAIRIOC* _FOpairObj)
        {
            //strFOPAIR _FOpairObj;

	 	 	// memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));

            if(BuyFarStatus==OPEN || BuyFarStatus== REPLACED || BuyFarStatus == PENDING)
            {
                BuyFarStatus = PENDING;

                cout<<"StrategyClass OnUnSubscriptionEventHandler BUY "<<(BUYSELL)BUY<<" FarFP.Token ="<<FarFP.Token<<endl<<endl;
                ORDER_CANCEL_IN_TR ( FarFP.Token, CreateSide);
            }
            if(SellFarStatus == OPEN || SellFarStatus == REPLACED|| SellFarStatus == PENDING)
            {
                SellFarStatus = PENDING;


                cout<<"StrategyClass OnUnSubscriptionEventHandler SELL \t"<<(BUYSELL)SELL<<" FarFP.Token "<<FarFP.Token<<endl<<endl;
                ORDER_CANCEL_IN_TR ( FarFP.Token, ReverseSide);

            }


            _BNSFMNQ=0;
            _BFSNMNQ =0;
            _BNSFMXQ =0;
            _BFSNMXQ =0;
            BFSNDIFF =0;
            BNSFDIFF =0;

            BuyFarStatus=NONE;
            SellFarStatus=NONE;
            CancelBuyOrder = true;
            CancelSellOrder= true;

            cout << " Unsubscription recieved in Strategy for CID " << ClientID << " PF "<< PFNumber<<endl;
       // cout << "PFNumber "<< PFNumber <<" New class OnUnSub PF " << PFNumber << " Far " << _FOpairObj->TokenFar << " Near "<< _FOpairObj->TokenNear<<endl<<endl;
        }

    int StrategyClass::GetExpectedProdPrice(BUYSELL BS, FinalPrice FP, int Ratio,bool reverse = false)
	{

			int RetVal = 0;

			if (!reverse)
			{
				// THis case calculates the price to generate buy spread
				RetVal = BS== (BUYSELL)BUY ? (FP.MINASK*Ratio* -1) : (FP.MAXBID*Ratio);
			}
			else
			{
				// Here in case of sale actual stg with buy mode token will be sold just to make a complete trade
				RetVal = BS == (BUYSELL)BUY ? (FP.MAXBID*Ratio ) : (FP.MINASK*Ratio * -1);
			}
			return RetVal;

	}



    int StrategyClass::GetExpectedBidPrice(BUYSELL BS,FinalPrice FP,int Ratio,bool Create= false)
    {
        int RetVal=0;

        if(Create)
        {
            RetVal = BS == (BUYSELL)BUY ? (FP.MAXBID * Ratio * -1) : (FP.MINASK * Ratio );

        }
        else
        {
            RetVal = BS == (BUYSELL)BUY ? (FP.MINASK * Ratio ) :  (FP.MAXBID * Ratio * -1 );

        }

    }


    int StrategyClass::MultipleValue(BUYSELL BS)
    {
        return BS == BUY ? -1 : 1;
    }

    int StrategyClass::GetBestData(BUYSELL BS)
    {
        return BS== BUY ? FarFP.MAXBID + 5: FarFP.MINASK -5 ;
    }

    int StrategyClass::GetDepthPrice(BUYSELL BS, int DIFF,BUYSELL FirstLeg)
    {
        int x=0;///FirstLeg Price
        int y=GetMKTData(BS);///Second Leg Price
        int z = DIFF;///DIFF
        if(BS==SELL && FirstLeg ==BUY)
            x = y + abs(z);
        else if(BS==BUY && FirstLeg ==SELL)
            x = y + abs(z);
        else if(BS==BUY && FirstLeg ==BUY)
            x =  y * -1 + abs(z);
        else if(BS==SELL && FirstLeg ==SELL)
            x = y - z;
       // return  abs(GetMKTData(BS) + DIFF) ;
       return abs(x);
    }

    int StrategyClass::GetMKTData(BUYSELL BS)
    {
        return BS == BUY ? NearFP.MINASK  : NearFP.MAXBID ;
    }

    int StrategyClass::GetMKTDataLeg2(BUYSELL BS)
    {
        return BS == BUY ? NearFP.MINASK  : NearFP.MAXBID * -1 ;
    }


    int StrategyClass::GetMKTDataLeg3(BUYSELL BS)
    {
        return BS == BUY ? ThirdFP.MINASK  : ThirdFP.MAXBID * -1 ;
    }

    int StrategyClass::GetMKTDataPrime(BUYSELL BS)
    {
        return BS == BUY ? FarFP.MINASK  : FarFP.MAXBID ;
    }


    int StrategyClass::GetJoinData(BUYSELL BS)
    {
        return BS == BUY ? FarFP.MAXBID  : FarFP.MINASK ;
    }

    int StrategyClass::GetBestMktPrime(BUYSELL BS)
    {
        return BS == BUY ? -5  : 5 ;
    }




        void StrategyClass::OnDataEventHandler(FinalPrice _fp)
        {

           if(FarFP.Token==0 && NearFP.Token==0)
            return;

            pthread_mutex_trylock(&count_mutex);

           // LoopCount++;

            if(_fp.MAXBID > 0 && _fp.MINASK >0)
            {

                if(FarFP.Token== _fp.Token)
                {
                        FarFP = _fp;
                        iLeg1BestCreatePrice = CreateSide==BUY ? FarFP.MAXBID : FarFP.MINASK;
                        iLeg1BestReversePrice = ReverseSide==BUY ? FarFP.MAXBID : FarFP.MINASK;
                }
                else if(NearFP.Token == _fp.Token)
                {
                        NearFP = _fp;
                        iLeg2MktCreatePrice = CreateSideLeg2 == BUY ? NearFP.MINASK : NearFP.MAXBID;
                        iLeg2MktReversePrice = ReverseSideLeg2 == BUY ? NearFP.MINASK : NearFP.MAXBID;

                }
                else if(ThirdFP.Token == _fp.Token)
                {
                        ThirdFP = _fp;
                        iLeg3MktCreatePrice = CreateSideLeg3 == BUY ? ThirdFP.MINASK : ThirdFP.MAXBID;
                        iLeg3MktReversePrice = ReverseSideLeg3 == BUY ? ThirdFP.MINASK : ThirdFP.MAXBID;

                }
            }


            if(FarFP.MINASK > 0 && FarFP.MAXBID >0 && NearFP.MAXBID >0 && NearFP.MINASK >0 && BLQ>0 )//&&  FarFP.MAXBID < FarFP.MINASK && NearFP.MAXBID < NearFP.MINASK)
            {
                if(ThirdFP.Token >0 && ThirdFP.MINASK <= 0 && ThirdFP.MAXBID<=0)
                return;


                 secondbuyProd = iLeg2MktCreatePrice *  MultipleValue(CreateSideLeg2);// GetExpectedProdPrice((BUYSELL)FOpairObj.TokenNearSide, NearFP, FOpairObj.TokenNearRatio); // Market rate for Leg2

                 firstbuyProd = InstanceGroup == Best ? iLeg1BestCreatePrice * Token1Ratio * MultipleValue(CreateSide)
                                                        : iLeg1BestReversePrice * Token1Ratio * MultipleValue(CreateSide);

                 thirdbuyprod = iLeg3MktCreatePrice * MultipleValue(CreateSideLeg3)  ;//ThirdFP.Token>0?GetExpectedProdPrice((BUYSELL)FOpairObj.TokenThreeSide, ThirdFP, FOpairObj.TokenThreeRatio):0;


                secondsellProd = iLeg2MktReversePrice *  MultipleValue(ReverseSideLeg2) ;//GetExpectedProdPrice((BUYSELL)FOpairObj.TokenNearSide,NearFP , FOpairObj.TokenNearRatio,true); //Market rate for leg2

                firstsellProd = InstanceGroup == Best ? iLeg1BestReversePrice * Token1Ratio * MultipleValue(ReverseSide):
                                                        iLeg1BestCreatePrice *  Token1Ratio * MultipleValue(ReverseSide);

                thirdsellprod = iLeg3MktReversePrice * MultipleValue(ReverseSideLeg3) ;//ThirdFP.Token>0?GetExpectedProdPrice((BUYSELL)FOpairObj.TokenThreeSide,ThirdFP , FOpairObj.TokenThreeRatio,true):0;


     dAsksDifference = firstsellProd+ (secondsellProd * Token2Ratio)+(thirdsellprod * Token3Ratio);// + thirdsellProd + fourthsellProd;
     dBidsDifference = firstbuyProd + (secondbuyProd  * Token2Ratio)+ ( thirdbuyprod * Token3Ratio);// + fourthbuyProd;


if(!CancelSellOrder)
{

   // cout << " PF "<<PFNumber << "Reverse secondsellProd "<< secondsellProd<< "  UD "<< BNSFDIFF << " Side "<< ReverseSide <<endl;


            if((SellFarStatus==(OrderStatus)TRADE || SellFarStatus==(OrderStatus)CANCEL || SellFarStatus==(OrderStatus)REJECTED || SellFarStatus==(OrderStatus)NONE) &&
               BNSFDIFF !=0  && _BNSFMNQ > 0 && _BNSFMXQ > 0 && SellTradeCounter < Token1MaxReverseRatio   && !CancelSellOrder )// abs(BNSFDIFF) <= abs(dAsksDifference) &&
            {
                int dFarMonthSellRate = 0;


                /// Time print  Reverse Entry 1
                switch(InstanceGroup)
                {
                    case Best:
                    {
                        if (abs(BNSFDIFF) < abs(dAsksDifference))
                        {
                            dFarMonthSellRate = iLeg1BestReversePrice - 5 ;
                        }
                        else if (abs(BNSFDIFF) == abs(dAsksDifference ))
                        {
                            //dFarMonthSellRate = GetMKTData(FOpairObj.TokenNearSide == BUY ? SELL :BUY) + abs(BNSFDIFF);
                            dFarMonthSellRate = iLeg1BestReversePrice;
                        }
                        break;
                    }
                    case Depth :
                    {
                            dFarMonthSellRate =abs( secondsellProd + thirdsellprod  + BNSFDIFF);//abs( diff  + mktprice);   //GetDepthPric(FOpairObj.TokenNearSide == BUY ? SELL :BUY,BNSFDIFF,ReverseSide) ;
                        break;
                    }
                }

                if(dFarMonthSellRate > 0  && iLeg1BestCreatePrice < iLeg1BestReversePrice)// GetBestData(CreateSide))// GetBestData(CreateSide) )//FarFP.MAXBID)
                {

                   // pthread_mutex_lock(&initsell_mutex);


                  SellFarStatus = (OrderStatus)PENDING;




                    /// Time print Reverse New 2
                    BOARD_LOT_IN_TRSell (FarFP.Token,(short)ReverseSide,
                                	ntohl(min(Token1MinReverseRatio, ( Token1MaxReverseRatio - SellTradeCounter )) * BLQ),
                                    ntohl(dFarMonthSellRate),_pfSell);



                    iSellModQuoteRate = dFarMonthSellRate;
                      //  cout << "PF " << PFNumber << " New Reverse Order Placed at "<< dFarMonthSellRate << " abs(BNSFDIFF) "  <<  abs(BNSFDIFF) << " < abs(dAsksDifference) "<< abs(dAsksDifference) <<" Loopcount "<< LoopCount<<endl;
                    //pthread_mutex_unlock(&initsell_mutex);



                }


            }

            else if((SellFarStatus == (OrderStatus)OPEN || SellFarStatus == (OrderStatus)REPLACED ))//&& abs(dAsksDifference) >= abs(BNSFDIFF) && !CancelSellOrder)
			{
			 //pthread_mutex_lock(&initsell_mutex);
                int dQuoteRateReverse;

                switch(InstanceGroup)
                {
                    case Best:
                    {
                        if (abs(BNSFDIFF) < abs(dAsksDifference) && !CancelSellOrder)
                        {
                            ORDER_MOD_IN_TR_RV(FarFP.Token,iLeg1BestReversePrice - 5,ReverseSide);
                        }
                        else if (abs(BNSFDIFF) == abs(dAsksDifference ) && !CancelSellOrder)
                        {
                           ORDER_MOD_IN_TR_RV(FarFP.Token,iLeg1BestReversePrice,ReverseSide);
                        }
                        else if(abs(dAsksDifference) < abs(BNSFDIFF) || CancelSellOrder)
                        {
                            SellFarStatus = (OrderStatus)PENDING;
                            ORDER_CANCEL_IN_TR ( FarFP.Token, ReverseSide);
                            //SellFarStatus = (OrderStatus)CANCEL;
                             //cout << " PF "<< PFNumber<< " Reverse Order Cancelled "<< " Loopcount "<< LoopCount<<endl;
                        }
                        break;
                    }
                    case Depth :
                    {
                        switch(CancelSellOrder)
                        {
                            case false:
                            {
                                ORDER_MOD_IN_TR_RV(FarFP.Token,abs( secondsellProd + thirdsellprod  + BNSFDIFF),ReverseSide);

                                if(dQuoteRateReverse <= GetMKTDataPrime(ReverseSide))
                                    cout << "PF " << PFNumber <<"  Reverse TradeHit Leg 1 @  " << dQuoteRateReverse << " Leg2 @ "  << GetMKTData(FOpairObj.TokenNearSide == BUY ? SELL :BUY)
                                    <<     " Leg3 @ " << abs(GetMKTDataLeg3(FOpairObj.TokenThreeSide == BUY ? SELL :BUY))
                                    << " Trade Diff " << dQuoteRateReverse - (GetMKTDataLeg2(FOpairObj.TokenNearSide == BUY ? SELL :BUY) +
                                    GetMKTDataLeg3(FOpairObj.TokenThreeSide == BUY ? SELL :BUY)) << " Loopcount "<< LoopCount <<endl;

                                break;
                            }
                            case true:
                            {
                                SellFarStatus = (OrderStatus)PENDING;
                                ORDER_CANCEL_IN_TR ( FarFP.Token, ReverseSide);
                               // SellFarStatus = (OrderStatus)CANCEL;
                              //  cout << " PF "<< PFNumber<< " Reverse Order Cancelled " << " Loopcount "<< LoopCount<<endl;
                                break;
                            }
                        }
                        break;
                 //   dQuoteRate =((BNSFDIFF) -(dAsksDifference)  ) + GetMKTDataPrime(ReverseSide); //GetDepthPrice(FOpairObj.TokenNearSide == BUY ? SELL :BUY,BNSFDIFF,ReverseSide) ;
                    }
                }
/*
                if(dQuoteRateReverse > 0 && dQuoteRateReverse > iLeg1BestCreatePrice && dQuoteRateReverse!=iSellModQuoteRate)// && dQuoteRate > GetBestData(CreateSide) )
                {

                     //SellFarStatus = (OrderStatus)REPLACED;
                    /// Time print Reverse Mod 2


                        SellFarStatus = (OrderStatus)PENDING;

                        cout << "Mod Token " << FarFP.Token <<" dQuoteRate " << dQuoteRateReverse << " Side Reverse"<<endl;
                        ORDER_MOD_IN_TR (FarFP.Token,
									          //      ntohl( OrdDetailSell.Qty *BLQ),
									 ntohl(dQuoteRateReverse),ReverseSide
									);
                        iSellModQuoteRate = dQuoteRateReverse;

                   // cout << " PF "<< PFNumber<< " Reverse Order Modified at "<< dQuoteRate << " Loopcount "<< LoopCount <<endl;

                }*/

               //  pthread_mutex_unlock(&initsell_mutex);
            }

        }

         //   cout <<"PFNumber"<< PFNumber << " dAsksDifference " << dAsksDifference<< " dBidsDifference "<< dBidsDifference << " BuyFarStatus " << BuyFarStatus << " BuyTradeCounter "<<BuyTradeCounter<<endl<<endl;
if(!CancelBuyOrder)
{

//cout << " PF "<<PFNumber << "Create secondbuyProd "<< secondbuyProd<< "  UD "<< BFSNDIFF << " Side "<< CreateSide <<endl;

             if((BuyFarStatus==(OrderStatus)TRADE || BuyFarStatus==(OrderStatus)CANCEL || BuyFarStatus==(OrderStatus)REJECTED || BuyFarStatus==(OrderStatus)NONE) &&
               BFSNDIFF !=0  && _BFSNMNQ > 0 && _BFSNMXQ > 0 && BuyTradeCounter < Token1MaxCreateRatio    && !CancelBuyOrder) //&& abs(BFSNDIFF) >= abs(dBidsDifference)
            {
                int dFarMonthBuyRate = 0;


                switch(InstanceGroup)
                {
                    case Best:
                    {

                        if (abs(BFSNDIFF) > abs(dBidsDifference))
                        {
                            dFarMonthBuyRate = iLeg1BestCreatePrice + 5 ;
                        }
                        else if (abs(BFSNDIFF) == abs(dBidsDifference ))
                        {
                              dFarMonthBuyRate = iLeg1BestCreatePrice;
                        }
                        break;
                    }
                    case Depth:
                    {
                            dFarMonthBuyRate =  abs(secondbuyProd + thirdbuyprod + BFSNDIFF);
                            break;
                    }
                }
                if(dFarMonthBuyRate > 0 && iLeg1BestCreatePrice < iLeg1BestReversePrice)// && dFarMonthBuyRate < GetBestData(ReverseSide)  )
                {

                  //  pthread_mutex_lock(&initbuy_mutex);
                    BuyFarStatus = (OrderStatus)PENDING;

                    /// Time print Create New 1

                    BOARD_LOT_IN_TRBuy (FarFP.Token,(short)CreateSide,
                                    ntohl( min(Token1MinCreateRatio, ( Token1MaxCreateRatio - BuyTradeCounter )) * BLQ),
                                    ntohl(dFarMonthBuyRate),_pfBuy
                                );

                    iBuyModQuoteRate = dFarMonthBuyRate;
                //cout << " PF "<< PFNumber<<" New Create Order Placed at "<< dFarMonthBuyRate << "  abs(BFSNDIFF) " << abs(BFSNDIFF) <<" abs(dBidsDifference) > "<< abs(dBidsDifference)  <<" Loopcount "<< LoopCount <<endl;
                   // pthread_mutex_unlock(&initbuy_mutex);


                }


            }

            else if((BuyFarStatus == (OrderStatus)OPEN || BuyFarStatus == (OrderStatus)REPLACED ))//&& abs(dBidsDifference) <= abs(BFSNDIFF) && !CancelBuyOrder)
			{
               // pthread_mutex_lock(&initbuy_mutex);

                switch(InstanceGroup)
                {
                    case Best:
                    {

                        if (abs(BFSNDIFF) > abs(dBidsDifference) && !CancelBuyOrder)
                        {
                            ORDER_MOD_IN_TR_CR (FarFP.Token,iLeg1BestCreatePrice + 5,CreateSide);
                        }
                        else if (abs(BFSNDIFF) == abs(dBidsDifference ) && !CancelBuyOrder)
                        {
                             ORDER_MOD_IN_TR_CR (FarFP.Token,iLeg1BestCreatePrice ,CreateSide);
                        }
                        else if(abs(dBidsDifference) > abs(BFSNDIFF)  || CancelBuyOrder)
                        {
                            BuyFarStatus = (OrderStatus)PENDING;
                            ORDER_CANCEL_IN_TR ( FarFP.Token, CreateSide);
                            //BuyFarStatus = (OrderStatus)CANCEL;
                           // cout << " PF "<< PFNumber<<" Create order cancelled " << " Loopcount "<< LoopCount<<endl;
                        }
                        break;
                    }
                    case Depth:
                    {

                        switch(CancelBuyOrder)
                        {
                            case false:
                            {
                                ORDER_MOD_IN_TR_CR (FarFP.Token,abs(secondbuyProd + thirdbuyprod + BFSNDIFF) ,CreateSide);


                                break;
                            }
                            case true:
                            {
                                    BuyFarStatus = (OrderStatus)PENDING;
                                    ORDER_CANCEL_IN_TR ( FarFP.Token, CreateSide);
                                    //BuyFarStatus = (OrderStatus)CANCEL;
                                   // cout << " PF "<< PFNumber<<" Create order cancelled " << " Loopcount "<< LoopCount<<endl;

                                    break;
                            }
                        }
                        break;
                    //dQuoteRate =((dBidsDifference) -  (BFSNDIFF) ) + GetMKTDataPrime(CreateSide); //GetDepthPrice(FOpairObj.TokenNearSide,BFSNDIFF,CreateSide)  ;
                    }
                }

                /*if(dQuoteRateCreate >0 && dQuoteRateCreate < iLeg1BestReversePrice && iBuyModQuoteRate!=dQuoteRateCreate)// && dQuoteRate < GetBestData(ReverseSide) )
                {
                        BuyFarStatus = (OrderStatus)PENDING;
                        cout << "Mod Token " << FarFP.Token <<" dQuoteRate " << dQuoteRateCreate << " Side Create"<<endl;
                    /// Time print Create Modify 2
                        ORDER_MOD_IN_TR_CR (FarFP.Token,
						         //ntohl(OrdDetailBuy.Qty *BLQ),
                                dQuoteRateCreate,CreateSide
						);
                        iBuyModQuoteRate = dQuoteRateCreate;
                }*/
               //  pthread_mutex_unlock(&initbuy_mutex);
            }

            }

        }
            pthread_mutex_unlock(&count_mutex);
       // lock.unlock();
      //  the_incondition_variable.notify_one();

        }



        void StrategyClass::OnDifferenceEventHandler(FOPAIRDIFFIOC* _INpairDiff)
        {
			//struct FOPAIRDIFF _INpairDiff;

           // memcpy(&_INpairDiff,buffer,sizeof(_INpairDiff));

            	_BNSFMNQ = _INpairDiff->BNSFMNQ;
                _BFSNMNQ = _INpairDiff->BFSNMNQ;
                _BNSFMXQ = _INpairDiff->BNSFMXQ;
                _BFSNMXQ = _INpairDiff->BFSNMXQ;
            //	DEPTHORBEST = _FOPAIRDIFF [PFNumber].Depth_Best ==Depth ? true  :false ;

                BFSNDIFF = (int)_INpairDiff->BFSNDIFF;
                BNSFDIFF = (int)_INpairDiff->BNSFDIFF;

                Token1MinCreateRatio = Token1Ratio * _BFSNMNQ;
                Token2MinCreateRatio = Token2Ratio * _BFSNMNQ;
                Token3MinCreateRatio = Token3Ratio * _BFSNMNQ;

                Token1MinReverseRatio = Token1Ratio * _BNSFMNQ;
                Token2MinReverseRatio = Token2Ratio * _BNSFMNQ;
                Token3MinReverseRatio = Token3Ratio * _BNSFMNQ;

                Token1MaxCreateRatio = Token1Ratio * _BFSNMXQ;
                Token2MaxCreateRatio = Token2Ratio * _BFSNMXQ;
                Token3MaxCreateRatio = Token3Ratio * _BFSNMXQ;

                Token1MaxReverseRatio = Token1Ratio * _BNSFMXQ;
                Token2MaxReverseRatio = Token2Ratio * _BNSFMXQ;
                Token3MaxReverseRatio = Token3Ratio * _BNSFMXQ;

                if(BuyTradeCounter <= 0)
                {
                    BuyTradeCounter = _INpairDiff->_FOPair.CreateQTY  * Token1Ratio;
                    BuyTradeCounterleg2 = _INpairDiff->_FOPair.CreateQTY  * Token2Ratio;
                    BuyTradeCounterleg3 = _INpairDiff->_FOPair.CreateQTY  * Token3Ratio;
                }

                if(SellTradeCounter <= 0)
                {
                    SellTradeCounter = _INpairDiff->_FOPair.ReverseQTY  * Token1Ratio;
                    SellTradeCounterleg2 = _INpairDiff->_FOPair.ReverseQTY  * Token2Ratio;
                    SellTradeCounterleg3 = _INpairDiff->_FOPair.ReverseQTY  * Token3Ratio;
                }



                CreateSide = _INpairDiff->_FOPair.TokenFarSide;
                ReverseSide = CreateSide == BUY ? SELL:BUY;

                CreateSideLeg2 = _INpairDiff->_FOPair.TokenNearSide;
                ReverseSideLeg2 = CreateSideLeg2 == BUY ? SELL:BUY;

                CreateSideLeg3 = _INpairDiff->_FOPair.TokenThreeSide;
                ReverseSideLeg3 = CreateSideLeg3 == BUY ? SELL:BUY;

               // cout << " New class OnDiff PF " << PFNumber << " BNSFDIFF " << _INpairDiff->BNSFDIFF << " BFSN "<< _INpairDiff->BFSNDIFF<<endl<<endl;
        }


void StrategyClass::BOARD_LOT_IN_TRBuy(int FarTokenNo,BUYSELL BS,int qty, int price, PFHolder _PF)  //-- 20000
		{// MS_OE_REQUEST_TR 110+26

          //  cout<<"Buy: "<<FarTokenNo<<" price "<<htonl(price)<<" size "<<sizeof (_NMPACK [FarTokenNo].FARMONTHBUY) <<endl;
                MS_OE_REQUEST_TR obj;
                  memset(&obj,0,138);
                // cout <<"PFNumber "<< PFNumber <<" New Buy Order location 1 from STG  Price : "<< htonl(price) <<" Volume :"<<htonl(qty)<<endl;

                    //cout<<"BOARD_LOT_IN_TRBuy  : 1 ";

                   // cout<<"BOARD_LOT_IN_TRBuy  : 2 ";

                    //cout<<"BOARD_LOT_IN_TRBuy  : 3 ";
                    obj=_NMPack.FARMONTH;
                    obj.Buy_SellIndicator =ntohs((short)BS);

                   // cout<<"BOARD_LOT_IN_TRBuy  : 4 ";
                    obj.DisclosedVolume=obj.Volume=qty;

                   // if(ntohl(qty) > BLQ)
                   // cout << " Check "<<endl;

	 				obj.Price=price;
	 				//cout<<"BOARD_LOT_IN_TRBuy  : 5 ";
	 				//ProcessToEnqueue((char*)&obj,136);
	 				//cout<<"BOARD_LOT_IN_TRBuy  : 6 ";

	 				_PF._oetr = obj;
                    _PF._size = 136;

                    //_PFHashMap.AddRecord(_PF,false);

                   _FillData.BidQueue(_PF);

                   /// Time print Create New 3

                   cout <<"PFNumber "<< PFNumber <<" New Buy Order sent from STG "<<  " Symbol "<< obj.Symbol<< " Instrument Name "<< obj.InstrumentName<<" Price : "<< htonl(price) <<" Volume :"<<htonl(obj.Volume)<<endl;

        }


 void StrategyClass::BOARD_LOT_IN_TRSell(int FarTokenNo,BUYSELL BS,int qty, int price, PFHolder _PF)  //-- 20000
		{// MS_OE_REQUEST_TR 110+26

//cout<<"Sell: "<<FarTokenNo<<" price "<<htonl(price)<<" size "<<sizeof (_NMPACK [FarTokenNo].FARMONTHSELL) <<endl;
 //cout << "PFNumber "<< PFNumber <<" New Sell Order location 1 from STG  Price : "<< htonl(price) <<" Volume :"<<htonl(qty)<<endl;
                   // cout<<"BOARD_LOT_IN_TRSell  : 1 ";
                    MS_OE_REQUEST_TR obj;
                  // cout<<"BOARD_LOT_IN_TRSell  : 2 ";
                    memset(&obj,0,136);
                  // cout<<"BOARD_LOT_IN_TRSell  : 3 ";
                    obj=_NMPack.FARMONTH;
                    obj.Buy_SellIndicator =ntohs((short)BS);
	 				obj.DisclosedVolume=obj.Volume=qty;

	 				// if(ntohl(qty)> BLQ)
                   // cout << " Check "<<endl;

	 				//cout<<"BOARD_LOT_IN_TRSell  : 4 ";
	 				obj.Price=price;
	 				//cout<<"BOARD_LOT_IN_TRSell  : 5 ";
	 				//ProcessToEnqueue((char*)&obj,136);
	 				//cout<<"BOARD_LOT_IN_TRSell  : 6 ";
                    _PF._oetr = obj;
                    _PF._size=136;

                     //cout<<"\nLength= "<<ntohl(obj.Length) <<" tcode "<<ntohl(obj.TransactionCode);

                    //_PFHashMap.AddRecord(_PF,false);

	 				_FillData.BidQueue(_PF);


                    /// Time print Reverse New 3
                    cout << "PFNumber "<< PFNumber <<" New Sell Order sent from STG "<<  " Symbol "<< obj.Symbol<< " Instrument Name "<< obj.InstrumentName<<" Price : "<< htonl(price) <<" Volume :"<<htonl(obj.Volume)<<endl;
		}


        void StrategyClass::ORDER_MOD_IN_TR_CR(const int TokenNo,const int price,BUYSELL buySell)// 20040
		{
                if(iBuyModQuoteRate!=price)
                {

		//MS_OM_REQUEST_TR 138+26=164
                 BuyFarStatus = (OrderStatus)PENDING;
                    //cout<<"ORDER_MOD_IN_TR  : 1 ";
                    MS_OM_REQUEST_TR obj;
                  //  cout<<"ORDER_MOD_IN_TR  : 2 ";
                    memset(&obj,0,164);
                  //  cout<<"ORDER_MOD_IN_TR  : 3 ";
  	 				if(buySell==CreateSide)
  	 				{
	 					obj=_NMPack.FARMONTHMODBUY;
	 				//	cout<<"ORDER_MOD_IN_TR  : 4 ";
                        //cout << " Mod Buy Order sent from STG "<<endl;
	 				}
	 				else if(buySell==ReverseSide)
	 				{
	 					obj=_NMPack.FARMONTHMODSELL;
	 					//cout << " Mod Sell Order sent from STG "<<endl;
                      //  cout<<"ORDER_MOD_IN_TR  : 5 ";
	 				}
	 				char TransCodeBytes[0];
                    //cout<<"ORDER_MOD_IN_TR  : 6 ";
	 				obj.TransactionCode=ModificationCode;
	 			//	obj.DisclosedVolume=obj.Volume=volume;
	 				//cout<<"ORDER_MOD_IN_TR  : 7 ";
	 				obj.Price=ntohl(price);
	 				//cout<<"ORDER_MOD_IN_TR  : 8 ";

                   // ProcessToEnqueue((char*)&obj,164);
                  //  cout<<"ORDER_MOD_IN_TR  : 9 ";
                    _pfBuy._size = 164;
                    _pfBuy._omtr= obj;
                  _FillData.BidQueue(_pfBuy);

                    iBuyModQuoteRate = price;

                    cout << " Modify Order for "<< htonl(obj.TokenNo) << " Side "<< htons( obj.Buy_SellIndicator ) << " Price "<< htonl(obj.Price)
                    << " Leg1CreatePrice "<< iLeg1BestCreatePrice<< " Leg1ReversePrice "<<iLeg1BestReversePrice<< " FarFP.MAXBID "<<FarFP.MAXBID
                    <<  " FarFP.MINASK "<<FarFP.MINASK  <<endl;

                  /// Time print Modify 4
                }
		}


  void StrategyClass::ORDER_MOD_IN_TR_RV(const int TokenNo,const int price,BUYSELL buySell)// 20040
		{
                if(iSellModQuoteRate!=price)
                {

		//MS_OM_REQUEST_TR 138+26=164
                 SellFarStatus = (OrderStatus)PENDING;
                    //cout<<"ORDER_MOD_IN_TR  : 1 ";
                    MS_OM_REQUEST_TR obj;
                  //  cout<<"ORDER_MOD_IN_TR  : 2 ";
                    memset(&obj,0,164);
                  //  cout<<"ORDER_MOD_IN_TR  : 3 ";
  	 				if(buySell==CreateSide)
  	 				{
	 					obj=_NMPack.FARMONTHMODBUY;
	 				//	cout<<"ORDER_MOD_IN_TR  : 4 ";
                        //cout << " Mod Buy Order sent from STG "<<endl;
	 				}
	 				else if(buySell==ReverseSide)
	 				{
	 					obj=_NMPack.FARMONTHMODSELL;
	 					//cout << " Mod Sell Order sent from STG "<<endl;
                      //  cout<<"ORDER_MOD_IN_TR  : 5 ";
	 				}
	 				char TransCodeBytes[0];
                    //cout<<"ORDER_MOD_IN_TR  : 6 ";
	 				obj.TransactionCode=ModificationCode;
	 			//	obj.DisclosedVolume=obj.Volume=volume;
	 				//cout<<"ORDER_MOD_IN_TR  : 7 ";
	 				obj.Price=ntohl(price);
	 				//cout<<"ORDER_MOD_IN_TR  : 8 ";

                   // ProcessToEnqueue((char*)&obj,164);
                  //  cout<<"ORDER_MOD_IN_TR  : 9 ";
                    _pfBuy._size = 164;
                    _pfBuy._omtr= obj;
                  _FillData.BidQueue(_pfBuy);

                    iSellModQuoteRate = price;

                    cout << " Modify Order for "<< htonl(obj.TokenNo) << " Side "<< htons( obj.Buy_SellIndicator ) << " Price "<< htonl(obj.Price)
                    << " Leg1CreatePrice "<< iLeg1BestCreatePrice<< " Leg1ReversePrice "<<iLeg1BestReversePrice<< " FarFP.MAXBID "<<FarFP.MAXBID
                    <<  " FarFP.MINASK "<<FarFP.MINASK  <<endl;

                  /// Time print Modify 4
                }
		}



 void StrategyClass::ORDER_CANCEL_IN_TR(int TokenNo, BUYSELL buySell)  //-- 20070
		{// MS_OM_REQUEST_TR 138+26

        //cout<<"ORDER_CANCEL_IN_TR  : 1 ";
		MS_OM_REQUEST_TR obj;
		//cout<<"ORDER_CANCEL_IN_TR  : 2 ";
        memset(&obj,0,164);
        //cout<<"ORDER_CANCEL_IN_TR  : 3 ";
  	 				if(buySell==CreateSide)
  	 				{
	 					obj=_NMPack.FARMONTHMODBUY;
                //    cout << " Cancel Buy Order sent Price"<<ntohl(obj.Price)<<" Volume :"<<ntohl(obj.Volume)<<endl;
	 				//	cout<<"ORDER_CANCEL_IN_TR  : 4 "<<" OrderNumber ="<<_NMPack.FARMONTHMODBUY.OrderNumber<<endl;
	 				}
	 				else if(buySell==ReverseSide)
	 				{

	 					obj=_NMPack.FARMONTHMODSELL;

	 				//	 cout << " Cancel Sell Order sent Price"<<ntohl(obj.Price)<<" Volume :"<<ntohl(obj.Volume)<<endl;
	 				//	cout << " Cancel Sell Order sent from STG  5"<<endl;
                     //   cout<<"ORDER_CANCEL_IN_TR  : 5 "<<" OrderNumber ="<<_NMPack.FARMONTHMODBUY.OrderNumber<<endl;
	 				}
	 			//	cout<<"ORDER_CANCEL_IN_TR  : 6 ";
	 				char TransCodeBytes[0];
	 				obj.TransactionCode=CancelCode;
	 				//cout<<"ORDER_CANCEL_IN_TR  : 7 ";
	 				if(obj.OrderNumber==0 || obj.OrderNumber== -1)
	 				{
                        cout << " OrderNumber invalid. Cancellation not sent " << endl;
                        return;
	 				}
                   // ProcessToEnqueue((char*)&obj,164);
                   // cout<<"ORDER_CANCEL_IN_TR  : 8\n ";

                   _pfBuy._size = 164;
                    _pfBuy._omtr= obj;
                  _FillData.BidQueue(_pfBuy);
		}


 void StrategyClass::ORDER_CONFIRMATION_TR (char *buffer) //-- 20073
		{

			 MS_OE_RESPONSE_TR obj;//156
			// cout<<"ORDER_CONFIRMATION_TR  : 1 ";
            memset(&obj,0,156);
           // cout<<"ORDER_CONFIRMATION_TR  : 2 ";
            memcpy(&obj,buffer,156);

       //cout<<"ORDER_CONFIRMATION_TR  : 3 ";
		short _BS = htons(obj.Buy_SellIndicator);
		int _TKN =htonl(obj.TokenNo);

      //  cout << "Token " << _TKN <<  " ORDER_CONFIRMATION_TR In" << endl;
        if(_TKN== FarFP.Token)
        {
            if(_BS == CreateSide )
            {
                 MS_OM_REQUEST_TR _obj;
                memset(&_obj,0,164);
                _obj=_NMPack.FARMONTHMODBUY;
                _obj.OrderNumber=obj.OrderNumber;
                _obj.DisclosedVolume =obj.DisclosedVolume ;
                _obj.DisclosedVolumeRemaining =obj.DisclosedVolumeRemaining ;
                _obj.TotalVolumeRemaining =obj.TotalVolumeRemaining ;
                _obj.Volume =obj.Volume ;
                _obj.VolumeFilledToday =obj.VolumeFilledToday ;
                _obj.Price =obj.Price ;
                _obj.EntryDateTime =obj.EntryDateTime ;
                _obj.LastModified =obj.LastModified ;
                _obj.filler =obj.filler ;
                _obj.Buy_SellIndicator = obj.Buy_SellIndicator;
    //     _NMPACK [_TKN].FARMONTHMODBUY=_obj;

                _NMPack.FARMONTHMODBUY=_obj;
                BuyFarStatus = (OrderStatus)OPEN;

              /*  if(CancelBuyOrder)
                {
                    if(BuyFarStatus== PENDING || BuyFarStatus == OPEN || BuyFarStatus== REPLACED )
                    ORDER_CANCEL_IN_TR ( FarFP.Token, CreateSide);
                }*/
            }
            else
            {
                  MS_OM_REQUEST_TR _obj;
                    memset(&_obj,0,164);
                    _obj=_NMPack.FARMONTHMODSELL;
                    _obj.OrderNumber=obj.OrderNumber;
                    _obj.DisclosedVolume =obj.DisclosedVolume ;
                    _obj.DisclosedVolumeRemaining =obj.DisclosedVolumeRemaining ;
                    _obj.TotalVolumeRemaining =obj.TotalVolumeRemaining ;
                    _obj.Volume =obj.Volume ;
                    _obj.VolumeFilledToday =obj.VolumeFilledToday ;
                     _obj.Price =obj.Price ;
                    _obj.EntryDateTime =obj.EntryDateTime ;
                    _obj.LastModified =obj.LastModified ;
                    _obj.filler =obj.filler ;
                    _obj.Buy_SellIndicator = obj.Buy_SellIndicator;
//cout<<"ORDER_CONFIRMATION_TR  : 11 ";


                    _NMPack.FARMONTHMODSELL=_obj;
                    SellFarStatus = (OrderStatus)OPEN;

                  /*  if(CancelSellOrder)
                    {
                          if(SellFarStatus== PENDING || SellFarStatus == OPEN || SellFarStatus== REPLACED )
                        ORDER_CANCEL_IN_TR ( FarFP.Token, ReverseSide);
                    }*/
            }

		}
       // cout <<"ORDER_CONFIRMATION_TR 6  "<<endl;
     //   ORDER_MOD_IN_TR(_TKN, 50,ntohl( _price+100),(BUYSELL)_BS);
	}

		 void StrategyClass::ORDER_CXL_CONFIRMATION_TR (char *buffer) //-- 20075
	{

			 MS_OE_RESPONSE_TR obj;//156
			//  cout<<"ORDER_CONFIRMATION_TR  : 1 ";
			 memset(&obj,0,156);
			 memcpy(&obj,buffer,156);
			//  cout<<"ORDER_CONFIRMATION_TR  : 2 ";
        short _BS = htons(obj.Buy_SellIndicator);
        int _TKN =htonl(obj.TokenNo);

		// cout<<"ORDER_CONFIRMATION_TR  : 3 ";
		if(_TKN == FarFP.Token)
		{
            if(_BS==CreateSide )
            {
               BuyFarStatus = (OrderStatus)CANCEL;
            }
            else if(_BS == ReverseSide)
            {
                SellFarStatus = (OrderStatus)CANCEL;
            }

        }
        else
        {
            short ErrorCode = htons(obj.ErrorCode);
            if(ErrorCode == 17070)
            {
               cout <<" Self Trade Prevention occured  ErrorCode "<< ErrorCode <<endl;

                MS_OE_REQUEST_TR obj_New;

                if(_TKN == NearFP.Token)
                {
                    obj_New = _NMPack.NEARMONTHMARKET;
                }
                else if(_TKN == ThirdFP.Token)
                {
                    obj_New = _NMPack.NEARMONTHMARKET2;
                }

                obj_New.Buy_SellIndicator = obj.Buy_SellIndicator ;
                obj_New.DisclosedVolume=obj_New.Volume=obj.Volume;

                _pfMKTSell._size = 136;
                _pfMKTSell._oetr= obj_New;

                        //_PFHashMap.AddRecord(_pfMKTSell,false);

                _FillData.MktQueue(_pfMKTSell);
                cout <<" Order placed for Market after SFT "<< ErrorCode <<endl;

            }
        }

	}
//***********************

		 void StrategyClass::ORDER_MOD_CONFIRMATION_TR (char *buffer) //-- 20074
		{
//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step1"<<endl;

       MS_OE_RESPONSE_TR obj;//156
//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step2"<<endl;
	   memset(&obj,0,156);
	//   cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step3"<<endl;
		memcpy(&obj,buffer,156);
		//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step4"<<endl;
		short _BS = htons(obj.Buy_SellIndicator);
		//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step5"<<endl;
		int _TKN = htonl(obj.TokenNo);
		//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step6"<<endl;
		double _OrderNo =obj.OrderNumber ;
		//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step7"<<endl;
		int _price = htonl(obj.Price);
		//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step8"<<endl;
        if(_TKN == FarFP.Token)
        {
            if(_BS == CreateSide)
			{
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step9"<<endl;
            MS_OM_REQUEST_TR _obj;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step10"<<endl;
            memset(&_obj,0,164);
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step11"<<endl;
            _obj=_NMPack.FARMONTHMODBUY;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step12"<<endl;
            _obj.OrderNumber=obj.OrderNumber;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step13"<<endl;
            _obj.DisclosedVolume =obj.DisclosedVolume ;
			// cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step14"<<endl;
             _obj.DisclosedVolumeRemaining =obj.DisclosedVolumeRemaining ;
			// cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step15"<<endl;
              _obj.TotalVolumeRemaining =obj.TotalVolumeRemaining ;
			  //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step16"<<endl;
               _obj.Volume =obj.Volume ;
			   //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step17"<<endl;
                _obj.VolumeFilledToday =obj.VolumeFilledToday ;
				//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step18"<<endl;
                 _obj.Price =obj.Price ;
				 //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step19"<<endl;
                  _obj.EntryDateTime =obj.EntryDateTime ;
				  //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step20"<<endl;
                   _obj.LastModified =obj.LastModified ;
				   //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step21"<<endl;
                    _obj.filler =obj.filler ;
					//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step22"<<endl;
                _obj.Buy_SellIndicator = obj.Buy_SellIndicator;
            //_NMPACK [_TKN].FARMONTHMODBUY=_obj;

            _NMPack.FARMONTHMODBUY=_obj;
            BuyFarStatus = (OrderStatus)REPLACED;

           /* if(CancelBuyOrder)
            {
                if(BuyFarStatus== PENDING || BuyFarStatus == OPEN || BuyFarStatus== REPLACED )
               ORDER_CANCEL_IN_TR ( FarFP.Token, CreateSide);
            }*/

//cout << " ORDER_MOD_CONFIRMATION_TR Buy Order recieved in STG "<<endl;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step25"<<endl;

			}
            else if(_BS == ReverseSide)
			{
			    MS_OM_REQUEST_TR _obj;
			//	cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step26"<<endl;
            memset(&_obj,0,164);
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step27"<<endl;
            _obj=_NMPack.FARMONTHMODSELL;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step28"<<endl;
            _obj.OrderNumber=obj.OrderNumber;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step29"<<endl;
            _obj.DisclosedVolume =obj.DisclosedVolume ;
			//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step30"<<endl;
             _obj.DisclosedVolumeRemaining =obj.DisclosedVolumeRemaining ;
			 //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step31"<<endl;
              _obj.TotalVolumeRemaining =obj.TotalVolumeRemaining ;
			  //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step32"<<endl;
               _obj.Volume =obj.Volume ;
			   //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step33"<<endl;
                _obj.VolumeFilledToday =obj.VolumeFilledToday ;
				//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step34"<<endl;
                 _obj.Price =obj.Price ;
				 //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step35"<<endl;
                  _obj.EntryDateTime =obj.EntryDateTime ;
				  //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step36"<<endl;
                   _obj.LastModified =obj.LastModified ;
				   //cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step37"<<endl;
                    _obj.filler =obj.filler ;
					//cout<<"ORDER_MOD_CONFIRMATION_TR =>"<<"step38"<<endl;
                _obj.Buy_SellIndicator = obj.Buy_SellIndicator;

				 _NMPack.FARMONTHMODSELL=_obj;
                  SellFarStatus = (OrderStatus)REPLACED;


                  /*  if(CancelSellOrder)
                    {
                         if(SellFarStatus== PENDING || SellFarStatus == OPEN || SellFarStatus== REPLACED )
                        ORDER_CANCEL_IN_TR ( FarFP.Token, ReverseSide);
                    }*/
                }
//cout << " ORDER_MOD_CONFIRMATION_TR Sell Order recieved in STG "<<endl;

			}
		}


	//	ORDER_CANCEL_IN_TR(_TKN,(BUYSELL)_BS);




		void StrategyClass::TRADE_CONFIRMATION_TR (char *buffer) //-- 20222
		{

          MS_TRADE_CONFIRM_TR obj_Trade;

          memset(&obj_Trade,0,153);

          memcpy(&obj_Trade,buffer,153);

            int _TKN =htonl(obj_Trade.Token);

            short _BS = htons(obj_Trade.Buy_SellIndicator);
			int Qty = htonl(obj_Trade.FillQuantity)/BLQ;
			int RemainingQty = htonl(obj_Trade.RemainingVolume)/BLQ;

          if (_TKN==FarToken)
          {

            int TradableQty =0;

            if(_BS==(short)FOpairObj.TokenFarSide)
            {
               BuyTradeCounter += Qty;
                TradableQty = BuyTradeCounter;
               _CreateRemainingQty = RemainingQty;
            }
			else
			{
               SellTradeCounter += Qty ;
               TradableQty = SellTradeCounter;
               _ReverseRemainingQty = RemainingQty;
            }


            ///Current T Qty = X
            /// Total T QTy = y

            ///Current Leg2 T Qty = ((((y+x) * MktLeg2HitRatio) * BLQ ) - (Leg 2 Traded Qty * BLQ))

            ///Current Leg3 T Qty = ((((y+x) * MktLeg3HitRatio) * BLQ ) - (Leg 3 Traded Qty * BLQ))

                int Leg2HitQty = floor(TradableQty * Leg2HitRatio) * BLQ;

                 Leg2HitQty = FOpairObj.TokenFarSide == _BS ? (Leg2HitQty - (BuyTradeCounterleg2 * BLQ))
                                                                : (Leg2HitQty - (SellTradeCounterleg2 * BLQ));

                int Leg3HitQty = floor(TradableQty * Leg3HitRatio) * BLQ;

                Leg3HitQty = FOpairObj.TokenFarSide == _BS ? (Leg3HitQty - (BuyTradeCounterleg3 * BLQ))
                                                                : (Leg3HitQty - (SellTradeCounterleg3 * BLQ));

                if(Leg2HitQty>0)
                {
                    MS_OE_REQUEST_TR obj_New;



                    obj_New = _NMPack.NEARMONTHMARKET;

                    short Leg1side = CreateSide == _BS ? (short)CreateSideLeg2 : (short)ReverseSideLeg2;

                    obj_New.Buy_SellIndicator =ntohs(Leg1side);
                    obj_New.DisclosedVolume=obj_New.Volume= htonl(Leg2HitQty) ;

                    _pfMKTSell._size = 136;
                    _pfMKTSell._oetr= obj_New;
                    _FillData.MktQueue(_pfMKTSell);

                }
                if(Leg3HitQty > 0)
                {
                    MS_OE_REQUEST_TR obj_New2;



                    obj_New2 = _NMPack.NEARMONTHMARKET2;
                    short Leg2side = CreateSide == _BS ? (short)CreateSideLeg3 : (short)ReverseSideLeg3;

                    obj_New2.Buy_SellIndicator =ntohs(Leg2side);

                    obj_New2.DisclosedVolume=obj_New2.Volume=htonl(Leg3HitQty);

                    _pfMKTSell2._size = 136;
                    _pfMKTSell2._oetr= obj_New2;
                    _FillData.MktQueue(_pfMKTSell2);

                }





                cout << " Token "<< _TKN <<" Qty "<< Qty << " _BS "<< _BS<<" Leg2HitQty " << Leg2HitQty << " Leg3HitQty "<<Leg3HitQty<<endl;

			}
			else if(_TKN== NearToken)
			 {
                if(_BS ==CreateSideLeg2)
                {
                    BuyTradeCounterleg2 +=Qty;
                }
                else if(_BS ==ReverseSideLeg2)
                {
                    SellTradeCounterleg2 +=Qty;
                }
                if(ThirdFP.Token ==0 && _BS ==CreateSideLeg2 && _CreateRemainingQty ==0)
                {
                    BuyFarStatus =(OrderStatus)TRADE;

                    cout <<" PFNumber "<< PFNumber << " BuyFarStatus set to Trade"<<endl;
                }
                else if(ThirdFP.Token ==0 && _BS ==ReverseSideLeg2 && _ReverseRemainingQty ==0)
                {

                    SellFarStatus =(OrderStatus)TRADE;

                    cout <<" PFNumber "<< PFNumber << " SellFarStatus set to Trade"<<endl;
                }

			  cout<<"PF " << PFNumber <<" Trade Token 2  _TKN: "<<_TKN<<" _BS: "<<_BS<<endl;

            }
            else if (_TKN==TokenThree)
            {
                if(_BS ==CreateSideLeg3)
                {
                     BuyTradeCounterleg3 +=Qty;
                }
                else if(_BS ==ReverseSideLeg3)
                {
                    SellTradeCounterleg3 +=Qty;
                }

                 if(_BS ==CreateSideLeg3 && _CreateRemainingQty ==0)
                {
                    BuyFarStatus =(OrderStatus)TRADE;

                    cout <<" PFNumber "<< PFNumber << " BuyFarStatus set to Trade"<<endl;
                }
                else if(_BS == ReverseSideLeg3 && _ReverseRemainingQty ==0)
                {
                    SellFarStatus =(OrderStatus)TRADE;
                    cout <<" PFNumber "<< PFNumber << " SellFarStatus set to Trade"<<endl;
                }
                cout<<"PF " <<  PFNumber << " Trade Token 3  _TKN: "<<_TKN<<" _BS: "<<_BS<<endl;

            }



		}




		 void StrategyClass::ORDER_CXL_REJ_OUT (char *buffer) //-- 2072
		{//MS_OE_REQUEST 240
//cout<<"ORDER_CXL_REJ_OUT =>"<<"step1"<<endl;
			 MS_OE_REQUEST obj;
	//		 cout<<"ORDER_CXL_REJ_OUT =>"<<"step2"<<endl;
			 memset(&obj,0,240);
		//	 cout<<"ORDER_CXL_REJ_OUT =>"<<"step3"<<endl;
             memcpy(&obj,buffer,240);
			// cout<<"ORDER_CXL_REJ_OUT =>"<<"step4"<<endl;
						    short _BS =htons(obj.Buy_SellIndicator);
				//					cout<<"ORDER_CXL_REJ_OUT =>"<<"step5"<<endl;
							int _TKN =htonl(obj.TokenNo);
					//		cout<<"ORDER_CXL_REJ_OUT =>"<<"step6"<<endl;
							short _Error = htons(obj.ErrorCode);
						//	cout<<"ORDER_CXL_REJ_OUT =>"<<"step7"<<endl;
							if(_BS == CreateSide)
							{
							//cout<<"ORDER_CXL_REJ_OUT =>"<<"step8"<<endl;
							if (_Error == 16273)

							BuyFarStatus = (OrderStatus)CANCEL;

							else if(BuyFarStatus != (OrderStatus)TRADE)
							BuyFarStatus = (OrderStatus)REPLACED;
							}
							else if(_BS == ReverseSide)
							{

							if (_Error == 16273)
							SellFarStatus = (OrderStatus)CANCEL;
							else if(SellFarStatus != (OrderStatus)TRADE)
							SellFarStatus = (OrderStatus)REPLACED;


							}
						//	cout<<"ORDER_CXL_REJ_OUT =>"<<"step9"<<endl;
		}


		void StrategyClass::ORDER_MOD_REJ_OUT (char *buffer) //-- 2042
		{//MS_OE_REQUEST 240
//cout<<"ORDER_MOD_REJ_OUT =>"<<"step1"<<endl;
			 MS_OE_REQUEST obj;
	//		 cout<<"ORDER_MOD_REJ_OUT =>"<<"step2"<<endl;
			 memset(&obj,0,240);
		//	 cout<<"ORDER_MOD_REJ_OUT =>"<<"step3"<<endl;
			  memcpy(&obj,buffer,240);
			//  cout<<"ORDER_MOD_REJ_OUT =>"<<"step4"<<endl;

                short _BS =htons(obj.Buy_SellIndicator);
				//cout<<"ORDER_MOD_REJ_OUT =>"<<"step5"<<endl;
				int _TKN =htonl(obj.TokenNo);
				//cout<<"ORDER_MOD_REJ_OUT =>"<<"step6"<<endl;
				short _Error = htons(obj.ErrorCode);
				//cout<<"ORDER_MOD_REJ_OUT =>"<<"step7"<<endl;


				if(_TKN == FarFP.Token)
                {
                if(_BS==CreateSide)
                {
				if (_Error == 16273)
				BuyFarStatus= (OrderStatus)CANCEL;
				else if(BuyFarStatus != (OrderStatus)TRADE)
				BuyFarStatus = (OrderStatus)REPLACED;
				}
				else
				{
				if (_Error == 16273)
				SellFarStatus = (OrderStatus)CANCEL;
				else if(SellFarStatus != (OrderStatus)TRADE)
				SellFarStatus = (OrderStatus)REPLACED;

				}
				}
				//cout<<"ORDER_MOD_REJ_OUT =>"<<"step8"<<endl;
	}

    void StrategyClass::ORDER_ERROR_OUT (char *buffer) //-- 2231
		{//MS_OE_REQUEST 240
//cout<<"ORDER_ERROR_OUT =>"<<"step1"<<endl;
		MS_OE_REQUEST obj;
	//	cout<<"ORDER_ERROR_OUT =>"<<"step2"<<endl;
		memset(&obj,0,240);
		//cout<<"ORDER_ERROR_OUT =>"<<"step3"<<endl;
        memcpy(&obj,buffer,240);
//cout<<"ORDER_ERROR_OUT =>"<<"step4"<<endl;
        short _BS =htons(obj.Buy_SellIndicator);
	//	cout<<"ORDER_ERROR_OUT =>"<<"step5"<<endl;
		int _TKN =htonl(obj.TokenNo);
		//cout<<"ORDER_ERROR_OUT =>"<<"step6"<<endl;

		if(_TKN == FarFP.Token )
		{
		if(_BS== CreateSide)
		BuyFarStatus = (OrderStatus)REJECTED;
        else
		SellFarStatus = (OrderStatus)REJECTED;


		}

cout<<"ORDER_ERROR_OUT =>"<<"step7"<<endl;
		}


