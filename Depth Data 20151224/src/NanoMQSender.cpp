#include "NanoMQSender.h"

NanoMQSender::NanoMQSender()
{

}
void NanoMQSender::Init(string DataPath)
{
    //ctor
        //char* addr = "tcp://192.168.100.227:7070";
       // char* addr = &DataPath;
        //char* addr = "tcp://192.168.100.36:7070";

        NanoMQSender::sock = nn_socket(AF_SP,NN_PUB);

        nn_bind(NanoMQSender::sock, DataPath.c_str());
        //printf("\n Nano socket binding complete \n");
}

void NanoMQSender::publisher (FinalPrice& data)
{
   // printf("\n Token Sub  BID ASK %u %u %u  %u ", data.Token,data.sub_token,data.MAXBID, data.MINASK);
    int ret = nn_send(NanoMQSender::sock,(void*)&data,sizeof(PriceArrray), 0);
   // printf("\n Data Sent Inside %u \n" , ret);
}

NanoMQSender::~NanoMQSender()
{
    //dtor
    nn_close(NanoMQSender::sock);
    NanoMQSender::sock= NULL;
}
