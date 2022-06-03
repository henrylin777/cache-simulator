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
    tag_length : the bit number of tag.
 **********************************************************/
unsigned int getTag(unsigned address_to_deci, int tag_length){      
    unsigned int tag_number = 0;
    bitset<32> binary(address_to_deci);
    for(int i = 31; i >= 32 - tag_length; i--){
        tag_number = tag_number << 1;
        tag_number = tag_number | binary[i];
    }
    return tag_number;
}

/*********************************************************** 
Get the index in decimal form.
Parameters
    address_to_deci : memory address in decimal.
    tag_length : the bit number of tag.
    index_length : the bit number of index.
 **********************************************************/
unsigned int getIndex(unsigned address_to_deci, int tag_length, int index_length){  
    unsigned int index_number = 0;
    bitset<32> binary(address_to_deci);
    for(int i = 31 - tag_length; i >= 32 - index_length - tag_length; i--){
        index_number = index_number << 1;
        index_number = index_number | binary[i];
    }
    return index_number;
}

/********************************************************************
First-In-First-Out(FIFO) algorithm.
Parameters
    cache_index : the array in row of index.
    address_tag : the new tag will replace the older one.
    words : the number of block in corresponding set.
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
    cache_index : the array in row of index.
    address_tag : the new tag will replace the older one.
    words : the number of block in corresponding set.
    visit_index : the block that is hit or victim(visit_index=0).
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
/* Other replace algorithm */
void PLRU(int* cache_index, int* frequency, unsigned int address_tag, int words, int visit_index){
    int count_zero = 0;
    if(words == 1)     // If words is one then change cache_index[0] with new tag
        cache_index[0] = address_tag;
    else{       // Renew the order if one tag is visited in a block
        for(int i = 0; i < words; i++) {     // Set position of visit_index in MRU Bit 1
            if(i == visit_index)    
                frequency[i] = 1;
            if(frequency[i] == 0)       // Count the number of zero in MRU Bit
                count_zero++;
        } 
    }
    if(count_zero == 0) {     // If there exists no 0
        for(int i = 0; i < words; i++) {     // Set position of visit_index in frequency 1, others are 0 
            if(i == visit_index)    // Not be written(Do not need to swap tag after this block)
                frequency[i] = 1;
            else
                frequency[i] = 0;
        }         
    }
}
int PLRU(int* cache_index, int* frequency, unsigned int address_tag, int words){
    int origin_tag, first_zero = 0;
    if(words == 1)     // If words is one then change cache_index[0] with new tag
        cache_index[0] = address_tag;
    else{       // Renew the order if one tag is visited in a block
        for(int i = 0; i < words; i++) {     // Set position of visit_index in frequency 1, others are 0 
            if(frequency[i] == 0 && first_zero < 1) {    // Get the first zero in MRU Bit and replace it
                origin_tag = cache_index[i];
                cache_index[i] = address_tag;
                frequency[i] = 1;   // Let the replace position in MRU Bit = 1
                break;
            }
        } 
    }
    return origin_tag;
}
/* Other replace algorithm */
int LFU(int* cache_index, int* frequency, unsigned int address_tag, int words){
    int origin_tag = cache_index[0];
    int temp_frequency = 1;
    int j;
    for(j = 0; j < words - 1; j++) {
        if(frequency[j + 1] == 0)
            break;
        else if(temp_frequency >= frequency[j + 1]) {
            cache_index[j] = cache_index[j+1];
            frequency[j] = frequency[j+1];
        }
        else
            break;
    }
    frequency[j] = temp_frequency;
    cache_index[j] = address_tag;   
    return origin_tag;
}
void LFUDROP(int* cache_index, int* frequency, int words){
    for(int j = 0; j < words - 1; j++) {
        if(frequency[j + 1] == 0)   // Indicates that there still have enough cache space
            return;
        else
            continue;
    }
    for(int j = 0; j < words - 1; j++) {
        cache_index[j] = cache_index[j+1];
        frequency[j] = frequency[j+1];
    }
    frequency[words-1] = 0;
    cache_index[words-1] = -1; 
}
void LFUHIT(int* cache_index, int* frequency, unsigned int address_tag, int words, int visit_index){
    if(words == 1)     // If words is one then change cache_index[0] with new tag
        frequency[0] += 1;
    else{      
        int temp_frequency = frequency[visit_index] + 1, temp_tag = address_tag;
        int j;
        for(j = visit_index; j < words - 1; j++) {
            if(frequency[j + 1] == 0)
                break;
            else if(temp_frequency >= frequency[j + 1]){
                cache_index[j] = cache_index[j+1];
                frequency[j] = frequency[j+1];
            }
            else
                break;
        }
        frequency[j] = temp_frequency;
        cache_index[j] = temp_tag;
        // LFUDROP(cache_index, frequency, words);
    }
}
void LFUMISS(int* cache_index, int* frequency, unsigned int address_tag, int words, int visit_index){
    if(words == 1){     // If words is one then change cache_index[0] with new tag
        frequency[0] += 1;
        cache_index[0] = address_tag;
    }
    else{      
        int temp_frequency = 1;
        int j;
        for(j = visit_index; j > 0; j--) {
            if(temp_frequency < frequency[j-1]) {
                cache_index[j] = cache_index[j-1];
                frequency[j] = frequency[j-1];
            }
            else 
                break; 
        }
        frequency[j] = temp_frequency;
        cache_index[j] = address_tag;
    }
}

