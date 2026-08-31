#include<iostream>
#include<cstdlib>
#include"account.h"

int mainMenu()
{
    int choice;
    system("cls");
    cout << "MAIN MENU" << endl
    << "[1] Register" << endl
    << "[2] Login" << endl
    << "[0] Exit" << endl
    << "Select [0-2] only: "; cin >> choice;
    getchar();
    return choice;
}

int transactionMenu()
{
    int choice;
    system("cls");
    cout << "TRANSACTION MENU" << endl
    << "[1] Balance" << endl
    << "[2] Withdraw" << endl
    << "[3] Deposit" << endl
    << "[4] Transfer" << endl
    << "[5] Change Pin" << endl
    << "[0] Logout" << endl
    << "Select [0-5] only: "; cin >> choice;
    getchar();
    return choice;
}

int main()
{
    srand(time(NULL));
    ATM atm;
    atm.load();
    Account newAcc;
    double amt;
    string recipAN, recipNM;
    while(true)
    {
        switch (mainMenu())
        {
        case 1:
            atm.registration(newAcc);
            break;
        case 2:
            atm.login(newAcc.accNum, newAcc.pin);
            while(atm.isLoggedIn())
            {
                switch(transactionMenu())
                {
                    case 1:
                        cout << "Your balance is: " << atm.balance() << endl;
                        system("pause");
                        break;
                    case 2:
                        cout << "Insert amount to withdraw: "; cin >> amt;
                        getchar();
                        atm.withdraw(amt);
                        break;
                    case 3:
                        cout << "Insert amount to deposit: "; cin >> amt;
                        getchar();
                        atm.deposit(amt);
                        break;
                    case 4:
                        cout << "Insert the Account Number of the recipient: "; 
                        getline(cin, recipAN);
                        cout << "Insert the Name of the recipient: "; 
                        getline(cin, recipNM);
                        cout << "Insert the amount to transfer: "; 
                        cin >> amt;
                        getchar();
                        atm.transfer(amt, recipAN, recipNM);
                        break;
                    case 5:
                        atm.pin();
                        break;
                    case 0:
                        atm.logout();
                        break;
                    default:
                        cout << "Select [0-5] only" << endl;
                        system("pause");
                        continue;
                }
            }
            break;
        case 0:
            cout << "Exiting..." << endl;
            atm.save();
            return 0;
            
        default:
            cout << "Select [0-2] only" << endl;
            system("pause");
            continue;
        }
    }

    return 0;
}