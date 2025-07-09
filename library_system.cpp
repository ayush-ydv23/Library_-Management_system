#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <limits>
using namespace std;

// Forward declarations
class Account;
class Book;
class User;

// Global variables (encapsulated in a Library class later)
map<string, Account> accounts;  // Changed from int to string
map<string, Book> books;
map<string, User*> users;  // Changed from int to string
int simulatedDate = 0;  // Global variable to track simulated date in seconds

// Forward declarations of file operations
void saveAccounts();
void loadAccounts();
void saveBooks();
void loadBooks();
void loadUsers();
int getCurrentDate();  // Forward declaration of getCurrentDate

class Book {
private:
    string isbn, title, author, publisher;
    int year;
    bool available, reserved;

public:
    Book(string t = "", string a = "", string p = "", int y = 0, string i = "", bool avail = true) 
        : title(t), author(a), publisher(p), year(y), isbn(i), available(avail), reserved(false) {}

    string getISBN() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    bool isAvailable() const { return available; }
    bool isReserved() const { return reserved; }
    
    void setISBN(const string& i) { isbn = i; }
    void setTitle(const string& t) { title = t; }
    void setAuthor(const string& a) { author = a; }
    void setPublisher(const string& p) { publisher = p; }
    void setYear(int y) { year = y; }
    void setAvailability(bool status) { available = status; }
    void setReserved(bool status) { reserved = status; }

    void displayISBN() const { cout << "ISBN: " << isbn << "\n"; }
    void displayTitle() const { cout << "Title: " << title << "\n"; }
    void displayAuthor() const { cout << "Author: " << author << "\n"; }
    void displayPublisher() const { cout << "Publisher: " << publisher << "\n"; }
    void displayYear() const { cout << "Year: " << year << "\n"; }
    void displayStatus() const { 
        cout << "Status: " << (available ? "Available" : "Borrowed")
             << " | " << (reserved ? "Reserved" : "Not Reserved") << "\n"; 
    }

    void display() const {
        cout << "ISBN: " << isbn << "\nTitle: " << title << "\nAuthor: " << author 
             << "\nPublisher: " << publisher << "\nYear: " << year
             << "\nStatus: " << (available ? "Available" : "Borrowed")
             << " | " << (reserved ? "Reserved" : "Not Reserved") << "\n";
    }

    void saveToFile(ofstream &outfile) const {
        outfile << isbn << "\n" << title << "\n" << author << "\n" 
                << publisher << "\n" << year << "\n" << available << " " << reserved << "\n";
    }
    
    void loadFromFile(ifstream &infile) {
        getline(infile, isbn);
        getline(infile, title);
        getline(infile, author);
        getline(infile, publisher);
        infile >> year;
        infile >> available >> reserved;
        infile.ignore();
    }
};

class Account {
private:
    friend class User;  // Allow User class to access private members
    string userID;
    map<string, int> borrowedBooks;  // ISBN -> due date in seconds
    map<string, int> lastFinePaidTime;  // ISBN -> last fine paid time in seconds
    mutable map<string, double> bookFines;  // ISBN -> current fine amount
    mutable double totalFine;
    bool isFaculty;
    int maxBooks;
    int maxDays;
    vector<pair<string, int>> borrowingHistory;

public:
    Account(string id = "", bool faculty = false) 
        : userID(id), totalFine(0), isFaculty(faculty), 
          maxBooks(faculty ? 5 : 3),  // Faculty can borrow 5 books, students 3
          maxDays(faculty ? 30 : 15) {}  // Faculty gets 30 days, students 15

    string getUserID() const { return userID; }
    double getTotalFine() const { return totalFine; }
    map<string, int>& getBorrowedBooks() { return borrowedBooks; }
    int getMaxBooks() const { return maxBooks; }
    int getMaxDays() const { return maxDays; }
    bool isFacultyMember() const { return isFaculty; }
    const vector<pair<string, int>>& getBorrowingHistory() const { return borrowingHistory; }

    double getBookFine(const string& isbn) const {
        auto it = bookFines.find(isbn);
        return (it != bookFines.end()) ? it->second : 0.0;
    }

    bool payBookFine(const string& isbn, double amount, int currentDate) {
        // Recalculate fines before payment
        updateFines(currentDate);
        
        if (!isFaculty) {  // Faculty members don't pay fines
            auto it = bookFines.find(isbn);
            if (it == bookFines.end() || amount != it->second) {
                cout << "\nPayment REJECTED!\n";
                cout << "Please pay the exact fine amount for this book: " << it->second << " rupees\n";
                return false;
            }

            cout << "\n=== Fine Payment Details ===\n";
            cout << "Book Fine amount: " << it->second << " rupees\n";
            cout << "Payment amount: " << amount << " rupees\n";
            
            // Remove the fine and reset due date
            totalFine -= it->second;
            bookFines.erase(isbn);
            
            // Set new due date (using actual days)
            int newDueDate = currentDate + (maxDays * 24 * 60 * 60);
            borrowedBooks[isbn] = newDueDate;
            lastFinePaidTime[isbn] = currentDate;  // Update last fine paid time
            
            cout << "\nPayment ACCEPTED!\n";
            cout << "Fine of " << amount << " rupees has been paid for this book.\n";
            cout << "Book has been reissued with new due date.\n";
            
            // Convert due date to human-readable format
            time_t dueTime = newDueDate;
            struct tm* dueTm = localtime(&dueTime);
            char dueStr[26];
            strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
            cout << "New due date: " << dueStr << "\n";
            cout << "Remaining total fine: " << totalFine << " rupees\n";
            
            return true;
        }
        // For faculty members, just reissue the book without requiring payment
        int newDueDate = currentDate + (maxDays * 24 * 60 * 60);
        borrowedBooks[isbn] = newDueDate;
        lastFinePaidTime[isbn] = currentDate;
        
        // Convert due date to human-readable format
        time_t dueTime = newDueDate;
        struct tm* dueTm = localtime(&dueTime);
        char dueStr[26];
        strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
        
        cout << "\nBook has been reissued.\n";
        cout << "New due date: " << dueStr << "\n";
        return true;
    }

