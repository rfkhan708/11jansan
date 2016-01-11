#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "Structure.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include "TokenWiseClass.h"
#include <boost/thread/thread.hpp>
#include "Socket.h"

using namespace std;

using namespace boost::signals2;

using namespace boost;

vector<struct Contract_File> cimlist;
map<int , TokenWiseClass*> _map;
boost::signals2::signal<void()> SigA;
boost::thread_group producer_threads;
const int MAXRCVSTRING = 255;

void GenerateTokenList()
{
    Contract_File obj;

    //ifstream file("/home/shailendra/Desktop/contract.txt");//Laptop
    ifstream file("/root/NNFServerTest/contract.txt");//36
    //ifstream file("/root/Nitin/NNFServerTest/contract.txt");//226
    // ifstream file("/root/Server_23/NNFServerTest/contract.txt");//226
     //ccount++;
//cout<< ccount<< endl;
	string linebuffer;
//	ccount++;
//cout<< ccount<< endl;
        char delimiter =',';
//        ccount++;
//cout<< ccount <<" b4 while "<< endl;
        while (file && getline(file, linebuffer)){


		if (linebuffer.length() == 0)continue;
//		ccount++;
//cout<< ccount<< endl;
               vector<string> result;
               string acc = "";

               for(int i = 0; i < linebuffer.size(); i++)
                {
                    if(linebuffer[i] == delimiter)
                    {
                        if(acc.length()>1)
                             result.push_back(acc);
                        acc = "";
                     }
                    else
                        acc += linebuffer[i];
                }
               if(result.size()> 1)
               {

                    obj.Token =atoi(result[0].c_str());
                    obj.AssetToken = atoi(result[1].c_str());
                    obj.InstrumentName = result[2];
                    obj.Symbol= result[3];
                    obj.Series= result[4];
                    obj.ExpiryDate = atoi(result[5].c_str());
                    obj.StrikePrice =atoi(result[6].c_str());
                    obj.OptionType = result[7];
                    obj.BoardLotQuantity =atoi( result[11].c_str());
                    obj.Name = result[18];
                    obj.EGMAGM = result[19];
                    cimlist.push_back(obj);

               }



             }
}


void BindClass()
{
    int Counter=0;

        for (vector<Contract_File>::iterator it2 = cimlist.begin(); it2 != cimlist.end(); it2++)
        {
            //printf("Token BLQ %u %u \n", it2->Token, it2->BoardLotQuantity );

            _map[it2->Token]= new TokenWiseClass(it2->Token);


  //s.connect(bind(&Testclass::hello,&_tclass));
            SigA.connect(bind(&TokenWiseClass::ClearData,_map[it2->Token]));

            Counter++;
            //TokenWiseClass &ob=*new TokenWiseClass(it2->Token);

           // _map[it2->Token]=&ob;

//cout<<"map :"<<_map.begin()->first<<endl;

        }


}

