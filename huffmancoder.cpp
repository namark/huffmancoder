
#include "huffmancoder.h"



HuffmanCoder::HuffmanCoder(const char *filename)
{
    FILE * file=fopen(filename,"rb");
    fseek(file,0,SEEK_END);
    data_length=ftell(file);
    fseek(file,0,SEEK_SET);
    data=new bit8[data_length];
    clean_data=true;
    fread(data,data_length,sizeof(bit8),file);
    fclose(file);
#ifdef OUTPUT_PROGRESS
    OutputProgress=0;
    //object=0;
    //OutputMemberFunction=0;
#endif
}

void HuffmanCoder::makeTree(bit8 ** codes)
{
    Queue< BinTree<unsint> > temp_leaf_queue(leaf_queue);
    while(temp_leaf_queue.getTail())
        temp_leaf_queue.insertSorted(((--temp_leaf_queue)+(--temp_leaf_queue)).code(codes));

    tree.destroy();
    tree=temp_leaf_queue[0];
}


void HuffmanCoder::encode()
{
    unsint *all=new unsint[MAX_ALPHABET_SIZE](); // will hold frequencies of all possible symbols
    bit8 *needed=new bit8[MAX_ALPHABET_SIZE]; // will hold the symbols with frequencies >0, which form the alphabet
    unsint i, alphabet_size=0;
    for(i=0;i<data_length;++i)
        if(!(all[data[i]]++)) needed[alphabet_size++]=data[i];


    leaf_queue.create(alphabet_size);
    for(i=0; i<alphabet_size; ++i)
        leaf_queue.insertSorted(BinTree<unsint>(needed[i],all[needed[i]]));
    delete [] needed;
    delete [] all;


    bit8 * codes[MAX_ALPHABET_SIZE];// will hold a code lengths and codes, which are stored in reverse
	std::fill_n(codes, MAX_ALPHABET_SIZE*(sizeof(bit8*)), nullptr );

    makeTree(codes);

    unsint size=0;// determine the size of the compressed data, and initialize
    for(i=0;i<alphabet_size;++i)
        size+= leaf_queue[i].getRootData() * (codes[leaf_queue[i].getRootLeafValue()][0]); // size in bits
    compressed.init((size>>3) + ( (size%8)>0 )); //convert to bytes and create compressed data
    compressed.SizeBits()=size;

#ifdef OUTPUT_PROGRESS

    unsint call_value=data_length/100;
    unsint percent=0;

#endif

    for(i=0; i<data_length; ++i)// encode
    {
        bit8 * code=codes[data[i]];// get the code for current symbol
        bit8 j=code[0];// get it's size, which is also an index to the first code symbol
        while(j)
        {
            if(code[j]) ~compressed; // BitMass is initialised to 0s, so just put 1s in appropriate places
            --j;
            ++compressed;
        }
#ifdef OUTPUT_PROGRESS

        if(!(i%call_value) && OutputProgress) OutputProgress(++percent);
        //if(object && OutputMemberFunction) (object->OutputMemberFunction)(i,data_length);

#endif
    }

    for(unsint i=0; i<MAX_ALPHABET_SIZE;++i) delete [] codes[i];
}

void HuffmanCoder::decode()
{
    // decode useing tree
    compressed.setBitPointer();
    data_length=tree.getRootData();
    data=new bit8[data_length];
    clean_data=true;
    unsint writer=0;
    BinTree<unsint> parser(tree);
    unsint size=compressed.SizeBits();
#ifdef OUTPUT_PROGRESS

    unsint call_value=size/100;
    unsint percent=0;

#endif
    while(size--)
    {
        if(compressed.getBit()) parser=parser.getBrunch1();
        else parser=parser.getBrunch0();
        if(parser.isLeaf())
        {
            data[writer]=parser.getRootLeafValue();
            ++writer;
            parser=tree;
        }
        ++compressed;
#ifdef OUTPUT_PROGRESS

        if(!(size%call_value) && OutputProgress) OutputProgress(++percent);
        //if(object && OutputMemberFunction) (object->OutputMemberFunction)(i,data_lenght);

#endif
    }
}

