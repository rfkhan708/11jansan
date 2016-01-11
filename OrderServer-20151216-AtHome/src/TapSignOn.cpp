#include "TapSignOn.h"

#include "md5.h"

#include "../PFMap.h"
#include "ClientHandler.h"

#include <boost/thread/mutex.hpp>
#include<boost/thread/condition_variable.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/lexical_cast.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;


md5_byte_t check[16];

ClientHandler _ClientHandler;
/*
 string ipaddresstap;
 string _BrokerName;
 int _UserID;
 int _VersionNumber;
 string _Password;
 string _BrokerCode;
 short _BranchID;
    short _Port;
*/

PFHashMap _PFHashMap;

TapSignOn::TapSignOn()
{
    try
    {
        TapSignOn::PubSock = nn_socket(AF_SP,NN_PUB);

        nn_bind(TapSignOn::PubSock,"inproc://eventpubsub");

        memset(&pkt,0,sizeof(Packets));
        LastSeq=0;
    }
    catch(int e)
    {
        cout << " Error binding PublishSock from TapSignOn "<<endl<<endl;
    }
     try
    {
        TapSignOn::AdminSock = nn_socket(AF_SP,NN_PUSH);

        nn_bind(TapSignOn::AdminSock,"tcp://192.168.168.36:8011");

    }
    catch(int e)
    {
        cout << " Error binding PublishSock from TapSignOn "<<endl<<endl;
    }

  /*  try
    {
        time_t t;
        time(&t);
        //date_time = boost::posix_time::second_clock::local_time();
        string s = boost::lexical_cast<string>(t);
        cout << " Binary file name "<< s<<endl;
        filewriter = fopen(s.c_str(),"wb");
    }
    catch(int e)
    {
        cout << "Error opening file "<<endl;
    }*/



}

TapSignOn::~TapSignOn()
{
    //dtor
   // fclose(filewriter);
}

