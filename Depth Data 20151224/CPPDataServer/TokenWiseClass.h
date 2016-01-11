#ifndef TOKENWISECLASS_H_INCLUDED
#define TOKENWISECLASS_H_INCLUDED
#include "NanoMQSender.h"
#include "Structure.h"
#include <map>
#include<iostream>
#include<string.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>

using namespace std;
using boost::multi_index::multi_index_container;
using boost::multi_index::ordered_non_unique;
using boost::multi_index::ordered_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::member;
using boost::multi_index::nth_index;
using boost::multi_index::get;


struct Data_entry
{
   Data_entry( const double& OID,
                   const int& Price,
                   int Qty):
                   Order_ID_(OID),
                   Price_rec_(Price),
                   Qty_rec_(Qty)
   {}
   double Order_ID_;
   int Price_rec_;
   int Qty_rec_;
};

struct change_Price
{
  change_Price(int new_Price):new_Price(new_Price){}

  void operator()(Data_entry& e)
  {
    e.Price_rec_=new_Price;
  }

private:
  int new_Price;
};

typedef multi_index_container<
Data_entry, indexed_by<
 ordered_unique<member<Data_entry, double
       , &Data_entry::Order_ID_> >,
   ordered_non_unique<member<Data_entry, int
       , &Data_entry::Price_rec_> >
   >
> Data_set;

class TokenWiseClass
{

    FinalPrice _FPbackup;
    Data_set _BuyPriceRecord;
    Data_set _SellPriceRecord;

    typedef nth_index<Data_set, 1>::type Price_view;
    typedef nth_index<Data_set, 0>::type Ord_Replace;

        public:
        FinalPrice _FP;



        int Token;
        static int tm;

       double BOID;
       double SOID;
        bool BIDASKerror;


    TokenWiseClass(int Tkn)
    {
        memset(&_FP,0,sizeof(_FP));
        //cout<<" Class Initiated for "<< Tkn << " Earlier " << Token<< endl;

        Token= Tkn;
        _FP.Token = Tkn;
        _FP.sub_token=Tkn;

        BOID=0;
        SOID=0;

       // insertrecbid = get<1>(_BuyPriceRecord);
       // insertrecask = get<1>(_SellPriceRecord);

        //updaterec= get<0>(m_Data);
    }


    void BidAsk(Order_Message *Omsg)
    {

	switch (Omsg->Order_Type)
	{
	case 'B':

		if (Omsg->Message_Type == 'N' )
		{
            Price_view& insertrec = get<1>(_BuyPriceRecord);
            insertrec.insert(Data_entry( Omsg->Order_Id,  Omsg->Price ,Omsg->Quantity ));
		}
        else if ( Omsg->Message_Type == 'M')
        {
            Ord_Replace& updaterec = get<0>(_BuyPriceRecord);
            Data_set::iterator it = updaterec.find(Omsg->Order_Id);
            if(it!=updaterec.end())
            {
                updaterec.modify(it,change_Price(Omsg->Price));
            }
            else
            {
                Price_view& insertrec = get<1>(_BuyPriceRecord);
                insertrec.insert(Data_entry( Omsg->Order_Id,  Omsg->Price ,Omsg->Quantity ));
            }
        }
		else if (Omsg->Message_Type == 'X')
        {
            Ord_Replace& updaterec = get<0>(_BuyPriceRecord);
            Data_set::iterator it = updaterec.find(Omsg->Order_Id);
            if(it!=updaterec.end())
            updaterec.erase(it);
        }


		break;

	case 'S':
        if (Omsg->Message_Type == 'N' )
		{
            Price_view& insertrec = get<1>(_SellPriceRecord);
            insertrec.insert(Data_entry( Omsg->Order_Id,  Omsg->Price ,Omsg->Quantity ));
		}
        else if ( Omsg->Message_Type == 'M')
        {
            Ord_Replace& updaterec = get<0>(_SellPriceRecord);
            Data_set::iterator it = updaterec.find(Omsg->Order_Id);
            if(it!=updaterec.end())
            {
                updaterec.modify(it,change_Price(Omsg->Price));
            }
            else
            {
                Price_view& insertrec = get<1>(_SellPriceRecord);
                insertrec.insert(Data_entry( Omsg->Order_Id,  Omsg->Price ,Omsg->Quantity ));
            }
        }
		else if (Omsg->Message_Type == 'X')
        {
            Ord_Replace& updaterec = get<0>(_SellPriceRecord);
            Data_set::iterator it = updaterec.find(Omsg->Order_Id);
            if(it!=updaterec.end())
                updaterec.erase(it);
        }

		break;
	  }



        _FP.MAXBID = SortBid();
		_FP.MINASK = SortAsk();


        if(((_FP.MAXBID != _FPbackup.MAXBID) || (_FPbackup.MINASK != _FP.MINASK) ) && _FP.MAXBID > 0 && _FP.MINASK > 0)
        {
                nanoObj.SendData(_FP);
                _FPbackup= _FP;
        }

        //cout<< "BIDASK " << "Token " << _FP.Token << " BID " << _FP.MAXBID << " ASK " << _FP.MINASK << endl;

        //cout << " " << endl;

            if(_FP.MAXBID > _FP.MINASK && _FP.MAXBID > 0 && _FP.MINASK> 0)
            {
                    //Price_view& eraserec = get<1>(_BuyPriceRecord);
                    //auto it = updaterec.find(Omsg->Order_Id);
                    //if(it!=updaterec.end())
                   // {
                   //     updaterec.modify(it,change_Price(Omsg->Price));
                    //}
                if(BIDASKerror)
                {
                    FinalPrice _FPCancel;
                    memset(&_FPCancel,0, sizeof(FinalPrice));
                    _FPCancel.Token=_FP.Token;
                    _FPCancel.sub_token=111;

                    ClearData();
                    cout << "BA BID > ASK BIDASKerror "<< BIDASKerror << endl;
                }
                else
                BIDASKerror = true;


            }
        }




