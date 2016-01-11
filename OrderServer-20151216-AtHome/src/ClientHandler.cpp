#include "ClientHandler.h"
#include "StaticClass.h"
#include <string.h>
#include <vector>
#include <boost/foreach.hpp>

#include "../PFMap.h"
//http://www.boost.org/doc/libs/1_59_0/doc/html/lockfree/examples.html#lockfree.examples.waitfree_single_producer_single_consumer_queue
//http://www.bogotobogo.com/cplusplus/Boost/boost_AsynchIO_asio_tcpip_socket_server_client_timer_bind_handler_multithreading_synchronizing_network_D.php
//http://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/tutorial/tutdaytime3.html

  concurrent_queue _Bidqueue;
  concurrent_queue _Mktqueue;
  Contract_queue _Contractqueue;

vector<ContractDetails> _ContMap;

void ClientHandler::HeartBeatEventHandler(char* buffer)
{

}
void ClientHandler::LoginEventHandler(char* buffer)
{

}
void ClientHandler::OrderEventHandler(char* buffer)
{

}
ClientHandler::ClientHandler()
{
    //ctor



    OnLogin.connect(bind(&ClientHandler::LoginEventHandler,this,"abcd"));
    OnHeartBeat.connect(bind(&ClientHandler::HeartBeatEventHandler,this,"ass"));
    OnOrder.connect(bind(&ClientHandler::OrderEventHandler,this,"asd"));

    try
    {
        ClientHandler::ReplySock = nn_socket(AF_SP, NN_REP);

        nn_bind(ClientHandler::ReplySock, "tcp://192.168.168.36:8003");// (char*) ("tcp://"+  _GlobalDefs.ConnectionIP +":8003").c_str());
    }
    catch(int e)
    {
        cout << " Error binding ReplySock"<<endl<<endl;
    }

    try
    {
        ClientHandler::PubSock = nn_socket(AF_SP,NN_PUB);

        nn_bind(ClientHandler::PubSock,"tcp://192.168.100.36:8002");
    }
    catch(int e)
    {
        cout << " Error binding PublishSock from ClientHandler"<<endl<<endl;
    }


       // cout << "PassRefOfMethod called " << endl;


    _EventThread = new boost::thread(&ClientHandler::RecieveDataAsServer, this);

cout << " Contract loaded successfully "<< cimlist.size() << endl<<endl;

    IPCSock = nn_socket(AF_SP,NN_PUB);

   int ibind = nn_bind(IPCSock,"ipc:///tmp/eventpubsub.ipc");


cout <<" IPC sock started"<<endl;
}

ClientHandler::~ClientHandler()
{
    //dtor


}
/*void ClientHandler::PublishPackToDataServer(PFUpdateMsg Data,int _size)
{
     int ret = nn_send(ClientHandler::IPCSock,(void*)&Data,_size, 0);
    if(ret)
    cout << "PublishPackToDataServer sent"<<endl;
    else
    cout << "PublishPackToDataServer failed"<<endl;

}*/
void ClientHandler::PublishPackToDataServer(PFUpdateMsg Data,int _size)
{
     int ret = nn_send(ClientHandler::IPCSock,(void*)&Data,_size, 0);
    if(ret)
    cout << "PublishPackToDataServer sent"<<endl;
    else
    cout << "PublishPackToDataServer failed"<<endl;

}

