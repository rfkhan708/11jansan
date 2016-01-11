#ifndef NANOMQSENDER_H
#define NANOMQSENDER_H

#include "../Structure.h"
#include<nanomsg/nn.h>
#include<nanomsg/pubsub.h>

class NanoMQSender
{
int sock;
    public:
        NanoMQSender();
        void Init();
         void publisher (FinalPrice& data);
        virtual ~NanoMQSender();
    protected:
    private:
};

static class NanoObj
{

    NanoMQSender _obj;
public:
  void BindNanoObj()
    {
        _obj.Init();
    }


    void SendData(FinalPrice& Data)
    {
      _obj.publisher(Data);
      //  printf("\n Data Sent \n");
    }
}nanoObj;
#endif // NANOMQSENDER_H
