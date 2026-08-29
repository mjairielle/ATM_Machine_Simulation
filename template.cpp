#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem>
#define MAX 50
using namespace std;
using filesystem::path;
using filesystem::directory_iterator;

struct Account{
    string accountNumber;
    string accountName;
    string birthDate;
    string contactNumber;
    string pinCode;
    double money;

    Account(){
        this -> accountNumber = "";
        this -> accountName = "";
        this -> birthDate = "";
        this -> contactNumber = "";
        this -> pinCode = "";
        this -> money = 0; 
    }

    Account(string accountNumber, string accountName, string birthDate, string contactNumber, string pinCode, double money){
        this -> accountNumber = accountNumber;
        this -> accountName = accountName;
        this -> birthDate = birthDate;
        this -> contactNumber = contactNumber;
        this -> pinCode = pinCode;
        this -> money = money; 
    }
};

class ATMMachine{
private:
    Account accounts[MAX];
    int last;
    string nameScanner(string name);
    int integerValidation(string prompt);
public:
    ATMMachine(){
        last = -1;
    }
    double doubleValidation(string prompt);
    path cardReader();
    void login();
    void deposit(double amount);
    void withdraw(int amount);
    void balanceInquiry();
    void fundTransfer(double amount);
    void changePin();
};

int ATMMachine :: integerValidation(string prompt){
    int number;
    cout << prompt;
    while (!(cin >> number)) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(10000, '\n'); 
    }
    return number;
}

double ATMMachine :: doubleValidation(string prompt){
    double number;
    cout << prompt;
    while (!(cin >> number)) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(10000, '\n'); 
    }
    return number;
}

path cardReader(){
    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            string driveLetter = string(1, 'A' + i) + ":\\";
            UINT type = GetDriveTypeA(driveLetter.c_str());

            if (type == DRIVE_REMOVABLE) {
                path candidate = driveLetter + "ATM_DATABASE\\account.txt";
                if(exists(candidate)) {
                    return candidate;
                }
            }
        }
    }
    path p;
    return p;
}

int main(){
    ATMMachine atm;
    cout<<500 + atm.doubleValidation("Number: ");
}