void ClientHandler::Login(char* obj)
{
    C_SignIn _loginpack;
    memset(&_loginpack,0,sizeof(C_SignIn));
    memcpy(&_loginpack,obj,sizeof(C_SignIn));

    cout<< " Login Method raised from event "<<endl<<endl;
    cout <<" ============================== "<<endl<<endl;
    cout << " StgType\t" << _loginpack.StgType<<endl<<endl;
    cout <<" ============================== "<<endl<<endl;

    switch(_loginpack.TransectionCode)
    {
        case 2320:
        {
            cout <<"Client Logout requested"<<endl<<endl;
            map<int,AutoClient*>::iterator _stgRes = _StgMap.find(_loginpack.ClintId);
            if(_stgRes!=_StgMap.end())
            {
                _stgRes->second->Dispose();

                _stgRes->second = NULL;

                _StgMap.erase(_stgRes->first);
                cout << "Strategy Instance found for CleintID " << _loginpack.ClintId << " Dispose called for the same " << endl;
            }
            else
            {
                cout << "Strategy Instance not found for ClientID " << _loginpack.ClintId << " you must dispose it to run new instance for the same id"<< endl;
            }
            break;
        }
        default:
        {
             map<int,AutoClient*>::iterator _stgRes = _StgMap.find(_loginpack.ClintId);

            if(_stgRes!=_StgMap.end())
            {
                 _stgRes->second->Dispose();

                _stgRes->second = NULL;

                _StgMap.erase(_stgRes->first);
                cout << "Current ID is already mapped with a running stg " << _loginpack.ClintId << endl;
                return;
            }
            else
            {
                _StgMap[_loginpack.ClintId]= new AutoClient();
                cout << "Creating Strategy Instance for CleintID " << _loginpack.ClintId << endl;
                AutoStgID++;
                sleep(1);
            }

            _StgMap[_loginpack.ClintId]->AutoCountID = AutoStgID;
            _StgMap[_loginpack.ClintId]->ClientIdAuto=_loginpack.ClintId;
            _StgMap[_loginpack.ClintId]->ClientIdAlgo = 201010001823000;



            _StgMap[_loginpack.ClintId]->InItClass(cimlist);



            ClientUpdateMsg _cmsg;
            memset(&_cmsg,0,sizeof(ClientUpdateMsg));
            _cmsg.TransectionCode = (short)eLOGIN;
            _cmsg.ClintId = _loginpack.ClintId;

           // _cmsg._csign.TransectionCode = _loginpack.TransectionCode;
           _loginpack.Status = (short)LogInStatus(LogIn);


           // _cmsg._csign.Status=(short)LogInStatus(LogIn);
            memcpy(_cmsg.buffer ,&_loginpack,sizeof(C_SignIn));

         int _sizeFE =  nn_send(_StgMap[_loginpack.ClintId]->FESock,&_cmsg,sizeof(ClientUpdateMsg),0);



  if(!loadCimList)
    {
        Contract_Filefun("");
        loadCimList= true;
    }

    cout << " Contract loaded successfully "<< cimlist.size() << endl<<endl;
    _StgMap[_loginpack.ClintId]->InItCim(cimlist);

        cout << " Data length sent to FE "<< _sizeFE <<endl;
          //  _StgMap[_loginpack.ClintId]->SendClientPacket(_cmsg,sizeof(ClientUpdateMsg));
           // ClientHandler::PushServerPacket(_cmsg,sizeof(ClientUpdateMsg));

            break;
        }

    }

}
void ClientHandler::PushOrderPacket(unsigned char * Data, int _size)
{
    int ret = nn_send(ClientHandler::PubSock,(void*)Data,_size, 0);
    if(ret)
    cout << "PushOrderPacket sent"<<endl;
    else
    cout << "PushOrderPacket failed"<<endl;
}

void ClientHandler::PushServerPacket(ClientUpdateMsg buff,int _size)
{
   /* char shortBuffer[5];
    char cidBuffer[10];
    char _tempBuffer[1024];

     sprintf(cidBuffer,"%ld",CID);
    sprintf(shortBuffer,"%d",MT);

    memcpy(_tempBuffer,shortBuffer,2);
    memcpy(_tempBuffer+2,cidBuffer,8);
    memcpy(_tempBuffer+10,buff,sizeof(C_SignIn));
    */



  ///  int ret = nn_send(ClientHandler::PubSock,(void*)&buff,_size, 0);


   /* if(ret)
    cout << "Login Confirmation sent"<<endl;
    else
    cout << "Login Confirmation failed"<<endl;*/
}
long long concat(long long x,long long y)
{
    long long temp = y;
    while (y != 0) {
        x *= 10;
        y /= 10;
    }
    return x + temp;
}