void TapSignOn::SendToExchange()
{

    PFHolder _out;

    while(true)
    {
       //sleep(1);
        //cout << "_FillData._Bidqueue.size() "<<_FillData._BidQueuesize()<<"\t"<< _Bidqueue.size() <<endl<<endl;
        //boost::mutex::scoped_lock lock(the_outmutex);
        if(pkt.InvitationCount > 0 &&  ( _FillData._BidQueuesize()> 0 || _FillData._MktQueuesize()>0 ) )
        {

             if(_FillData._MktQueuesize()>0)
             {
               _out= _Mktqueue.try_pop();
             }
             else if(_FillData._BidQueuesize() >0 )
             {
                //popped=_FillData._GetBidData(_out);
                _out = _Bidqueue.try_pop();
             }

              /*  if(_out.PF<=0)
                {
                    cout << " PF not found in Out queue"<<endl;
                }*/

               // if(_out.PF>-1)
               // {

                switch(_out._size)
                {

                    case 136:
                    {



                  /*  memset(&timer,0,sizeof(cpu_timer));
                    _PFHashMap.AddRecord(_out,false);
                    times=timer.elapsed();


                    cout<<"map wall :"<<times.wall<<" user :"<<times.user<<" system :"<<times.system<<endl;
*/





                        pkt.SeqNo++;

                       /* if(LastSeq+1 != pkt.SeqNo)
                        {
                            cout << " Seq Error LastSeq "<<LastSeq << " pkt.SeqNo " << pkt.SeqNo <<endl;
                        }
                        else
                        {
                            LastSeq = pkt.SeqNo;
                        }*/

                       // cout<<"\n \n ****** SendToExchange 136 Length= "<<ntohs(_out._oetr.Length) <<" tcode "<<ntohs(_out._oetr.TransactionCode) <<" UserId= "<< ntohl(_out._oetr.UserId)<< " pkt.SeqNo "<<pkt.SeqNo<<" Side "<<ntohs(_out._oetr.Buy_SellIndicator)  << " ***** "<<endl<<endl;

                        short pktLen=_out._size;
                        //memcpy(&pktLen,_out.buff,out._size);

                        _out._oetr.SequenceNumber=ntohl(pkt.SeqNo);

                        _out._oetr.Open_Close='O';

                    // memcpy(_out.oetr.SequenceNumber,&pkt.SeqNo,4);
                        pkt.md5CheckSum((const unsigned char*)&_out._oetr+24,pktLen-24,check);
                        memcpy(_out._oetr.CheckSum,&check,16);

                        sock.send(&_out._oetr,pktLen);

                        pkt.InvitationCount--;
                  // mtxwrite.lock();

//                   memset(&timer2,0,sizeof(cpu_timer));
                  _QueueHolder.push_back(_out);
  //                 times2=timer2.elapsed();

    //                cout<<"vector wall :"<<times2.wall<<" user2 :"<<times2.user<<" system2 :"<<times2.system<<endl;

               // mtxwrite.unlock();




                     //   cout<<"\n \n ****** Sent = "<<ntohs(_out._oetr.Length) <<" tcode "<<ntohs(_out._oetr.TransactionCode) <<" UserId= "<< ntohl(_out._oetr.UserId)<< " pkt.SeqNo "<<pkt.SeqNo<<" Side "<<ntohs(_out._oetr.Buy_SellIndicator)  << " ***** "<<endl<<endl;
                    }
                    break;
                    case 164:
                    {

                        pkt.SeqNo=pkt.SeqNo+1;
//cout<<"\n\n ****before SendToExchange 164 Modify Length= "<<ntohs(_out._omtr.Length) <<" tcode "<<ntohs(_out._omtr.TransactionCode) <<" UserId= "<< ntohl(_out._omtr.UserId)<< " pkt.SeqNo "<<pkt.SeqNo <<" Side "<<ntohs(_out._omtr.Buy_SellIndicator)<< " ***"<<endl<<endl;
                        /*if(LastSeq+1 != pkt.SeqNo)
                        {
                            cout << " Seq Error LastSeq+1  "<< LastSeq+1<< " pkt.SeqNo "<<pkt.SeqNo <<endl;
                        }
                        else
                        {
                            LastSeq = pkt.SeqNo;
                        }*/

                        short pktLen=_out._size;
                        //memcpy(&pktLen,_out.buff,out._size);

                        _out._omtr.SequenceNumber=ntohl(pkt.SeqNo);
                        _out._omtr.Open_Close ='O';
                    // memcpy(_out.oetr.SequenceNumber,&pkt.SeqNo,4);
                        pkt.md5CheckSum((const unsigned char*)&_out._omtr+24,pktLen-24,check);
                        memcpy(_out._omtr.CheckSum,&check,16);



                        //_PFHashMap.AddRecord(_out);

                        //Send to exchange

                        sock.send(&_out._omtr,pktLen);

                        pkt.InvitationCount--;
                        //cout<<"\n\n ****Sent = "<<ntohs(_out._omtr.Length) <<" tcode "<<ntohs(_out._omtr.TransactionCode) <<" UserId= "<< ntohl(_out._omtr.UserId)<< " pkt.SeqNo "<<pkt.SeqNo <<" Side "<<ntohs(_out._omtr.Buy_SellIndicator)<< " ***"<<endl<<endl;
                    }
                    break;
                }


                //}

        }
        // lock.unlock();
       // the_outcondition_variable.notify_one();
        //if(pkt.InvitationCount > 0 && )
    }
}