    bool payFine(double amount, int currentDate) {
        // Faculty members don't need to pay fines
        if (isFaculty) {
            cout << "\nFaculty members do not incur fines.\n";
            return true;
        }
        
        // Recalculate fines before payment
        updateFines(currentDate);
        
        cout << "\n=== Fine Payment Details ===\n";
        cout << "Total fine amount: " << totalFine << " rupees\n";
        cout << "Payment amount: " << amount << " rupees\n";

        if (amount < totalFine) {
            cout << "\nPayment REJECTED!\n";
            cout << "Payment amount is less than the total fine.\n";
            cout << "Remaining amount to pay: " << (totalFine - amount) << " rupees\n";
            cout << "Please pay the exact amount of " << totalFine << " rupees.\n";
            return false;
        }
        if (amount > totalFine) {
            cout << "\nPayment REJECTED!\n";
            cout << "Payment amount is more than the total fine.\n";
            cout << "Excess amount: " << (amount - totalFine) << " rupees\n";
            cout << "Please pay the exact amount of " << totalFine << " rupees.\n";
            return false;
        }
        
        cout << "\nPayment ACCEPTED!\n";
        cout << "Fine of " << totalFine << " rupees has been paid successfully.\n";
        
        // Update last fine paid time and reissue all books with new due dates
        for (auto& book : borrowedBooks) {
            lastFinePaidTime[book.first] = currentDate;
            // For testing: 10 seconds = 1 day
            int newDueDate = currentDate + (maxDays * 24 * 60 * 60);  // Changed from (24 * 60 * 60) to 10 for testing
            book.second = newDueDate;
            
            // Convert due date to human-readable format for display
            time_t dueTime = newDueDate;
            struct tm* dueTm = localtime(&dueTime);
            char dueStr[26];
            strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
            
            cout << "\nBook " << book.first << " has been reissued.\n";
            cout << "New due date: " << dueStr << "\n";
        }
        
        totalFine = 0;
        bookFines.clear();  // Clear all book fines after total payment
        
        return true;
    }

    void addToHistory(const string& isbn, int returnDate) {
        borrowingHistory.push_back({isbn, returnDate});
    }

    bool returnBook(const string& isbn, int currentDate) {
        // Update fines before checking
        updateFines(currentDate);
        
        auto it = borrowedBooks.find(isbn);
        if (it == borrowedBooks.end()) {
            cout << "Book not found in borrowed list.\n";
            return false;
        }

        // Convert dates to time_t for proper comparison
        time_t dueTime = it->second;
        time_t currentTime = currentDate;
        
        // Calculate overdue days using difftime (which returns seconds)
        double secondsOverdue = difftime(currentTime, dueTime);
        int daysOverdue = static_cast<int>(secondsOverdue / (24 * 60 * 60));
        
        // Only show overdue message and calculate fine if actually overdue
        if (daysOverdue > 0) {
            double fine = daysOverdue * 10.0;  // 10 rupees per day
            
            // Convert dates to human-readable format
            struct tm* dueTm = localtime(&dueTime);
            struct tm* currentTm = localtime(&currentTime);
            char dueStr[26], currentStr[26];
            strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
            strftime(currentStr, sizeof(currentStr), "%Y-%m-%d %H:%M:%S", currentTm);
            
            cout << "\n=== Book is Overdue ===\n";
            cout << "Due Date: " << dueStr << "\n";
            cout << "Current Date: " << currentStr << "\n";
            cout << "Days Overdue: " << daysOverdue << "\n";
            cout << "Fine Rate: 10 rupees per day\n";
            cout << "Fine Amount: " << fine << " rupees\n";
            
            cout << "\nYou must pay the fine before returning the book.\n";
            cout << "Would you like to pay the fine now? (1 for yes, 0 for no): ";
            int choice;
            cin >> choice;
            
            if (choice == 1) {
                cout << "Enter amount to pay (" << fine << " rupees): ";
                double amount;
                cin >> amount;
                if (!payBookFine(isbn, amount, currentDate)) {
                    cout << "Return cancelled due to payment failure.\n";
                    return false;
                }
            } else {
                cout << "Return cancelled. Please pay the fine first.\n";
                return false;
            }
        }

        // If we get here, either the book is not overdue or the fine has been paid
        addToHistory(isbn, currentDate);
        borrowedBooks.erase(it);
        books[isbn].setAvailability(true);
        cout << "\nBook returned successfully.\n";
        cout << "Book status updated to: Available\n";
        return true;
    }

    void updateFines(int currentDate) const {
        if (isFaculty) return; // Faculty members don't get fines

        double newTotalFine = 0;
        
        for (const auto& book : borrowedBooks) {
            const string& isbn = book.first;
            int dueDate = book.second;
            
            // Convert dates to time_t for proper comparison
            time_t dueTime = dueDate;
            time_t currentTime = currentDate;
            
            // Calculate overdue days using difftime
            double secondsOverdue = difftime(currentTime, dueTime);
            int daysOverdue = static_cast<int>(secondsOverdue / (24 * 60 * 60));
            
            if (daysOverdue > 0) {
                // Calculate fine based on days overdue
                double fine = daysOverdue * 10.0;  // 10 rupees per day
                bookFines[isbn] = fine;
                newTotalFine += fine;
                
                // Convert dates to human-readable format
                struct tm* dueTm = localtime(&dueTime);
                struct tm* currentTm = localtime(&currentTime);
                char dueStr[26], currentStr[26];
                strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
                strftime(currentStr, sizeof(currentStr), "%Y-%m-%d %H:%M:%S", currentTm);

                cout << "\n=== Fine Update for Book " << isbn << " ===\n";
                auto bookIt = books.find(isbn);
                if (bookIt != books.end()) {
                    cout << "Title: " << bookIt->second.getTitle() << "\n";
                }
                cout << "Due Date: " << dueStr << "\n";
                cout << "Current Date: " << currentStr << "\n";
                cout << "Days Overdue: " << daysOverdue << "\n";
                cout << "Fine Rate: 10 rupees per day\n";
                cout << "Current Fine: " << fine << " rupees\n";
            }
        }
        
        // Update total fine with new calculations
        totalFine = newTotalFine;
        
        if (totalFine > 0) {
            cout << "\nTotal Fine Amount: " << totalFine << " rupees\n";
        }
    }

