#ifndef LRUCACHE_H_INCLUDED
#define LRUCACHE_H_INCLUDED

#include <iostream>
#include <string>
#include <fstream>
//#include <stdio.h>
//#include <stdlib.h>

using namespace std;

//#define CACHESIZE 1000
//#define BS_4K 4096
//#define BS_2K 2048
//#define SECTORSIZE 512

#define BLOCKSIZE 4096
#define D_Address 8         //------>BLOCKSIZE/SECTORSIZE
#define POLICY "WB"


ofstream result_1("Result_1.txt",ios::app|ios::out),result_2("Result_2.txt",ios::app|ios::out);

class LRUcache;

class Request{

            //double Time;
            //string Event;
            string Type;
            long long Address;
            int Size;

        public:
            long long putAddress(){return Address;}
            friend istream  &operator>>(istream&,Request&);
            friend ostream  &operator<<(ostream&, const Request&);
            friend LRUcache;
            };
//Get a Request (e.g. From the file)
istream &operator>>(istream &in,Request &R)
{
	//in>>R.Time>>R.Event>>R.Type>>R.Address>>R.Size;
	in>>R.Type>>R.Address>>R.Size;

	return in;
}

//Put a Request (e.g. in the File)
ostream &operator<<(ostream &out, const Request &R)
{
    //out<<R.Time<<' '<<R.Event<<' '<<R.Type<<' '<<R.Address<<' '<<R.Size<<'\n';
	out<<R.Type<<' '<<R.Address<<' '<<R.Size<<'\n';
	return out;
}

struct CacheBlock{        //a Block of Cache
        CacheBlock *left;
        CacheBlock *right;
        int data;
        long long Address;
        bool DirtyBit;
            };

// Create a Free CacheBlock
struct CacheBlock * get_CacheBlock()
 {
     CacheBlock *p;
     p = new struct CacheBlock;
     return p;
 }

// Create a cacheBlock with value
struct CacheBlock * get_CacheBlock(int data,long long Address,bool DirtyBit)
 {
     struct CacheBlock *p;
     p = new struct CacheBlock;
     p->data=data;
     p->Address=Address;
     p->DirtyBit=DirtyBit;
     p->left=p;
     p->right=p;
     return p;
 }

//insert a Block in the first of the Cache
 void insertf(CacheBlock *&first,int data,long long Address,bool DirtyBit)
 {
     struct CacheBlock *p;
     p=get_CacheBlock();
     p -> data = data;
     p -> Address = Address;
     p -> DirtyBit = DirtyBit;
     p->right =first;
     p->left=first->left;
     first->left->right=p;
     first->left=p;
     first=p;
 }

 //insert a Block in the middle of the cache
 void insertm (CacheBlock *q,int data,long long Address,bool DirtyBit)
{
    struct CacheBlock *p;
    p=get_CacheBlock();
    p ->data=data;
    p->Address=Address;
    p->DirtyBit=DirtyBit;
    p->right=q->right;
    p->left=q;
    q->right->left=p;
    q->right=p;
}

//insert a block in the end of the cache
void inserte(CacheBlock *first,int data,long long Address,bool DirtyBit)
{
    struct CacheBlock *p;
    p=get_CacheBlock();
    p->data=data;
    p->Address=Address;
    p->DirtyBit=DirtyBit;
    p->right=first;
    p->left=first->left;
    first->left->right=p;
    first->left=p;
}

//delete a Block from the first of the Cache
void dltf(CacheBlock **first)
{
    if((*first)->left==(*first))
    {
        delete *first;
        return;
    }
    struct CacheBlock *p;
    p=*first;
    (*first)->left->right=(*first)->right;
    (*first)->right->left=(*first)->left;
    (*first)=(*first)->right;
    delete p;
}

 //delete a Block from the middle of the cache
void dltm(CacheBlock *q)
{
    if(q->left==q)
    {
        delete q;
        return;
    }
    q->left->right=q->right;
    q->right->left=q->left;
    delete q;
}

 //delete a Block from the end of the cache
void dlte (CacheBlock *first)
{
    if(first->left==first)
    {
        delete first;
        return;
    }
    first->left->left->right=first;
    CacheBlock *q=first->left;
    first->left=first->left->left;
    delete q;
}

// Delete all cache blocks
void dltAll (CacheBlock *first)
{
    while(first->left!=first)
    {
        dlte(first);
    }
    delete first;
}

// delete all cache blocks with data=x
void dltdata(struct CacheBlock *first,int x)
{
    struct CacheBlock *p;
    p=first;
    while(p->right!=first)
    {

        if(p->data==x)
        {
            p=p->left;
            dltm(p->right);
        }
        p=p->right;
    }
    if(first->left->data==x)
        dlte(first);
}

int show_CacheBlockData(struct CacheBlock *p)
{
    return p->data;
}

// if the block exist in the cache Then return Pointer to the Block, Else return NULL
CacheBlock * Search_CacheBlock(CacheBlock *first,long long Address)
{
    CacheBlock *p;
    p=first;
    do{
            if(p->Address==Address)
                return p;
            p=p->right;
    }while(p!=first);
    return NULL;
}

class IOcacheInfo{
                    int TotalHit;
                    int ReadHit;
                    int WriteHit;
                    int TotalMiss;
                    int DiskAccess;
                    int ReadCache;
                    int WriteCache;
                    int TotalRequest;
                    int ReadRequest;
                    int WriteRequest;
                public:
                    IOcacheInfo(){TotalHit=0;ReadHit=0;WriteHit=0;TotalMiss=0;DiskAccess=0;ReadCache=0;WriteCache=0;TotalRequest=0;ReadRequest=0;WriteRequest=0;}
                    void ClearInfo(){TotalHit=0;ReadHit=0;WriteHit=0;TotalMiss=0;DiskAccess=0;ReadCache=0;WriteCache=0;TotalRequest=0;ReadRequest=0;WriteRequest=0;}
                    void PrintCSVfile(ostream &out);
                    friend ostream  &operator<<(ostream&, const IOcacheInfo&);
                    friend LRUcache;
                    };



