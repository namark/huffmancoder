#include "bitmass.h"

BitMass::BitMass()
{
    mass=cur_byte=0;
    sizeBits=sizeBytes=0;
    cur_bit_offs=0;
}

BitMass::BitMass(unsint sB)
{
    sizeBits=0;
    sizeBytes=sB;
    cur_byte=mass=new Byte[sB]();
    cur_bit_offs=0;
}

BitMass::BitMass(Byte* m, unsint sB)
{
    cur_byte=mass=m;
    sizeBytes=sB;
    sizeBits=0;
    cur_bit_offs=0;
}

BitMass::BitMass(const BitMass &x)
{
    sizeBits=x.sizeBits;
    sizeBytes=x.sizeBytes;
    cur_byte=mass=new Byte[sizeBytes];
    std::copy(x.mass,x.mass + x.sizeBytes, mass);
    cur_bit_offs=0;
}

BitMass::~BitMass()
{
    delete [] mass;
}

bool BitMass::getBit()
{
    return (*cur_byte) & 1ULL<<cur_bit_offs;
}

bool BitMass::getBit(unsint bit_index)
{
    return (mass[bit_index>>bpb_exp]) & 1ULL<<(bit_index & ~(~0ULL << bpb_exp)); // i think this may be faster than %, though it turned out more complicated than i thought
}

bool BitMass::getBit(unsint byte_index, unschar bit_offset)
{
    return (mass[byte_index]) & 1ULL<<bit_offset;
}


void BitMass::setBit(bool value)
{
    if(value) *cur_byte |= 1ULL<<cur_bit_offs;
    else *cur_byte &= ~(1ULL<<cur_bit_offs);
}

void BitMass::setBit(unsint bit_index, bool value)
{
    if(value) mass[bit_index>>bpb_exp] |= 1ULL<<(bit_index & ~(~0ULL << bpb_exp)); // i think this may be faster than %, though it turned out more complicated than i thought
    else mass[bit_index>>bpb_exp] &= ~(1ULL<<(bit_index & ~(~0ULL << bpb_exp)));
}

void BitMass::setBit(unsint byte_index, unschar bit_offset, bool value)
{
    if(value) mass[byte_index] |= 1ULL<<bit_offset;
    else mass[byte_index] &= ~(1ULL<<bit_offset);
}


void BitMass::switchBit(unsint bit_index)
{
    mass[bit_index>>bpb_exp] ^= 1ULL<<(bit_index & ~(~0ULL << bpb_exp));
}

void BitMass::switchBit(unsint byte_index, unschar bit_offset)
{
    mass[byte_index] ^= 1ULL<<bit_offset;
}

void BitMass::operator ~()
{
    *cur_byte ^= 1ULL<<cur_bit_offs;
}


Byte & BitMass::operator [](unsint byte_index) const
{
    return mass[byte_index];
}
Byte * BitMass::operator +(unsint some_offset) const
{
    return mass+some_offset;
}



void BitMass::operator ++()
{
    if(++cur_bit_offs>=(1ULL<<bpb_exp))
    {
        ++cur_byte;
        cur_bit_offs=0;
    }
}

void BitMass::operator --()
{
    if(cur_bit_offs==0)
    {
        --cur_byte;
        cur_bit_offs=(1ULL<<bpb_exp)-1;
    }
    else --cur_bit_offs;
}

void BitMass::setBitPointer()
{
    cur_byte=mass;
    cur_bit_offs=0;
}

void BitMass::setBitPointer(unsint bit_index)
{
    cur_byte = mass+(bit_index>>bpb_exp);
    cur_bit_offs = bit_index & ~(~0ULL << bpb_exp);
}

void BitMass::setBitPointer(unsint byte_index, unschar bit_offset)
{
    cur_byte = mass+byte_index;
    cur_bit_offs = bit_offset;
}

BitPointer BitMass::getCurrentPointer()
{
    return BitPointer(cur_byte,0,cur_bit_offs);
}

BitMass & BitMass::operator =(const BitMass &x)
{
    if(this == &x) return *this;
    delete [] mass;
    sizeBits=x.sizeBits;
    sizeBytes=x.sizeBytes;
    cur_byte=mass=new Byte[sizeBytes];
	std::copy(x.mass, x.mass + x.sizeBytes, mass);
    cur_bit_offs=0;
    return *this;
}