    void displayAccountDetails(int currentDate) const {
        // Recalculate fines before displaying
        updateFines(currentDate);
        
        cout << "\n=== Account Details ===\n";
        cout << "User ID: " << userID << "\n";
        cout << "User Type: " << (isFaculty ? "Faculty" : "Student") << "\n";
        cout << "Maximum Books Allowed: " << maxBooks << "\n";
        cout << "Maximum Days Allowed: " << maxDays << " days\n";
        if (!isFaculty) {
            cout << "Current Total Fine: " << totalFine << " rupees\n";
        }
        
        cout << "\n=== Currently Borrowed Books ===\n";
        if (borrowedBooks.empty()) {
            cout << "No books currently borrowed.\n";
        } else {
            for (const auto& book : borrowedBooks) {
                auto bookIt = books.find(book.first);
                if (bookIt != books.end()) {
                    cout << "\nBook Details:\n";
                    cout << "ISBN: " << book.first << "\n";
                    cout << "Title: " << bookIt->second.getTitle() << "\n";
                    cout << "Author: " << bookIt->second.getAuthor() << "\n";
                    
                    // Convert due date to human-readable format
                    time_t dueTime = book.second;
                    struct tm* dueTm = localtime(&dueTime);
                    char dueStr[26];
                    strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
                    cout << "Due Date: " << dueStr << "\n";
                    
                    // Calculate if overdue
                    int secondsOverdue = currentDate - book.second;
                    int daysOverdue = secondsOverdue / (24 * 60 * 60);
                    if (daysOverdue > 0) {
                        cout << "Status: OVERDUE by " << daysOverdue << " days\n";
                        if (!isFaculty) {
                            cout << "Fine Amount: " << (daysOverdue * 10.0) << " rupees\n";
                        }
                    } else {
                        cout << "Status: On time\n";
                    }
                    cout << "------------------------\n";
                }
            }
        }
        
        cout << "\n=== Borrowing History ===\n";
        if (borrowingHistory.empty()) {
            cout << "No borrowing history.\n";
        } else {
            for (const auto& history : borrowingHistory) {
                auto bookIt = books.find(history.first);
                if (bookIt != books.end()) {
                    cout << "\nBook Details:\n";
                    cout << "ISBN: " << history.first << "\n";
                    cout << "Title: " << bookIt->second.getTitle() << "\n";
                    cout << "Author: " << bookIt->second.getAuthor() << "\n";
                    
                    // Convert return date to human-readable format
                    time_t returnTime = history.second;
                    struct tm* returnTm = localtime(&returnTime);
                    char returnStr[26];
                    strftime(returnStr, sizeof(returnStr), "%Y-%m-%d %H:%M:%S", returnTm);
                    cout << "Return Date: " << returnStr << "\n";
                    cout << "------------------------\n";
                }
            }
        }
    }

    void saveToFile(ofstream &outfile) const {
        outfile << userID << "\n" 
               << fixed << totalFine << "\n" 
               << (isFaculty ? "1" : "0") << "\n" 
               << maxBooks << "\n" 
               << maxDays << "\n";
        
        // Save borrowed books and their details
        outfile << borrowedBooks.size() << "\n";
        for (const auto &book : borrowedBooks) {
            outfile << book.first << "\n"  // ISBN
                   << book.second << "\n"  // Due date
                   << (lastFinePaidTime.count(book.first) ? lastFinePaidTime.at(book.first) : book.second) << "\n"  // Last fine paid time
                   << (bookFines.count(book.first) ? bookFines.at(book.first) : 0.0) << "\n";  // Current fine
        }
        
        // Save borrowing history
        outfile << borrowingHistory.size() << "\n";
        for (const auto &history : borrowingHistory) {
            outfile << history.first << "\n"   // ISBN
                   << history.second << "\n";   // Return date
        }
    }
    
    void loadFromFile(ifstream &infile, int currentDate) {
        infile >> userID >> totalFine;
        
        string facultyStr;
        infile >> facultyStr;
        isFaculty = (facultyStr == "1");
        
        infile >> maxBooks >> maxDays;
        infile.ignore();

        int numBorrowed;
        infile >> numBorrowed;
        infile.ignore();
        
        borrowedBooks.clear();
        lastFinePaidTime.clear();
        bookFines.clear();
        
        for (int i = 0; i < numBorrowed; i++) {
            string isbn;
            int dueDate, lastPaidTime;
            double fine;
            getline(infile, isbn);
            infile >> dueDate >> lastPaidTime >> fine;
            infile.ignore();
            
            borrowedBooks[isbn] = dueDate;
            lastFinePaidTime[isbn] = lastPaidTime;
            if (fine > 0) bookFines[isbn] = fine;
        }

        // Recalculate fines after loading to ensure consistency
        updateFines(currentDate);

        int numHistory;
        infile >> numHistory;
        infile.ignore();
        
        borrowingHistory.clear();
        for (int i = 0; i < numHistory; i++) {
            string isbn;
            int returnDate;
            getline(infile, isbn);
            infile >> returnDate;
            infile.ignore();
            borrowingHistory.push_back({isbn, returnDate});
        }
    }
};

class User {
protected:
    string id;
    string name;
    string password;
    Account &account;

public:
    User(string userId, string userName, string pwd, bool isFaculty = false) 
        : id(userId), name(userName), password(pwd), account(accounts[userId]) {
        // Create a new account only if one doesn't exist
        if (accounts.find(userId) == accounts.end()) {
            accounts[userId] = Account(userId, isFaculty);
        } else {
            // Update existing account's faculty status
            accounts[userId].isFaculty = isFaculty;
            accounts[userId].maxBooks = isFaculty ? 5 : 3;
            accounts[userId].maxDays = isFaculty ? 30 : 15;
        }
        // Update the account ID to match the user ID
        accounts[userId].userID = userId;
    }

    virtual ~User() {}

    Account& getAccount() { return account; }
    string getID() const { return id; }
    string getName() const { return name; }
    string getPassword() const { return password; }  // Added getter for password

    virtual bool borrowBook(const string& isbn, int currentDate) = 0;
    virtual bool returnBook(const string& isbn, int currentDate) = 0;
    virtual void displayMenu() = 0;
};

class Student : public User {
public:
    Student(string id, string name, string pwd) : User(id, name, pwd, false) {}

