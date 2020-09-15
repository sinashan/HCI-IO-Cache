#ifndef GLOBALCONTROLLER_H_INCLUDED
#define GLOBALCONTROLLER_H_INCLUDED

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "LRUcache.h"

using namespace std;

#define MaxRAndomCAcheSize 6
#define MaxNodes 20
#define MaxVMs 20
#define MinCacheSize 20
#define isSequentialNumber 20
#define isNotSequentialNumber 10

class VM;

class sequential{
                int ns; //number of requests that their address is smaller than previous address when isSequential=true and their address is bigger than previous address when isSequential=false
                long long OnePreviousAddress;
                long long TwoPreviousAddress;
                bool isSequential;
            public:
                sequential(){ns=0;OnePreviousAddress=0;TwoPreviousAddress=0;isSequential=true;}
                void checkSequential(long long newAddress);
                void clearInfo(){sequential();}
                friend VM;
                };
void sequential::checkSequential(long long newAddress)
{
    if(isSequential&&((TwoPreviousAddress<=OnePreviousAddress&&OnePreviousAddress>newAddress)||(TwoPreviousAddress>=OnePreviousAddress&&OnePreviousAddress<newAddress)))
    {
        ns++;
        if(ns==isNotSequentialNumber)
        {
            ns=0;
            isSequential=false;
        }
    }
    else if((!isSequential)&&((TwoPreviousAddress>=OnePreviousAddress&&OnePreviousAddress>=newAddress)||(TwoPreviousAddress<=OnePreviousAddress&&OnePreviousAddress<=newAddress)))
    {
        ns++;
        if(ns==isSequentialNumber)
        {
            ns=0;
            isSequential=true;
        }
    }
    TwoPreviousAddress=OnePreviousAddress;
    OnePreviousAddress=newAddress;
}

class VM{
        ifstream fi;    // Request File of the VM
        ifstream fi_workloadinfo;
        ofstream fo_Result;
        string WorkloadName;
        LRUcache c;
        string NodeNumber_Cache;       //the Nodes that give to VM cache
        sequential seq;

    public:
        void Reconfigure(string workloadFile,string workloadInfoFile,string resultFile); //open VM files
        void setCacheSize(int cSize,string NodeNumber){c.Reconfigure(cSize);c.EmptyCache();NodeNumber_Cache=NodeNumber;}
        void RunVM(int n);  //Read (next) n Request from the VM_file
        int putTotalSize(){return c.putTotalSize(); }
        int readURD(){int x; fi_workloadinfo>>x; return x;}
        void WriteInResultFile(string s){fo_Result<<s;}
        bool isSeq(){return seq.isSequential;}
        };
void VM::Reconfigure(string workloadFile,string workloadInfoFile,string resultFile)
{
    fi.open(workloadFile.c_str(),ios::in);

    fi_workloadinfo.open(workloadInfoFile.c_str(),ios::in);
    string wn;
    fi_workloadinfo>>wn;
    WorkloadName=wn;

    fo_Result.open(resultFile,ios::app|ios::out);
    fo_Result<<wn<<"\n";

}
void VM:: RunVM(int n)
{

    c.ClearIOcacheInfo();
    seq.clearInfo();

    Request R;

    for(int i=0;i<n;i++)
    {
        fi>>R;
        c.GetRequest(R);
        seq.checkSequential(R.putAddress());
    }
    string isSeqType=isSeq()?"True":"False";
    cout<<c;
    cout<<"Nodes Number(Cache):\t"<<NodeNumber_Cache<<"\nis Sequential:\t"<<isSeqType<<"\n\n";

    result_1<<c;
    result_1<<"Nodes Number(Cache):\t"<<NodeNumber_Cache<<"\nis Sequential:\t"<<isSeqType<<"\n\n";

    result_2<<c;
    result_2<<"Nodes Number(Cache):\t"<<NodeNumber_Cache<<"\nis Sequential:\t"<<isSeqType<<"\n\n";

    c.PrintCSVfile(fo_Result);
    fo_Result<<NodeNumber_Cache<<","<<isSeqType<<"\n";

}

class Node{
            VM v[MaxVMs];
            int numberVM=0;
            int TotalCacheSize;
            int AllocatedCacheSize=0;

