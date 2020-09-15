#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <stdlib.h>
#include "LRUcache.h"
#include "GlobalController.h"
using namespace std;

#define File "E:\\tpcc.txt"
#define ConfigFile "Config_IOcache.txt"


int main()
{

    int numberNode,numberVM[MaxNodes],nodeCacheSize;
    string status;
    int Steps;      //Steps to run the program (number of run the GC.RunVMs(NumberRequestPerStep) )
    int NumberRequestPerStep;  //number of Read Requests at each Step
    GlobalController GC;

//cout<<sizeof(long long);
//return 0;

    /*
    Test
    cout<<"Enter number of Node: ";
    cin>>numberNode;
    cout<<"Enter number of VM: ";
    cin>>numberVM;
    cout<<"Enter Node cache Size: ";
    cin>>nodeCacheSize;
    cout<<"\n\n";
*/

/*
Test
ofstream r6("R.csv",ios::app|ios::out);

r6<<"\nab"<<'\t'<<"cd";

return 0;
*/

    string s;
    ifstream fi;
    fi.open(ConfigFile,ios::in);
    if(!fi)
    {
        cout<<"\nConfig_IOcache.txt could not be opened\n";
        return 0;
    }

    fi>>s>>status;

    if(status=="1"||status=="Local")
    {
        status="Local";
    }
    else if(status=="2"||status=="Global-Traffic")
    {
        status="Global-Traffic";
    }
    else if(status=="3"||status=="Global-Load")
    {
        status="Global-Load";
    }
    else
    {
        cout<<"\nConfig_IOcache.txt could not be found\n";
        return 0;
    }
    cout<<"Status: "<<status<<'\n';

    fi>>s>>numberNode;
    if(s!="Number_of_Node:"&&numberNode<MaxNodes)
    {
        cout<<"\nConfig_IOcache.txt could not be found\n";
        return 0;
    }
    cout<<s<<numberNode<<'\n';

    int sumVM=0;
    fi>>s;
    for(int i=0;i<numberNode;i++)
    {
        fi>>numberVM[i];
        sumVM+=numberVM[i];
    }
    for(int i=0;i<numberNode;i++)
        if(s!="Number_of_VM:"&&numberVM[i]<MaxVMs)
        {
            cout<<"\nConfig_IOcache.txt could not be found\n";
            return 0;
        }
    cout<<s;
    for(int i=0;i<numberNode;i++)
        cout<<numberVM[i]<<'\t';

    fi>>s>>nodeCacheSize;

    if(s!="Node_cache_Size:")
    {
        cout<<"\n\nConfig_IOcache.txt could not be found";
        return 0;
    }
    cout<<s<<nodeCacheSize<<'\n';

    fi>>s>>Steps;
    if(s!="Number_of_Steps:")
    {
        cout<<"\nConfig_IOcache.txt could not be found";
        return 0;
    }
    cout<<s<<Steps<<'\n';

    fi>>s>>NumberRequestPerStep;
    if(s!="Number_request_per_Step:")
    {
        cout<<"\nConfig_IOcache.txt could not be found";
        return 0;
    }
    cout<<s<<NumberRequestPerStep<<'\n';

    for(int i=0;i<numberNode;i++)
        if((numberVM[i]*2)>nodeCacheSize)
        {
            cout<<"\nCache Size is small for Node:"<<i<<" (minimum cache size = numberVM(Node:"<<i<<")*2 = "<<numberVM[i]*2<<")\n";
            return 0;
        }

    GC.Reconfigure(status,numberNode,numberVM,nodeCacheSize,MinCacheSize);

    for(int k=0;k<Steps;k++)
    {
        cout<<"\n--- Step:"<<k+1<<" ----";
        result_1<<"---- Step:"<<k+1<<" ----\n";
        result_2<<"---- Step:"<<k+1<<" ----\n";
        GC.RunVMs(NumberRequestPerStep,k);

    }
    fi.close();
    result_1.close();
    result_2.close();
    return 0;
}
