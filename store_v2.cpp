# include <iostream>
# include <fstream>
# include <cstdlib>
# include <cmath>
# include <vector>
# include <map>

using namespace std;

int TLB_MAX = 1000000;

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

void tlb_to_page(long long key, string value, map<long long,string> &TLB){
    map<long long, string>::iterator iter;
    fstream fout;
    int page = classify(key);
    fout.open("./storage/page"+ to_string(page) +".txt",ios::app);
    // move all keys that in same page to file
    fout << key << " " << value << "\n";
    for(iter = TLB.begin();iter != TLB.end();){
        if(classify(iter->first) == page){
            fout << iter->first << " " << iter->second << "\n";
            TLB.erase(iter++);
        }else{
            iter++;
        }
    }
    fout.close();
}

void put_tlb(long long key,string value, map<long long,string> &TLB){
    if(TLB.size() < 2){
        TLB[key] = value;
    }else{
        // TLB is full
        tlb_to_page(key,value,TLB);
    }
}

void get_tlb(long long key, string filename){

}

void get(long long key , string filename){
    ifstream fin;
    fstream fout;
    long long tempKey;
    string tempValue;
    string finalValue = "EMPTY";
    int page = classify(key);
    fin.open("./storage/page" + to_string(page) + ".txt");
    while(fin >> tempKey){
        fin >> tempValue;
        if(tempKey == key){
            finalValue = tempValue;
        }
    }
    fin.close();
    fout.open(filename + ".output",ios::app);
    fout << finalValue << "\n";
    fout.close();
}

void scan(long long key1 , long long key2 , string filename){
    ifstream fin;
    fstream fout;
    long long tempKey;
    string tempValue;
    string finalValue[key2 - key1 + 1];
    int page1 = classify(key1);
    int page2 = classify(key2);

    for(int i=0;i<key2-key1 + 1;i++){
        finalValue[i] = "EMPTY";
    }
    if(page1 == page2){
        fin.open("./storage/page" + to_string(page1) + ".txt");
        while(fin >> tempKey){
            fin >> tempValue;
            if(key1 <= tempKey  && tempKey <= key2){
                finalValue[tempKey - key1] = tempValue;
            }
        }
        fin.close();
    }else{
        fin.open("./storage/page" + to_string(page1) + ".txt");
        while(fin >> tempKey){
            fin >> tempValue;
            if(key1 <= tempKey  && tempKey <= key2){
                finalValue[tempKey - key1] = tempValue;
            }
        }
        fin.close();
        fin.open("./storage/page" + to_string(page2) + ".txt");
        while(fin >> tempKey){
            fin >> tempValue;
            if(key1 <= tempKey  && tempKey <= key2){
                finalValue[tempKey - key1] = tempValue;
            }
        }
        fin.close();
    }
    fout.open(filename + ".output",ios::app);
    for(int i=0;i<key2 - key1 + 1;i++){
        fout << finalValue[i] << "\n";
    }
    fout.close();
}

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

    // read input
    ifstream fin;
    fin.open(argv[1]);
    while(fin >> readString){
        fin >> readKey;
        if(readString == "PUT"){
            fin >> readValue;
            put_tlb(readKey , readValue,TLB);
        }
        // else if(readString == "GET"){
        //     get(readKey , filename);
        // }else if(readString == "SCAN"){
        //     fin >> readKey2;
        //     scan(readKey , readKey2 , filename);
        // }
        // for(map<long long, string>::iterator it=TLB.begin();it!=TLB.end();it++){
        //     cout << it->first << " "<< it->second << "\n";
        // }
        // cout<<"\n";

    }
    flush(TLB);
    fin.close();
    

}





