#ifndef BITMASS_H
#define BITMASS_H
#include <memory>

//Note about STL bitset:
//Biggest issue, I think, is, that it has a fixed size, which is very inconvenient in my case.
//It does not seem to have bit switching functuanlity, which, sometimes, I believe, may be the fastest way to work with bits.
//It's probably meant to be memory efficient alternative for booleans(flags... variables representing true or false), hence it's... "not good"... for Huffman codeing.

typedef unsigned char Byte; // can change to any other type
typedef unsigned char unschar;
typedef unsigned int unsint;
const unschar bpb_exp=3; // 2^bpb_exp bits per Byte... change it according to Byte type above and machine specs

class BitPointer;

class BitMass // i could derive this one from BitPoiter... but i made this first...
{
    Byte *mass;
    unsint sizeBits; // size in bits, variable, maximum valid value is sizeBytes^bpb_exp, currently used only for operator += and should be handled externally
    unsint sizeBytes; // size of the mass in bytes
    Byte *cur_byte; // cur_byte, cur_bit_off together form a bit pointer... sort of
    unschar cur_bit_offs;
public:
    BitMass();
    BitMass(unsint sB);
    BitMass(Byte* m, unsint sB);
    BitMass(const BitMass &x);
    BitMass & operator =(const BitMass &x);
    ~BitMass();

    unsint & SizeBits() { return sizeBits;} // or just make it public
    unsint getSizeInBits() const{return sizeBits;}
    void setSizeInBits(unsint sbit){ sizeBits=sbit;}
    BitMass & set(const Byte* m, unsint sB);
    BitMass & init(unsint sB);
    unsint getSizeInBytes() const{return sizeBytes;}

    bool getBit(); // specified by bit pointer
    bool getBit(unsint bit_index);
    bool getBit(unsint byte_index, unschar bit_offset);
    void setBit(bool value=true); // specified by bit pointer
    void setBit(unsint bit_index, bool value=true);
    void setBit(unsint byte_index, unschar bit_offset, bool value=true);
    void switchBit(unsint bit_index);
    void switchBit(unsint byte_index, unschar bit_offset);
    void operator ~(); // switch bit specified by bit pointer

    // accessing as an array
    Byte& operator [](unsint byte_index) const;
    Byte* operator +(unsint some_offset) const;

    // increment and decrement bit pointer
    void operator ++();
    void operator --();

    void operator +=(unsint bits);
    void operator -=(unsint bits);

    BitMass& operator +=(const BitMass & x); // append a BitMass
    void setBitPointer(); // sets it to beginning
    void setBitPointer(unsint bit_index);
    void setBitPointer(unsint byte_index, unschar bit_offset);

    BitPointer getCurrentPointer();
};

class BitPointer // can work with BitMass, or with an array of Bytes
{
    Byte * Bp;
    unschar bit_offs;
public:
    BitPointer();
    BitPointer(const BitMass &x, unschar bit_index);
    BitPointer(const BitMass &x, unsint byte_index=0, unschar bit_offset=0);
    BitPointer(Byte * x, unsint byte_index=0, unschar bit_offset=0);
    BitPointer(Byte * x, unschar bit_index);
    BitPointer(const BitPointer &x);


    BitPointer& set(const BitMass &x, unschar bit_index);
    BitPointer& set(const BitMass &x, unsint byte_index=0, unschar bit_offset=0);
    BitPointer& set(Byte * x, unsint byte_index=0, unschar bit_offset=0);
    BitPointer& set(Byte * x, unschar bit_index);
    BitPointer& operator =(const BitPointer &x);
    BitPointer& operator =(const BitMass &x);
    BitPointer& operator =(Byte *x);

    BitPointer& operator ++();
    BitPointer& operator --();
    BitPointer& operator +=(unsint bits);
    BitPointer& operator -=(unsint bits);

    bool getBit(); // specified by bit pointer
    void setBit(bool value=true); // specified by bit pointer
    void operator ~(); // switch bit specified by bit pointer
};

#endif // BITMASS_H