    bool borrowBook(const string& isbn, int currentDate) override {
        // Update fines before checking
        account.updateFines(currentDate);
        
        // Check current fine
        if (account.getTotalFine() > 0) {
            cout << "\nCannot borrow books due to unpaid fines.\n";
            cout << "Current total fine: " << account.getTotalFine() << " rupees\n";
            cout << "Please pay your fines before borrowing more books.\n";
            return false;
        }

        if (account.getBorrowedBooks().size() >= account.getMaxBooks()) {
            cout << "\nMaximum number of books already borrowed.\n";
            cout << "Student limit: " << account.getMaxBooks() << " books\n";
            cout << "Currently borrowed: " << account.getBorrowedBooks().size() << " books\n";
            return false;
        }

        auto it = books.find(isbn);
        if (it == books.end() || !it->second.isAvailable()) {
            cout << "Book not available for borrowing.\n";
            return false;
        }

        // Set due date in seconds (using actual days)
        int dueDate = currentDate + (account.getMaxDays() * 24 * 60 * 60);
        account.getBorrowedBooks()[isbn] = dueDate;
        it->second.setAvailability(false);
        
        // Convert due date to human-readable format
        time_t dueTime = dueDate;
        struct tm* dueTm = localtime(&dueTime);
        char dueStr[26];
        strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);

        cout << "\nBook borrowed successfully!\n";
        cout << "Due date: " << dueStr << "\n";
        cout << "Borrowing period: " << account.getMaxDays() << " days\n";
        cout << "Fine rate: 10 rupees per day if overdue\n";
        return true;
    }

    bool returnBook(const string& isbn, int currentDate) override {
        // Update fines before checking
        account.updateFines(currentDate);
        
        auto it = account.getBorrowedBooks().find(isbn);
        if (it == account.getBorrowedBooks().end()) {
            cout << "Book not found in borrowed list.\n";
            return false;
        }

        // Convert dates to time_t for proper comparison
        time_t dueTime = it->second;
        time_t currentTime = currentDate;
        
        // Calculate overdue days using difftime (which returns seconds)
        double secondsOverdue = difftime(currentTime, dueTime);
        int daysOverdue = static_cast<int>(secondsOverdue / (24 * 60 * 60));
        
        // Only show overdue message and calculate fine if actually overdue
        if (daysOverdue > 0) {
            double fine = daysOverdue * 10.0;  // 10 rupees per day
            
            // Convert dates to human-readable format
            struct tm* dueTm = localtime(&dueTime);
            struct tm* currentTm = localtime(&currentTime);
            char dueStr[26], currentStr[26];
            strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
            strftime(currentStr, sizeof(currentStr), "%Y-%m-%d %H:%M:%S", currentTm);
            
            cout << "\n=== Book is Overdue ===\n";
            cout << "Due Date: " << dueStr << "\n";
            cout << "Current Date: " << currentStr << "\n";
            cout << "Days Overdue: " << daysOverdue << "\n";
            cout << "Fine Rate: 10 rupees per day\n";
            cout << "Fine Amount: " << fine << " rupees\n";
            
            cout << "\nYou must pay the fine before returning the book.\n";
            cout << "Would you like to pay the fine now? (1 for yes, 0 for no): ";
            int choice;
            cin >> choice;
            
            if (choice == 1) {
                cout << "Enter amount to pay (" << fine << " rupees): ";
                double amount;
                cin >> amount;
                if (!account.payBookFine(isbn, amount, currentDate)) {
                    cout << "Return cancelled due to payment failure.\n";
                    return false;
                }
            } else {
                cout << "Return cancelled. Please pay the fine first.\n";
                return false;
            }
        }

        // If we get here, either the book is not overdue or the fine has been paid
        account.addToHistory(isbn, currentDate);
        account.getBorrowedBooks().erase(it);
        books[isbn].setAvailability(true);
        cout << "\nBook returned successfully.\n";
        cout << "Book status updated to: Available\n";
        return true;
    }

    void displayMenu() override {
        cout << "\n\n=== Student Menu ===\n";
        cout << "1. View Available Books\n";
        cout << "2. Borrow a Book (Max " << account.getMaxBooks() << " books, " << account.getMaxDays() << " days)\n";
        cout << "3. Return a Book\n";
        cout << "4. View Borrowed Books\n";
        cout << "5. View Fine (Current: " << account.getTotalFine() << " rupees)\n";
        cout << "6. Pay Fine\n";
        cout << "7. View Account Details\n";
        cout << "8. Exit\n\n";
    }
};

class Faculty : public User {
public:
    Faculty(string id, string name, string pwd) : User(id, name, pwd, true) {}

    bool borrowBook(const string& isbn, int currentDate) override {
        // Check if book exists and is available
        auto it = books.find(isbn);
        if (it == books.end()) {
            cout << "Book not found.\n";
            return false;
        }
        if (!it->second.isAvailable()) {
            cout << "Book is not available.\n";
            return false;
        }

        // Check if user has reached maximum books
        if (account.getBorrowedBooks().size() >= account.getMaxBooks()) {
            cout << "\nMaximum number of books already borrowed.\n";
            cout << "Faculty limit: " << account.getMaxBooks() << " books\n";
            cout << "Currently borrowed: " << account.getBorrowedBooks().size() << " books\n";
            return false;
        }

        // Check for any book overdue by more than 60 days
        for (const auto& borrowed : account.getBorrowedBooks()) {
            int secondsOverdue = currentDate - borrowed.second;
            int daysOverdue = secondsOverdue / (24 * 60 * 60);  // Convert seconds to actual days
            if (daysOverdue > 60) {
                cout << "Cannot borrow: You have a book overdue by more than 60 days.\n";
                cout << "Please return all overdue books first.\n";
                return false;
            }
        }

        // Borrow the book
        int dueDate = currentDate + (account.getMaxDays() * 24 * 60 * 60);  // 30 days for faculty
        account.getBorrowedBooks()[isbn] = dueDate;
        it->second.setAvailability(false);
        
        // Convert due date to human-readable format
        time_t dueTime = dueDate;
        struct tm* dueTm = localtime(&dueTime);
        char dueStr[26];
        strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);

