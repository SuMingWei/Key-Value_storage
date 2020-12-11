# include <iostream>
# include <fstream>
# include <cstdlib>
# include <random>
# include <ctime>
# include <cmath>

using namespace std;

char alpha[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

string randomString(){
    string tmp = "";
    for(int i=0;i<128;i++){
        tmp += alpha[rand() % (sizeof(alpha) - 1)];
    }
    return tmp;
}

long long randomLongLong(){
    long long temp = 0;
    for(int i=0;i<63;i++){
        if(rand()%2 == 0){
            temp += pow(2,i);
        }
    }
    return temp;
}

void generate_put(int size){
    ofstream fout;
    fout.open("only_put_"+ to_string(size) + ".input");
    for(int i=0;i<size;i++){
        if(i==size-1){
            fout << "PUT " << randomLongLong() << " " << randomString();
            break;    
        }
        fout << "PUT " << randomLongLong() << " " << randomString() << "\n";
    }
    fout.close();
}

void generate_get(int size){
    ofstream fout;
    fout.open("only_get_"+ to_string(size) + ".input");
    for(int i=0;i<size;i++){
        if(i==size-1){
            fout << "GET " << randomLongLong();
            break;    
        }
        fout << "GET " << randomLongLong() << "\n";
    }
    fout.close();
}

void generate_all(int size){
    int temp;
    long long head;
    ofstream fout;
    fout.open("test_"+ to_string(size) + ".input");
    for(int i=0;i<size;i++){
        temp = rand() % 5;
        if(i==size-1){
            if(temp == 0 || temp == 3){
                fout << "PUT " << randomLongLong() << " " << randomString();
            }else if(temp == 1 || temp == 4){
                fout << "GET " << randomLongLong();
            }else{
                head = randomLongLong();
                fout << "SCAN " << head << " " << head + (rand()%40) + 1;
            }
            break;    
        }
        if(temp == 0 || temp == 3){
            fout << "PUT " << randomLongLong() << " " << randomString() << "\n";
        }else if(temp == 1 || temp == 4){
            fout << "GET " << randomLongLong() << "\n";
        }else{
            head = randomLongLong();
            fout << "SCAN " << head << " " << head + (rand()%40) + 1 << "\n";
        }
    }
    fout.close();
}

void generate_scan(int size){
    ofstream fout;
    long long head;
    fout.open("scan.input");
    for(int i=0;i<size;i++){
        if(i==size-1){
            head = randomLongLong();
            fout << "SCAN " << head << " " << head + (rand()%2) + 1;
            break;    
        }
        head = randomLongLong();
        fout << "SCAN " << head << " " << head + (rand()%2) + 1 << "\n";
    }
    fout.close();
}

int main(int argc, char** argv){
    srand(time(NULL));
    
    //generate_put(27500000);
    //generate_get(175000000);
    //generate_all(55000000);
    generate_scan(100000000);
    return 0;

}