 void TrdMsg(Trade_Message *Tmsg)
    {

        _FP.LTP = Tmsg->Trade_Price;

            Ord_Replace& erasebid = get<0>(_BuyPriceRecord);
            Data_set::iterator itbid = erasebid.find(Tmsg->Buyorder_Id);
            if(itbid!=erasebid.end())
                erasebid.erase(itbid);

            Ord_Replace& eraseask = get<0>(_SellPriceRecord);
            Data_set::iterator itask = eraseask.find(Tmsg->Selloder_Id);
            if(itask!=eraseask.end())
                eraseask.erase(itask);


        _FP.MAXBID = SortBid();

        _FP.MINASK =SortAsk();

        //cout<< "\t LTP " << "Token " << _FP.Token << " BID " << _FP.MAXBID << " ASK " << _FP.MINASK << endl;
        //cout << " " << endl;

        nanoObj.SendData(_FP);

        if(_FP.MAXBID > _FP.MINASK && _FP.MAXBID > 0 && _FP.MINASK> 0)
            cout << "LTP BID > ASK"<< endl;
            else
            BIDASKerror = false;

    }



    int SortBid()
    {
        int MAXBID=0;

        Price_view& insertrec = get<1>(_BuyPriceRecord);



       // auto val =_BuyPriceRecord.rbegin();


         //   if(MAXBID==0)
         //       MAXBID = val->Price_rec_;


        for(Price_view::reverse_iterator it = insertrec.rbegin(), it_end(insertrec.rend()); it != it_end; ++it)
        {
           // if(MAXBID==0)
                MAXBID = it->Price_rec_;
                break;
            std::cout << "Bid " << (long long)it->Order_ID_  <<" "
                 << it->Price_rec_ << "  "
                 << it->Qty_rec_ << std::endl;
        }

        return MAXBID;
    }


    int SortAsk()
    {
        int MINASK=0;

        Price_view& insertrec = get<1>(_SellPriceRecord);

       // auto val =_BuyPriceRecord.end();
       //     if(MINASK==0)
        //        MINASK = val->Price_rec_;

        for(Price_view::iterator it = insertrec.begin(), it_end(insertrec.end()); it != it_end; ++it)
        {
           // if(MINASK==0)
                MINASK = it->Price_rec_;
            break;
            std::cout << " Ask "<< (long long)it->Order_ID_  <<" "
                 << it->Price_rec_ << "  "
                 << it->Qty_rec_ << std::endl;
        }

        return MINASK;
    }



void ClearData()
{
    _BuyPriceRecord.clear();
    _SellPriceRecord.clear();
//    _SellPriceRecord_D.clear();
  //  _BuyPriceRecord_D.clear();
    cout <<"Data Cleared for Token " << _FP.Token <<endl;

}
~TokenWiseClass()
{
    //dtor
}
};

#endif // TOKENWISECLASS_H_INCLUDED