// file structure:
// size of alphabet 4B  (should change to 1B, 0 will be interpreted as 256)
// alphabet symbols and their frequencies, sorted
// size of compressed data in bits 4B
// size of compressed data in bytes 4B
// compressed data

bool HuffmanCoder::writeToFile(FILE *f)
{
    unsint alphabet_size=leaf_queue.getMassSize()*sizeof(bit8), header_size=alphabet_size*5+12; // header contains : alphabet size(4B) + alphabet(5B=symbol(1B)+count(4B)) + compressed data size in bits(4B) + compr. data size in bytes(4B)= ("alphabet size"*5+12)B
    bit8 * header= new bit8[header_size];
    *((unsint *) header)=alphabet_size;
    unsint j=4;
    for(unsint i=0; i<alphabet_size; ++i, j+=5)
    {
        header[j]=leaf_queue[i].getRootLeafValue();
        *((unsint *) (header+j+1))=leaf_queue[i].getRootData();
    }
    *((unsint *) (header+j))=compressed.getSizeInBits();
    *((unsint *) (header+j+4))=compressed.getSizeInBytes();
    fwrite(header,header_size,sizeof(bit8),f);
    fwrite(compressed+0,compressed.getSizeInBytes(),sizeof(Byte),f);
    delete [] header;
    return true;
}

bool HuffmanCoder::writeToFile(const char *fn)
{
    FILE *f = fopen(fn,"wb");
    if(!f) return false;
    bool ret = writeToFile(f);
    fclose(f);
    return ret;
}

bool HuffmanCoder::readFromFile(FILE *f)
{
    fseek(f,0,SEEK_END);
    unsint s= ftell(f);
    fseek(f,0,SEEK_SET);
    bit8 * read= new bit8[s];
    fread(read,s,sizeof(bit8),f);
    setCompressed(read);
    delete [] read;
    return true;
}

bool HuffmanCoder::readFromFile(const char *fn)
{
    FILE *f = fopen(fn,"rb");
    if(!f) return false;
    bool ret = readFromFile(f);
    fclose(f);
    return ret;
}


void HuffmanCoder::setCompressed(const bit8 *x)
{
    unsint alphabet_size=((unsint *)x)[0]; // first 4B contain size of the alphabet
    leaf_queue.create(alphabet_size);
    unsint j=4;
    for(unsint i=0; i<alphabet_size; ++i, j+=5) leaf_queue+=BinTree<unsint>(x[j],*((unsint *)(x+j+1))); // then comes alphabet itself, stored(1B for symbol,4B for count) a as pairs of symbols and their counts(calculated from initial file),, sorted
    makeTree();
    // then 4B for size of compressed data in bits, then 4B for same in bytes, then the actual compredssed data
    compressed.set(x+j+8,*((unsint *) (x+j+4)));
    compressed.setSizeInBits(*((unsint *)(x+j)));
}


bool HuffmanCoder::setData(const char *fn)
{
    if(clean_data) delete [] data;

    FILE * file=fopen(fn,"rb");
    if(!file) return false;
    fseek(file,0,SEEK_END);
    data_length=ftell(file);
    fseek(file,0,SEEK_SET);
    data=new bit8[data_length];
    clean_data=true;
    fread(data,data_length,sizeof(bit8),file);
    fclose(file);
#ifdef OUTPUT_PROGRESS
    OutputProgress=0;
    //object=0;
    //OutputMemberFunction=0;
#endif

    return true;
}

bool HuffmanCoder::writeDataToFile(FILE *f)
{
    fwrite(data,data_length,sizeof(bit8),f);
    return true;
}

bool HuffmanCoder::writeDataToFile(const char* fn)
{
    FILE * file=fopen(fn,"wb");
    if(!file) return false;
    bool ret = writeDataToFile(file);
    fclose(file);
    return ret;
}
