#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <ctime>

using namespace std;
using filesystem::path;
using filesystem::directory_iterator;

struct Account
{
    string accNum;
    string name;
    string birthday;
    string contact;
    double balance;
    string pin;
    bool locked;

    Account()
    {
        accNum = "";
        name = "";
        birthday = "";
        contact = "";
        balance = 0;
        pin = "";
        locked = false;
    }

    Account(string an, string n, string bday, string cn, double m, string p, bool lck)
    {
        accNum = an;
        name = n;
        birthday = bday;
        contact = cn;
        balance = m;
        pin = p;
        locked = lck;
    }
};

struct Node
{
    Account data;
    Node *next;

    Node(Account X)
    {
        data = X;
    }
};

class ATM
{
    private:
        Node *head;
        Account *currentAcc;

        bool stringToBool(string T);
        bool searchAccNum(string AN);
        bool isAllDigits(string P);
        int integerValidation(string prompt);
        double doubleValidation(string prompt);
        string encryptCaesar(string P, int shift = 3);
        string decryptCaesar(string P, int shift = 3);
        bool read(string &AN, string &P);
        bool write(string AN, string P);
    public:
        ATM()
        {
            head = NULL;
            currentAcc = NULL;
        }
        ~ATM()
        {
            Node *curr;
            while (head != NULL)
            {
                curr = head;
                head = head->next;
                delete curr;
            }
        }
        void addToList(Account x);
        void registration(Account X);
        void login(string AN, string P);
        double balance();
        void withdraw(double N);
        void deposit(double N);
        void transfer(double N, string AN, string NM);
        void pin(string P);
        path cardReader();
        void save();
        void load();
};

void ATM :: addToList(Account x){

}

void ATM :: save(){
    ofstream file("cs2a.csv");
    if (!file){
        cout<<"File Error";
        return;
    }else{
        Node *curr;
        curr=head;
        while(curr != NULL){
            file<<curr->data.accNum<<","
                <<curr->data.name<<","
                <<curr->data.birthday<<","
                <<curr->data.balance<<","
                <<curr->data.pin<<","
                <<curr->data.locked
                <<endl;
            curr = curr -> next;
        }
    }
    file.close();
}

void ATM :: load(){
    ifstream file("cs2a.csv");
    if(!file){
        cout<<"File Error";
        return;
    }else{
        string line;
        string balance, locked;
        Account x;
        while(getline(file, line)){
            if(line.empty()) continue;
            stringstream ss(line);
            getline(ss, x.accNum, ',');
            getline(ss, x.name, ',');
            getline(ss, x.birthday, ',');
            getline(ss, balance, ',');
            getline(ss, x.pin, ',');
            getline(ss, locked);
            x.balance = stoi(balance);
            x.locked = stringToBool(locked);
            addToList(x);
        }
    }
    file.close();
}


path ATM :: cardReader(){
    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            string driveLetter = string(1, 'A' + i) + ":\\";
            UINT type = GetDriveTypeA(driveLetter.c_str());

            if (type == DRIVE_REMOVABLE) {
                path candidate = driveLetter + "binary.txt";
                if(exists(candidate)) {
                    return candidate;
                }
            }
        }
    }
    path p;
    return p;
}

int ATM :: integerValidation(string prompt){
    int number;
    cout << prompt;
    while (!(cin >> number)) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(10000, '\n'); 
    }
    return number;
}

double ATM :: doubleValidation(string prompt){
    double number;
    cout << prompt;
    while (!(cin >> number)) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(10000, '\n'); 
    }
    return number;
}