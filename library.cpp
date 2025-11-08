#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
using namespace std;
string trim(const string &s)
{
    string result = s;
    while (!result.empty() && isspace(result.front()))
        result.erase(result.begin());
    while (!result.empty() && (isspace(result.back()) || result.back() == '\r'))
        result.pop_back();
    return result;
}

vector<string> split(string line, char sep) 
{
    vector<string> tokens;
    string temp = "";
    for (int i = 0; i < line.length(); i++) 
    {
        if (line[i] == sep) 
        {
            if (!temp.empty()) tokens.push_back(trim(temp));
            temp = "";
        } 
        else 
            temp += line[i];
    }
    if (!temp.empty()) tokens.push_back(trim(temp));
    return tokens;
}
bool isLeap(int y) 
{
    if (y % 400 == 0) 
        return true;
    if (y % 100 == 0) 
        return false;
    return (y % 4 == 0);
}

int daysInMonth(int m, int y) 
{
    if (m == 4 || m == 6 || m == 9 || m == 11)
        return 30;
    if (m == 2) 
        return isLeap(y) ? 29 : 28;
    return 31;
}


class Book 
{
protected:
    string title;
    string author;
    int quantity;
public:
    Book() 
    {

    }
    Book(string t, string a, int q) 
    {
        title = t;
        author = a;
        quantity = q;
    }
    string getTitle() 
    { 
        return title; 

    }
    string getAuthor() 
    { 
        return author; 
    }
    int getQuantity() 
    { 
        return quantity; 
    }
    void setQuantity(int q) 
    { 
        quantity = q; 
    }
    void displayBook() 
    {
        cout << left << setw(50) << title
             << setw(50) << author
             << "Qty: " << quantity << endl;
    }
};

class Borrower : public Book 
{
    string borrowerName;
    string dueDate;
    int finePerDay = 5;
public:
    Borrower() 
    {

    }
    Borrower(string bName, string t, string a, int q, string d) : Book(t, a, q) 
    {
        borrowerName = bName;
        dueDate = d;
    }
    void displayBorrower() 
    {
        cout << left << setw(50) << borrowerName
             << setw(50) << title
             << setw(15) << dueDate << endl;
    }
    int calculateDues(string currentDate);
};

int dateToDays(string date) 
{
    vector<string> parts = split(date, ' ');
    if (parts.size() != 3)
        return -1;
    int d = stoi(parts[0]);
    int m = stoi(parts[1]);
    int y = stoi(parts[2]);
    if (m < 1 || m > 12 || d < 1 || (d > daysInMonth(m, y)))
        return -1;
    int days = d;
    for (int i = 1; i < m; i++)
        days += daysInMonth(i, y);
    days += y * 365 + y / 4 - y / 100 + y / 400;
    return days;
}
int Borrower::calculateDues(string currentDate) 
{
    int due = dateToDays(dueDate);
    int curr = dateToDays(currentDate);
    if (due == -1 || curr == -1) 
        return -1;
    int diff = curr - due;
    if (diff>2*365 || diff<-365)
    {
        cout<<"ERROR WITH THE CURRENT DATE\n";
        return -1;
    }
    return (diff > 0) ? diff * finePerDay : 0;
}

class Library 
{
public:
    void addBook();
    void viewBooks();
    void borrowBook();
    void returnBook();
    void viewBorrowers();
};

void Library::addBook() 
{
    ofstream fout("books.txt", ios::app);
    string title, author;
    int qty;
    cout << "Enter book title: ";
    getline(cin >> ws, title);
    cout << "Enter author: ";
    getline(cin, author);
    cout << "Enter quantity: ";
    cin >> qty;
    if(qty<1)
    {
        cout<<"wrong entry of quantity\n";
        return;
    }
    fout << title << "," << author << "," << qty << "\n";
    fout.close();
    cout << "Book added successfully!\n";
}

void Library::viewBooks() 
{
    ifstream fin("books.txt");
    string line;
    cout << "\n------------ Available Books --------------\n";
    cout << left << setw(50) << "Title"
         << setw(50) << "Author"
         << "Quantity\n";
    cout << string(120, '-') << endl;
    while (getline(fin, line)) 
    {
        vector<string> data = split(line, ',');
        if (data.size() == 3) 
        {
            Book b(data[0], data[1], stoi(data[2]));
            b.displayBook();
        }
    }
    fin.close();
}