//Print a IOcacheInfo (e.g. in the File)
ostream &operator<<(ostream &out, const IOcacheInfo &ICI)
{

	out<<"Total number of request:\t"<<ICI.TotalRequest<<"\nTotal number of read request:\t"<<ICI.ReadRequest
    <<"\nTotal number of write request:\t"<<ICI.WriteRequest<<"\nTotal Miss:\t"<<ICI.TotalMiss
    <<"\nTotal Hit:\t"<<ICI.TotalHit<<"\nRead Hit:\t"<<ICI.ReadHit<<"\nWrite Hit:\t"<<ICI.WriteHit
    <<"\nDisk Access:\t"<<ICI.DiskAccess<<"\nRead Cache:\t"<<ICI.ReadCache<<"\nWrite Cache:\t"<<ICI.WriteCache<<"\n";
	return out;
}

void IOcacheInfo::PrintCSVfile(ostream &out)
{
    out<<TotalRequest<<","<<ReadRequest<<","<<WriteRequest<<","<<TotalMiss
        <<","<<TotalHit<<","<<ReadHit<<","<<WriteHit<<","<<DiskAccess<<","<<ReadCache<<","<<WriteCache<<",";
}

class LRUcache {
                CacheBlock *First; //first Block of the Cache (first of the queue)
                int Size;       //Number of Blocks in the cache
                int BlockSize;
                string Policy;
                int TotalSize=0;    //Number of Blocks that can be save in cache when cache is free
                IOcacheInfo ICI;
            public:
                LRUcache ();
                void GetRequest(Request);   //send a request to the cache for cache it
                void AddCacheBlock (string Type,long long Address);    //Add a Block to the cache
                void Reconfigure (int cSize);
                int putTotalSize(){return TotalSize;}
                void EmptyCache(){dltAll(First);First = get_CacheBlock(NULL,NULL,0);Size=1;}
                void ClearIOcacheInfo(){ICI.ClearInfo();}
                void PrintCSVfile(ostream &out);
                friend ostream  &operator<<(ostream&, const LRUcache&);
                };
//Print a LRUcacheInfo (e.g. in the File)
ostream &operator<<(ostream &out, const LRUcache &LC)
{
    out<<LC.ICI;
	out<<"BlockSize:\t"<<LC.BlockSize<<"\nCache Size:\t"<<LC.TotalSize<<"\n";
	return out;
}

void LRUcache::PrintCSVfile(ostream &out)
{
    ICI.PrintCSVfile(out);
	out<<BlockSize<<","<<TotalSize<<",";
}

LRUcache :: LRUcache ()
{
    First = get_CacheBlock(NULL,NULL,0);
    Size=1;
    BlockSize=BLOCKSIZE;
    Policy=POLICY;
    TotalSize=0;
}

void LRUcache ::Reconfigure(int cSize)
{
    TotalSize=cSize;

}

void LRUcache :: AddCacheBlock (string Type,long long Address)
{
    if(Size==TotalSize)
    {
        if(Policy=="WB"&&First->left->DirtyBit==1) //eviction
        {
            ICI.ReadCache++;
            ICI.DiskAccess++;
        }
        dlte(First);
    }
    else
    {

        Size++;
    }
    bool DB=1;
    if(Type[0]=='R')
    {
        ICI.DiskAccess++;
        ICI.ReadCache++;
        DB=0;
    }
    else if(Policy=="WT")
        ICI.DiskAccess++;
    ICI.WriteCache++;

    insertf(First,NULL,Address,DB);
}

void LRUcache :: GetRequest (Request R)
{
    string Type = R.Type;
    long long Address = R.Address;
    int i,n=R.Size/BlockSize,m=0;   //n=request size / cache block size  , m = miss flag
    CacheBlock *p;
                        n=1; //one request save in one block of cache (with any size)

    ICI.TotalRequest++;
    if(Type[0]=='W')
    {
       ICI.WriteRequest++;
    }
    else
    {
        ICI.ReadRequest++;
    }

    for(i=0;i<n;i++)
    {
        p=Search_CacheBlock(First,Address+(i*D_Address));
        if(p!=NULL)
        {
            if(Type[0]=='W')
            {
                if(Policy=="WB")
                {
                    p->DirtyBit=1;
                }
                else if(Policy=="WT")
                {
                    ICI.DiskAccess++;
                }
                ICI.WriteCache++;
            }
            else
            {
                ICI.ReadCache++;
            }

            insertf(First,p->data,p->Address,p->DirtyBit);
            dltm(p);
        }
        else
        {
            if(Type[0]=='R'||Policy=="WT")
            {
                m=1;    // miss request
            }
            AddCacheBlock(Type,Address+(i*D_Address));
        }
    }
    if(m)
    {
        ICI.TotalMiss++;
        //cout<<R<<" Miss\n\n";
        result_1<<R<<"Miss\n\n";
    }
    else
    {
        ICI.TotalHit++;
        if(Type[0]=='W')
        {
            ICI.WriteHit++;
        }
        else
        {
            ICI.ReadHit++;
        }
        //cout<<R<<" Hit\n\n";
        result_1<<R<<"Hit\n\n";
    }
}


#endif // LRUCACHE_H_INCLUDED