BitMass & BitMass::set(const Byte *m, unsint sB)
{
    delete [] mass;
    sizeBytes=sB;
    cur_byte=mass=new Byte[sizeBytes];
    std::copy(m,m + sB, mass);
    cur_bit_offs=0;
    return *this;
}

BitMass & BitMass::init(unsint sB)
{
    delete [] mass;
    sizeBytes=sB;
    cur_byte=mass=new Byte[sB]();
    cur_bit_offs=0;
    return *this;
}


void BitMass::operator +=(unsint bits)
{
}

void BitMass::operator -=(unsint bits)
{
}

BitMass & BitMass::operator +=(const BitMass & x) // some optimization can be done here
{
    unsint i=x.sizeBits;
    sizeBits+=i;
    BitPointer bp_to(*this,sizeBits);
    BitPointer bp_from(x);
    while(i)
    {
        bp_to.setBit(bp_from.getBit());
        ++bp_to;
        ++bp_from;
        --i;
    }
    return *this;
}


///////////////////////////////////////////////////////////////////

BitPointer::BitPointer()
{
    Bp=0;
    bit_offs=0;
}

BitPointer::BitPointer(const BitMass &x, unschar bit_index)
{
    Bp = x+(bit_index<<bpb_exp);
    bit_offs = bit_index & ~(~0ULL << bpb_exp);
}

BitPointer::BitPointer(const BitMass &x, unsint byte_index, unschar bit_offset)
{
    Bp=x+byte_index;
    bit_offs=bit_offset;
}

BitPointer::BitPointer(Byte * x, unsint byte_index, unschar bit_offset)
{
    Bp=x+byte_index;
    bit_offs=bit_offset;
}

BitPointer::BitPointer(Byte * x, unschar bit_index)
{
    Bp = x+(bit_index<<bpb_exp);
    bit_offs = bit_index & ~(~0ULL << bpb_exp);
}

BitPointer::BitPointer(const BitPointer &x)
{
    Bp=x.Bp;
    bit_offs=x.bit_offs;
}

BitPointer & BitPointer::set(const BitMass &x, unschar bit_index)
{
    Bp = x+(bit_index<<bpb_exp);
    bit_offs = bit_index & ~(~0ULL << bpb_exp);
    return *this;
}

BitPointer & BitPointer::set(const BitMass &x, unsint byte_index, unschar bit_offset)
{
    Bp=x+byte_index;
    bit_offs=bit_offset;
    return *this;
}

BitPointer & BitPointer::set(Byte * x, unsint byte_index, unschar bit_offset)
{
    Bp=x+byte_index;
    bit_offs=bit_offset;
    return *this;
}

BitPointer & BitPointer::set(Byte * x, unschar bit_index)
{
    Bp = x+(bit_index<<bpb_exp);
    bit_offs = bit_index & ~(~0ULL << bpb_exp);
    return *this;
}

BitPointer & BitPointer::operator =(const BitPointer &x)
{
    Bp=x.Bp;
    bit_offs=x.bit_offs;
    return *this;
}

BitPointer & BitPointer::operator =(const BitMass &x)
{
    Bp=x+0;
    bit_offs=0;
    return *this;
}

BitPointer & BitPointer::operator =(Byte *x)
{
    Bp=x;
    bit_offs=0;
    return *this;
}

BitPointer & BitPointer::operator ++()
{
    if(++bit_offs>=(1ULL<<bpb_exp))
    {
        ++Bp;
        bit_offs=0;
    }
    return *this;
}

BitPointer & BitPointer::operator --()
{
    if(bit_offs==0) // or maybe just if(bit_offs--){stuff}
    {
        --Bp;
        bit_offs=(1ULL<<bpb_exp)-1;
    }
    else --bit_offs;
    return *this;
}

bool BitPointer::getBit()
{
    return (*Bp) & 1ULL<<bit_offs;
}

void BitPointer::setBit(bool value)
{
    if(value) *Bp |= 1ULL<<bit_offs;
    else *Bp &= ~(1ULL<<bit_offs);
}

void BitPointer::operator ~()
{
    *Bp ^= 1ULL<<bit_offs;
}

BitPointer & BitPointer::operator +=(unsint bits)
{

    return *this;
}

BitPointer & BitPointer::operator -=(unsint bits)
{

    return *this;
}