        cout << "\nBook borrowed successfully!\n";
        cout << "Due date: " << dueStr << "\n";
        cout << "Maximum borrowing period: " << account.getMaxDays() << " days\n";
        cout << "Books borrowed: " << account.getBorrowedBooks().size() << " of " << account.getMaxBooks() << "\n";
        return true;
    }

    bool returnBook(const string& isbn, int currentDate) override {
        auto it = account.getBorrowedBooks().find(isbn);
        if (it == account.getBorrowedBooks().end()) {
            cout << "Book not found in borrowed list.\n";
            return false;
        }

        // Calculate days overdue
        int secondsOverdue = currentDate - it->second;
        int daysOverdue = secondsOverdue / (24 * 60 * 60);  // Convert seconds to actual days
        
        if (daysOverdue > 0) {
            cout << "\n=== Book is Overdue ===\n";
            cout << "Due Date: " << it->second << "\n";
            cout << "Current Date: " << currentDate << "\n";
            cout << "Days Overdue: " << daysOverdue << "\n";
            cout << "Note: Faculty members do not incur fines for overdue books.\n";
        }

        account.addToHistory(isbn, currentDate);
        account.getBorrowedBooks().erase(it);
        books[isbn].setAvailability(true);
        cout << "\nBook returned successfully.\n";
        cout << "Book status updated to: Available\n";
        return true;
    }

    void displayMenu() override {
        cout << "\n=== Faculty Menu ===\n";
        cout << "1. View Available Books\n";
        cout << "2. Borrow a Book (Max " << account.getMaxBooks() << " books, " << account.getMaxDays() << " days)\n";
        cout << "3. Return a Book\n";
        cout << "4. View Borrowed Books\n";
        cout << "5. View Borrowing History\n";
        cout << "6. View Account Details\n";
        cout << "7. Exit\n";
    }
};

class Librarian : public User {
public:
    Librarian(string id, string name, string pwd) : User(id, name, pwd, false) {}

    bool borrowBook(const string& isbn, int currentDate) override {
        cout << "Librarians cannot borrow books.\n";
        return false;
    }

    bool returnBook(const string& isbn, int currentDate) override {
        cout << "Librarians cannot return books.\n";
        return false;
    }

    void addUser(string id, string name, string password, bool isFaculty) {
        if (users.find(id) != users.end()) {
            cout << "Error: User ID already exists!\n";
            return;
        }

        User* newUser;
        if (isFaculty) {
            newUser = new Faculty(id, name, password);
        } else {
            newUser = new Student(id, name, password);
        }
        users[id] = newUser;
        cout << "User added successfully!\n";
    }
    
    void addBook(string isbn, string title, string author, string publisher, int year) {
        if (books.find(isbn) == books.end()) {
            books[isbn] = Book(title, author, publisher, year, isbn, true);
            cout << "Book added successfully!\n";
        } else {
            cout << "Book already exists!\n";
        }
    }

    void removeUser(string userId) {
        if (users.find(userId) != users.end()) {
            delete users[userId];
            users.erase(userId);
            accounts.erase(userId);
            cout << "User removed successfully!\n";
        } else {
            cout << "User not found!\n";
        }
    }
    
    void removeBook(string isbn) {
        if (books.find(isbn) != books.end()) {
            books.erase(isbn);
            cout << "Book removed successfully!\n";
        } else {
            cout << "Book not found!\n";
        }
    }

    void updateBook(string isbn, string newTitle, string newAuthor, string newPublisher, int newYear) {
        if (books.find(isbn) != books.end()) {
            books[isbn] = Book(newTitle, newAuthor, newPublisher, newYear, isbn, books[isbn].isAvailable());
            books[isbn].setReserved(books[isbn].isReserved());
            cout << "Book updated successfully!\n";
        } else {
            cout << "Book not found!\n";
        }
    }

    void displayMenu() override {
        cout << "\nLibrarian Menu:\n"
             << "1. Add User\n"
             << "2. Remove User\n"
             << "3. Add Book\n"
             << "4. Remove Book\n"
             << "5. Update Book\n"
             << "6. View All Books\n"
             << "7. View All Users\n"
             << "8. Exit\n";
    }
};

class Library {
public:
    void displayBooks() const {
        cout << "\nLibrary Books:\n";
        for (const auto& p : books) {
            p.second.display();
        }
    }

    void displayUsers() const {
        cout << "\nLibrary Users:\n";
        for (const auto& p : users) {
            cout << "ID: " << p.second->getID() 
                 << ", Name: " << p.second->getName() 
                 << ", Type: " << (dynamic_cast<Librarian*>(p.second) ? "Librarian" :
                                 dynamic_cast<Faculty*>(p.second) ? "Faculty" : "Student")
                 << "\n";
        }
    }

    void saveAllData() {
        cout << "Saving all data...\n";
        try {
            saveAccounts();
            saveBooks();
            saveUsers();
            cout << "All data saved successfully.\n";
        } catch (const exception& e) {
            cerr << "Error saving data: " << e.what() << "\n";
        }
    }

    void loadAllData() {
        cout << "Loading all data...\n";
        try {
            loadAccounts();
            loadBooks();
            loadUsers();
            cout << "All data loaded successfully.\n";
        } catch (const exception& e) {
            cerr << "Error loading data: " << e.what() << "\n";
        }
    }

