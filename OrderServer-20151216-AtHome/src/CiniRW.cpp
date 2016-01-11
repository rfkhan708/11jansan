#include "CiniRW.h"
#include <iostream>

using namespace std;

CiniRW::CiniRW() {}
CiniRW::CiniRW(char *FileName) {
    iniFileName = FileName;

stream1 = new fstream();
stream1->open(FileName,ios::in);

//prepare f to throw if failbit gets set
std::ios_base::iostate exceptionMask = stream.exceptions() | std::ios::failbit;
stream.exceptions(exceptionMask);

try {

 stream.open(FileName,ios::in);
}
catch (std::ios_base::failure& e) {
  std::cerr << e.what() << '\n';
}

}
CiniRW::~CiniRW() {
}
void  CiniRW::setFileName(char *FileName){
    iniFileName = FileName;
    stream.open(FileName);
}
string CiniRW::getFileName() {
    return iniFileName;
}
void CiniRW::close() {
    if ( stream.is_open() ) {
        stream.clear();
        stream.close();
    }
}
void CiniRW::open() {
    const char *p;
    p = &iniFileName[0];
    stream.open(p);
}
bool CiniRW::getSectionName(std::string sectionName) {
    sName = sectionName;
    if ( stream.is_open() == false ) {
        cout << "unable to read file" << endl;
        return false;
    }
    stream.clear();
    stream.seekg(0,ios::beg);
    while( !stream.eof()) {
        getline(stream, buffer);
        string temp;
        int start;
        start = buffer.find_first_of('[');
        if ( start != -1  ) {
            temp = buffer.substr( start+1, buffer.find_last_of(']')-1 );
            if ( sName.compare(temp) == 0 )
                return true;
        }
    }
    return false;
}
void CiniRW::getSectionData() {

    bool sectionMarked = false;
    string temp;
        //  the global map for store the section pair data
    data.clear();
    while (!stream.eof()) {
        getline(stream, buffer);
        string::size_type begin = buffer.find_first_not_of(" \t\f\r\v\n");
        // skip the blank line
        if ( begin == string::npos) continue;
        // skip the comments
        if ( string("#;").find(buffer[begin]) != string::npos ) continue;

        if ( buffer.find_first_of('[') != -1  ) {
            break;
        }
        string::size_type end = buffer.find('=', begin);
        string key = buffer.substr(begin,end-begin);
        key.erase( key.find_last_not_of(" \t\v\f") + 1);
        // skip if key is blank
        if( key.empty() ) continue;
        begin = buffer.find_first_not_of(" \f\n\r\t\v", end + 1 );
        end   = buffer.find_last_not_of(" \f\n\r\t\v" ) + 1;
        string value = buffer.substr( begin, end);
        data.insert(std::make_pair(key,value));
    }
}

int CiniRW::count() {
    if ( data.empty() ) return 0;
    return data.size();
}

string CiniRW::getKeyValue(string kName) {
    return data[kName];
}
// return the current working section name
string CiniRW::curSecName() {
    return sName;
}
