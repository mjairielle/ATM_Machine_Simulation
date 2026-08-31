#include<iostream>
#include<cstdlib>
#include<string>
#include<ctime>
#include<fstream>
#include<sstream>
#include<Windows.h>

using namespace std;

const string fd_fn = "account.csv";
const string card_fn = "card.csv";

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

        bool searchAccNum(string AN);
        bool isAllDigits(string P);
        bool stringToBool(const string &S);
        string encryptCaesar(string P, int shift = 3);
        string decryptCaesar(string P, int shift = 3);
        string detectDrive();
        void insertNode(Account X);
        Node *findNode(string AN);
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
        void registration(Account X);
        void login(string AN, string P);
        double balance();
        void withdraw(double N);
        void deposit(double N);
        void transfer(double N, string AN, string NM);
        void pin();
        void save();
        void load();
        bool read(string &AN, string &P);
        bool write(string AN, string P);
        void logout();
        bool isLoggedIn();
};

bool ATM::searchAccNum(string AN)
{
    Node *curr;
    curr = head;
    while (curr != NULL)
    {
        if(curr->data.accNum == AN)
        {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

Node* ATM::findNode(string AN)
{
    Node *curr = head;
    while (curr != NULL)
    {
        if(curr->data.accNum == AN)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

bool ATM::isAllDigits(string P)
{
    for(int i = 0; i < P.length(); i++)
    {
        if(P[i] < '0' || P[i] > '9')
        {
            return false;
        }
    }
    return true;
}

string ATM::encryptCaesar(string P, int shift)
{
    for(int i = 0; i < P.length(); i++)
    {
        if(P[i] < '0' || P[i] > '9')
        {
            continue;
        }
        P[i] = ((P[i] - '0' + shift) % 10) + '0';
    }

    return P;
}

string ATM::decryptCaesar(string P, int shift)
{
    for(int i = 0; i < P.length(); i++)
    {
        if(P[i] < '0' || P[i] > '9')
        {
            continue;
        }
        P[i] = ((P[i] - '0' - shift + 10) % 10) + '0';
    }

    return P;
}

string ATM::detectDrive()
{
    while (true)
    {
        DWORD drives = GetLogicalDrives();

        for (int i = 0; i < 26; i++)
        {
            if (drives & (1 << i))
            {
                string driveLetter = string(1, 'A' + i) + ":\\";

                if (GetDriveTypeA(driveLetter.c_str()) == DRIVE_REMOVABLE)
                {
                    return driveLetter;
                }
            }
        }

        Sleep(500);
    }
}

bool ATM::stringToBool(const string &S)
{
    return (S == "true" || S == "1");
}

void ATM::insertNode(Account X)
{
    Node *prev, *curr, *newNode;
    prev = curr = head;
    newNode = new Node(X);

    while (curr != NULL && newNode->data.name > curr->data.name)
    {
        prev = curr;
        curr = curr->next;
    }

    if(curr == head)
    {
        head = newNode;
    } else
    {
        prev->next = newNode;
    }
    newNode->next = curr;
}

void ATM::registration(Account X)
{
    int rNum = 10000 + (rand() % 90000);
    X.accNum = to_string(rNum);
    while(searchAccNum(X.accNum))
    {
        rNum = 10000 + (rand() % 90000);
        X.accNum = to_string(rNum);
    }
    cout << "Account Number: " << X.accNum << endl;

    while (X.balance < 5000)
    {
        cout << "Minimum deposit is 5000: "; cin >> X.balance;
        getchar();
    }
    cout << "Balance: " << X.balance << endl;

    cout << "Insert Name: "; getline(cin, X.name);
    cout << "Insert Birthday(MM/DD/YYYY): "; getline(cin, X.birthday);
    cout << "Insert Contact Number: "; getline(cin, X.contact);
    cout << "Create New Pin: "; getline(cin, X.pin);
    while (X.pin.length() != 6 || !isAllDigits(X.pin))
    {
        cout << "Invalid pin." << endl;
        cout << "Create New Pin: "; getline(cin, X.pin);
    }
    X.pin = encryptCaesar(X.pin);

    insertNode(X);

    if(write(X.accNum, X.pin))
    {
        save();
        cout << "Successfully registered!" << endl;
    } else {
        cout << "Registration error" << endl;
    }

    system("pause");
}

bool ATM::write(string AN, string P)
{
    string flash_drive = detectDrive();
    string path = flash_drive + card_fn;
    string line = AN + "," + P;
    string e_line = encryptCaesar(line);

    ofstream file(path);
    if(!file)
    {
        cout << "File error" << endl;
        system("pause");
        return false;
    }

    file << e_line << endl;
    file.close();
    return true;
}

bool ATM::read(string &AN, string &P)
{
    bool alert = false;
    while (true)
    {
        string flash_drive = detectDrive();
        string path = flash_drive + card_fn;
    
        ifstream file(path);
        if(!file)
        {
            if(alert == false)
            {
                cout << "Please Insert Card." << endl;
                alert = true;
            }
            Sleep(500);
            continue;
        }
        
        string line;
        getline(file, line);
        string d_acc = decryptCaesar(line);
        stringstream ss(d_acc);
        getline(ss, AN, ',');
        getline(ss, P, ',');

        file.close();
        return true;
    }
}

void ATM::save()
{
    ofstream file(fd_fn);
    if(!file)
    {
        cout << "File error" << endl;
        Sleep(500);
        return;
    }

    Node *curr = head;
    while (curr != NULL)
    {
        file << curr->data.accNum << ","
        << curr->data.name << ","
        << curr->data.birthday << ","
        << curr->data.contact << ","
        << curr->data.balance << ","
        << curr->data.pin << ","
        << curr->data.locked << endl;
        curr = curr->next;
    }
    file.close();
}

void ATM::load()
{
    ifstream file(fd_fn);
    if(!file)
    {
        cout << "File error" << endl;
        Sleep(500);
        return;
    }

    Account N;
    string line;
    string bal, s_lck;
    while (getline(file, line))
    {
        if(line.empty())
        {
            continue;
        }
        stringstream ss(line);

        getline(ss, N.accNum, ',');
        getline(ss, N.name, ',');
        getline(ss, N.birthday, ',');
        getline(ss, N.contact, ',');
        getline(ss, bal, ',');
        getline(ss, N.pin, ',');
        getline(ss, s_lck);
        N.balance = stod(bal);
        N.locked = stringToBool(s_lck);

        insertNode(N);
    }
    file.close();
}

void ATM::login(string AN, string P)
{
    read(AN, P);

    int i = 0;
    string typedPin;

    Node* curr = findNode(AN);
    if(curr == NULL)
    {
        cout << "Account doesn't exist" << endl;
        return;
    }

    if(curr->data.locked == true)
    {
        cout << "Account is locked, please see admin" << endl;
        return;
    }

    if(P != curr->data.pin)
    {
        cout << "Card error" << endl;
        return;
    }

    while (i < 3)
    {
        cout << "Enter Pin: "; getline(cin, typedPin);
        while (typedPin.length() != 6 || !isAllDigits(typedPin))
        {
            cout << "Invalid Pin" << endl;
            cout << "Enter Pin: "; getline(cin, typedPin);
        }

        typedPin = encryptCaesar(typedPin);
        if(typedPin == curr->data.pin)
        {
            currentAcc = &curr->data;
            cout << "Login successful" << endl;
            return;
        } else
        {
            i++;
            cout << "Incorrect Pin" << endl;
            system("pause");
            continue;
        }
    }
    curr->data.locked = true;
    save();
    cout << "Account is locked, please see admin" << endl;
    return;
}

double ATM::balance()
{
    return currentAcc->balance;
}

void ATM::withdraw(double N)
{
    while(N > currentAcc->balance || N < 0)
    {
        cout << "Invalid amount, your balance is: " << currentAcc->balance << endl;
        cout << "Withdraw valid amount: "; cin >> N;
    }

    currentAcc->balance -= N;
    save();
    return;
}

void ATM::deposit(double N)
{
    while (N < 0)
    {
        cout << "Invalid amount, your balance is: " << currentAcc->balance << endl;
        cout << "Deposit valid amount: "; cin >> N;
    }
    
    currentAcc->balance += N;
    save();
    return;
}

void ATM::transfer(double N, string AN, string NM)
{
    if(currentAcc->accNum == AN)
    {
        cout << "Transfer to own account is invalid" << endl;
        return;
    }

    Node *recip = findNode(AN);
    if(recip == NULL)
    {
        cout << "Recipient doesn't exist" << endl;
        return;
    }

    if(recip->data.name != NM)
    {
        cout << "Name doesn't match" << endl;
        return;
    }
    
    if(recip->data.locked == true)
    {
        cout << "Recipient's account is locked" << endl;
        return;
    }

    while(N > currentAcc->balance || N < 0)
    {
        cout << "Invalid amount, your balance is: " << currentAcc->balance << endl;
        cout << "Withdraw valid amount: "; cin >> N;
    }

    currentAcc->balance -= N;
    recip->data.balance += N;
    save();
    return;
}

void ATM::pin()
{
    int i = 0;
    string typedPin;
    string newPin;
    string confirmation;
    while(i < 3)
    {
        cout << "Enter pin: " << endl;
        getline(cin, typedPin);
        while(typedPin.length() != 6 || !isAllDigits(typedPin))
        {
            cout << "Invalid pin" << endl;
            cout << "Enter pin: "; getline(cin, typedPin);
        }

        typedPin = encryptCaesar(typedPin);
        if(typedPin == currentAcc->pin)
        {
            cout << "Enter new pin: "; getline(cin, newPin);
            cout << "Enter again the pin(confirmation): "; getline(cin, confirmation);
            while(newPin != confirmation || newPin.length() != 6 || !isAllDigits(newPin)
                || confirmation.length() != 6 || !isAllDigits(confirmation))
            {
                cout << "Enter new pin: "; getline(cin, newPin);
                cout << "Enter again the pin(confirmation): "; getline(cin, confirmation);
            }
            newPin = encryptCaesar(newPin);
            currentAcc->pin = newPin;
            save();
            write(currentAcc->accNum, currentAcc->pin);
            cout << "Pin changed successfully" << endl;
            return;
        }else
        {
            i++;
            cout << "Incorrect pin" << endl;
            system("pause");
            continue;
        }
    }
    currentAcc->locked = true;
    save();
    return;
}

bool ATM::isLoggedIn()
{
    return currentAcc != NULL;
}

void ATM::logout()
{
    currentAcc = NULL;
}

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