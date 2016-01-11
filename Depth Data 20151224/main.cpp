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

#include <boost/algorithm/string.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>


//Socket Geader
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

#define MAX_LEN  1024


#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

using namespace std;

using namespace boost::signals2;

using namespace boost;



map<int, int > _PClose;

map<int , TokenWiseClass*> _map;
boost::signals2::signal<void()> SigA;
boost::thread_group producer_threads;
const int MAXRCVSTRING = 255;




long long concat(long long x, long long y)
{
    long long temp = y;
    while (y != 0) {
        x *= 10;
        y /= 10;
    }
    return x + temp;
}


class MatchID
{
    double _ID;

public:
    MatchID(const double &ID) : _ID(ID) {}

    bool operator()(const Order_Message &item) const
    {
        return item.Order_Id == _ID;
    }
};

void fileReader_Fun()
{
        ifstream myfile;
        string line;
		string row[100];
		string acc = "";
		int cnt = 0;
myfile.open("Bhav.txt");

        std::vector<string> splt;


        while (getline(myfile, line))
            {



                boost::split(splt, line,boost::is_any_of(",") );

                if(splt.size()>1)
                {
                    _PClose[atoi(splt[0].c_str())]=atoi(splt[1].c_str());

                }




            }
            myfile.close();
}

 void Eventsubscriber (boost::property_tree::ptree pt)
 {

    int ClientIdAuto=0;
    int sock;
	const char* addr = "ipc:///tmp/eventpubsub.ipc";

	cout << "Eventsubscriber Start: "<<addr<<"  ClientIdAuto: "<<ClientIdAuto<<endl;

	int* msg1;

	sock = nn_socket(AF_SP, NN_SUB);

	//int setBufSize= 32 * 1024 * 1024 ;

	//nn_setsockopt(sock,NN_SOL_SOCKET,NN_RCVBUF,&setBufSize,sizeof(setBufSize));

        cout << "After socket connection" << endl;
        std::string mx = pt.get<std::string>("DATA.MAXCLIENT");

        short MAXCLIENT =boost::lexical_cast<short>(mx) ;

        cout << "MAXCLIENT " << MAXCLIENT << endl<< endl;
        long lng=0;
        for(short imax = 0 ; imax <= MAXCLIENT; imax++)
        {
        //boost::lexical_cast<short>(
       // cout << endl <<"before CID " << endl;
            ClientIdAuto =boost::lexical_cast<long>(pt.get<std::string>("DATA.CLIENT" +boost::lexical_cast<std::string>(imax)));
           // cout << "ClientIdAuto " << ClientIdAuto <<"(short)(MessageType)eIOCPAIR :"<<(short)(MessageType)eIOCPAIR<< endl<< endl;
            lng =concat((short)(MessageType)eFOPAIR,ClientIdAuto);
            //lng =concat(2300,2010100020);
            nn_setsockopt(sock, NN_SUB,NN_SUB_SUBSCRIBE,&lng , sizeof(lng));

           //  cout << "ClientIdAuto " << ClientIdAuto <<"(short)(MessageType)eIOCPAIR :"<<(short)(MessageType)eIOCPAIR<< endl<< endl;

            lng =concat((short)(MessageType)eIOCPAIR,ClientIdAuto);
            nn_setsockopt(sock, NN_SUB,NN_SUB_SUBSCRIBE,&lng , sizeof(lng));

             cout << "lng :"<<lng << endl<< endl;
        }




/// Call the reading routine here and put data in _PClose<int ,double>

fileReader_Fun();



	nn_connect(sock, addr) ;

	char _buffer[1024];
	char buffer[1024];

	short TransCode;

	 while (true)
	 {


        	memset(&_buffer,0,1024);

	        int size = nn_recv(sock, _buffer,1024, 0);

	        if(size<1)
            {
				cout << "Some error occured in eventubsub"<<endl<<endl;
			        continue;
            }



	 	 long suId;

       		suId=*(long*)_buffer;

           // short MsgType =suId/1000000000000000;
            short MsgType =suId/1000;

	 	 memset(buffer,0,1024);
	 	 memcpy(buffer,_buffer+8,size-8);

		switch ((MessageType)MsgType)//(MessageType)BitConverter.ToInt16(_IncomingData, 0))
		{

			case (MessageType)eFOPAIR:
            {
                strFOPAIR _FOpairObj;
                memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));

                if(_map.find(_FOpairObj.TokenFar)==_map.end())
                {
                    _map[_FOpairObj.TokenFar]= new TokenWiseClass(_FOpairObj.TokenFar,_PClose[_FOpairObj.TokenFar] );
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenFar]));
                }
                 if(_map.find(_FOpairObj.TokenNear)==_map.end())
                {
                    _map[_FOpairObj.TokenNear]= new TokenWiseClass(_FOpairObj.TokenNear,_PClose[_FOpairObj.TokenNear]);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenNear]));
                }

                cout << "FO Subscription recieved  " << endl;
                break;
            }

            case (MessageType)eIOCPAIR:
            {
                FOPAIRLEG2 _FOpairObj;
                memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));

                if(_map.find(_FOpairObj.TokenNear)==_map.end())
                {
                    _map[_FOpairObj.TokenNear]= new TokenWiseClass(_FOpairObj.TokenNear,_PClose[_FOpairObj.TokenNear]);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenNear]));
                }
                 if(_map.find(_FOpairObj.TokenFar)==_map.end())
                {
                    _map[_FOpairObj.TokenFar]= new TokenWiseClass(_FOpairObj.TokenFar,_PClose[_FOpairObj.TokenFar]);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenFar]));
                }
                if(_FOpairObj.TokenThree > 0)
                if(_map.find(_FOpairObj.TokenThree)==_map.end())
                {
                    _map[_FOpairObj.TokenThree]= new TokenWiseClass(_FOpairObj.TokenThree,_PClose[_FOpairObj.TokenThree]);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenThree]));
                }
               /* if(_FOpairObj.Token4 > 0)
                if(_map.find(_FOpairObj.Token4)==_map.end())
                {
                    _map[_FOpairObj.Token4]= new TokenWiseClass(_FOpairObj.Token4);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.Token4]));
                }*/
                cout << "IOC Subscription recieved  " << endl;
                break;
            }

				break;
			default:
				break;
		}


}
}


