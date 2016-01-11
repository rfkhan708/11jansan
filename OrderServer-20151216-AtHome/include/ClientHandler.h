#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer/timer.hpp>

#include <nn.h>
#include <pubsub.h>
#include <reqrep.h>

#include "Enums.h"
#include "AutoClient.h"
#include <iostream>

//#include <string.h>

using namespace boost;
using namespace boost::signals2;
using namespace Enums;
using namespace boost::timer;
using namespace std;


class ClientHandler
{
    public:
        ClientHandler();
        virtual ~ClientHandler();
        void PushServerPacket(ClientUpdateMsg buff,int _size);
        void PushOrderPacket(unsigned char * Data, int _size);
        map<int,ContractDetails> cimlist;
    protected:
        boost::signals2::signal<void(char *)> OnLogin;
        boost::signals2::signal<void(char *)> OnHeartBeat;
        boost::signals2::signal<void(char *)> OnOrder;
        void OrderEventHandler(char *);
        void HeartBeatEventHandler(char *);
        void LoginEventHandler(char *);
        void Login(char * obj);
        boost::thread* _EventThread;
        void RecieveDataAsServer();

        int ReplySock;
        int PubSock;
        int IPCSock;
        bool loadCimList;
        void ClientHandler::Contract_Filefun(string CONTRACTFILEPATH);
        void PublishPackToDataServer(PFUpdateMsg _pfupdate,int _size);
        map<int,AutoClient*> _StgMap;

    private:
        int AutoStgID;
        ContractDetails obj;
};



#endif // CLIENTHANDLER_H
