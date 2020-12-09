# include <iostream>
# include <fstream>
# include <cstdlib>
# include <cmath>
# include <vector>
# include <map>

using namespace std;

int TLB_MAX = 1000000;
int BUFFER_MAX = 32000000;

int classify(long long key){
    // spilt to 32 files
    return (int)(key / pow(2,58));
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

// check whether the key is in TLB
bool is_key_in_tlb(long long key, map<long long,string> &TLB){
    map<long long, string>::iterator iter = TLB.find(key);
    if(iter != TLB.end()){
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

// put
void tlb_to_page(long long key, string value, map<long long,string> &TLB){
    fstream fout;
    int page = classify(key);
    fout.open("./storage/page"+ to_string(page) +".txt",ios::app);
    // move all keys that in same page to file
    fout << key << " " << value << "\n";
    for(map<long long, string>::iterator iter = TLB.begin();iter != TLB.end();){
        if(classify(iter->first) < page){
            iter++;
        }else if(classify(iter->first) == page){
            fout << iter->first << " " << iter->second << "\n";
            TLB.erase(iter++);
        }else if(classify(iter->first) > page){
            break;
        }
    }
    fout.close();
}

void put_tlb(long long key,string value, map<long long,string> &TLB){
    if(TLB.size() < TLB_MAX){
        TLB[key] = value;
    }else{
        // TLB is full
        tlb_to_page(key,value,TLB);
    }
}

// get
void get_tlb(long long key, string &output_buffer, map<long long,string> &TLB, string filename){
    ifstream fin;
    long long tempKey;
    string tempValue;
    string finalValue = "EMPTY";
    int page = classify(key);
    // check TLB
    map<long long, string>::iterator iter = TLB.find(key);
    if(iter != TLB.end()){
        finalValue = iter->second;
    }else{
        // the key is not in TLB
        fin.open("./storage/page" + to_string(page) + ".txt");
        while(fin >> tempKey){
            fin >> tempValue;
            if(tempKey == key){
                finalValue = tempValue;
            }
        }
        fin.close();
    }
    output_buffer += finalValue + "\n";
    cout << output_buffer.size() << endl;
    // if buffer is full, then write file 
    if(output_buffer.size() >= BUFFER_MAX){
        output_file(output_buffer,filename);
    }
}

// scan
void flush(map <long long,string> &TLB){
    fstream fout;
    for(int i=0;i<32;i++){
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

void page_to_tlb(int page, map<long long,string> &TLB){
    ifstream fin;
    long long tempKey;
    string tempValue;
    // move page to tlb
    fin.open("./storage/page" + to_string(page) + ".txt");
    while(fin >> tempKey){
        fin >> tempValue;

        if(TLB.size()<TLB_MAX || is_key_in_tlb(tempKey,TLB)){
            TLB[tempKey] = tempValue;
        }
    }
    fin.close();
}

void scan_tlb(long long key1, long long key2, string &output_buffer, map<long long,string> &TLB, string filename){
    string finalValue;
    map<long long, string>::iterator iter;
    // flush TLB
    int page = classify(key1);
    int cur_page; 
    flush(TLB);
    page_to_tlb(page,TLB);
    // find value
    for(long long i=key1;i<key2+1;i++){
        // update TLB
        cur_page = classify(i);
        if(cur_page != page){
            page = cur_page;
            flush(TLB);
            page_to_tlb(page,TLB);
        }

        iter = TLB.find(i);
        if(iter != TLB.end()){
            finalValue = iter->second;
            output_buffer += finalValue + "\n";
            // if buffer is full, then write file 
            if(output_buffer.size() >= BUFFER_MAX){
                output_file(output_buffer,filename);
            }
        }else{
            // the key is not in TLB
            get_tlb(i,output_buffer,TLB,filename);
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
    // get file name
    filename += argv[1];
    filename = filename.substr(0,filename.length()-6);
    filename = get_filename(filename);
    // memory , max size == 1000000
    map <long long,string> TLB;
    string output_buffer = "";
    // read input
    ifstream fin;
    fin.open(argv[1]);
    while(fin >> readString){
        fin >> readKey;
        if(readString == "PUT"){
            fin >> readValue;
            put_tlb(readKey , readValue,TLB);
        }else if(readString == "GET"){
            get_tlb(readKey,output_buffer,TLB,filename);
        }else if(readString == "SCAN"){
            fin >> readKey2;
            scan_tlb(readKey,readKey2,output_buffer,TLB,filename);
        }
    }
    flush(TLB);
    if(output_buffer.empty() == false){
        output_file(output_buffer,filename);
    }
    fin.close();
    
}