    void initializeDefaultData() {
        // Add default books (at least 10)
        books["1"] = Book("Design Patterns", "Erich Gamma", "Addison-Wesley", 1994, "1");
        books["2"] = Book("Clean Code", "Robert Martin", "Prentice Hall", 2008, "2");
        books["3"] = Book("Introduction to Algorithms", "Thomas Cormen", "MIT Press", 2009, "3");
        books["4"] = Book("Code Complete", "Steve McConnell", "Microsoft Press", 2004, "4");
        books["5"] = Book("Refactoring", "Martin Fowler", "Addison-Wesley", 1999, "5");
        books["6"] = Book("Head First Java", "Kathy Sierra", "O'Reilly", 2005, "6");
        books["7"] = Book("The Pragmatic Programmer", "Andrew Hunt", "Addison-Wesley", 1999, "7");
        books["8"] = Book("Effective C++", "Scott Meyers", "Addison-Wesley", 2005, "8");
        books["9"] = Book("Programming Pearls", "Jon Bentley", "Addison-Wesley", 1999, "9");
        books["10"] = Book("The Art of Computer Programming", "Donald Knuth", "Addison-Wesley", 1968, "10");

        // Add default users
        // 1 Librarian
        users["100"] = new Librarian("100", "Admin", "admin123");
        
        // 3 Faculty members
        users["101"] = new Faculty("101", "Dr. Smith", "faculty123");
        users["102"] = new Faculty("102", "Prof. Johnson", "faculty123");
        users["103"] = new Faculty("103", "Dr. Williams", "faculty123");
        
        // 5 Students
        users["201"] = new Student("201", "John Doe", "student123");
        users["202"] = new Student("202", "Jane Smith", "student123");
        users["203"] = new Student("203", "Bob Wilson", "student123");
        users["204"] = new Student("204", "Alice Brown", "student123");
        users["205"] = new Student("205", "Charlie Davis", "student123");

        cout << "\nInitialized library with:\n";
        cout << "- 10 books\n";
        cout << "- 1 librarian (ID: 100, password: admin123)\n";
        cout << "- 3 faculty members (IDs: 101-103, password: faculty123)\n";
        cout << "- 5 students (IDs: 201-205, password: student123)\n";
    }

private:
    void saveUsers() {
        ofstream file("users.txt", ios::out);  // Open in write mode, create if doesn't exist
        if (!file) {
            cerr << "Error: Unable to create/open users.txt for writing!\n";
            return;
        }
        file << users.size() << "\n";
        for (const auto& p : users) {
            file << p.first << "\n" << p.second->getName() << "\n"
                 << p.second->getPassword() << "\n"
                 << (dynamic_cast<Librarian*>(p.second) ? 2 :
                     dynamic_cast<Faculty*>(p.second) ? 1 : 0) << "\n";
        }
        file.close();
    }

    void loadUsers() {
        ifstream file("users.txt");
        if (!file) {
            cout << "No existing users file found. Will create new file when saving.\n";
            return;
        }
        int numUsers;
        file >> numUsers;
        file.ignore();
        cout << "Loading " << numUsers << " users...\n";
        
        // Don't clear existing users, merge with loaded data
        for (int i = 0; i < numUsers; i++) {
            string id, name, password;
            int type;
            getline(file, id);
            getline(file, name);
            getline(file, password);
            file >> type;
            file.ignore();
            
            // Only create new user if it doesn't exist
            if (users.find(id) == users.end()) {
                User* user = nullptr;
                switch (type) {
                    case 2: user = new Librarian(id, name, password); break;
                    case 1: user = new Faculty(id, name, password); break;
                    case 0: user = new Student(id, name, password); break;
                }
                if (user) users[id] = user;
            }
        }
        file.close();
        cout << "Users loaded successfully.\n";
    }
};

void saveAccounts() {
    ofstream file("accounts.txt", ios::out);  // Open in write mode, create if doesn't exist
    if (!file) {
        cerr << "Error: Unable to create/open accounts.txt for writing!\n";
        return;
    }
    file << accounts.size() << "\n";
    for (const auto& pair : accounts) {
        pair.second.saveToFile(file);
    }
    file.close();
}

void loadAccounts() {
    ifstream file("accounts.txt");
    if (!file) {
        cout << "No existing accounts file found. Will create new file when saving.\n";
        return;
    }
    int numUsers;
    file >> numUsers;
    cout << "Loading " << numUsers << " accounts...\n";
    
    // Don't clear existing accounts, merge with loaded data
    for (int i = 0; i < numUsers; i++) {
        Account acc;
        acc.loadFromFile(file, getCurrentDate());
        string userId = acc.getUserID();
        accounts[userId] = acc;  // Update or add the account
    }
    file.close();
    cout << "Accounts loaded successfully.\n";
}

void saveBooks() {
    ofstream file("books.txt", ios::out);  // Open in write mode, create if doesn't exist
    if (!file) {
        cerr << "Error: Unable to create/open books.txt for writing!\n";
        return;
    }
    file << books.size() << "\n";
    for (const auto& p : books) {
        p.second.saveToFile(file);
    }
    file.close();
}

void loadBooks() {
    ifstream file("books.txt");
    if (!file) {
        cout << "No existing books file found. Will create new file when saving.\n";
        return;
    }
    int numBooks;
    file >> numBooks;
    file.ignore();
    cout << "Loading " << numBooks << " books...\n";
    
    // Don't clear existing books, merge with loaded data
    for (int i = 0; i < numBooks; i++) {
        Book book;
        book.loadFromFile(file);
        books[book.getISBN()] = book;  // Update or add the book
    }
    file.close();
    cout << "Books loaded successfully.\n";
}

int getCurrentDate() {
    if (simulatedDate > 0) {
        return simulatedDate;
    }
    time_t now = time(0);
    return static_cast<int>(now); // Return seconds instead of days
}

