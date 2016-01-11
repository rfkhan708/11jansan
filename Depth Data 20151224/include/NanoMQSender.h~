#ifndef NANOMQSENDER_H
#define NANOMQSENDER_H

#include "../Structure.h"
#include<nanomsg/nn.h>
#include<nanomsg/pubsub.h>

class NanoMQSender
{

    public:
        int sock;
        NanoMQSender();
        void Init(string DataPath);
         void publisher (FinalPrice& data);
        virtual ~NanoMQSender();
    protected:
    private:
};

static class NanoObj
{

    NanoMQSender _obj;
public:
  void BindNanoObj(string DataPath)
    {
        _obj.Init(DataPath);
    }


    void SendData(FinalPrice& Data)
    {
      _obj.publisher(Data);
      //  printf("\n Data Sent \n");
    }
}nanoObj;
#endif // NANOMQSENDER_H