void Library::borrowBook() 
{
    string name, title, dueDate, currDate;
    cout << "Enter your name: ";
    getline(cin >> ws, name);
    cout << "Enter book title: ";
    getline(cin, title);
    cout << "Enter due date (DD MM YYYY): ";
    getline(cin, dueDate);
    cout << "Enter current date (DD MM YYYY): ";
    getline(cin, currDate);

    int due = dateToDays(dueDate);
    int curr = dateToDays(currDate);
    if (due == -1 || curr == -1 || (due - curr > 365) || (due - curr < 1)) 
    {
        cout << "Invalid or unrealistic due date!\n";
        return;
    }

    ifstream fin("books.txt");
    vector<vector<string>> books;
    string line;
    while (getline(fin, line))
        books.push_back(split(line, ','));
    fin.close();

    bool found = false;
    string matchedAuthor = "";
    for (int i = 0; i < books.size(); i++) 
    {
        if (books[i].size() == 3 && trim(books[i][0]) == trim(title) && stoi(books[i][2]) > 0) 
        {
            books[i][2] = to_string(stoi(books[i][2]) - 1);
            matchedAuthor = books[i][1];
            found = true;
            break;
        }
    }

    if (!found) 
    {
        cout << "Book not found or out of stock!\n";
        return;
    }

    ofstream fout("books.txt");
    for (auto &b : books)
        if (b.size() == 3)
            fout << b[0] << "," << b[1] << "," << b[2] << "\n";
    fout.close();

    ofstream bout("borrowers.txt", ios::app);
    bout << name << "," << title << "," << matchedAuthor << ",1," << dueDate << "\n";
    bout.close();

    cout << "Book borrowed successfully!\n";
}

void Library::returnBook() 
{
    string name, title, currDate;
    cout << "Enter your name: ";
    getline(cin >> ws, name);
    cout << "Enter book title: ";
    getline(cin, title);
    cout << "Enter current date (DD MM YYYY): ";
    getline(cin, currDate);

    ifstream fin("borrowers.txt");
    vector<vector<string>> borrowers;
    string line;
    while (getline(fin, line))
        borrowers.push_back(split(line, ','));
    fin.close();

    bool found = false;
    int foundIndex = -1;
    Borrower borrower;

    for (int i = 0; i < borrowers.size(); i++) 
    {
        if (borrowers[i].size() == 5 && trim(borrowers[i][0]) == trim(name) && trim(borrowers[i][1]) == trim(title))
        {
            borrower = Borrower(borrowers[i][0], borrowers[i][1], borrowers[i][2], stoi(borrowers[i][3]), borrowers[i][4]);
            found = true;
            foundIndex = i;
            break;
        }
    }

    if (!found) 
    {
        cout << "No record found for this borrower and book!\n";
        return;
    }

    int dues = borrower.calculateDues(currDate);
    if (dues == -1)
        return;

    borrowers.erase(borrowers.begin() + foundIndex);
    ofstream fout("borrowers.txt");
    for (auto &b : borrowers)
        if (b.size() == 5)
            fout << b[0] << "," << b[1] << "," << b[2] << "," << b[3] << "," << b[4] << "\n";
    fout.close();

    ifstream bin("books.txt");
    vector<vector<string>> books;
    while (getline(bin, line))
        books.push_back(split(line, ','));
    bin.close();

    for (auto &b : books)
        if (b.size() == 3 && trim(b[0]) == trim(title) && trim(b[1]) == trim(borrower.getAuthor()))
            b[2] = to_string(stoi(b[2]) + 1);

    ofstream bout("books.txt");
    for (auto &b : books)
        if (b.size() == 3)
            bout << b[0] << "," << b[1] << "," << b[2] << "\n";
    bout.close();

    if (dues > 0)
        cout << "You have a fine of Rs" << dues << " for overdue days.\n";
    else
        cout << "Book returned successfully. No dues.\n";
}

void Library::viewBorrowers() 
{
    ifstream fin("borrowers.txt");
    string line;
    cout << "\n--- Borrower Records ---\n";
    cout << left << setw(20) << "Name"
         << setw(25) << "Book Title"
         << setw(15) << "Due Date" << endl;
    cout << string(60, '-') << endl;
    while (getline(fin, line)) 
    {
        vector<string> data = split(line, ',');
        if (data.size() == 5) 
        {
            Borrower b(data[0], data[1], data[2], stoi(data[3]), data[4]);
            b.displayBorrower();
        }
    }
    fin.close();
}

int main() 
{   ofstream("books.txt", ios::app).close();
    ofstream("borrowers.txt", ios::app).close();
    Library lib;
    int choice;
    do {
        cout << "\n===== LIBRARY MENU =====\n";
        cout << "1. Add Book\n";
        cout << "2. View Books\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. View Borrowers\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        switch (choice) 
        {
            case 1: 
                lib.addBook(); 
                break;
            case 2: 
                lib.viewBooks(); 
                break;
            case 3: 
                lib.borrowBook(); 
                break;
            case 4: 
                lib.returnBook(); 
                break;
            case 5: 
                lib.viewBorrowers(); 
                break;
            case 0: 
                cout << "Goodbye!\n"; break;
            default: 
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    return 0;
}
