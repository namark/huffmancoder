#ifndef HUFFMANCODER_H
#define HUFFMANCODER_H
#include "bitmass.h"
#include "Queue.h"
#include <stdio.h>

#define OUTPUT_PROGRESS

// this thing is getting messy... i hope i'll not fail horribly... i like this monster!
typedef unschar bit8;
const unsint MAX_ALPHABET_SIZE=256;

template <class NodeType>
class BinTree
{
    struct BinNode
    {
        NodeType data; // holds a frequency, or added frequencies
        BinNode * parent;
        BinNode * child1;
        BinNode * child0;

        BinNode(NodeType d, BinNode * p=0) { data=d; parent=p; child1=child0=0;}
        ~BinNode() {delete child1; delete child0;}
        virtual bit8 getLeafValue(){return 0;}

        void code(bit8 bit, bit8 ** code_array) // a step in construction of codes: appends bit to the code of symbols, specified by leaf values, in code array
        {
            if(child0) // if node has a child, then it has both children (from construction of Huffman tree)
            {
                child0->code(bit,code_array); //proceed recursively
                child1->code(bit,code_array);
            }
            else // if node has no child0, it has no child1 either, hence it's a leaf
            {
                bit8 *& code=code_array[ ((BinLeaf*)this) ->getLeafValue() ];// write a code
                if(!code)
                {
                    code=new bit8[MAX_ALPHABET_SIZE](); //maximum length of the code is MAX_ALPHABET_SIZE-1, +1 for code length
                }
                code[++code[0]]=bit; // code[0] holds the length of the code
            }
        }
    };

    struct BinLeaf : public BinNode
    {
        bit8 lv; // holds a symbol

        BinLeaf(bit8 leaf_value, NodeType d, BinNode * p=0): BinNode(d,p)
        { lv=leaf_value; }
        bit8 getLeafValue(){return  lv;}
    };

    BinNode * root;
public:
    BinTree() {root=0;}
    BinTree(NodeType data) {root = new BinNode(data);}
    BinTree(bit8 leaf_value, NodeType data) {root = new BinLeaf(leaf_value,data);}
    BinTree(BinNode * x){ root=x;}
    //BinTree(const BinTree & x);
    ~BinTree() {/*delete root;*/}
    void destroy(){ delete root;}
    NodeType getRootData() {return root->data;}
    bit8 getRootLeafValue(){return root->getLeafValue();}
    void setRootData(NodeType d) {root->data=d;}
//    BinNode * release() // very specialized to use with operator+
//    {
//        BinNode * ret=root;
//        root = 0;
//        return ret;
//    }

    BinNode * getBrunch0() {return root->child0;}
    BinNode * getBrunch1() {return root->child1;}
    BinNode * getRoot() {return root;}
    bool isLeaf() {return !(root->child0) /* && !(root->child1) */; }

    // BinTree& operator=(const BinTree & x);
    BinTree & operator=(BinNode * x) { root=x; return *this;}
    BinTree operator+(const BinTree & x) const //a step in the construction of Huffman tree: unites two trees as subtrees of a new tree, data for the root of which is the sum of subtrees' root data , no copying only pointer assignments
    {
        BinTree sum(this->root->data + x.root->data);
        sum.root->child0=this->root;
        sum.root->child1=x.root;
        return sum;
    }
    BinTree & code(bit8 ** code_array) // appends 0(1) to the codes of all symbols of child0(child1); used to construct codes, while constructing the tree
    {
        if(code_array)
        {
            root->child0->code(0,code_array);
            root->child1->code(1,code_array);
        }
        return *this;
    }
    bool operator <(const BinTree & x) const {return root->data<x.root->data;}
    bool operator >(const BinTree & x) const {return root->data>x.root->data;}
    bool operator <=(const BinTree & x) const {return root->data<=x.root->data;}
    bool operator >=(const BinTree & x) const {return root->data>=x.root->data;}
    NodeType operator -(const BinTree & x) const {return root->data-x.root->data;}


};


class HuffmanCoder
{
    bit8 * data; // uncompressed
    unsint data_length;
    BinTree<unsint> tree; // huffmann tree
    Queue< BinTree<unsint> > leaf_queue; // contains an alphabet, with (and sorted by)frequencies
    BitMass compressed;
    bool clean_data;
#ifdef OUTPUT_PROGRESS

public:
    typedef void (*ProgressOutputFunction)(unsint); // (percentage complete)
private:
    ProgressOutputFunction OutputProgress;
    //ClassType * object; // should be typedefed
    //MemberFunctionPointer OutputMemberFunction; // should be typedefed

#endif

public:
    HuffmanCoder()
    {
        data=0; data_length=0; clean_data=false;
#ifdef OUTPUT_PROGRESS
        OutputProgress=0;
        //object=0;
        //OutputMemberFunction=0;
#endif
    }
    HuffmanCoder(bit8 * d, unsint lenght)
    {
        data=d; data_length=lenght; clean_data=false;
#ifdef OUTPUT_PROGRESS
        OutputProgress=0;
        //object=0;
        //OutputMemberFunction=0;
#endif
    }
    HuffmanCoder(const char * filename); //reads data from file;
    ~HuffmanCoder()
    {
        tree.destroy();
        if(clean_data) delete [] data;
    }
    void makeTree(bit8 ** codes=0); // constructs a tree over leaf_queue(must be sorted), creates a code for each symbol in codes[symbol]

    void encode();// data -> leaf_queue, tree, compressed
    void decode();// compressed, leaf_queue -> tree, data

    bool writeToFile(FILE *f);// writes huffman code
    bool writeToFile(const char *fn);
    bool readFromFile(FILE *f);// reads huffman code
    bool readFromFile(const char *fn);

    bool writeDataToFile(FILE *f); // writes data to the file
    bool writeDataToFile(const char* fn);

    void setData(bit8 * d, unsint lenght){if(clean_data)delete [] data; data=d; data_length=lenght; clean_data=false;}
    bool setData(const char * fn); // read data from the file
    void setCompressed(const bit8 * x); // x -> leaf_queue, compressed; used in readFromFile()

    bit8 * getData(){ return data;}
    unsint getDataLenght(){return data_length;}
    const BitMass & getBitMass(){return compressed;}
    //BitMass* getBitMassAddr(){return &compressed;}
    Byte * getCompressed(){ return compressed+0;}
    unsint getCompressedLength() {return compressed.getSizeInBits();}
    unsint getHeaderSize() { return leaf_queue.getMassSize()*sizeof(bit8)*5+12;}

#ifdef OUTPUT_PROGRESS

    void setOutputProgressFunc(ProgressOutputFunction OP){ OutputProgress=OP;}
    //void setOutputProgressFunc(ClassType * obj, MemberFunctionPointer MF) { object=obj; OutputMemberFunction=MF;}

#endif
};

#endif // HUFFMANCODER_H
