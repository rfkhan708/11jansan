#include "PF_On_Queue.h"
#include "StaticClass.h"
#include <boost/ptr_container/ptr_map.hpp>

 typedef boost::ptr_map<long, concurrent_queue> _PFOrder;
    _PFOrder _InDataHolder1;

PF_On_Queue::PF_On_Queue()
{
    //ctor

    PFHolder _MyPF;
    _MyPF.CID=789654;
    _MyPF.PF = 741258;

    _InDataHolder1[123].push(_MyPF);

    PFHolder _MyPF1;

    _MyPF1 = _InDataHolder1[123].try_pop();

}

PF_On_Queue::~PF_On_Queue()
{
    //dtor
}
