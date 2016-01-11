#ifndef TAPSIGNON_H
#define TAPSIGNON_H

#include "Socket.h"
#include "../Packets.h"

#include <boost/thread/thread.hpp>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "StaticClass.h"
#include "structure.h"

#include <nn.h>
#include <pubsub.h>
#include <bus.h>
#include <pipeline.h>
#include <stdio.h>
#include <vector>
#include <mutex>


class TapSignOn
{
    public:

        TapSignOn();
       void BindConnection();
        void SendToExchange();
        virtual ~TapSignOn();
        PFHolder GetPF(int Token,int Price, int Qty, short side,double OrderNumber);
        void Init();
        TCPSocket sock;
        int AdminSock;
        Packets pkt;
        boost::thread_group producer_threads;
        boost::thread* _inDataThread;
        boost::thread* _outDataThread;
        mutable boost::mutex the_inmutex;
        boost::condition_variable the_incondition_variable;

        mutable boost::mutex the_outmutex;
        boost::condition_variable the_outcondition_variable;

        mutable boost::mutex the_Hashmutex;
        boost::condition_variable the_Hashcondition_variable;

        mutex mtxread;
        mutex mtxwrite;

        vector<PFHolder> _QueueHolder;

         int PubSock;


       // MapQueue _InDataHolder;

    protected:

   MS_OM_REQUEST_TR ReturnCancelPack( MS_OE_RESPONSE_TR _OrderResp );





 PFHolder _pfOrphan;

    FILE *filewriter;
    boost::posix_time::ptime date_time;
    private:
int LastSeq;
};



#endif // TAPSIGNON_H