void TapSignOn::BindConnection()
{

    cout << " Thread 1 Started inside "<<endl;

    try
    {
        cout<<"\nSize MS_ERROR_RESPONSE = "<<sizeof(MS_ERROR_RESPONSE);

		const string servAddress =  "192.168.168.36";	// First arg: server address
		const unsigned short Port = 9601;
		sock.remoteAdd=servAddress;
        sock.remotePort= Port;
		sock.TcpInit();

		PacketFormat RecvPacketFormat;
		RecvPacket incomingPkt;

		int bytesReceived = 0;
		short recvPacketSize = 0;
		int counter = 0;
		short tCode = 0;



         MS_OE_RESPONSE_TR _OrderResp;
        memset(&_OrderResp,0,sizeof(MS_OE_RESPONSE_TR));

        char *Data;//[1024];
        Data = (char * )malloc(8 +sizeof( MS_OE_RESPONSE_TR ) );


        MS_TRADE_CONFIRM_TR _TOrderResp;
        memset(&_TOrderResp,0,sizeof(MS_TRADE_CONFIRM_TR));


        char *TData;
        TData = (char * )malloc(8 +sizeof( MS_TRADE_CONFIRM_TR ) );

        PFHolder _PH;
        memset(&_PH,0,sizeof(PFHolder));

        ClientUpdateMsg _clnt;
        memset(&_clnt,0,sizeof(ClientUpdateMsg));
        _clnt.TransectionCode=(short)((MessageType)eORDER);


		while (true)
		{

         //   boost::mutex::scoped_lock inlock(the_inmutex);

			bytesReceived = sock.recv(&RecvPacketFormat.Length,2);

			/*if (bytesReceived<0)
			{
				cerr << "Unable to read";
					//exit(1);
            }*/
				recvPacketSize = htons(RecvPacketFormat.Length);

				bytesReceived = sock.recv(&incomingPkt, recvPacketSize - 2);

				/*if (bytesReceived<0)
				{
					cerr << "Unable to read";
					//exit(1);
				}*/
				if (bytesReceived <= 0)
				{
					cout << "\nConnection Lost... press any key to continue ...";
					char ch;
					cin >> ch;
					exit(1);
				}

				memcpy(&tCode, incomingPkt.Data, 2);
				tCode = htons(tCode);
				//cout << "\n\n %%%%%%%% bytesReceived= " << bytesReceived << " tCode= " << tCode<<" %%%%%%"<<endl<<endl;
				switch (tCode)
				{
				case 15000:
				{
                    pkt.InvitationCount += 1;
							//  cout << "\n15000 Invitation Count Received";
				}
					break;


					case 17101:
					{

					 cout << "\n 17101 Cancel By  Exchange "<<endl;
					break;
					}

					case 17071:
					{

					 cout << "\n 17071 Cancel By  Exchange "<<endl;
					break;
					}
				case 2301:
				{
							 MS_SIGN_ON_OUT_2301 LoginConfirm;
							 memcpy(&LoginConfirm, incomingPkt.Data, sizeof(MS_SIGN_ON_OUT_2301));
							 cout << "\n\n2301  Received";
							 cout << "\n\nError Code= " << htons(LoginConfirm.Header.ErrorCode);
				}
					break;
				case 1601:
				{
							 cout << "\n\n1601  Received";

				}
					break;
				case 2321:
				{
                cout << "\n\n2321 Received";
				}
					break;


						case 2042:///Modify rejection
						case 2072:///Cancel Rejection
						case 2231:///order error
                        {
                        boost::mutex::scoped_lock lockhash(the_Hashmutex);
                        MS_OE_REQUEST Md_Obj;
                        memcpy(&Md_Obj, incomingPkt.Data, sizeof(MS_OE_REQUEST));
                        PFHolder _PH;
                        memset(&_PH,0,sizeof(PFHolder));

                        short errorcode =htons(Md_Obj.ErrorCode);

                        char *Data;//[1024];
                        Data = (char * )malloc(8 +sizeof( MS_OE_REQUEST ) );
                        if(Md_Obj.OrderNumber>0)
                        {
                        _PH =  _PFHashMap.GetRecordByOrder(Md_Obj.OrderNumber);
                        }
                        if(_PH.PF==0)
                        {
                         _PH = GetPF(Md_Obj.TokenNo,Md_Obj.Price,Md_Obj.Volume,Md_Obj.Buy_SellIndicator,Md_Obj.OrderNumber);
                        }
                        if(_PH.PF==0)
                        {
                            cout << "Break  From PF ZERO 2042 2072 Error Code " << errorcode <<" TokenNo :"<<htonl(Md_Obj.TokenNo)
                            <<" Price :"<<htonl(Md_Obj.Price)<<" Volume :"<< htonl(Md_Obj.Volume)<<" Buy_SellIndicator :"<<htons(Md_Obj.Buy_SellIndicator)<<endl;
                        }

                        if(_PH.PF>0)
                        {

                            memcpy(Data, &_PH.SubscriptionTag,8);
                            memcpy(Data + 8, &incomingPkt.Data,sizeof( MS_OE_REQUEST ) );
                            int ret = nn_send(TapSignOn::PubSock,Data,8+sizeof(MS_OE_REQUEST), 0);
                           /* ClientUpdateMsg _clnt;
                            memset(&_clnt,0,sizeof(ClientUpdateMsg));
                            _clnt.TransectionCode=(short)((MessageType)eORDER);
                            _clnt.ClintId= _PH.CID ;
                            _clnt.PF = _PH.PF;
                             memcpy(_clnt.buffer,&incomingPkt.Data,sizeof(MS_OE_REQUEST));
                            _ClientHandler.PushServerPacket(_clnt,sizeof(ClientUpdateMsg));*/
                            cout << "\n\2042  2072 2231 Received Tcode Code "<< tCode<<endl<<endl;


                            free(Data);
                        }
                        cout<< "\n\ **** ################################### ***** "<<endl<<endl;
                        cout << "\n\ **** 2042  2072 2231 Received Error Code ***** "<<ntohs(Md_Obj.ErrorCode)<<endl<<endl;
                        cout << "\n\ **** ################################### ***** "<<endl<<endl;
                        lockhash.unlock();
                        the_Hashcondition_variable.notify_one();
                        }

                    break;


					case 20073: //Order_Confirmation_IN_TR
					case 20075:
					case 20074:
					{

                        //boost::mutex::scoped_lock lockhash(the_Hashmutex);



                        memcpy(&_OrderResp,incomingPkt.Data,sizeof( MS_OE_RESPONSE_TR ) );




                        //long isend= 6520101000171;

                        //memcpy(Data, &isend, 8);



                        switch(tCode)
                        {
                            case 20073:
                                _PH = GetPF(_OrderResp.TokenNo,_OrderResp.Price,_OrderResp.Volume,_OrderResp.Buy_SellIndicator,_OrderResp.OrderNumber);

                           //cout<<" TokenNo "<<htonl(_OrderResp.TokenNo)<<" Volume "<<" Price "<<htonl(_OrderResp.Price)<<htonl(_OrderResp.Volume)<<" Buy_SellIndicator "<<htons(_OrderResp.Buy_SellIndicator)<<endl<<endl;
                            break;
                            case 20074:
                            case 20075:
                                _PH =  _PFHashMap.GetRecordByOrder(_OrderResp.OrderNumber);
                                break;
                        }

                        //cout << " Error code from cancellation "<< htons(_OrderResp.ErrorCode)<<endl;

                      //  cout << " Packet recieved in BindConnection 1 "<<endl<<endl;

                        if(_PH.PF>0)
                        {
                        memcpy(Data, &_PH.SubscriptionTag,8);
                        // cout << " Subscription tag TapSignOn "<< _PH.SubscriptionTag<<endl<<endl;
                        memcpy(Data + 8, &incomingPkt.Data,sizeof( MS_OE_RESPONSE_TR ) );
                       //cout << " Packet recieved in BindConnection 2 PF "<< _PH.PF<<" CID "<< _PH.CID
                        // <<" Side "<< ntohs(_OrderResp.Buy_SellIndicator) <<endl<<endl;

                            int ret = nn_send(TapSignOn::PubSock,Data,8+sizeof(MS_OE_RESPONSE_TR), 0);
                           // cout << " nn_send after "<<endl;

                               /* ClientUpdateMsg _clnt;
                                memset(&_clnt,0,sizeof(ClientUpdateMsg));
                                _clnt.TransectionCode=(short)((MessageType)eORDER);
                                _clnt.ClintId= _PH.CID ;
                                _clnt.PF = _PH.PF;
                                memcpy(_clnt.buffer,&incomingPkt.Data,sizeof(MS_OE_RESPONSE_TR));
                            _ClientHandler.PushServerPacket(_clnt,sizeof(ClientUpdateMsg));*/
                       // _ClientHandler.PushOrderPacket(Data,8+sizeof(MS_OE_RESPONSE_TR));

                        //free(Data);
                        }

                        if(_PH.PF==0)
                        {
                            cout << "Break  From PF ZERO Code "<< tCode << " OrderNumber "<< boost::lexical_cast<string>(_OrderResp.OrderNumber) << " Token "<< htonl(_OrderResp.TokenNo) << " Price "<< htonl(_OrderResp.Price)<< " Volume "<<htonl(_OrderResp.Volume) << " B/S "<< htons(_OrderResp.Buy_SellIndicator)  <<endl;



                            if(tCode==20073)
                            {
                                 cout << "Cancelling Order client unknown "<<endl;
                                _pfOrphan._size = 164;
                                _pfOrphan._omtr= ReturnCancelPack(_OrderResp);
                            //_PFHashMap.AddRecord(_pfMKTSell,false);
                                _FillData.MktQueue(_pfOrphan);

                            }

                        }
                        //cout << " PF "<<_RetVal.PF << " CID "<< _RetVal.CID<<endl;
                       // lockhash.unlock();
                       // the_Hashcondition_variable.notify_one();
					}

					break;
					case 20222:
					{


					 //boost::mutex::scoped_lock lockhash(the_Hashmutex);


                        memcpy(&_TOrderResp,incomingPkt.Data,sizeof( MS_TRADE_CONFIRM_TR ) );




                        _PH =  _PFHashMap.GetRecordByOrder(_TOrderResp.ResponseOrderNumber,ntohl(_TOrderResp.RemainingVolume));


                        if(_PH.PF>0)
                        {
                            memcpy(TData, &_PH.SubscriptionTag,8);
                            memcpy(TData + 8, &incomingPkt.Data,sizeof( MS_TRADE_CONFIRM_TR ) );

                          /*  cout << " Packet recieved in BindConnection 2 PF "<< _PH.PF<<" CID "<< _PH.CID
                            <<" Side "<< ntohs(_OrderResp.Buy_SellIndicator) <<endl<<endl;*/

                            int ret = nn_send(TapSignOn::PubSock,TData,8+sizeof(MS_TRADE_CONFIRM_TR), 0);


                            _clnt.ClintId= _PH.CID;
                            _clnt.PF = _PH.PF;
                            memcpy(_clnt.buffer,&incomingPkt.Data,sizeof(MS_TRADE_CONFIRM_TR));
                            //_ClientHandler.PushServerPacket(_clnt,sizeof(ClientUpdateMsg));


                       //_ClientHandler.PushOrderPacket(Data,8+sizeof(MS_OE_RESPONSE_TR));
                             nn_send(TapSignOn::AdminSock,&_clnt,sizeof(ClientUpdateMsg),0);

                        //free(Data);
                        }

                        if(_PH.PF==0)
                        {
                            cout << "Break  From PF ZERO 20222 OrderNumber "<< boost::lexical_cast<string>(_TOrderResp.ResponseOrderNumber) << " Token "<< htonl(_TOrderResp.Token) << " Price "<< htonl(_TOrderResp.Price)<< " Volume "<<htonl(_TOrderResp.OriginalVolume) << " B/S "<< htons(_TOrderResp.Buy_SellIndicator)  <<endl;
                        }

                      /*  if (filewriter)
                        {
                            ///http://www.codingunit.com/c-tutorial-binary-file-io
                            fwrite(&_OrderResp, sizeof(struct MS_TRADE_CONFIRM_TR), 1, filewriter);
                        }*/

                         //lockhash.unlock();
                        //the_Hashcondition_variable.notify_one();
                        }



					break;


				}

				counter++;

				switch (counter)
				{
				case 1:
				{
				/*"Hut@7895"*/
						  pkt.SeqNo = 1;
						  LastSeq = pkt.SeqNo;
						  MS_SIGN_ON_2300 Login = pkt.LoginPacket_2300(28823,"Aa@33333", "","12468", 4,93800, 0, "1234567","DIVYA PORTFOLIO PVT LTD");
						  sock.send(&Login, sizeof(MS_SIGN_ON_2300));
						  cout << "\nLogin Packet Sent"<<"2300 size ="<<sizeof(MS_SIGN_ON_2300)<<endl;

						  pkt.InvitationCount  -= 1;

                }
                break;
				case 2:
				{
                          sleep(1);
						  pkt.SeqNo = 2;
						  LastSeq = pkt.SeqNo;
						  MS_SYSTEM_INFO_REQ_1600 SystemInfo = pkt.SystemInfoRequestPacket_1600(28823);
						  sock.send(&SystemInfo, sizeof(MS_SYSTEM_INFO_REQ_1600));
						  cout << "\nSystemInfo Packet Sent"<<"1600 size ;"<<sizeof(MS_SYSTEM_INFO_REQ_1600)<<endl;
						  pkt.InvitationCount -= 1;
				}
					break;

                case 3:
                {
                   /* sleep(10);
                    SignOutRequest_2320 TapLogOut = pkt.Fun_SignOut_2320(32865, pkt.SeqNo+1);
                    sock.send(&TapLogOut, sizeof(SignOutRequest_2320));
                    cout << "\nLogOut Packet Sent";
                    pkt.InvitationCount = pkt.InvitationCount - 1;
                    */
                }
                break;

				}

                    //  inlock.unlock();
                    //    the_incondition_variable.notify_one();
			}
			cout << "\nFor Logout Enter 5";
			int logout;
			cin >> logout;

			if (logout == 5)
			{
				SignOutRequest_2320 TapLogOut = pkt.Fun_SignOut_2320(28823, pkt.SeqNo+1);
				sock.send(&TapLogOut, sizeof(SignOutRequest_2320));
				cout << "\nLogOut Packet Sent";
				pkt.InvitationCount = pkt.InvitationCount - 1;
			}


		}
		catch (SocketException &e)
		{
			cerr << e.what() << endl;
			exit(1);
		}
}

