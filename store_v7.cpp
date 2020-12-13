# include <iostream>
# include <fstream>
# include <cstdlib>
# include <cmath>
# include <vector>
# include <map>
# include <random>

using namespace std;

int TLB_MAX = 1000000;
int BUFFER_MAX = 1024; //256000000
int MAX_ENTRY = 12000000; // 2GB ÷ 160 = 13421772(13000000)
int ROM_NUM = 2;
int PAGE_NUM = 128;

int classify(long long key){
    // spilt to 128 files
    return (int)(key / pow(2,56));
}

string get_filename(string s){
    vector<string> buffer;
    string tmp;
    int current = 0, next;
    while(1){
        next = s.find_first_of('/',current);
        if(next != current){
            tmp = s.substr(current,next-current);
            if(tmp.size() != 0){
                buffer.push_back(tmp);
            }
        }
        if(next == string::npos){
            break;
        }
        current = next + 1;
    }
    return buffer.at(buffer.size()-1);
}

// check whether the page is in page_list
bool is_page_in_pagelist(int page, map<int,long long> &page_list){
    map<int,long long>::iterator iter = page_list.find(page);
    if(iter != page_list.end()){
        return true;
    }else{
        return false;
    }
}

// output file
void output_file(string &buffer,string filename){
    fstream fout;
    fout.open(filename + ".output",ios::app);
    fout << buffer;
    buffer.clear();
    fout.close();
}

// update page from TLB
void update(int page,vector<long long> &start_key, map <long long,string> &TLB,map<long long,string> &ROM,map<int,long long>&page_list){
    // check whether the page is in ROM 
    bool in_rom = is_page_in_pagelist(page,page_list);
    // move all keys that in same page to file
    // check whether TLB has key 
    if(start_key.at(page) != -1){
        fstream fout;
        fout.open("./storage/page"+ to_string(page) +".txt",ios::app);
        for(map<long long, string>::iterator iter = TLB.find(start_key.at(page));iter != TLB.end();){
            if(classify(iter->first) == page){
                fout << iter->first << " " << iter->second << "\n";
                // update ROM
                if(in_rom){
                    ROM[iter->first] = iter->second;
                }
                TLB.erase(iter++);
            }else if(classify(iter->first) > page){
                break;
            }
        }
        start_key.at(page) = -1;
        fout.close();
    } 
}

// check memory
bool check_rom_size(map<long long,string> &ROM,map<int,long long>&page_list){
    if(page_list.size() < ROM_NUM){
        return false;
    }else if(page_list.size() == ROM_NUM){
        int current_entry = ROM.size();
        int average_entry = current_entry / page_list.size(); 
        int total_entry = current_entry + average_entry;
        if(total_entry < MAX_ENTRY && ROM_NUM < PAGE_NUM){
            ROM_NUM ++;
            return false;
        }else{
            return true;
        }
    }else{
        return true;
    }
}

// release ROM
void release_rom(map<long long,string> &ROM,map<int,long long>&page_list){
    int num = rand() % ROM_NUM;
    map<int,long long>::iterator list_iter = page_list.begin();
    for(int i=0;i<num;i++){
        list_iter++;
    }
    // erase page in ROM
    int page = list_iter->first;
    long long key = list_iter->second;
    for(map<long long, string>::iterator it = ROM.find(key);it != ROM.end();){
        if(classify(it->first) == page){
            ROM.erase(it++);
        }else if(classify(it->first) > page){
            break;
        }
    }
    // erase page in page_list
    page_list.erase(list_iter); 
}

// erase ROM
void erase_rom(map<long long,string> &ROM,map<int,long long>&page_list,int specific_page){
    map<int,long long>::iterator list_iter = page_list.find(specific_page);
    // erase page in ROM
    int page = list_iter->first;
    long long key = list_iter->second;
    for(map<long long, string>::iterator it = ROM.find(key);it != ROM.end();){
        if(classify(it->first) == page){
            ROM.erase(it++);
        }else if(classify(it->first) > page){
            break;
        }
    }
    // erase page in page_list
    page_list.erase(list_iter); 
    // adjust ROM NUMBER
    if(ROM_NUM > 0){
        ROM_NUM--;
    }
}


// put
void put_tlb(long long key,string value, vector<long long> &start_key,map<long long,string> &TLB,map<long long,string> &ROM,map<int,long long>&page_list){
    // put value
    TLB[key] = value;
    // update start_key
    int page = classify(key);
    if(key < start_key.at(page) || start_key.at(page) == -1){
        start_key.at(page) = key;
    }
    // if TLB is full
    if(TLB.size() >= TLB_MAX){
        update(page,start_key,TLB,ROM,page_list);
    }
}

