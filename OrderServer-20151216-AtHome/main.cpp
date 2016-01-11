#include <iostream>           // For cerr and cout
#include <fstream>

#include "TapSignOn.h"
#include <vector>

#include "Enums.h"
#include <boost/algorithm/string.hpp>
#include "CiniRW.h"
#include <boost/lexical_cast.hpp>

using namespace std;

using namespace boost;

extern vector<ContractDetails> _ContMap;
extern vector<IniInputs> _IniMap;

int main()
{
    IniInputs _iniinp;


    CiniRW *iReader = new CiniRW("settings.ini");
    iReader->getSectionName("SECTION");
    iReader->getSectionData();

    _iniinp.SECTION =  boost::lexical_cast<int>(iReader->getKeyValue("ID"));
    iReader->getSectionName(iReader->getKeyValue("ID"));
    iReader->getSectionData();


    _iniinp.BROKERID = boost::lexical_cast<int>(iReader->getKeyValue("BROKERID"));
    _iniinp.CONTRACTFILEPATH = iReader->getKeyValue("CONTRACTFILEPATH");
    _iniinp.BRANCHID =boost::lexical_cast<int>(iReader->getKeyValue("BRANCHID"));
    _iniinp.USERID =  boost::lexical_cast<int>(iReader->getKeyValue("USERID"));

    _iniinp.VERSION =  boost::lexical_cast<int>(iReader->getKeyValue("VERSION"));
    _iniinp.SERVERIP =  iReader->getKeyValue("SERVERIP");
    _iniinp.SERVERPORT =   boost::lexical_cast<int>(iReader->getKeyValue("SERVERPORT"));
    _iniinp.DATANANOPATH =   iReader->getKeyValue("DATANANOPATH");

    _IniMap.push_back(_iniinp);

    cout << "_iniinp.SECTION "<<_iniinp.SECTION << " _iniinp.BROKERID " << _iniinp.BROKERID<< " _iniinp.DATANANOPATH "<<_iniinp.DATANANOPATH<<endl;

/*
BROKERID=12468
BRANCHID=4
USERID=31620
SERVERIP=192.168.100.137
SERVERPORT=9602
VERSION=93700
CONTRACTFILEPATH=contract.txt
DATANANOPATH=tcp://192.168.100.137:7172*/

string data("contract.txt");

    ifstream in(data.c_str());
    std::vector<string> strs;
    ContractDetails obj;
   // if (!in.is_open()) return 1;




    string line;

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

				//_PFHashMap.cimlist[obj.Token]= obj;

               _ContMap.push_back(obj);
				//cout << " Token "<< obj.Token<<endl;
				}
            }
       // cout << "\n"<< line << endl;
    }







    //SockConn.BindConnection1();


    TapSignOn _obj;

   _obj.Init();

//    ClientHandler _obj1;



   /* PFHolder _pf;

    memset(&_pf,0,sizeof(_pf));
    _pf.PF=10;
    _pf.CID=2;
    _pf.HashKey =65421;
    _pf._size=136;
    _FillData.BidQueue(_pf);
*/


//// cout << sizeof(C_SignIn) <<endl;

int iinp;
cin >> iinp;

		return 0;
}
