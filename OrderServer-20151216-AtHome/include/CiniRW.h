#ifndef CINIRW_H
#define CINIRW_H
#ifdef _cplusplus
extern c
{
#endif
#include <ostream>
#include <fstream>
#include <string>
#include <map>

// add the class in the std namespace
namespace std {
    class CiniRW {
    public:
        // constructor with INI file name;
        CiniRW(char *FileName);
        // without INI file name
        CiniRW();
        // destructor
        ~CiniRW();

        // set the INI file name if build without file
        void setFileName(char *FileName);
        // get the section name from INI file if the section is not found
        // return false otherwise return true
        bool getSectionName(std::string sName);
        // return the current working section
        string curSecName();
        // return the specific section datas
        void getSectionData();
        // return the section specified key name
        std::string getKeyName(std::string);
        // return the specifed key value from the section data
        std::string getKeyValue(std::string);
        // count the number of data in the section
        int count();
        // create a new section if you need
        void createSection(string newSecName);
        // write the key into the ini file (section, key, value)
        void setWriteKey(std::string, std::string, std::string);

        // return the current INI file name
        string getFileName();
        // reopen the stream and file
        void open();
        // close the stream and file
        void close();
    public:
        string sName;
        string iniFileName;
        fstream stream;
        fstream* stream1;
        string  buffer;
        // public data container ,contain the (key, value) pair
        std::map<std::string,std::string> data;
    };
}
#ifdef _cplusplus
}
#endif
#endif
