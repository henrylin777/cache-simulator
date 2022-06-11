#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <math.h>
using namespace std;

/********************************************************
Transfer the hexadecimal address to decimal address.
Parameters
    address : memory address in hexadecimal.
*******************************************************/
unsigned int HexToDeci(string address){     
    stringstream ss;    // Create a stringstream
    ss << hex << address;
    unsigned decimal;
    ss >> decimal;
    return decimal;
}

/*********************************************************** 
Get the tag in decimal form.
Parameters 
    address_to_deci : memory address in decimal.
    row : the number of index(row).
 **********************************************************/
unsigned int getTag(unsigned address_to_deci, unsigned row){      
    unsigned int tag_number = 0;
    // float base = 0;
    // bitset<32> binary(address_to_deci);
    // for(int i = 32-tag_length+1; i <= 32; i++){
        // tag_number = tag_number << 1;
        // tag_number = tag_number | binary[i];
        // if (binary[i] != 0){tag_number = tag_number + pow(2, base);}
        // base = base + 1;
    // }
    tag_number = address_to_deci / row;
    return tag_number;
}

/*********************************************************** 
Get the index in decimal form.
Parameters
    address_to_deci : memory address in decimal.
    row : the number of index(row).
 **********************************************************/
unsigned int getIndex(unsigned address_to_deci, unsigned row){  
    unsigned int index_number = 0;
    // bitset<32> binary(address_to_deci);
    // for(int i = 31 - tag_length; i >= 32 - index_length - tag_length; i--){
        // index_number = index_number << 1;
        // index_number = index_number | binary[i];
    // }
    index_number = address_to_deci % row;
    return index_number;
}

/********************************************************************
First-In-First-Out(FIFO) algorithm.
Parameters
    cache_index : the array in row of index.
    address_tag : the new tag will replace the older one.
    words : the number of block in corresponding set.
Return
    origin_tag : victim_tag
 *******************************************************************/
int FIFO(int* cache_index, unsigned int address_tag, int words){
    int origin_tag = cache_index[0];
    if(words == 1)     // If words is one then change cache_index[0] with new tag
        cache_index[0] = address_tag;
    else
        for(int i = 1; i < words; i++)
            cache_index[i-1] = cache_index[i];
        cache_index[words - 1] = address_tag;     // Renew the last position element
    return origin_tag;
}

/**************************************************************************
Least-Recently-Used(LRU) algorithm.
Parameters
    cache_index : the index of row.
    address_tag : the new tag will replace the older one.
    words : the number of block in corresponding set(column).
    visit_index : the index of column. the block that is hit or victim(visit_index=0).

Return
    origin_tag : victim_tag

 *************************************************************************/
int LRU(int* cache_index, unsigned int address_tag, int words, int visit_index){
    int origin_tag = cache_index[0];
    if(words == 1)     // If words is one then change cache_index[0] with new tag
        cache_index[0] = address_tag;
    else{       // Renew the order if one tag is visited in a block
        int temp = address_tag;
        int i;
        for(i = visit_index; i < words - 1; i++){
            if(cache_index[i+1] == -1)   // Not be written(Do not need to swap tag after this block)
                break;
            cache_index[i] = cache_index[i+1];
        }        
        cache_index[i] = temp;      // renew the last position element
    }
    return origin_tag;
}


/***********************************************************************
Judge whether is hit or miss or victim.
Parameters
    cache_index : the array in row of index.
    address_tag : the new tag that will be searched in cache.
    words : the number of block in corresponding set.
    replace_algorithm : which algorithm will be used when there exists victim. 
Return
    victim tag
 ***********************************************************************/