PFHolder TapSignOn::GetPF(int Token,int Price, int Qty, short side,double OrderNumber)
{

   /*
    StructHash _TrdHash;
    memset(&_TrdHash,0,sizeof(StructHash));

    _TrdHash.Token1= htonl(Token);
    _TrdHash.Price1 = htonl(Price);
    _TrdHash.Qty1 = htonl(Qty);
    _TrdHash.side1 = htons(side);

    //cout << "TapSignOn::GetPF Token "<< _TrdHash.Token1<< " Price "<<_TrdHash.Price1 << " Qty "<< _TrdHash.Qty1<<" Side "<< _TrdHash.side1<< " OrderNumber "<<OrderNumber<<endl<<endl;

    if(_TrdHash.Qty1==0)
        cout << " Error "<<endl;

    long HashKey = MyHash<StructHash>()(_TrdHash);*/
     PFHolder _RetVal;


        memset(&_RetVal ,0 ,sizeof(PFHolder));
        //_RetVal =  _PFHashMap.GetRecord(HashKey);

                mtxread.lock();
                std::vector<PFHolder>::iterator it = std::find_if(_QueueHolder.begin(),_QueueHolder.end(),
                FindByToken(Token,  Price, Qty, side ));

                if(it != _QueueHolder.end())
                {
                    _RetVal = *it;
                    _QueueHolder.erase(it);


                }


                mtxread.unlock();

        _RetVal.OrderNo = OrderNumber;
        _RetVal.HashKey=LastSeq;
        _PFHashMap.AddRecord(_RetVal,true);

    return _RetVal;

}

