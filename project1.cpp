#include<iostream>
#include<cstdlib>
#include<string>
#include<ctime>

using namespace std;

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
        string encryptCaesar(string P, int shift = 3);
        string decryptCaesar(string P, int shift = 3);
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
        void pin(string P);
        void save();
        void load();
        bool read(string &AN, string &P);
        bool write(string AN, string P);
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
        P[i] = ((P[i] - '0' + shift) % 10) + '0';
    }

    return P;
}

string ATM::decryptCaesar(string P, int shift)
{
    for(int i = 0; i < P.length(); i++)
    {
        P[i] = ((P[i] - '0' - shift + 10) % 10) + '0';
    }

    return P;
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

    //save/write
    
    cout << "Successfully registered!" << endl;
    system("pause");
}