int judgeHit(int* cache_index, unsigned int address_tag, int words, int replace_algorithm){
    for(int i = 0; i < words; i++){
        if(cache_index[i] == address_tag){ // Hit

            if(replace_algorithm == 1 || replace_algorithm == 2){ // LRU to renew the position of tag
                LRU(cache_index, address_tag, words, i);
                return -1;
            }
        }
        else if(cache_index[i] == -1){ //  Block is empty, write tag to block

            cache_index[i] = address_tag;
            return -1;    
        }
        else{       // Not match
            continue;
        }
    }
    // After execution of for loop and did not return indicates there exists a Victim
    int origin_tag;
    if(replace_algorithm == 0){  // FIFO
        origin_tag = FIFO(cache_index, address_tag, words);     // Get origin tag
    }
    else if(replace_algorithm == 1 || replace_algorithm == 2){   // LRU
        origin_tag = LRU(cache_index, address_tag, words, 0);   // Get origin tag
    } 
    return origin_tag;    
}






int main(int argc, char *argv[]){

    fstream file_in, file_out;  // Create a file_in object
    int cache_size, block_size, associativity, replace_algorithm;
    unsigned int row, column, tag_length, index_length, offset_length;
    unsigned int address_to_deci, tag_pos, index_pos;
    int output_number;
    file_in.open(argv[1], ios::in); // Read the file_in depends on argv[1]
    file_out.open(argv[2], ios::out); // Write the file_out depends on argv[2]
    file_in >> cache_size >> block_size >> associativity >> replace_algorithm;
    cout << "cache_size: " << cache_size << " block_size: " << block_size << " associativity: " << associativity << " replace_algorithm: " << replace_algorithm << endl;

    // ============ Determine the array size of cache ============
    if(associativity == 0)  // Directed mapped
    {   
        column = 1;
        row = (int)(cache_size / block_size / column);
        // index_length = (int)log2(cache_size * 1024 / block_size);
        index_length = (int)log2(cache_size / block_size);
        offset_length = (int)log2(block_size);
        tag_length = 32 - offset_length - index_length;
    }
    else if(associativity == 1) // 4-way set associativity
    {
        column = 4;
        row = (int)(cache_size / block_size / column);
        // index_length = (int)log2(cache_size * 256 / block_size);
        index_length = (int)log2(row / column);
        offset_length = (int)log2(block_size);
        tag_length = 32 - offset_length - index_length;
    }
    else // Full associativity
    {
        row = 1;
        column = (int)(cache_size / block_size);
        index_length = 0;
        offset_length = (int)log2(block_size);
        tag_length = 32 - offset_length - index_length;
    }

    // ============ Initialize the cache ============
    int **cache = new int*[row];
    for(int i = 0; i < row; i++) 
        cache[i] = new int[column];

    int **frequency = new int*[row];
    for(int i = 0; i < row; i++) 
        frequency[i] = new int[column];

    // Initial zero to all elements in array
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
            cache[i][j] = -1;


    // for(int i = 0; i < row; i++)
    //     for(int j = 0; j < column; j++)
    //         frequency[i][j] = 0;
    // cout << "Size : " << sizeof(cache) << " Row : " << row << " Column : " << column << endl;
    // cout << "Tag length : " << tag_length << " Index length : " << index_length << " Offset length : " << offset_length << endl; 
    
    // ============ Parse input files ============
    float count=0;
    float num_of_miss=0;
    // If the file achieve to EOF, then "file_in >> str" will return 0 and break while loop
    // string str;

    while(file_in >> address_to_deci)  
    {
        // address_to_deci = HexToDeci(str);
        // cout << "block addr.: " << address_to_deci << endl;
        tag_pos = getTag(address_to_deci, row);
        // cout << "tag_pos: " << tag_pos << endl; 
        index_pos = getIndex(address_to_deci, row);
        // cout << "index_pos: " << index_pos << endl;
        output_number = judgeHit(cache[index_pos], tag_pos, column, replace_algorithm);
        file_out << output_number << endl;
        if (output_number != -1){num_of_miss = num_of_miss + 1;};
        count = count + 1;
        // cout << "-------------" << endl;

    }
    file_out << "Miss rate = " << num_of_miss/count << endl;     
    file_in.close(); // Close read file_in
    file_out.close(); // Close write file_out
    return 0;
}