void ClientHandler::RecieveDataAsServer()
{
    cout << " ClientHandler started"<<endl<<endl;



    try
    {

        char buffer[1024];
        char _buffCut[1024];
        short tCode=0;
        while(true)
        {
            memset(&buffer,0,1024);

	        int _size = nn_recv(ReplySock, buffer,1024, 0);

	        if(_size<1)
            {
				cout << "Some error occured in eventubsub"<<endl<<endl;
			        continue;
            }

            InHeader _header;

            memset(&_header,0,sizeof(InHeader));

            memcpy(&_header,buffer,sizeof(InHeader));

           // cout << " Client Certificate ID "<< _header.ClintId << " TransectionCode "<<_header.TransectionCode<<endl<<endl;
            memset(_buffCut,0,1024);
            memcpy(_buffCut,buffer+6,_size-6);

            switch((MessageType) _header.TransectionCode )
            {
                case eLOGIN:
                            //OnLogin(_buffCut);
                            ClientHandler::Login(_buffCut);
                    break;
                case eCANCELALL:
                case eSTOP_ALL:
                case eIOCPAIR:
                case eIOCPAIRUNSUB:
                case eIOCPAIRDIFF:
                case eDelete:
                case eManualOrd:
                    {
                        //map<long,AutoClient*>::iterator _stgRes = _StgMap.find(_header.ClintId);
                map<int,AutoClient*>::iterator _stgRes = _StgMap.find(_header.ClintId);
                        if(_stgRes!=_StgMap.end())
                        {
                            cout << "Event recieved for running stg " << _header.ClintId << endl;



                        switch((MessageType) _header.TransectionCode)
                        {
                            case eIOCPAIR:
                                {
                                    strFOPAIRIOC _FOpairObj;

                                    memset(&_FOpairObj,0,sizeof(strFOPAIRIOC));
                                    memcpy(&_FOpairObj,_buffCut,sizeof(strFOPAIRIOC));

                                    _StgMap[_header.ClintId]->HandleOnFOPairSubscription(_FOpairObj);

                                    PFUpdateMsg _pfUpdate;
                                    memset(&_pfUpdate,0,sizeof(_pfUpdate));
                                   // _pfUpdate.TransectionCode = (short)(MessageType)eIOCPAIR;
                                   // _pfUpdate.ClintId =2010100020;
                                   _pfUpdate.TransectionCode=concat((short)(MessageType)eIOCPAIR,_header.ClintId);
                                    memcpy(_pfUpdate.buffer,&_FOpairObj,sizeof(strFOPAIRIOC));

                                    cout << " Data Subscription code  "<< _pfUpdate.TransectionCode<<endl;
                                    ClientHandler::PublishPackToDataServer(_pfUpdate,sizeof(PFUpdateMsg));

                                    break;
                                }
                            case eIOCPAIRDIFF:
                            {
                                    FOPAIRDIFFIOC _INpairDiff;
                                    memset(&_INpairDiff,0,sizeof(FOPAIRDIFFIOC));
                                    memcpy(&_INpairDiff,_buffCut,sizeof(FOPAIRDIFFIOC));
                                   _StgMap[_header.ClintId]->HandleOnFOPairDiff(_INpairDiff);
                                    break;

                            }
                            case eIOCPAIRUNSUB:
                            {
                                    strFOPAIRIOC _FOpairObj;
                                    memset(&_FOpairObj,0,sizeof(strFOPAIRIOC));
                                    memcpy(&_FOpairObj,_buffCut,sizeof(strFOPAIRIOC));
                                    _StgMap[_header.ClintId]->HandleOnFOPairUnSubscription(_FOpairObj);
                                    cout <<  " Unsubscription recieved in ClientHandler for CID "<< _header.ClintId<< " for PF "<< _FOpairObj.PORTFOLIONAME<<endl;
                                    break;

                            }
                            case eManualOrd:
                            {

                       /* TEmp_Header Th_obj;
                        memset(&Th_obj,0,sizeof(TEmp_Header));
                        memcpy(&Th_obj,_buffCut,sizeof(TEmp_Header));*/

                                memcpy(&tCode, _buffCut+26, 2);
                                tCode = htons(tCode);
                                switch(tCode)
                                {
                                    case 20000:
                                    {
                                        MS_OE_REQUEST_TR _objmoe;
                                        memset(&_objmoe,0,sizeof(MS_OE_REQUEST_TR));
                                        memcpy(&_objmoe,_buffCut,sizeof(MS_OE_REQUEST_TR));
                                        _StgMap[_header.ClintId]->ManualOrderEntry(_objmoe);
                                        break;
                                    }
                                    case 20040:
                                    {

                                        MS_OM_REQUEST_TR _objmom;
                                        memset(&_objmom,0,sizeof(MS_OM_REQUEST_TR));
                                        memcpy(&_objmom,_buffCut,sizeof(MS_OM_REQUEST_TR));
                                        _StgMap[_header.ClintId]->ManualOrder_Mod_CNclEntry(_objmom);
                                        break;
                                    }
                                    case 20070:
                                    {
                                        MS_OM_REQUEST_TR _objmom;
                                        memset(&_objmom,0,sizeof(MS_OM_REQUEST_TR));
                                        memcpy(&_objmom,_buffCut,sizeof(MS_OM_REQUEST_TR));
                                        _StgMap[_header.ClintId]->ManualOrder_Mod_CNclEntry(_objmom);
                                        break;
                                    }
                       }

                           }

                        }
                        }

                    }
                    break;
            }


            int ret = nn_send(ClientHandler::ReplySock,(void*)buffer,_size, 0);
            if(ret)
            cout <<"Packet sent"<<endl;
            else
            cout <<"Sending failed"<<endl;
        }
    }
    catch(int e)
    {

    }

}

