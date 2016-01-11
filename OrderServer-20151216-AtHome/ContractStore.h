#ifndef CONTRACTSTORE_H_INCLUDED
#define CONTRACTSTORE_H_INCLUDED
#include <boost/thread/mutex.hpp>
#include<boost/thread/condition_variable.hpp>

using namespace Enums;
/*
 * C++ Program to Implement Hash Tables Chaining with List Heads
 */
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
using namespace std;
const int TABLE_SIZE = 5;
/*
 * Link List Class Declaration
 */

class LinkedHash
{
	public:
        int key;
        ContractDetails value;
        LinkedHash *next;
        LinkedHash(int key, ContractDetails value)
        {
            this->key = key;
            this->value = value;
            this->next = NULL;
        }
};
/*
 * HashMap Class Declaration
 */
class HashMap
{
    private:
        LinkedHash **htable;
    public:
        HashMap()
        {
            htable = new LinkedHash*[TABLE_SIZE];
            for (int i = 0; i < TABLE_SIZE; i++)
            {
                htable[i] = NULL;
            }
        }
        ~HashMap()
        {
            for (int i = 0; i < TABLE_SIZE; i++)
            {
                if (htable[i] != NULL)
                {
                    LinkedHash *prev = NULL;
                    LinkedHash *entry = htable[i];
                    while (entry != NULL)
                    {
                        prev = entry;
                        entry = entry->next;
                        delete prev;
                    }
                }
                delete[] htable;
            }
        }
        /*
	     * Hash Function
         */
        int HashFunc(int key)
		{
            return key % TABLE_SIZE;
        }
        /*
	     * Insert Element at a key
         */
        void Insert1(int key, ContractDetails value)
        {
            cout << " Insert1"<<endl;

            int hash_val = HashFunc(key);
            if (htable[hash_val] == NULL)
            //if (htable[hash_val] == NULL)
                htable[hash_val] = new LinkedHash(key, value);
            else
            {
                LinkedHash *entry = htable[hash_val];
                while (entry->next != NULL)
                    entry = entry->next;
                    if (entry->key == key)
                        entry->value = value;
                    else
                        entry->next = new LinkedHash(key, value);
            }
        }
        /*
	     * Search Element at a key
         */
        ContractDetails Find(int key)
        {
            int hash_val = HashFunc(key);
            if (htable[hash_val] == NULL)
            {
                ContractDetails _ctr;
                _ctr.Token=-1;
                return _ctr;
            }
            else
            {
                LinkedHash *entry = htable[hash_val];
                while (entry != NULL && entry->key != key)
                    entry = entry->next;
                if (entry == NULL)
                      {
                ContractDetails _ctr;
                _ctr.Token=-1;
                return _ctr;
            }
                else
                    return entry->value;
            }
        }
        /*
	     * Delete Element at a key
         */
        void Delete(int key)
        {
            int hash_val = HashFunc(key);
            if (htable[hash_val] != NULL)
            {
                LinkedHash *entry = htable[hash_val];
                LinkedHash *prev = NULL;
                while (entry->next != NULL && entry->key != key)
                {
                    prev = entry;
                    entry = entry->next;
                }
                if (entry->key == key)
                {
                    if (prev == NULL)
                    {
                        LinkedHash *next = entry->next;
                        delete entry;
                        htable[hash_val] = next;
                    }
                    else
                    {
                        LinkedHash *next = entry->next;
                        delete entry;
                        prev->next = next;
                    }
                }
            }
        }
};


class ContractHashMap
{
private:
 HashMap hash;
int key;
 ContractDetails value;

    mutable boost::mutex the_mutex;
    boost::condition_variable the_condition_variable;
public:

ContractHashMap()
{

}

void AddRecord(ContractDetails _PF)
{
    boost::mutex::scoped_lock lock(the_mutex);

    cout <<"\n\n &&&&&&&&&&&&&&&&&&&&&& 1 HashKey while creating holder &&&&&&&&&&&&&&&&&&&&&&"<< endl;

        hash.Insert1(_PF.Token, _PF);


     lock.unlock();
    the_condition_variable.notify_one();


    cout <<"\n\n &&&&&&&&&&&&&&&&&&&&&& 2 HashKey after creating holder &&&&&&&&&&&&&&&&&&&&&&"<<endl;
}



ContractDetails GetRecord(int Key)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return hash.Find(key);
    }


};

extern ContractHashMap _ContHashMap;

#endif // CONTRACTSTORE_H_INCLUDED
