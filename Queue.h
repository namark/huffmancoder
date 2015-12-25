#ifndef QUEUE_H
#define QUEUE_H

typedef unsigned int unsint;
typedef unsigned char unschar;

template <class El, bool ascending = false>
class Queue
{
    El *mass;
    unsint size;
    unsint tail; // on last element
public:
    Queue(El * m=0, unsint s=0, unsint element_count=0){mass=m; size=s; tail=element_count-1;}
    Queue(unsint s){mass = new El[s]; size=s; tail=-1;}
    Queue(const Queue & x)
    {
        size=x.size;
        tail=x.tail;
        mass=new El[size];
        for(unsint i=0; i<size; ++i)
            mass[i]=x.mass[i];
    }

    Queue & create(unsint s)
    {
        delete [] mass;
        mass = new El[s];
        size=s;
        tail=-1;
        return *this;
    }


    Queue & set(El * m=0, unsint s=0, unsint element_count=0) // does not copy
    {
        delete [] mass;
        mass=m;
        size=s;
        tail=element_count-1;
    }
    Queue & set(Queue & x) // does not copy
    {
        delete [] mass;
        mass=x.mass;
        size=x.size;
        tail=x.tail;
    }

    ~Queue(){delete [] mass;}
    unsint getMassSize() const {return size;}
    unsint getTail() const { return tail;}
    Queue& operator+=(const El & x){ mass[++tail]=x; return *this;} // add an x to the end of the queue
    El operator--(){ return mass[tail--];} // remove and return an element from the end of the queue
    Queue& operator-=(const unsint c){ tail-=c; return *this;} // remove c elements from the end of the queue
    const El& operator [](int index) const {return mass[index];} // hmmm...
    El& operator [](int index) {return mass[index];}

    Queue & operator =(const Queue & x)
    {
        if(this == &x) return x; // happy?
        delete [] mass;
        size=x.size;
        tail=x.tail;
        mass=new El[size];
        for(unsint i=0; i<size; ++i)
            mass[i]=x.mass[i];
        return *this;
    }

    int insertSorted(const El & x) // descending order
    {
        int i=(++tail)>>1, j=0;
        while(i)
        {
            if(mass[j+i]>x) j=j+i;
            i>>=1;
        }
        if(tail && mass[j]>x) ++j;
        i=tail;
        while(i>j)
            mass[i--]=mass[i];
        mass[j]=x;
        return j;
    }
};



template <class El>
class Queue<El,true>
{
    El *mass;
    unsint size;
    unsint tail; // on last element
public:
    Queue(El * m=0, unsint s=0, unsint element_count=0){mass=m; size=s; tail=element_count-1;}
    Queue(unsint s){mass = new El[s]; size=s; tail=-1;}
    Queue(const Queue & x)
    {
        size=x.size;
        tail=x.tail;
        mass=new El[size];
        for(unsint i=0; i<size; ++i)
            mass[i]=x.mass[i];
    }

    Queue & create(unsint s)
    {
        delete [] mass;
        mass = new El[s];
        size=s;
        tail=-1;
        return *this;
    }


    Queue & set(El * m=0, unsint s=0, unsint element_count=0) // does not copy
    {
        delete [] mass;
        mass=m;
        size=s;
        tail=element_count-1;
    }
    Queue & set(Queue & x) // does not copy
    {
        delete [] mass;
        mass=x.mass;
        size=x.size;
        tail=x.tail;
    }

    ~Queue(){delete [] mass;}
    unsint getMassSize() const {return size;}
    unsint getTail() const { return tail;}
    Queue& operator+=(const El & x){ mass[++tail]=x; return *this;} // add an x to the end of the queue
    El operator--(){ return mass[tail--];} // remove and return an element from the end of the queue
    Queue& operator-=(const unsint c){ tail-=c; return *this;} // remove c elements from the end of the queue
    const El& operator [](int index) const {return mass[index];} // hmmm...
    El& operator [](int index) {return mass[index];}

    Queue & operator =(const Queue & x)
    {
        if(this == &x) return x; // happy?
        delete [] mass;
        size=x.size;
        tail=x.tail;
        mass=new El[size];
        for(unsint i=0; i<size; ++i)
            mass[i]=x.mass[i];
        return *this;
    }

    int insertSorted(const El & x) // ascending order
    {
        int i=(++tail)>>1, j=0;
        while(i)
        {
            if(mass[j+i]<x) j=j+i;
            i>>=1;
        }
        if(tail && mass[j]<x) ++j;
        i=tail;
        while(i>j)
            mass[i--]=mass[i];
        mass[j]=x;
        return j;
    }
};

#endif // QUEUE_H
