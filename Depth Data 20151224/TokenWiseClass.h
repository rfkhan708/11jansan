#ifndef TOKENWISECLASS_H_INCLUDED
#define TOKENWISECLASS_H_INCLUDED
#include "NanoMQSender.h"
#include "Structure.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

//#include <map>
#include<iostream>
#include<string.h>

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <map>

using namespace boost::bimaps;

using namespace std;


class BimapDataClass:boost::noncopyable
{
    private:

    /// std:greater is not working as expected. Need to check and fix it.
    typedef bimap<double,multiset_of<int,std::greater<int> > > bm_Data;

    typedef map<double , int> OrderBasedQty;

    typedef map<int , int> PriceBasedQty;

    bm_Data _BidData;
    bm_Data _AskData;

    int upper,lower;

    OrderBasedQty _BidOQty;
    OrderBasedQty _AskOQty;


    PriceBasedQty _BidPQty;
    PriceBasedQty _AskPQty;

    //PriceArrray _FP;

    FinalPrice _FP;
    mutable boost::mutex _mtx;

    public:
//    int Token;


    BimapDataClass(int Token, int Pclose)
    {
       // boost::mutex::scoped_lock lock(_mtx);

        memset(&_FP,0 ,sizeof(FinalPrice));
        _FP.Token = Token;
        _FP.sub_token = Token;

        int range;


        range = (Pclose * 4 )/100;
        upper =  Pclose + range;
        lower = Pclose- range;


    }

    bool CrossCheckPrice(int Price)
    {
        /*bool retval= false;

        if (_FP.LTP==0)
        {retval= true;}
        else
        {
            if(Price >= lower && Price <= upper)
            {
                retval = true;
            }
            else
            {
                retval = false;
            }

        }

        return retval;*/
        return true;

    }


    void Insert(double OrderNumber, int Price, int Qty , short Side)
    {
       // boost::mutex::scoped_lock lock(_mtx);

        if(CrossCheckPrice(Price))
        {

            if(Side==1)///BUY
            {

                /*if(Price > _FP.MINASK && _FP.MINASK >0)
                {
                    cout << "Insert Some error Please check Price " << Price << " BID "<< _FP.MAXBID << " ASK "<< _FP.MINASK <<endl;
                }*/

                _BidData.insert( bm_Data::value_type(OrderNumber ,Price) );
                //_BidPQty[Price]+=Qty;
                //_BidOQty[OrderNumber]=Qty;
            }
            else
            {
                _AskData.insert( bm_Data::value_type(OrderNumber ,Price) );
                //_AskPQty[Price]+=Qty;
               // _AskOQty[OrderNumber]=Qty;
            }
        }

    }

    void Upate(double OrderNumber, int Price, int Qty, short Side)
    {
    //boost::mutex::scoped_lock lock(_mtx);


        if(CrossCheckPrice(Price))
        {

        if(Side == 1)///Bid
        {
                /*if(Price > _FP.MINASK && _FP.MINASK >0)
                {
                    cout << "Update Some error Please check Price " << Price << " BID "<< _FP.MAXBID << " ASK "<< _FP.MINASK <<endl;
                }*/

            bm_Data::left_iterator it = _BidData.left.find(OrderNumber);

            if(it->first == OrderNumber)
            {
                int _LastPrice = it->second;

                bool successful_replace = _BidData.left.replace_data( it, Price);
               /* if(successful_replace )
                {

                    int LastTotQty = _BidPQty[_LastPrice];
                    int OrdQty = _BidOQty[OrderNumber];

                    _BidPQty[_LastPrice] = LastTotQty - OrdQty;
                    if( _BidPQty[_LastPrice]<=0)
                        DeletePriceBasedQty( _LastPrice,Side);

                    _BidPQty[Price]+=Qty;
                    _BidOQty[OrderNumber]=Qty;

                }*/
            }
            else
            {
                Insert(OrderNumber,Price,Qty,Side);
            }

        }
        else
        {
             bm_Data::left_iterator it = _AskData.left.find(OrderNumber);

            if(it->first == OrderNumber)
            {
                int _LastPrice = it->second;

                bool successful_replace = _AskData.left.replace_data( it, Price);
               /* if(successful_replace )
                {

                    int LastTotQty = _AskPQty[_LastPrice];
                    int OrdQty = _AskOQty[OrderNumber];

                    _AskPQty[_LastPrice] = LastTotQty - OrdQty;
                    if( _AskPQty[_LastPrice]<=0)
                        DeletePriceBasedQty( _LastPrice,Side);

                    _AskPQty[Price]+=Qty;
                    _AskOQty[OrderNumber]=Qty;

                }*/
            }
            else
            {
                Insert(OrderNumber,Price,Qty,Side);
            }


        }

    }
        //cout << " Key " << it->first << " Data " << it->second<<endl;
    }