/***********************************************************************
Judge whether is hit or miss or victim.
Parameters
    cache_index : the array in row of index.
    address_tag : the new tag that will be searched in cache.
    words : the number of block in corresponding set.
    replace_algorithm : which algorithm will be used when there exists victim. 
 ***********************************************************************/
int judgeHit(int* cache_index, unsigned int address_tag, int words, int replace_algorithm){
    for(int i = 0; i < words; i++){
        if(cache_index[i] == address_tag){      // Hit
            if(replace_algorithm == 1 || replace_algorithm == 2)      // LRU to renew the position of tag
                LRU(cache_index, address_tag, words, i);
            return -1;
        }
        else if(cache_index[i] == -1){       //  Block is empty, write tag to block
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
    string str;
    int cache_size, block_size, associativity, replace_algorithm;
    int row, column, tag_length, index_length, offset_length;
    int address_to_deci, tag_pos, index_pos;
    int output_number;
    file_in.open(argv[1], ios::in); // Read the file_in depends on argv[1]
    file_out.open(argv[2], ios::out); // Write the file_out depends on argv[2]
    file_in >> cache_size >> block_size >> associativity >> replace_algorithm;
    // cout << "Information : " << cache_size << "\t" << block_size << "\t" << associativity << "\t" << replace_algorithm << endl;

    // ============ Determine the array size of cache ============
    if(associativity == 0)  // Directed mapped
    {   
        column = 1;
        row = (int)(cache_size * 1024 / block_size / column);
        index_length = (int)log2(cache_size * 1024 / block_size);
        offset_length = (int)log2(block_size);
        tag_length = 32 - offset_length - index_length;
    }
    else if(associativity == 1) // 4-way set associativity
    {
        column = 4;
        row = (int)(cache_size * 256 / block_size / column);
        index_length = (int)log2(cache_size * 256 / block_size);
        offset_length = (int)log2(block_size);
        tag_length = 32 - offset_length - index_length;
    }
    else // Full associativity
    {
        row = 1;
        column = (int)(cache_size * 1024 / block_size);
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
    for(int i = 0; i < row; i++)
        for(int j = 0; j < column; j++)
            frequency[i][j] = 0;
    // cout << "Size : " << sizeof(cache) << " Row : " << row << " Column : " << column << endl;
    // cout << "Tag length : " << tag_length << " Index length : " << index_length << " Offset length : " << offset_length << endl; 
    
    // ============ Parse input files ============
    int count;
    int num_of_hits;
    float miss_rate;
    // If the file achieve to EOF, then "file_in >> str" will return 0 and break while loop
    while(file_in >> str)  
    {
        address_to_deci = HexToDeci(str);
        tag_pos = getTag(address_to_deci, tag_length);
        index_pos = getIndex(address_to_deci, tag_length, index_length);
        output_number = judgeHit(cache[index_pos], tag_pos, column, replace_algorithm);
        file_out << output_number << endl;
        if (output_number == -1){num_of_hits++;}
        count++;

    }
    miss_rate = num_of_hits/count;
    file_out << "Miss rate = " << miss_rate << endl;     
    file_in.close(); // Close read file_in
    file_out.close(); // Close write file_out
    return 0;
}