void ClientHandler::Contract_Filefun(string CONTRACTFILEPATH)
	{


        BOOST_FOREACH(ContractDetails _cd, _ContMap)
        {
            cimlist[_cd.Token] = _cd;
        }

/*
    std::fstream in("contract.txt",std::ifstream::in);
    if (!in.is_open()) return 1;



    string line;
 std::vector<string> strs;

    while (getline(in,line))
    {


        boost::split(strs, line, boost::is_any_of("|"));
            if(strs.size()>3)
            {
                if(atoi(strs[1].c_str()) > 0)
                {
                memset(&obj,0,sizeof(ContractDetails));

                obj.Token = atoi(strs[0].c_str());



				obj.AssetToken = atol(strs[1].c_str());


                memcpy(&obj.InstrumentName,strs[2].c_str(),sizeof(obj.InstrumentName));

                 memcpy(&obj.Symbol,strs[3].c_str(),sizeof(obj.Symbol));
                  memcpy(&obj.Series,strs[4].c_str(),sizeof(obj.Series));


				obj.ExpiryDate = atoi(strs[6].c_str());
				obj.StrikePrice = atoi(strs[7].c_str());


                memcpy(&obj.OptionType,strs[8].c_str(),sizeof(obj.OptionType));

				//obj.OptionType=strs[8];

				obj.BoardLotQuantity  = atoi(strs[30].c_str());

				cimlist[obj.Token]= obj;

				cout << " Token "<< obj.Token<<endl;
				}
            }



    }

*/



      //  cout<<"Contract_Filefun Loaded"<<": cnt="<<cnt<<endl;

	}

/*
void ClientHandler::Contract_Filefun(string CONTRACTFILEPATH)
	{

		//Contract_File obj;

        //cimlist[]
      //  map<int,ContractDetails> _pair;




    ifstream inFile("contract.txt",std::ifstream::in);//<-- file opened and checked for errors elsewhere

    std::string delimiter = "|";
    string input;
    size_t pos = 0;
    std::string token;


    for(int i = 0; i < 1; i++)
      getline(inFile,input);

    while(getline(inFile,input))
    {
        //cout << input<<endl<<endl;//<-- token
        //cout<< " ******************************"<<endl;
         int j=0;
         pos=0;

         ContractDetails _cont;//= new ContractDetails();
        while ((pos = input.find(delimiter)) != std::string::npos)
        {

            token = input.substr(0, pos);
           // std::cout << token << std::endl;

            switch(j)
            {
                case 0:
                    //cout << " Token "<< token<<endl;
                    //obj.Token = atoi(token.c_str());
                    _cont.Token = atoi(token.c_str());
                    break;
                case 1:
            //cout << " AssetToken "<< token<<endl;
            //    obj.AssetToken = atol(token.c_str());
                    _cont.AssetToken = atol(token.c_str());

                    break;
                case 2:
               // cout << " InstrumentName "<< token<<endl;
                //obj.InstrumentName = token.c_str();

                //obj.InstrumentName.assign(token);

                //strcpy(obj.InstrumentName , token);//,sizeof(obj.InstrumentName));

                  sprintf(_cont.InstrumentName ,"%s",token.c_str());

                 // cout << " _cont.InstrumentName "<<_cont.InstrumentName<<endl;
                    break;
                case 3:
                //cout << " Symbol "<< token<<endl;
                //obj.Symbol = token.c_str();
                sprintf(_cont.Symbol ,"%s",token.c_str());

                    break;
                case 4:
               // cout << " Series "<< token<<endl;
                //obj.Series = token.c_str();


                     sprintf(_cont.Series ,"%s",token.c_str());


                break;
                case 6:
                      // cout << " ExpiryDate "<< token<<endl;
                  //     obj.ExpiryDate = atoi(token.c_str());

                break;
                case 7:
                      // cout << " StrikePrice "<< token<<endl;
                      //obj.StrikePrice = atoi(token.c_str());


                        _cont.StrikePrice = atoi(token.c_str());
                break;
                case 8:
                       //cout << " OptionType "<< token<<endl;
                     //  obj.OptionType = token.c_str();

                         sprintf(_cont.OptionType ,"%s",token.c_str());


                break;
                case 30:

                //cout << " BLQ "<< token<<endl;
               // obj.BoardLotQuantity  = atoi(token.c_str());
               _cont.BoardLotQuantity  = atoi(token.c_str());
                break;

            }

                j++;
            input.erase(0, pos + delimiter.length());
            if(j>30)
            break;
        }


       // _ContHashMap.AddRecord(_cont);

        cimlist[_cont.Token]= _cont;

      //  cimlist.insert(ContractStore::value_type(_cont.Token,_cont));



      //  _pair.insert(pair<int,ContractDetails>(_cont.Token,_cont));

        //cimlist=boost::assign::map_list_of( _cont.Token, _cont );
        //cout<< " ******************************"<<endl;
       // sleep(1);
    }

    inFile.close();

   // cimlist = _pair;

   // cimlist.insert(_pair.begin(),_pair.end());
   // std::copy(_pair.begin(), _pair.end(), std::inserter(cimlist, cimlist.end()) );


      //  cout << " Hello "<<endl<<endl;
cout << " Contract loaded successfully "<< cimlist.size() << endl<<endl;

	}*/