    void Delete (double OrderNumber , short Side)
    {
       // boost::mutex::scoped_lock lock(_mtx);


        if(Side ==1 )
        {
            bm_Data::left_iterator it = _BidData.left.find(OrderNumber);

            //cout << " After Buy iterator" <<endl;
            if(it!=_BidData.left.end() )
            {
            //    cout << " inside Buy iterator" <<endl;
            int Price = it->second;

           /* PriceBasedQty::iterator _it = _BidPQty.find(Price);
            if(_it!= _BidPQty.end())
            {
                _BidPQty[Price]=(_BidPQty[Price] - _BidOQty[OrderNumber]);
            }
            else
            {
                _BidPQty[Price]=_BidOQty[OrderNumber];
            }


            if( _BidPQty[Price]<=0)
                DeletePriceBasedQty( Price,Side);

            DeleteOrderBasedQty(OrderNumber,Side);*/

            _BidData.left.erase( it );
            }
        }
       else
       {
            bm_Data::left_iterator it = _AskData.left.find(OrderNumber);
            //cout << " After Sell iterator" <<endl;
            if(it!=_AskData.left.end())
            {
            //cout << " inside Sell iterator" <<endl;
            int Price = it->second;

           /* PriceBasedQty::iterator _it = _AskPQty.find(Price);
            if(_it!= _AskPQty.end())
            {
                _AskPQty[Price]=(_AskPQty[Price] - _AskOQty[OrderNumber]);
            }
            else
            {
                _AskPQty[Price]=_AskOQty[OrderNumber];
            }



            if( _AskPQty[Price]<=0)
                DeletePriceBasedQty( Price,Side);

            DeleteOrderBasedQty(OrderNumber,Side);*/

            _AskData.left.erase( it );
            }
       }

    }

    void DeletePriceBasedQty(int Price, short Side)
    {
   // boost::unique_lock<boost::mutex> lock(_mtx);
        //lock.lock();
        if(Side ==1 )///Buy
        {
            PriceBasedQty::iterator _it = _BidPQty.find(Price);
            if(_it!= _BidPQty.end())
            {
                _BidPQty.erase(_it);
            }
        }
        else
        {
            PriceBasedQty::iterator _it = _AskPQty.find(Price);
            if(_it!= _AskPQty.end())
            {
                _AskPQty.erase(_it);
            }

        }
       // lock.unlock();
    }

    void DeleteOrderBasedQty(double OrderNumber, short Side)
    {
   // boost::unique_lock<boost::mutex> lock(_mtx);
       // lock.lock();
        if(Side ==1)///BUY
        {
            OrderBasedQty::iterator _it = _BidOQty.find(OrderNumber);
            if(_it!= _BidOQty.end())
            {
                _BidOQty.erase(_it);
            }
        }
        else
        {
           OrderBasedQty::iterator _it = _AskOQty.find(OrderNumber);
            if(_it!= _AskOQty.end())
            {
                _AskOQty.erase(_it);
            }

        }
       // lock.unlock();
    }

    FinalPrice ReturnPrice(int LTP = 0)
    {
       // boost::mutex::scoped_lock lock(_mtx);

        int BidCount =0;
        int AskCount = 0;

        if(LTP >0)
        {
            int range;

            _FP.LTP = LTP;
           /* range = (LTP * 4 )/100;
            upper =  LTP + range;
            lower = LTP- range;*/
        }
        for( bm_Data::right_const_iterator Bid_iter = _BidData.right.begin(),iend= _BidData.right.end();Bid_iter != iend; ++Bid_iter )
        {

                /// Need to get unique data point from Bimap to protect price repeat in depth
               /* _FP._FP[BidCount].MAXBID = Bid_iter->first;
                _FP._FP[BidCount].BIDQ = _BidPQty[Bid_iter->first];
                BidCount++;

                cout << " Token " << _FP.Token << " Bid Depth Index "<< BidCount <<" Price " << _FP._FP[BidCount].MAXBID << " Qty " <<_FP._FP[BidCount].BIDQ << " LTP "<< _FP.LTP <<endl;

                  if(BidCount > 4)*/

                  _FP.MAXBID =  Bid_iter->first;


                break;
        }

//for(PriceList::reverse_iterator it = _PriceList.rbegin(), it_end(_PriceList.rend()); it != it_end; ++it)
      //  {

        for( bm_Data::right_map::reverse_iterator Ask_iter = _AskData.right.rbegin(),iend= _AskData.right.rend();Ask_iter != iend; ++Ask_iter )
        {


               /* _FP._FP[AskCount].MINASK = Ask_iter->first;
                _FP._FP[AskCount].ASKQ = _AskPQty[Ask_iter->first];

                AskCount++;


                cout << " Token " << _FP.Token << " Ask Depth Index "<< AskCount <<" Price " << _FP._FP[AskCount].MINASK << " Qty " <<_FP._FP[AskCount].ASKQ << " LTP "<< _FP.LTP <<endl;

                  if(AskCount > 4)*/

                  _FP.MINASK = Ask_iter->first;
                break;
        }

        return _FP;

    }

};


