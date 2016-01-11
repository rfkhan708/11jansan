#include "Enums.h"
#include <fstream>

using namespace Enums;

static class LoadContract
{

public :
    map<int,ContractDetails> cimlist;
		//vector<struct Contract_File> cimlist;
	//	vector<struct Contract_File> first;
	void Contract_Filefun(string CONTRACTFILEPATH)
	{

		//Contract_File obj;

        //cimlist[]
        ContractDetails _cont;

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
                //cout << " InstrumentName "<< token<<endl;
                //obj.InstrumentName = token.c_str();

                //obj.InstrumentName.assign(token);

                //strcpy(obj.InstrumentName , token);//,sizeof(obj.InstrumentName));

                    _cont.InstrumentName = token.c_str();
                    break;
                case 3:
                //cout << " Symbol "<< token<<endl;
                //obj.Symbol = token.c_str();
                    _cont.Symbol = token.c_str();
                    break;
                case 4:
               // cout << " Series "<< token<<endl;
                //obj.Series = token.c_str();

                    _cont.Series = token.c_str();
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

                     _cont.OptionType = token.c_str();
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
        cimlist[_cont.Token]= _cont;

        //cout<< " ******************************"<<endl;
       // sleep(1);
    }
        inFile.close();
      //  cout << " Hello "<<endl<<endl;
cout << " Contract loaded successfully "<<endl<<endl;

	}
}_ldContract;




