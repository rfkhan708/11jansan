#include "NanoMQSender.h"

NanoMQSender::NanoMQSender()
{

}
void NanoMQSender::Init()
{
    //ctor
        //char* addr = "tcp://192.168.100.227:7070";
        char* addr = "tcp://192.168.100.36:7070";
        //char* addr = "tcp://192.168.100.36:7070";

        sock = nn_socket(AF_SP,NN_PUB);

        nn_bind(sock, addr);
        //printf("\n Nano socket binding complete \n");
}

void NanoMQSender::publisher (FinalPrice& data)
{
    //printf("\n Token BID ASK %u %u %u  ", data.Token,data.MAXBID, data.MINASK);
    int ret = nn_send(sock,(void*)&data,sizeof(FinalPrice), 0);
   // printf("\n Data Sent Inside %u \n" , ret);
}

NanoMQSender::~NanoMQSender()
{
    //dtor
    nn_close(sock);
    sock= NULL;
}