// get
void get_tlb(long long key, string &output_buffer, vector<long long> &start_key, map<long long,string> &TLB, string filename,map<long long,string> &ROM,map<int,long long>&page_list){
    string finalValue = "EMPTY";
    int page = classify(key);
    // check TLB and check ROM
    map<long long, string>::iterator iter1 = TLB.find(key);
    map<long long, string>::iterator iter2 = ROM.find(key);
    if(iter1 != TLB.end()){
        finalValue = iter1->second;
    }else if(is_page_in_pagelist(page,page_list)){ 
        if(iter2 != ROM.end()){
            finalValue = iter2->second;
        }else{
            finalValue = "EMPTY";
        }
    }else{
        // the key is not in TLB or ROM
        bool full_flag = false;
        long long tempKey;
        string tempValue;
        ifstream fin;
        fin.open("./storage/page" + to_string(page) + ".txt");
        // the page is not empty
        if(!fin.eof()){
            // check whether ROM is full
            if(check_rom_size(ROM,page_list)){
                release_rom(ROM,page_list);
            }
            // assign page list
            page_list[page] = -1;
            // read page
            while(fin >> tempKey){
                fin >> tempValue;
                if(tempKey == key){
                    finalValue = tempValue;
                }
                // move to ROM
                if(ROM.size() < MAX_ENTRY){
                    ROM[tempKey] = tempValue;
                    if(tempKey < page_list[page] || page_list[page] == -1){
                        page_list[page] = tempKey;
                    }
                }else{
                    // when memory is exhausted
                    full_flag = true;
                }   
            }
            if(full_flag){
                erase_rom(ROM,page_list,page);
            }
        }
        fin.close();

    }
    output_buffer += finalValue + "\n";
    // if buffer is full, then write file 
    if(output_buffer.size() >= BUFFER_MAX){
        output_file(output_buffer,filename);
    }
    
}

// scan
void scan_tlb(long long key1, long long key2, string &output_buffer, vector<long long> &start_key,map<long long,string> &TLB, string filename,map<long long,string> &ROM,map<int,long long>&page_list){
    for(long long i=key1;i<key2+1;i++){
        get_tlb(i,output_buffer,start_key,TLB,filename,ROM,page_list);
    }
}

void flush_tlb(map <long long,string> &TLB){
    fstream fout;
    for(int i=0;i<PAGE_NUM;i++){
        fout.open("./storage/page"+ to_string(i) +".txt",ios::app);
        for(map<long long, string>::iterator iter=TLB.begin();iter!=TLB.end();){
            if(classify(iter->first) == i){
                fout << iter->first << " " << iter->second << "\n";
                TLB.erase(iter++);
            }else{
                break;
            }
        }
        fout.close();
        if(TLB.empty()){
            break;
        }
    }
}

int main(int argc,char** argv){
    string readString;
    long long readKey;
    long long readKey2;
    string readValue;
    string filename = "";
    
    system("mkdir ./storage");
    srand(time(NULL));
    // get file name
    filename += argv[1];
    filename = filename.substr(0,filename.length()-6);
    filename = get_filename(filename);
    // memory , max size == 1000000
    map <long long,string> TLB;
    string output_buffer = "";
    map <long long,string> ROM;
    // record ROM page (in time order)
    map <int,long long> page_list; 
    // record the first key of each page in TLB   
    vector<long long> start_key;
    for(int i=0;i<PAGE_NUM;i++){
        start_key.push_back(-1);
    }
    // read input
    ifstream fin;
    fin.open(argv[1]);
    while(fin >> readString){
        fin >> readKey;
        if(readString == "PUT"){
            fin >> readValue;
            put_tlb(readKey ,readValue ,start_key,TLB,ROM,page_list);
        }else if(readString == "GET"){
            get_tlb(readKey,output_buffer,start_key,TLB,filename,ROM,page_list);
        }else if(readString == "SCAN"){
            fin >> readKey2;
            scan_tlb(readKey,readKey2,output_buffer,start_key,TLB,filename,ROM,page_list);
        }
    }
    fin.close();
    if(TLB.empty() == false){
        flush_tlb(TLB);    
    }
    if(output_buffer.empty() == false){
        output_file(output_buffer,filename);
    }

    return 0;
}

