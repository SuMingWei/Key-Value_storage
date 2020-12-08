# include <iostream>
# include <fstream>
# include <cstdlib>
# include <cmath>
# include <vector>

using namespace std;

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

void put(long long key,string value){
    fstream fout;
    int page = classify(key);
    fout.open("./storage/page"+ to_string(page) +".txt",ios::app);
    fout << key << " " << value << "\n";
    fout.close();
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
    for(long long i=key1;i<key2+1;i++){
        get(i,filename);
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
    // read input
    ifstream fin;
    fin.open(argv[1]);
    while(fin >> readString){
        fin >> readKey;
        if(readString == "PUT"){
            fin >> readValue;
            put(readKey , readValue);
        }else if(readString == "GET"){
            get(readKey , filename);
        }else if(readString == "SCAN"){
            fin >> readKey2;
            scan(readKey , readKey2 , filename);
        }
    }

}





