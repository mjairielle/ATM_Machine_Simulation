#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<string>
#include<ctime>
#include<fstream>
#include<sstream>
#include<Windows.h>

using namespace std;

const string fd_fn = "account.csv";
const string card_fn = "card.csv";

const string ADMIN_USER = "admin";
const string ADMIN_PASS = "admin123";

const int CIPHER_A = 7;
const int CIPHER_B = 4;

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
        next = NULL;
    }
};

class ATM
{
    private:
        Node *head;
        Node *currentAcc;
        string cardDrive;

        bool searchAccNum(string AN);
        bool stringToBool(const string &S);
        bool isValidDate(int month, int day, int year);
        bool isAllAlphabet(string N);
        string toSentenceCase(string N);
        string findRemovableDrive();
        string detectDrive(bool requiredCardPresent);
        string encryptAffine(string P);
        void insertNode(Account X);
        Node *findNode(string AN);
        bool authenticate(Node *acc);
        string promptFixedDigits(string label, int len);
        string promptAmount(string label, double minAmount);
        int promptTransactionAmount(string label);
    public:
        ATM()
        {
            head = NULL;
            currentAcc = NULL;
            cardDrive = "";
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
        bool isAllDigits(string P);
        bool isValidAmount(string B);
        bool isValidTransaction(string amount);
        void registration();
        void login();
        double balance();
        void withdraw();
        void deposit();
        void transfer();
        void pin();
        void save();
        void load();
        void read(string &AN, string &P, string &drive);
        bool write(string AN, string P, string drive);
        void logout();
        bool isLoggedIn();
        bool isRegistered();
        void displayAcc();
        void unlock(string AN);

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
    if (P.empty()) 
    {
        return false;
    }

    for(int i = 0; i < P.length(); i++)
    {
        if(P[i] < '0' || P[i] > '9')
        {
            return false;
        }
    }
    return true;
}

bool ATM::isAllAlphabet(string N)
{
    if (N.empty())
    {
        return false;
    }
    for(int i = 0; i < N.length(); i++)
    {
        if(N[i] != ' ' && (tolower(N[i]) < 'a' || tolower(N[i]) > 'z'))
        {
            return false;
        }
    }
    return true;
}

string ATM::toSentenceCase(string N)
{
    string temp;
    bool isCapNext = true;
    bool prevSpace = false;

    for (int i = 0; i < N.length(); i++)
    {
        if(N[i] == ' ')
        {
            if(!prevSpace)
            {
                temp += ' ';
            }
            prevSpace = true;
            isCapNext = true;
        }
        else
        {
            temp += (char)(isCapNext ? toupper(N[i]) : tolower(N[i]));
            isCapNext = false;
            prevSpace = false;
        }
    }
    return temp;
}

bool ATM::isValidAmount(string B)
{
    if (B.empty() || B.length() > 15)
    {
        return false;
    }

    bool seenDot = false;

    for (int i = 0; i < B.length(); i++)
    {
        if(B[i] >= '0' && B[i] <= '9')
        {
            continue;
        }
        else if(B[i] == '.' && !seenDot)
        {
            seenDot = true;
            continue;
        }
        else
        {
            return false;
        }
    }
    if(B == ".")
    {
        return false;
    }
    return true;
}


string ATM::encryptAffine(string P)
{
    for(int i = 0; i < P.length(); i++)
    {
        if(P[i] < '0' || P[i] > '9')
        {
            continue;
        }
        int d = P[i] - '0';
        P[i] = ((CIPHER_A * d + CIPHER_B) % 10) + '0';
    }

    return P;
}

string ATM::findRemovableDrive()
{
    while (true)
    {
        system("cls");
        cout << "Insert A Flash Drive!" << endl;
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

string ATM::detectDrive(bool requiredCardPresent)
{
    while (true)
    {
        string driveLetter = findRemovableDrive();
        string path = driveLetter + card_fn;

        ifstream file(path);
        bool hasCard = (bool)file;
        file.close();

        if(hasCard == requiredCardPresent)
        {
            return driveLetter;
        }
        Sleep(500);
    }
}

bool ATM::stringToBool(const string &S)
{
    return (S == "true" || S == "1");
}

bool ATM :: isValidDate(int month, int day, int year)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int maxYear = 1900 + ltm->tm_year;

    if (year < 1900 || year > maxYear) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
    {
        return false;
    }

    if (month == 2)
    {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        
        if (isLeapYear && day > 29) return false;
        if (!isLeapYear && day > 28) return false;
    }
    return true;
}

bool ATM :: isValidTransaction(string amount)
{
    if(!isAllDigits(amount) || amount.length() > 9)
        {
            cout << "Invalid amount, please enter digits only" << endl;
            return false;
        }
    else if(stoi(amount) < 100)
        {
            cout << "Invalid amount, minimum transaction of 100" << endl;
            return false;
        }
    else if(stoi(amount) % 100 != 0)
        {
            cout << "Invalid amount, must be divisible by a hundred" << endl;
            return false;
        }
    return true;
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

string ATM::promptFixedDigits(string label, int len)
{
    string val;
    while (true)
    {
        cout << label;
        getline(cin, val);
        if((int)val.length() == len && isAllDigits(val))
        {
            return val;
        }
        cout << "Invalid input" << endl;
    }
}

string ATM::promptAmount(string label, double minAmount)
{
    string val;
    while (true)
    {
        cout << label;
        getline(cin, val);
        if(isValidAmount(val) && stod(val) >= minAmount)
        {
            return val;
        }
        cout << "Invalid amount" << endl;
    }
}

int ATM::promptTransactionAmount(string label)
{
    string val;
    while (true)
    {
        cout << label;
        getline(cin, val);
        if(isValidTransaction(val))
        {
            return stoi(val);
        }
    }
}

bool ATM::authenticate(Node *acc)
{
    int i = 0;
    string typedPin;
    while (i < 3)
    {
        typedPin = promptFixedDigits("Enter pin: ", 6);
        typedPin = encryptAffine(typedPin);
        if(typedPin == acc->data.pin)
        {
            return true;
        }
        else
        {
            i++;
            cout << "Incorrect pin" << endl;
            system("pause");
            system("cls");
            continue;
        }
    }
    acc->data.locked = true;
    save();
    logout();
    cout << "Account is locked, please see admin" << endl;
    system("pause");
    system("cls");
    return false;
}

void ATM::registration()
{   
    if(isRegistered()){
        cout<<"Account Already Registered."<<endl;
        system("pause");
        return;
    }
    system("cls");
    Account X;
    string inputMonth, inputDay, inputYear;
    int month, day, year;
    int rNum = 10000 + (rand() % 90000);
    X.accNum = to_string(rNum);
    while(searchAccNum(X.accNum))
    {
        rNum = 10000 + (rand() % 90000);
        X.accNum = to_string(rNum);
    }
    cout << "=========Registration=========" << endl;
    cout << "Account Number: " << X.accNum << endl;

    X.balance = stod(promptAmount("Minimum deposit is 5000: ", 5000));
    cout << "Balance: " << X.balance << endl;

    string surname, firstname, middlename;
    do{
    cout << "Insert Surname: "; getline(cin, surname);
    cout << "Insert First Name: "; getline(cin, firstname);
    cout << "Insert Middle Name (leave blank if none): "; getline(cin, middlename);
    if(isAllAlphabet(surname) && isAllAlphabet(firstname) && (middlename.empty() || isAllAlphabet(middlename)))
    {
        break;
    }
    else
    {
        cout<<"Invalid Name Format"<<endl;
    }
    }while(true);
    X.name = firstname + " ";
    if(!middlename.empty())
    {
        X.name += middlename + " ";
    }
    X.name += surname;
    X.name = toSentenceCase(X.name);

    while(true){
    cout << "Insert Birthdate: "<<endl;
    cout << "Month[MM]: "; getline(cin, inputMonth);
    cout << "Day[DD]: "; getline(cin, inputDay);
    cout << "Year[YYYY]: "; getline(cin, inputYear);
    if (isAllDigits(inputMonth) && isAllDigits(inputDay) && isAllDigits(inputYear)){
        month = stoi(inputMonth);
        day = stoi(inputDay);
        year = stoi(inputYear);
        if (isValidDate(month, day, year)) break;
        else cout<<"Error: That date does not exist. Please try again."<<endl;
        }
    else{
        cout<<"Error: Please Enter Numbers Only."<<endl;
        }
    }
    X.birthday = (month < 10 ? "0" : "") + to_string(month) + '-' + (day < 10 ? "0" : "") + to_string(day) + '-' + to_string(year);

    X.contact = promptFixedDigits("Insert Contact Number: ", 11);
    string confirmedPin;
    do{
        X.pin = encryptAffine(promptFixedDigits("Create New Pin: ", 6));
        confirmedPin = encryptAffine(promptFixedDigits("Confirm Pin: ", 6));
    }while(confirmedPin != X.pin);
    string drive = detectDrive(false);
    system("cls");
    if(write(X.accNum, X.pin, drive))
    {
        insertNode(X);
        save();
        cout << "Successfully registered!" << endl;
    } else {
        cout << "Registration error" << endl;
    }

    system("pause");
}

bool ATM::write(string AN, string P, string drive)
{   
    string path = drive + card_fn;
    string line = AN + "," + P;

    ofstream file(path);
    if(!file)
    {
        cout << "File error" << endl;
        system("pause");
        return false;
    }

    file << line << endl;
    file.close();
    return true;
}

void ATM::read(string &AN, string &P, string &drive)
{
    bool alert = false;
    while (true)
    {
        drive = detectDrive(true);
        string path = drive + card_fn;
    
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
        stringstream ss(line);
        getline(ss, AN, ',');
        getline(ss, P, ',');

        file.close();
        return;
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

        if(!isValidAmount(bal))
        {
            continue;
        }

        N.balance = stod(bal);
        N.locked = stringToBool(s_lck);

        insertNode(N);
    }
    file.close();
}

void ATM::login()
{
    string AN, P, drive;
    read(AN, P, drive);
    Node* curr = findNode(AN);
    if(curr == NULL)
    {
        cout << "Account doesn't exist" << endl;
        system("pause");
        return;
    }

    if(curr->data.locked == true)
    {
        cout << "Account is locked, please see admin" << endl;
        system("pause");
        return;
    }

    if(P != curr->data.pin)
    {
        cout << "Card error" << endl;
        system("pause");
        return;
    }
    system("cls");

    if(!authenticate(curr))
    {
        return;
    }

    cardDrive = drive;
    currentAcc = curr;
    cout << "Login successful" << endl;
    system("pause");
    system("cls");
}

double ATM::balance()
{
    return currentAcc->data.balance;
}

void ATM::withdraw()
{
    int N = promptTransactionAmount("Insert amount to withdraw [Minimum 100]: ");

    if(N > currentAcc->data.balance)
    {
        cout << "Invalid amount, your balance is: " << currentAcc->data.balance << endl;
        system("pause");
        return;
    }

    currentAcc->data.balance -= N;
    save();
    cout << "Withdrawal successful. New balance: " << currentAcc->data.balance << endl;
    system("pause");
}

void ATM::deposit()
{
    int N = promptTransactionAmount("Insert amount to deposit [Minimum 100]: ");

    currentAcc->data.balance += N;
    save();
    cout << "Deposit successful. New balance: " << currentAcc->data.balance << endl;
    system("pause");
}

void ATM::transfer()
{
    string AN, NM;
    cout << "Insert the Account Number of the recipient: "; getline(cin, AN);
    cout << "Insert the Name of the recipient: "; getline(cin, NM);

    if(currentAcc->data.accNum == AN)
    {
        cout << "Transfer to own account is invalid" << endl;
        system("pause");
        return;
    }

    Node *recip = findNode(AN);
    if(recip == NULL)
    {
        cout << "Recipient doesn't exist" << endl;
        system("pause");
        return;
    }

    if(recip->data.name != toSentenceCase(NM))
    {
        cout << "Name doesn't match" << endl;
        system("pause");
        return;
    }
    
    if(recip->data.locked == true)
    {
        cout << "Recipient's account is locked" << endl;
        system("pause");
        return;
    }

    double N = stod(promptAmount("Insert the amount to transfer: ", 0.01));
    if(N > currentAcc->data.balance)
    {
        cout << "Invalid amount, your balance is: " << currentAcc->data.balance << endl;
        system("pause");
        return;
    }

    currentAcc->data.balance -= N;
    recip->data.balance += N;
    save();
    cout << "Transfer successful" << endl;
    system("pause");
}

void ATM::pin()
{
    if(!authenticate(currentAcc))
    {
        return;
    }

    string newPin, confirmation;
    do
    {
        newPin = promptFixedDigits("Enter new pin: ", 6);
        confirmation = promptFixedDigits("Enter again the pin(confirmation): ", 6);
        if(newPin != confirmation)
        {
            cout << "Pins do not match" << endl;
        }
    } while(newPin != confirmation);

    newPin = encryptAffine(newPin);
    if(write(currentAcc->data.accNum, newPin, cardDrive))
    {
        currentAcc->data.pin = newPin;
        save();
        cout << "Pin changed successfully" << endl;
        system("pause");
    }
    else
    {
        cout << "Pin change failed, card not updated" << endl;
        system("pause");
    }
}

bool ATM::isRegistered()
{
    string driveLetter = findRemovableDrive();
    ifstream file(driveLetter + card_fn);
    bool exists = (bool)file;
    file.close();
    return exists;
}

bool ATM::isLoggedIn()
{
    return currentAcc != NULL;
}

void ATM::logout()
{
    currentAcc = NULL;
}

void ATM::displayAcc(){
    if(head == NULL){
        cout<<"No accounts Available."<<endl;
        return;
    }
    Node *curr = head;
    cout<<left<<setw(20)<<"Account ID"
        <<left<<setw(30)<<"Account Name"
        <<left<<setw(10)<<"Status"
        <<endl;
    while(curr != NULL){
        cout<<left<<setw(20)<<curr -> data.accNum
            <<left<<setw(30)<<curr -> data.name
            <<left<<setw(10)<<((curr -> data.locked == true) ? "Locked":"Unlocked")
            <<endl;
        curr = curr -> next;
    }
}

void ATM::unlock(string AN)
{
    Node *curr = findNode(AN);
    if(curr == NULL)
    {
        cout << "Account doesn't exist" << endl;
        system("pause");
        return;
    }

    if(curr->data.locked == false)
    {
        cout << "Account is not locked" << endl;
        system("pause");
        return;
    }

    curr->data.locked = false;
    save();
    cout << "Account unlocked successfully" << endl;
    system("pause");
}

int mainMenu()
{
    int choice;
    system("cls");
    cout << "MAIN MENU" << endl
    << "[1] Register" << endl
    << "[2] Login" << endl
    << "[3] Admin" << endl
    << "[0] Exit" << endl
    << "Select [0-3] only: "; 
    cin >> choice;
    getchar();
    return choice;
}

int adminMenu()
{
    int choice;
    system("cls");
    cout << "ADMIN MENU" << endl
    << "[1] Unlock Account" << endl
    << "[0] Back" << endl
    << "Select [0-1] only: "; 
    cin >> choice;
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
    string unlockAN;

    while(true)
    {   
        switch (mainMenu())
        {
        case 1:
            atm.registration();
            break;
        case 2:
            atm.login();
            while(atm.isLoggedIn())
            {
                switch(transactionMenu())
                {
                    case 1:
                        cout << "Your balance is: " << fixed << setprecision(2) << atm.balance() << endl;
                        system("pause");
                        break;
                    case 2:
                        atm.withdraw();
                        break;
                    case 3:
                        atm.deposit();
                        break;
                    case 4:
                        atm.transfer();
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
        case 3:
        {
            string user, pass;
            cout << "Admin Username: ";
            getline(cin, user);
            cout << "Admin Password: ";
            getline(cin, pass);

            if(user == ADMIN_USER && pass == ADMIN_PASS)
            {
                bool inAdmin = true;
                while (inAdmin)
                {
                    switch (adminMenu())
                    {
                    case 1:
                        atm.displayAcc();
                        cout << "Insert Account Number to unlock: "; getline(cin, unlockAN);
                        atm.unlock(unlockAN);
                        break;
                    case 0:
                        inAdmin = false;
                        break;
                    default:
                        cout << "Select [0-1] only" << endl;
                        system("pause");
                        continue;
                    }
                }
            } else
            {
                cout << "Invalid admin credentials" << endl;
                system("pause");
            }
            break;
        }
        case 0:
            cout << "Exiting..." << endl;
            atm.save();
            return 0;
            
        default:
            cout << "Select [0-3] only" << endl;
            system("pause");
            continue;
        }
    }
    return 0;
}