//char *
MS_OM_REQUEST_TR TapSignOn::ReturnCancelPack( MS_OE_RESPONSE_TR _OrderResp )
{

    MS_OM_REQUEST_TR obj2;
    memset(&obj2,0,sizeof(MS_OM_REQUEST_TR));

    obj2.TransactionCode=htons(20070);
    obj2.UserId=_OrderResp.UserId;
    obj2.Modified_CancelledBy='T';
    obj2.TokenNo=_OrderResp.TokenNo;
    strcpy(obj2.InstrumentName,_OrderResp.InstrumentName);
    //obj2.InstrumentName=_OrderResp.InstrumentName;
    strcpy(obj2.Symbol,_OrderResp.Symbol);
    //obj2.Symbol=_OrderResp.Symbol;
    obj2.ExpiryDate=_OrderResp.ExpiryDate;
    obj2.StrikePrice=_OrderResp.StrikePrice;
  //  obj2.OptionType=_OrderResp.OptionType;
     strcpy(obj2.OptionType,_OrderResp.OptionType);
    obj2.OrderNumber=_OrderResp.OrderNumber;
   // obj2.AccountNumber=_OrderResp.AccountNumber;
   strcpy(obj2.AccountNumber,_OrderResp.AccountNumber);
    obj2.BookType=_OrderResp.BookType;
    obj2.Buy_SellIndicator=_OrderResp.Buy_SellIndicator;
    obj2.DisclosedVolume=_OrderResp.DisclosedVolume;
    obj2.DisclosedVolumeRemaining=_OrderResp.DisclosedVolumeRemaining;
    obj2.TotalVolumeRemaining=_OrderResp.TotalVolumeRemaining;
    obj2.Volume=_OrderResp.Volume;
    obj2.VolumeFilledToday=_OrderResp.VolumeFilledToday;
    obj2.Price=_OrderResp.Price;
    obj2.GoodTillDate=_OrderResp.GoodTillDate;
    obj2.EntryDateTime=_OrderResp.EntryDateTime;
    obj2.LastModified=_OrderResp.LastModified;
     obj2.FlagIn=8;
     obj2.FlagOut=16;

    obj2.BranchId=_OrderResp.BranchId;
    obj2.TraderId=_OrderResp.TraderId;
   // obj2.BrokerId=_OrderResp.BrokerId;
   strcpy(obj2.BrokerId,_OrderResp.BrokerId);
    obj2.Open_Close=_OrderResp.Open_Close;
    //obj2.Settlor=_OrderResp.Settlor;
    strcpy(obj2.Settlor,_OrderResp.Settlor);
    obj2.Pro_ClientIndicator=_OrderResp.Pro_ClientIndicator;
    obj2.Reservedadd=_OrderResp.Reserved1l;
    obj2.filler=_OrderResp.filler;
    obj2.nnffield=_OrderResp.nnffield;

    obj2.Length =ntohs(sizeof(MS_OM_REQUEST_TR));
	obj2.MsgCount =ntohs(1);



    return obj2;
}



void TapSignOn::Init()
{

    cout << " Thread 1 Started "<<endl;
    _inDataThread = new boost::thread(&TapSignOn::BindConnection, this);
    producer_threads.add_thread(_inDataThread);

    cout << " Thread 2 Started "<<endl;
    _outDataThread = new boost::thread(&TapSignOn::SendToExchange, this);
    producer_threads.add_thread(_outDataThread);


}