void BindStreamAlternate(unsigned short StreamID, string multicastAddress,string LanIp,unsigned short port)
{

    cout << " multicastAddress " << multicastAddress << " LanIp " << LanIp << " port " << port << " StreamID " << StreamID <<endl;
    int sock;
	int flag_on = 1;
	struct sockaddr_in multicast_addr;
	char message_received[MAX_LEN+1];
	int msgrecv_len;
	struct ip_mreq mc_req;

	struct sockaddr_in from_addr;
	unsigned int from_len;

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("socket() failed");
		exit(1);
	}


	if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag_on,
	sizeof(flag_on))) < 0)
	{
		perror("setsockopt() failed");
		exit(1);
	}


	memset(&multicast_addr, 0, sizeof(multicast_addr));
	multicast_addr.sin_family      = AF_INET;
	multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	multicast_addr.sin_port        = htons(port);


	if ((::bind(sock, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr))) < 0)
	{
		perror("bind() failed");
		exit(1);
	}


	mc_req.imr_multiaddr.s_addr = inet_addr(multicastAddress.c_str());
	mc_req.imr_interface.s_addr = inet_addr(LanIp.c_str());
                                    //htonl(INADDR_ANY);


	if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &mc_req, sizeof(mc_req))) < 0)
	{
		perror("setsockopt() failed");
		exit(1);
	}

	vector<Order_Message> _QueueHolder;

    int LastSeq=0;

	STREAM_HEADER* _stmHd = (STREAM_HEADER*)calloc(8, sizeof(STREAM_HEADER));
	Order_Message* _OrdMsg = (Order_Message*)calloc(38, sizeof(Order_Message));
	Trade_Message* _TrdMsg = (Trade_Message*)calloc(45, sizeof(Trade_Message));


    while(1)
	{
		memset(message_received, 0, sizeof(message_received));
		from_len = sizeof(from_addr);
		memset(&from_addr, 0, from_len);


		if ((msgrecv_len = recvfrom(sock, message_received, MAX_LEN, 0, (struct sockaddr*)&from_addr, &from_len)) < 0)
		{
			perror("recvfrom() failed");
			break;
		}

            memset(_stmHd, 0, sizeof(STREAM_HEADER));
			memcpy(_stmHd, message_received, sizeof(STREAM_HEADER));

			if(_stmHd->msg_len <=13)
			{
                cout << "HeartBeat recieved" << endl;

                continue;
            }

           // cout << "LastSeq " << LastSeq<< endl;
			if (LastSeq + 1 == _stmHd->seq_no)
			{

				LastSeq++;

				if (_stmHd->msg_len == 38)//NMX
				{

					memset(_OrdMsg, 0, sizeof(Order_Message));
					memcpy(_OrdMsg, message_received, sizeof(Order_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_OrdMsg->Token);

					if (_val != _map.end())
					{

                        _val->second->BidAsk(_OrdMsg);
                      //  cout << " Token "<< _OrdMsg->Token << " Price "<< _OrdMsg->Price<< " Side "<< _OrdMsg->Order_Type<<endl;
                      //  cout << "BA Token received in packet "<< _OrdMsg->Token << " Token in class "<< _val->second->Token<< " Token in map "<< _val->first<<endl;

                    }

					//delete _OrdMsg;
				}
				else if (_stmHd->msg_len == 45)//T
				{



					memset(_TrdMsg, 0, sizeof(Trade_Message));
					memcpy(_TrdMsg, message_received, sizeof(Trade_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_TrdMsg->Token);

					if (_val != _map.end())
					{
                        _val->second->TrdMsg(_TrdMsg);
                       // cout << "LTP Token received in packet "<< _TrdMsg->Token << " Token in class "<< _val->second->Token << " Token in map "<< _val->first <<endl;
                    }


					//cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
					//delete _TrdMsg;
				}
			}//Seq No Check
			else
			{
			 // SigA();
              LastSeq = _stmHd->seq_no;
             // cout << " Sequence Error "<<endl;
         // cout << "Recovery occurred . Raising event to all classes to clear Holder" << endl;
        //Raise event from here to clear all the Data from Holder.
			}




    }

	if ((setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*) &mc_req, sizeof(mc_req))) < 0)
	{
		perror("setsockopt() failed");
		exit(1);
	}
	close(sock);

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

    vector<Order_Message> _QueueHolder;

    int LastSeq=0;

    boost::mutex the_mutex;

	STREAM_HEADER* _stmHd = (STREAM_HEADER*)calloc(8, sizeof(STREAM_HEADER));
	Order_Message* _OrdMsg = (Order_Message*)calloc(38, sizeof(Order_Message));
	Trade_Message* _TrdMsg = (Trade_Message*)calloc(45, sizeof(Trade_Message));

		for (;;)
		{
            boost::unique_lock<boost::mutex> lock(the_mutex);
            memset(&recvString,0,sizeof(recvString));

			int bytesRcvd = sock.recvFrom(recvString, MAXRCVSTRING, sourceAddress,sourcePort);

			recvString[bytesRcvd] = '\0';     // Terminate string

			memset(_stmHd, 0, sizeof(STREAM_HEADER));
			memcpy(_stmHd, recvString, sizeof(STREAM_HEADER));

			if(_stmHd->msg_len <=13)
			{
                cout << "HeartBeat recieved" << endl;

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

                        _val->second->BidAsk(_OrdMsg);
                      //  cout << " Token "<< _OrdMsg->Token << " Price "<< _OrdMsg->Price<< " Side "<< _OrdMsg->Order_Type<<endl;
                      //  cout << "BA Token received in packet "<< _OrdMsg->Token << " Token in class "<< _val->second->Token<< " Token in map "<< _val->first<<endl;

                    }

					//delete _OrdMsg;
				}
				else if (_stmHd->msg_len == 45)//T
				{



					memset(_TrdMsg, 0, sizeof(Trade_Message));
					memcpy(_TrdMsg, recvString, sizeof(Trade_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_TrdMsg->Token);

					if (_val != _map.end())
					{
                        _val->second->TrdMsg(_TrdMsg);
                       // cout << "LTP Token received in packet "<< _TrdMsg->Token << " Token in class "<< _val->second->Token << " Token in map "<< _val->first <<endl;
                    }


					//cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
					//delete _TrdMsg;
				}
			}//Seq No Check
			else
			{
			 // SigA();
              LastSeq = _stmHd->seq_no;
             // cout << " Sequence Error "<<endl;
         // cout << "Recovery occurred . Raising event to all classes to clear Holder" << endl;
        //Raise event from here to clear all the Data from Holder.
			}
			//cout << "Structure Data msg_len "<< _stmHd->msg_len << " SeqNo " << _stmHd->seq_no << " StreamID " << _stmHd->stream_id<<endl;
			//delete _stmHd;
			// cout << "Received " << recvString << " from " << sourceAddress << ": "
			// << sourcePort << endl;
			lock.unlock();
		}
  }
   catch (SocketException &e)
  {
    cerr << e.what() << endl;
    //exit(1);
  }
}