        public:
            friend class GlobalController;
            void AddVM(){numberVM++;}
            Node(int n,int s){numberVM=n; TotalCacheSize=s;}
            void Reconfigure(int n,int s){numberVM=n; TotalCacheSize=s;}
            Node(){}
            void ReconfigureVM(int VMnumber,string s1,string s2,string s3){v[VMnumber].Reconfigure(s1,s2,s3);}
            bool setVMcacheSize(int VMnumber,int cSize,string NodeNumber);
            void setVMcacheSize2(int VMnumber,int cSize,string NodeNumber){v[VMnumber].setCacheSize(cSize,NodeNumber);}
            int readVMURD(int VMnumber){return  v[VMnumber].readURD();}
            void RunVm(int VMnumber,int Rn){v[VMnumber].RunVM(Rn);}
            int putFreeCacheSize(){return TotalCacheSize-AllocatedCacheSize;}
            void DecreaseCacheSize(int Size){AllocatedCacheSize+=Size;}
            void RemoveCache(){AllocatedCacheSize=0;}
            void WriteInVMresultFile(int VMnumber,string s){v[VMnumber].WriteInResultFile(s);}
            int putCacheSize(){return TotalCacheSize;}
            bool isVMseq(int VMnumber){return v[VMnumber].isSeq();}
            };

bool Node::setVMcacheSize(int VMnumber,int cSize,string NodeNumber)
{
    int x=v[VMnumber].putTotalSize();
    if(AllocatedCacheSize+cSize-x>TotalCacheSize) // There is not enough space
    {
        return 0;
    }

    v[VMnumber].setCacheSize(cSize,NodeNumber);
    AllocatedCacheSize+=cSize;
    return 1;
}

class GlobalController{
                        Node node[MaxNodes];
                        int numberNode;
                        int TotalCacheSize_AllNode=0;
                        int AllocatedCasheSize_AllNode=0;
                        string status;

                    public:
                        void Reconfigure(string sts,int nNode,int numberVM[],int nodeCacheSize,int vmCacheSize);
                        void RunVMs(int numberRequest,int Step);
                        };
void GlobalController::Reconfigure(string sts,int nNode,int numberVM[],int nodeCacheSize,int vmCacheSize)
{
    // status={ Local|Global-Traffic|Global-Load }
    status=sts;

    srand (time(NULL));

    numberNode=nNode;
    TotalCacheSize_AllNode=0;
    for(int i=0;i<numberNode;i++)
    {
        TotalCacheSize_AllNode+=nodeCacheSize;
        node[i].Reconfigure(numberVM[i],nodeCacheSize);
    }

    for(int i=0;i<numberNode;i++) // Open VM Files and Workload info files
    {
        for(int j=0;j<numberVM[i];j++)
        {
            stringstream ss;

            string s1;
            ss<<"RequestFiles/R_N"<<i<<"_V"<<j<<".txt";
            s1 = ss.str();

            string s2;
            ss.str("");
            ss<<"Workload_info/Wi_N"<<i<<"_V"<<j<<".txt";
            s2 = ss.str();

            string s3;
            ss.str("");
            ss<<"ResultFiles/Result_N"<<i<<"_V"<<j<<".csv";
            s3 = ss.str();

            node[i].ReconfigureVM(j,s1,s2,s3);

            string s4="step,Total number of request:,Total number of read request:,Total number of write request:";
                    s4+=",Total Miss:,Total Hit:,Read Hit:,Write Hit:,Disk Access:,Read Cache:";
                    s4+=",Write Cache:,BlockSize:,Cache Size:,Nodes Number(Cache):,is Sequential:\n";
            node[i].WriteInVMresultFile(j,s4);
        }
    }

    for(int i=0;i<numberNode;i++) // Set Default Cache Size for all VMs
    {
        for(int j=0;j<numberVM[i];j++)
        {
            string s;
            char s1[50];
            sprintf(s1,"Node%d=%d ",i,vmCacheSize);
            s=string(s1); // s= Specifies the node that provides the  cache for VM and size of the cache
            node[i].setVMcacheSize(j,vmCacheSize,s);
            AllocatedCasheSize_AllNode+=vmCacheSize;
        }
    }
}