int main() {
    cout << "Starting Library Management System...\n";
    Library library;
    
    // Try to load existing data first
    cout << "\nLoading previous session data...\n";
    library.loadAllData();  // This will load existing data or create new files

    // Initialize with default data only if no users exist
    if (users.empty()) {
        cout << "No existing users found. Creating initial setup...\n";
        library.initializeDefaultData();
    }
    
    // Save current state
    library.saveAllData();
    cout << "System initialized and data saved.\n";

    while (true) {
        string currentUserId;
        string password;
        bool loginSuccess = false;
        User* currentUser = nullptr;

        do {
            cout << "\n=== Library Management System ===\n";
            cout << "Enter your user ID: ";
            cin >> currentUserId;
            cout << "Enter your password: ";
            cin >> password;

            auto it = users.find(currentUserId);
            if (it == users.end()) {
                cout << "User not found!\n";
                continue;
            }

            currentUser = it->second;
            if (currentUser->getPassword() == password) {
                loginSuccess = true;
                cout << "Login successful! Welcome " << currentUser->getName() << "!\n";
            } else {
                cout << "Invalid password!\n";
                currentUser = nullptr;
            }
        } while (!loginSuccess);

        int choice;
        int currentDate = getCurrentDate();

        do {
            // Update current time and fines at the start of each menu iteration
            currentDate = getCurrentDate();
            cout << "\nCurrent Time: " << currentDate << "\n";
            currentUser->getAccount().updateFines(currentDate);

            currentUser->displayMenu();
            cout << "Enter your choice: ";
            cin >> choice;

            // Add date simulation option for testing
            if (choice == 0 && dynamic_cast<Librarian*>(currentUser)) {
                cout << "\n=== Date Simulation ===\n";
                cout << "Current simulated date: " << currentDate << "\n";
                cout << "Enter new date (seconds from epoch) or 0 to use real date: ";
                cin >> simulatedDate;
                currentDate = getCurrentDate();
                cout << "Date updated to: " << currentDate << "\n";
                continue;
            }

            switch (choice) {
                case 1: // View Available Books
                    if (!dynamic_cast<Librarian*>(currentUser)) {
                        library.displayBooks();
                    }
                    break;
                case 2: // Borrow Book
                    if (!dynamic_cast<Librarian*>(currentUser)) {
                        string isbn;
                        cout << "Enter ISBN to borrow: ";
                        cin >> isbn;
                        currentUser->borrowBook(isbn, currentDate);
                    }
                    break;
                case 3: // Return Book
                    if (!dynamic_cast<Librarian*>(currentUser)) {
                        string isbn;
                        cout << "Enter ISBN to return: ";
                        cin >> isbn;
                        currentUser->returnBook(isbn, currentDate);
                    }
                    break;
                case 4: // View Borrowed Books
                    if (!dynamic_cast<Librarian*>(currentUser)) {
                        cout << "\n=== Currently Borrowed Books ===\n";
                        const auto& borrowedBooks = currentUser->getAccount().getBorrowedBooks();
                        if (borrowedBooks.empty()) {
                            cout << "No books currently borrowed.\n";
                        } else {
                            for (const auto& book : borrowedBooks) {
                                auto bookIt = books.find(book.first);
                                if (bookIt != books.end()) {
                                    cout << "\nBook Details:\n";
                                    cout << "ISBN: " << book.first << "\n";
                                    cout << "Title: " << bookIt->second.getTitle() << "\n";
                                    cout << "Author: " << bookIt->second.getAuthor() << "\n";
                                    
                                    // Convert due date to human-readable format
                                    time_t dueTime = book.second;
                                    struct tm* dueTm = localtime(&dueTime);
                                    char dueStr[26];
                                    strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
                                    cout << "Due Date: " << dueStr << "\n";
                                    
                                    // Calculate if overdue
                                    int secondsOverdue = currentDate - book.second;
                                    int daysOverdue = secondsOverdue / (24 * 60 * 60);
                                    if (daysOverdue > 0) {
                                        cout << "Status: OVERDUE by " << daysOverdue << " days\n";
                                        if (!currentUser->getAccount().isFacultyMember()) {
                                            cout << "Fine Amount: " << (daysOverdue * 10.0) << " rupees\n";
                                        }
                                    } else {
                                        cout << "Status: On time\n";
                                    }
                                    cout << "------------------------\n";
                                }
                            }
                        }
                    }
                    break;
                case 5: // View Fine (Student) or View History (Faculty)
                    if (dynamic_cast<Student*>(currentUser)) {
                        // Recalculate fines before displaying
                        currentUser->getAccount().updateFines(currentDate);
                        cout << "\n=== Fine Details ===\n";
                        cout << "Current Total Fine: " << currentUser->getAccount().getTotalFine() << " rupees\n";
                        
                        // Display individual book fines
                        cout << "\nIndividual Book Fines:\n";
                        bool hasOverdueBooks = false;
                        for (const auto& book : currentUser->getAccount().getBorrowedBooks()) {
                            // Calculate overdue days
                            int secondsOverdue = currentDate - book.second;
                            int daysOverdue = secondsOverdue / (24 * 60 * 60);
                            
                            auto bookIt = books.find(book.first);
                            if (bookIt != books.end()) {
                                cout << "\nBook: " << bookIt->second.getTitle() << "\n";
                                cout << "ISBN: " << book.first << "\n";
                                
                                // Convert due date to human-readable format
                                time_t dueTime = book.second;
                                struct tm* dueTm = localtime(&dueTime);
                                char dueStr[26];
                                strftime(dueStr, sizeof(dueStr), "%Y-%m-%d %H:%M:%S", dueTm);
                                cout << "Due Date: " << dueStr << "\n";
                                
                                if (daysOverdue > 0) {
                                    hasOverdueBooks = true;
                                    double bookFine = daysOverdue * 10.0; // 10 rupees per day
                                    cout << "Days Overdue: " << daysOverdue << "\n";
                                    cout << "Fine Rate: 10 rupees per day\n";
                                    cout << "Fine Amount: " << bookFine << " rupees\n";
                                } else {
                                    cout << "Status: On time (No fine)\n";
                                }
                                cout << "------------------------\n";
                            }
                        }
                        
                        if (!hasOverdueBooks && !currentUser->getAccount().getBorrowedBooks().empty()) {
                            cout << "\nNo overdue books. No fines to pay.\n";
                        } else if (currentUser->getAccount().getBorrowedBooks().empty()) {
                            cout << "\nNo books currently borrowed.\n";
                        }
                    } else if (dynamic_cast<Faculty*>(currentUser)) {
                        cout << "\n=== Borrowing History ===\n";
                        const auto& history = currentUser->getAccount().getBorrowingHistory();
                        if (history.empty()) {
                            cout << "No borrowing history.\n";
                        } else {
                            for (const auto& item : history) {
                                auto bookIt = books.find(item.first);
                                if (bookIt != books.end()) {
                                    cout << "\nBook Details:\n";
                                    cout << "ISBN: " << item.first << "\n";
                                    cout << "Title: " << bookIt->second.getTitle() << "\n";
                                    cout << "Author: " << bookIt->second.getAuthor() << "\n";
                                    
                                    // Convert return date to human-readable format
                                    time_t returnTime = item.second;
                                    struct tm* returnTm = localtime(&returnTime);
                                    char returnStr[26];
                                    strftime(returnStr, sizeof(returnStr), "%Y-%m-%d %H:%M:%S", returnTm);
                                    cout << "Return Date: " << returnStr << "\n";
                                    cout << "------------------------\n";
                                }
                            }
                        }
                    }
                    break;
                case 6: // Pay Fine (Student) or View Account Details (Faculty)
                    if (dynamic_cast<Student*>(currentUser)) {
                        double amount;
                        cout << "\n=== Pay Fine ===\n";
                        cout << "Current fine amount: " << currentUser->getAccount().getTotalFine() << " rupees\n";
                        cout << "Enter amount to pay: ";
                        cin >> amount;
                        
                        if (currentUser->getAccount().payFine(amount, currentDate)) {
                            cout << "Fine paid successfully.\n";
                        }
                    } else if (dynamic_cast<Faculty*>(currentUser)) {
                        currentUser->getAccount().displayAccountDetails(currentDate);
                    }
                    break;
                case 7: // View Account Details (Student) or Exit (Faculty)
                    if (dynamic_cast<Student*>(currentUser)) {
                        currentUser->getAccount().displayAccountDetails(currentDate);
                    }
                    break;
                case 8: // Exit (Student) or View All Books (Librarian)
                // No action needed here - the exit check is handled in the do-while condition
                break;
                case 9: // View All Users (Librarian)
                if (dynamic_cast<Librarian*>(currentUser)) {
                    library.displayUsers();
                }
                break;
                default:
                    cout << "Invalid choice!\n";
            }

            // Handle Librarian-specific menu options
            if (dynamic_cast<Librarian*>(currentUser)) {
                switch (choice) {
                    case 1: // Add User
                        {
                            string id, name, password;
                            bool isFaculty;
                            
                            cout << "\n=== Add New User ===\n";
                            
                            // Get user ID
                            cout << "Enter new user ID: ";
                            cin >> id;
                            
                            // Get name
                            cin.ignore();
                            cout << "Enter user name: ";
                            getline(cin, name);
                            
                            // Get password
                            cout << "Enter password: ";
                            getline(cin, password);
                            
                            // Get user type
                            do {
                                cout << "Is faculty? (1 for yes, 0 for no): ";
                                if (!(cin >> isFaculty)) {
                                    cout << "Invalid input. Please enter 1 or 0.\n";
                                    cin.clear();
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                }
                            } while (!cin);
                            
                            dynamic_cast<Librarian*>(currentUser)->addUser(id, name, password, isFaculty);
                        }
                        break;
                    case 2: // Remove User
                        {
                            string userId;
                            cout << "\n=== Remove User ===\n";
                            cout << "Enter user ID to remove: ";
                            if (!(cin >> userId)) {
                                cout << "Invalid input. Please enter a number.\n";
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                break;
                            }
                            dynamic_cast<Librarian*>(currentUser)->removeUser(userId);
                        }
                        break;
                    case 3: // Add Book
                        {
                            string isbn, title, author, publisher;
                            int year;
                            cin.ignore();
                            cout << "\n=== Add New Book ===\n";
                            
                            // Get and validate ISBN
                            do {
                                cout << "Enter ISBN (non-empty): ";
                                getline(cin, isbn);
                                if (isbn.empty()) {
                                    cout << "ISBN cannot be empty.\n";
                                }
                            } while (isbn.empty());
                            
                            // Get and validate title
                            do {
                                cout << "Enter title (non-empty): ";
                                getline(cin, title);
                                if (title.empty()) {
                                    cout << "Title cannot be empty.\n";
                                }
                            } while (title.empty());
                            
                            // Get and validate author
                            do {
                                cout << "Enter author (non-empty): ";
                                getline(cin, author);
                                if (author.empty()) {
                                    cout << "Author cannot be empty.\n";
                                }
                            } while (author.empty());
                            
                            // Get and validate publisher
                            do {
                                cout << "Enter publisher (non-empty): ";
                                getline(cin, publisher);
                                if (publisher.empty()) {
                                    cout << "Publisher cannot be empty.\n";
                                }
                            } while (publisher.empty());
                            
                            // Get and validate year
                            do {
                                cout << "Enter year (1900-2024): ";
                                if (!(cin >> year) || year < 1900 || year > 2024) {
                                    cout << "Invalid year. Please enter a year between 1900 and 2024.\n";
                                    cin.clear();
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                }
                            } while (year < 1900 || year > 2024);
                            
                            dynamic_cast<Librarian*>(currentUser)->addBook(isbn, title, author, publisher, year);
                        }
                        break;
                    case 4: // Remove Book
                        {
                            string isbn;
                            cout << "\n=== Remove Book ===\n";
                            cout << "Enter ISBN to remove: ";
                            cin >> isbn;
                            dynamic_cast<Librarian*>(currentUser)->removeBook(isbn);
                        }
                        break;
                    case 5: // Update Book
                        {
                            string isbn, title, author, publisher;
                            int year;
                            cin.ignore();
                            cout << "\n=== Update Book ===\n";
                            cout << "Enter ISBN to update: ";
                            getline(cin, isbn);
                            
                            if (books.find(isbn) == books.end()) {
                                cout << "Book not found!\n";
                                break;
                            }
                            
                            // Get and validate new details
                            do {
                                cout << "Enter new title (non-empty): ";
                                getline(cin, title);
                                if (title.empty()) {
                                    cout << "Title cannot be empty.\n";
                                }
                            } while (title.empty());
                            
                            do {
                                cout << "Enter new author (non-empty): ";
                                getline(cin, author);
                                if (author.empty()) {
                                    cout << "Author cannot be empty.\n";
                                }
                            } while (author.empty());
                            
                            do {
                                cout << "Enter new publisher (non-empty): ";
                                getline(cin, publisher);
                                if (publisher.empty()) {
                                    cout << "Publisher cannot be empty.\n";
                                }
                            } while (publisher.empty());
                            
                            do {
                                cout << "Enter new year (1900-2024): ";
                                if (!(cin >> year) || year < 1900 || year > 2024) {
                                    cout << "Invalid year. Please enter a year between 1900 and 2024.\n";
                                    cin.clear();
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                }
                            } while (year < 1900 || year > 2024);
                            
                            dynamic_cast<Librarian*>(currentUser)->updateBook(isbn, title, author, publisher, year);
                        }
                        break;
                    case 6: // View All Books
                        library.displayBooks();
                        break;
                    case 7: // View All Users
                        library.displayUsers();
                        break;
                    case 8: // Exit
                        break;
                    default:
                        cout << "Invalid choice!\n";
                }
            }

            // Save data after EVERY operation that modifies data
            if (choice >= 1 && choice <= 7) {
                library.saveAllData();
                cout << "\nChanges saved successfully.\n";
            }

        } while (choice != (dynamic_cast<Librarian*>(currentUser) ? 8 : 
                          dynamic_cast<Faculty*>(currentUser) ? 7 : 8));

        // Save data before user logs out
        library.saveAllData();
        cout << "\nLogging out. All data saved.\n";
    }

    return 0;
}