string SECTION;
string CONTRACTFILEPATH;
string LanIP;
string MCASTIP;
string MPORT;
string counter;
string DATAIP;
short MCASTPORT;
boost::property_tree::ptree pt;

int main()
{



 boost::property_tree::ini_parser::read_ini("settings.ini", pt);
 producer_threads.add_thread(new boost::thread(Eventsubscriber,pt));

 sleep(2);

        SECTION = pt.get<std::string>("SECTION.ID");
      //CONTRACTFILEPATH =pt.get<std::string>(SECTION +".CONTRACTFILEPATH");
        DATAIP = pt.get<std::string>("DATA.BCASTADDR");
        LanIP = pt.get<std::string>("DATA.LANIP");
        //cout << "Generating Token List" << endl;
       // GenerateTokenList(CONTRACTFILEPATH);
        cout << "Binding class " << endl;
        //BindClass();
        nanoObj.BindNanoObj(DATAIP);
        cout <<"How many streams do you want to read ?"<< endl;

        int StreamCounter=0;

         cin >> StreamCounter;


        for(int icount = 0 ; icount < StreamCounter ; icount++)
        {
          // counter = patch::to_string(icount);
           counter = boost::lexical_cast<std::string>(icount);



            MCASTIP =  pt.get<std::string>("DATA.MCASTIP" + counter);



            MPORT=  pt.get<std::string>("DATA.MCASTPORT" + counter);



            MCASTPORT =boost::lexical_cast<short>(MPORT);


            producer_threads.add_thread(new boost::thread(BindStreamAlternate,icount, MCASTIP ,LanIP,MCASTPORT));
            cout << " Thread " << icount << " started for port " << MPORT << endl;
            sleep(1);

        }

        //producer_threads.join_all();
//boost::thread_group producer_threads[4];

    cout << "joining all stream threads" << endl;
   producer_threads.join_all();




    return 0;
}