void GlobalController::RunVMs(int numberRequest,int step)
{
    int xr,sumSize=0,tempSize[MaxNodes][MaxVMs];
    step++;
    for(int i=0;i<numberNode;i++) //Calculate Cache size for each VM (Now it's Random)
    {
        for(int j=0;j<node[i].numberVM;j++)
        {
            cout<<"--- Node:"<<i<<" VM:"<<j<<" ---\n";
            result_1<<"--- Node:"<<i<<" VM:"<<j<<" ---\n";
            result_2<<"--- Node:"<<i<<" VM:"<<j<<" ---\n";

            string s;
            stringstream ss;
            ss << step;
            s = ss.str();
            s+=",";
            node[i].WriteInVMresultFile(j,s);

            node[i].RunVm(j,numberRequest);
            //xr=rand()%(MaxRAndomCAcheSize-1)+2;
            xr=node[i].readVMURD(j);

            xr++;
            if(xr<MinCacheSize)
                xr=MinCacheSize;
            sumSize+=xr;
            tempSize[i][j]=xr;
            node[i].RemoveCache();
        }
    }

    // status={ Local|Global-Traffic|Global-Load }
    if(status=="Local")
    {
        for(int i=0;i<numberNode;i++)
        {
            int sum=0;
            for(int j=0;j<node[i].numberVM;j++)
            {
                sum+=tempSize[i][j];
            }
            if(sum>=(2*node[i].putCacheSize()))
            {
                int x=sum/node[i].putCacheSize();
                for(int j=0;j<node[i].numberVM;j++)
                {

                    int y=tempSize[i][j];
                    y/=x;
                    if(y<MinCacheSize)
                        y=MinCacheSize;
                    sum-=(tempSize[i][j]-y);
                    tempSize[i][j]=y;
                }
            }
            while(sum>node[i].putCacheSize())
            {
               for(int j=0;j<node[i].numberVM;j++)
                {
                    if(tempSize[i][j]>MinCacheSize)
                    {
                        tempSize[i][j]--;
                        sum--;
                    }
                }
            }
        }
    }
    else
    {

        if(sumSize>=(2*TotalCacheSize_AllNode))
        {
            int x=sumSize/TotalCacheSize_AllNode;
            for(int i=0;i<numberNode;i++)
            {
                for(int j=0;j<node[i].numberVM;j++)
                {

                    int y=tempSize[i][j];
                    y/=x;
                    if(y<MinCacheSize)
                        y=MinCacheSize;
                    sumSize-=(tempSize[i][j]-y);
                    tempSize[i][j]=y;
                }
            }
        }

        while(sumSize>TotalCacheSize_AllNode) // if Total of VMs cache size that Calculate in previous step is greater than total cache size of All Nodes, then reduce it
        {
            for(int i=0;i<numberNode;i++)
            {
                for(int j=0;j<node[i].numberVM;j++)
                {
                    if(tempSize[i][j]>MinCacheSize)
                    {
                        tempSize[i][j]--;
                        sumSize--;
                    }
                }
            }
        }
    }
    AllocatedCasheSize_AllNode=0;

    bool PriorityStatus=false;
    if(status=="Global-Traffic")
        PriorityStatus=true;

AssignCache:
    for(int i=0;i<numberNode;i++) //Assign cache to VMs
    {
        for(int j=0;j<node[i].numberVM;j++)
        {
            if(PriorityStatus&&node[i].isVMseq(j)==false)
            {
                continue;
            }
            else if(status=="Global-Traffic"&&PriorityStatus==false&&node[i].isVMseq(j))
            {
                continue;
            }

            AllocatedCasheSize_AllNode+=tempSize[i][j];
            if(tempSize[i][j]<=node[i].putFreeCacheSize())
            {
                string s;
                char s1[50];
                sprintf(s1,"Node%d=%d ",i,tempSize[i][j]);
                s=string(s1); // s= Specifies the node that provides the  cache for VM and size of the cache
                node[i].setVMcacheSize(j,tempSize[i][j],s);
            }
            else
            {
                string s="";
                int temp=tempSize[i][j];
                if(node[i].putFreeCacheSize()>0)
                {
                    temp-=node[i].putFreeCacheSize();
                    char s1[50];
                    sprintf(s1,"Node%d=%d  ",i,node[i].putFreeCacheSize());
                    s=string(s1); // s= Specifies the node that provides the  cache for VM and size of the cache
                    node[i].DecreaseCacheSize(node[i].putFreeCacheSize());
                }
                int num=0;
                while(temp>0)
                {
                    int minSize=node[num].putFreeCacheSize();
                    if(minSize>0)
                    {
                        if(temp<minSize)
                        {
                            minSize=temp;
                        }
                        temp-=minSize;
                        node[num].DecreaseCacheSize(minSize);
                        char s1[50];
                        sprintf(s1,"Node%d=%d  ",num,minSize);
                        s+=string(s1); // s= Specifies the nodes that provide the cache for VM and size of each cache
                    }
                    num++;
                }
                node[i].setVMcacheSize2(j,tempSize[i][j],s);
            }
        }
    }
    if(status=="Global-Traffic"&&PriorityStatus)
    {
        PriorityStatus=false;
        goto AssignCache;
    }

}



#endif // GLOBALCONTROLLER_H_INCLUDED