class TokenWiseClass:private boost::noncopyable
{
    BimapDataClass * _bmp;

       double BOID;
       double SOID;
        bool BIDASKerror;

        mutable boost::mutex the_mutex;

public:
 int Token;


boost::condition_variable the_condition_variable;

    TokenWiseClass(int Tkn, int PClose)
    {
        Token = Tkn;
       _bmp= new BimapDataClass(Tkn,PClose);

    }


    void BidAsk(Order_Message *Omsg)
    {

     //   cout << " Inside BidAsk 1 "<< Token  <<endl;

       boost::unique_lock<boost::mutex> lock(the_mutex);

       // cout << "BA Incoming Pack Token "<< Omsg->Token << " MyToken "<< Token << " Side "<< Omsg->Order_Type << " Price " << Omsg->Price<<endl;



        //the_condition_variable.wait(lock);
        //cout << " Inside BidAsk 2 "<< Token <<endl;
	switch (Omsg->Order_Type)
	{
	case 'B':
       // cout << " Inside BidAsk 3 "<< Token <<endl;
		if (Omsg->Message_Type == 'N' )
		{
            _bmp->Insert(Omsg->Order_Id,Omsg->Price,Omsg->Quantity,1);
		}
        else if ( Omsg->Message_Type == 'M')
        {
            _bmp->Upate(Omsg->Order_Id,Omsg->Price,Omsg->Quantity,1);
        }
		else if (Omsg->Message_Type == 'X')
        {
            _bmp->Delete(Omsg->Order_Id,1);
        }

       // cout << " Inside BidAsk 4 "<< Token <<endl;

		break;

	case 'S':
       // cout << " Inside BidAsk 5 "<< Token <<endl;
        if (Omsg->Message_Type == 'N' )
		{
            _bmp->Insert(Omsg->Order_Id,Omsg->Price,Omsg->Quantity,2);
		}
        else if ( Omsg->Message_Type == 'M')
        {
            _bmp->Upate(Omsg->Order_Id,Omsg->Price,Omsg->Quantity,2);
        }
		else if (Omsg->Message_Type == 'X')
        {
            _bmp->Delete(Omsg->Order_Id,2);
        }
       // cout << " Inside BidAsk 6 "<< Token <<endl;
		break;
	  }

            FinalPrice _pr = _bmp->ReturnPrice();

          //  cout << "BA Token " << _pr.Token << " BID "<< _pr.MAXBID << " ASK "<< _pr.MINASK<<" LTP "<< _pr.LTP<<endl;


            nanoObj.SendData(_pr);

           lock.unlock();
           // cout << " Inside BidAsk 13 "<< Token <<endl;
         //   the_condition_variable.notify_one();
            //cout << " Inside BidAsk 14 "<< Token <<endl;
        //cout<< "BIDASK " << "Token " << _FP.Token << " BID " << _FP.MAXBID << " ASK " << _FP.MINASK << endl;

        //cout << " " << endl;


        }




 void TrdMsg(Trade_Message *Tmsg)
    {

        boost::unique_lock<boost::mutex> lock(the_mutex);
        //cout << " Inside Trade Event "<<endl;

       // cout << "Trade Incoming Pack Token "<< Tmsg->Token << " MyToken "<< Token <<endl;


        _bmp->Delete(Tmsg->Buyorder_Id,1);

       // cout << " Deleted Buy Order from Trade Event "<<endl;

        _bmp->Delete(Tmsg->Selloder_Id,2);

//cout << " Deleted Sell Order from Trade Event "<<endl;

        FinalPrice _pr = _bmp->ReturnPrice(Tmsg->Trade_Price);
       // cout << "LTP Token " << _pr.Token << " BID "<< _pr.MAXBID << " ASK "<< _pr.MINASK<<" LTP "<< _pr.LTP<<endl;

//cout << " Preparing data from array "<<endl;

        ///Need to make sure Strategy recieves the depth in depth bidding
        nanoObj.SendData(_pr);
//cout << " Data sent "<<endl;
            lock.unlock();

         //   the_condition_variable.notify_one();

    }






void ClearData()
{
   // _BuySorter.clear();
   // _SellSorter.clear();
//    _SellPriceRecord_D.clear();
  //  _BuyPriceRecord_D.clear();
    cout <<"Data Cleared for Token " << Token <<endl;

}
~TokenWiseClass()
{
    //dtor
    delete _bmp;
}
};

#endif // TOKENWISECLASS_H_INCLUDED
