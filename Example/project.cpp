#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cmath>

using namespace std;

// ./project cache1.org reference1.lst index.rpt

class cache_block{
public:
    cache_block(): NRU(1){};
    int NRU;
    string address;
};

int main(int argc, char *argv[]){
    // open the files
    ifstream ORG;
    ifstream REF;
    ofstream OUT;
    int address_bit, block_size, cache_sets, associativity;
    string str, file_name;
    ORG.open(argv[1]);
    REF.open(argv[2]);
    OUT.open(argv[3]);
    // read .org file
    ORG >> str >> address_bit;
    ORG >> str >> block_size;
    ORG >> str >> cache_sets;
    ORG >> str >> associativity;
    // read reference file
    int cache_count = 0;
    REF >> str >> file_name >> str;
    vector<string> cache;
    while(str != ".end"){
        cache_count++;
        cache.push_back(str);
        REF >> str;
    }
    // get the offset bit number
    int offset_bit = 0;
    for(int i = block_size; i > 1; i /= 2, offset_bit++);
    // get the cache set bit number
    int cache_set_bit = 0;
    for(int i = cache_sets; i > 1; i /= 2, cache_set_bit++);
    // create the cache blocks
    map<int, vector<cache_block> > cb;
    for(int i = 0; i < cache_sets; i++){
        for(int j = 0; j < associativity; j++)
            cb[i].push_back(cache_block());
    }
    // read cache into the cache block
    int total_miss = 0;
    vector<bool> hit(cache_count, false);
    for(int i = 0; i < cache_count; i++){
        // the number of set
        int tag_bit = address_bit - cache_set_bit - offset_bit;
        int set_num = 0;
        for(int j = 0; j < cache_set_bit; j++)
            set_num += (cache[i][j + tag_bit] - '0') * pow(2, cache_set_bit - j - 1);
        string tag = cache[i].substr(0, tag_bit);
        // return hit
        for(int j = 0; j < cb[set_num].size(); j++){
            if(cb[set_num][j].address == tag){
                cb[set_num][j].NRU = 0;
                hit[i] = true;
                break;
            }
        }
        /*
        for(auto c : cb[set_num]){
            if(c.address == tag){
                c.NRU = 0;
                hit[i] = true;
                break;
            }
        }
        */
        // return miss
        if(!hit[i]){
            total_miss++;
            bool has_NRU_1 = false;
            // if there's a block in this set that NRU bit is 1
            for(int j = 0; j < associativity; j++){
                if(cb[set_num][j].NRU == 1){
                    cb[set_num][j].NRU = 0;
                    cb[set_num][j].address = tag;
                    has_NRU_1 = true;
                    break;
                }
            }
            // all blocks in this set that NRU bit is 0
            if(!has_NRU_1){
                for(int j = 0; j < associativity; j++) cb[set_num][j].NRU = 1;
                cb[set_num][0].address = tag;
                cb[set_num][0].NRU = 0;
            }
        }
    }
    // create the output file
    OUT << "Address bits: " << address_bit << endl;
    OUT << "Block size: " << block_size << endl;
    OUT << "Cache sets: " << cache_sets << endl;
    OUT << "Associativity: " << associativity << endl << endl;
    OUT << "Offset bit count: " << offset_bit << endl;
    OUT << "Indexing bit count: " << cache_set_bit << endl;
    OUT << "Indexing bits: ";
    for(int i = offset_bit + cache_set_bit - 1; i >= offset_bit; i--){
        if(i != offset_bit) OUT << i << " ";
        else OUT << i << endl << endl;
    }
    OUT << ".benchmark " << file_name << endl;
    for(int i = 0; i < cache_count; i++){
        OUT << cache[i] << " ";
        if(hit[i] == true) OUT << "hit";
        else OUT << "miss";
        OUT << endl;
    }
    OUT << ".end" << endl << endl;
    OUT << "Total cache miss count: " << total_miss;
    // close the files
    ORG.close();
    REF.close();
    OUT.close();
    return 0;
}