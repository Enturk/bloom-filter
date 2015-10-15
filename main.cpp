#include "primes.h"
#include <iostream>
#include <string>
#include <bitset>
#include <ctime>
#include <stdlib.h>
#include <fstream>
#include <sstream>
using namespace std;

#define TABLE_SIZE 800000
#define NUM_HASHES 9

struct Hasher {
    unsigned int a;
    unsigned int b;
    unsigned int p;
    unsigned int n;

    Hasher(unsigned int table_size){
        a = rand() % table_size + 1;
        b = rand() % table_size + 1;
        p = random_prime(table_size, 2*table_size);
        n = table_size;
    };

    ~Hasher(){};

    unsigned int hash(string key){
        unsigned int ans = 0;
        for(unsigned int i = 0; i < key.length(); ++i){
            ans = (ans + (key[i]*pow_mod(a, i, p) % p)) % p;
        }
        return ans % n;
    }
};

class Bloom {

    private:
        bitset<TABLE_SIZE> table;
        Hasher* hashers;

    public:
        Bloom(){
            hashers = (Hasher *) malloc(sizeof(Hasher)*NUM_HASHES);
            for (int i = 0; i < NUM_HASHES; ++i){
                new (hashers+i) Hasher(TABLE_SIZE);
            }
        };
        
        Bloom(string filename){
            hashers = (Hasher *) malloc(sizeof(Hasher)*NUM_HASHES);
            for (int i = 0; i < NUM_HASHES; ++i){
                new (hashers+i) Hasher(TABLE_SIZE);
            }
            readFromBinary(filename);
        };
        
        ~Bloom(){
            for (int i = 0; i < NUM_HASHES; ++i){
                (hashers+i)->~Hasher();
            };
            
            free(hashers);
        };

        void dumpToBinary(string filename){
            //First the table
            //Then a,b,p for each hash
            ofstream outfile(filename.c_str(), ios::binary);
            outfile.write(reinterpret_cast<char*>(&table), sizeof(table));
            for (int i = 0; i < NUM_HASHES; ++i){
                outfile.write(reinterpret_cast<char*>(&(hashers[i].a)), sizeof(hashers[i].a));
                outfile.write(reinterpret_cast<char*>(&(hashers[i].b)), sizeof(hashers[i].b));
                outfile.write(reinterpret_cast<char*>(&(hashers[i].p)), sizeof(hashers[i].p));
            }
        };
        
        void readFromBinary(string filename){
            //First the table
            //Then a,b,p for each hash
            ifstream infile(filename.c_str(), ios::binary);
            char* p2table = reinterpret_cast<char*>(&table);
            infile.read(p2table, sizeof(table));
            for (int i = 0; i < NUM_HASHES; ++i){
                unsigned int *p2int = &hashers[i].a;
                infile.read(reinterpret_cast<char*>(p2int), sizeof(hashers[i].a));
                p2int = &hashers[i].b;
                infile.read(reinterpret_cast<char*>(p2int), sizeof(hashers[i].b));
                p2int = &hashers[i].p;
                infile.read(reinterpret_cast<char*>(p2int), sizeof(hashers[i].p));
            }
            infile.close();
        };
        
        void insert(string key){
            int hashed_index = 0;
            for (int i = 0; i < NUM_HASHES; ++i){
                hashed_index = hashers[i].hash(key);
                table.set(hashed_index, true);
            }
        };
        
        bool is_in(string key){
            int hashed_index = 0;
            for (int i = 0; i < NUM_HASHES; ++i){
                hashed_index = hashers[i].hash(key);
                if (table[hashed_index] == 0){
                    return false;
                }
            }
            return true;
        };
};


int main(){
    srand( time(0) );
    Bloom newbloom("bloom.bin");
    // Bloom newbloom;
    // ifstream infile("wrds.txt");//wrds.txt");
    // string word;
    // while (getline(infile, word)){
    //     newbloom.insert(word);
    // }
    // infile.close();
    // newbloom.dumpToBinary("bloom.bin");

    ifstream itfile("test_words.txt");
    string test_word;
    while (getline(itfile, test_word)){
        if (newbloom.is_in(test_word)){
            cout << test_word << " is in the dictionary." << endl;
        } else {
            cout << test_word << " is NOT in." << endl;
        }
    }
    itfile.close();
    return 0;
};