void BindStream(unsigned short StreamID, string multicastAddress,string LanIp,unsigned short port)
{

cout << " multicastAddress " << multicastAddress << " LanIp " << LanIp << " port " << port << " StreamID " << StreamID <<endl;
 //string multicastAddress ="239.70.70.21";//st_obj.multicastAddress-1; //"233.1.4.3";       // First arg:  multicast address
//  string LanIp="192.168.100.105";//st_obj.LanIp;//"192.168.168.36";
//  unsigned short port =4444;//atoi(st_obj.port);     // Second arg:  port

  try {
    UDPSocket sock(port);

    sock.joinGroup(multicastAddress,LanIp);

    char recvString[MAXRCVSTRING + 1];// Buffer for echo string + \0
    string sourceAddress;             // Address of datagram source
    unsigned short sourcePort;        // Port of datagram source

    //short msglen=0 ;
   // int seq=0;
    int LastSeq=0;
		STREAM_HEADER* _stmHd = (STREAM_HEADER*)calloc(8, sizeof(STREAM_HEADER));
		Order_Message* _OrdMsg = (Order_Message*)calloc(38, sizeof(Order_Message));
		Trade_Message* _TrdMsg = (Trade_Message*)calloc(45, sizeof(Trade_Message));

		for (;;)
		{
			int bytesRcvd = sock.recvFrom(recvString, MAXRCVSTRING, sourceAddress,
				sourcePort);
			recvString[bytesRcvd] = '\0';     // Terminate string

			memset(_stmHd, 0, sizeof(STREAM_HEADER));
			memcpy(_stmHd, recvString, sizeof(STREAM_HEADER));

			if(_stmHd->msg_len <=13)
			{
                cout << "HeartBeat recieved" << endl;
                FinalPrice _FPHB;
                memset (&_FPHB ,0, sizeof(FinalPrice));
                _FPHB.Token=111;
                _FPHB.sub_token=111;
                _FPHB.MAXBID=-1;
                _FPHB.MINASK=-1;
                _FPHB.LTP=-1;
                 nanoObj.SendData(_FPHB);
                continue;
            }

           // cout << "LastSeq " << LastSeq<< endl;
			if (LastSeq + 1 == _stmHd->seq_no)
			{
				LastSeq++;
				if (_stmHd->msg_len == 38)//NMX
				{
					memset(_OrdMsg, 0, sizeof(Order_Message));
					memcpy(_OrdMsg, recvString, sizeof(Order_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_OrdMsg->Token);

					if (_val != _map.end())
					{
					//cout << "NMX ORD NO " << (long long)_OrdMsg->Order_Id << " MsgType " << _OrdMsg->Message_Type << endl;
                   // cout<<"_val :"<<_val->second->Token<<endl;
						_val->second->BidAsk(_OrdMsg);


					}
					//delete _OrdMsg;
				}
				else if (_stmHd->msg_len == 45)//T
				{
					memset(_TrdMsg, 0, sizeof(Trade_Message));
					memcpy(_TrdMsg, recvString, sizeof(Trade_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_TrdMsg->Token);

					if (_val != _map.end())
					//if(_TrdMsg->Token==65937)
					{
					//	cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
						_val->second->TrdMsg(_TrdMsg);

						//_map[_TrdMsg->Token]->TrdMsg(_TrdMsg);
					//	cout<<"event calling here..."<<endl;

					}
					//cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
					//delete _TrdMsg;
				}
			}//Seq No Check
			else
			{
			 // SigA();
              LastSeq = _stmHd->seq_no;
         // cout << "Recovery occurred . Raising event to all classes to clear Holder" << endl;
        //Raise event from here to clear all the Data from Holder.
			}
			//cout << "Structure Data msg_len "<< _stmHd->msg_len << " SeqNo " << _stmHd->seq_no << " StreamID " << _stmHd->stream_id<<endl;
			//delete _stmHd;
			// cout << "Received " << recvString << " from " << sourceAddress << ": "
			// << sourcePort << endl;
		}
  }
   catch (SocketException &e)
  {
    cerr << e.what() << endl;
    //exit(1);
  }
}


int main()
{
        cout << "Generating Token List" << endl;

        GenerateTokenList();

        cout << "Binding class " << endl;

        BindClass();

        nanoObj.BindNanoObj();

        cout <<"How many streams do you want to read ?"<< endl;

        int StreamCounter=0;

         cin >> StreamCounter;

        if(StreamCounter > 5 )
            StreamCounter=5;

//string multicastAddress ="239.70.70.21";//st_obj.multicastAddress-1; //"233.1.4.3";       // First arg:  multicast address
//  string LanIp="192.168.100.105";//st_obj.LanIp;//"192.168.168.36";
//  unsigned short port =4444;//atoi(st_obj.port);     // Second arg:  port

        //string LanIP ="10.230.60.61"; //227
       // string LanIP ="10.230.145.8"; //226
        string LanIP ="10.230.145.13"; //36

        for(int icount = 0 ; icount < StreamCounter ; icount++)
        {
            cout << "Stream Count " << icount << endl<<endl;
           // producer_threads.create_thread(boost::bind(BindStream,boost.cref(McastAddr)));
           //producer_threads.add_thread(new boost::thread(BindStream,icount, "239.70.70.25","10.230.60.61",10825));
            if(icount==0)
           producer_threads.add_thread(new boost::thread(BindStream,icount, "239.70.70.21",LanIP,10821));
           else if(icount==1)
           producer_threads.add_thread(new boost::thread(BindStream,icount, "239.70.70.24",LanIP,10824));
            else if(icount==2)
           producer_threads.add_thread(new boost::thread(BindStream,icount, "239.70.70.23",LanIP,10823));
           else if(icount==3)
           producer_threads.add_thread(new boost::thread(BindStream,icount, "239.70.70.22",LanIP,10822));

           sleep(1);

        }

        //producer_threads.join_all();
//boost::thread_group producer_threads[4];

    cout << "joining all stream threads" << endl;
   producer_threads.join_all();




    return 0;
}
