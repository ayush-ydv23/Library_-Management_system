#include <iostream>
#include <memory>
#include <string>
#include "Library.h"
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

// Forward declarations
class Account;
class Book;
class User;

// Global variables
map<int, Account> accounts;
vector<Book> books;

class Account {
    // ... existing Account class code ...
};

class User {
protected:
    int id;
    string name;
    Account& account;  // Changed to reference

public:
    User(int id, string name) : id(id), name(name), account(accounts[id]) {
        // If account doesn't exist in the map, create it
        if (accounts.find(id) == accounts.end()) {
            accounts[id] = Account(id);
        }
    }

    Account& getAccount() { return account; }
    const Account& getAccount() const { return account; }
    int getID() const { return id; }
    string getName() const { return name; }
};

class Student : public User {
public:
    Student(string name, int id) : User(id, name) {
        // Initialize student-specific account settings
        if (accounts.find(id) == accounts.end()) {
            accounts[id] = Account(id, false);  // false for student
        }
    }

    void borrowBook(string isbn, int dueDate) {
        if (books[stoi(isbn)].isAvailable() && !books[stoi(isbn)].isReserved() && 
            account.getBorrowedBooks().size() < account.getMaxBooks() && 
            account.getTotalFine() == 0) {
            account.getBorrowedBooks()[isbn] = dueDate;
            books[stoi(isbn)].setAvailability(false);
        } else {
            cout << "Request denied" << endl;
        }
    }

    void payFine(double amount) { 
        account.addFine(-amount); 
    }

    double getFine() const { 
        return account.getTotalFine(); 
    }

    void markFine() { 
        account.payFee(); 
    }

    void returnBook(string isbn) {
        if (account.getBorrowedBooks().find(isbn) != account.getBorrowedBooks().end()) {
            account.getBorrowedBooks().erase(isbn);
            books[stoi(isbn)].setAvailability(true);
        }
    }
};

class Faculty : public User {
public:
    Faculty(string name, int id) : User(id, name) {
        // Initialize faculty-specific account settings
        if (accounts.find(id) == accounts.end()) {
            accounts[id] = Account(id, true);  // true for faculty
        }
    }

    void borrowBook(string isbn, int dueDate) {
        if (books[stoi(isbn)].isAvailable() && !books[stoi(isbn)].isReserved() && 
            account.getBorrowedBooks().size() < account.getMaxBooks()) {
            account.getBorrowedBooks()[isbn] = dueDate;
            books[stoi(isbn)].setAvailability(false);
        } else {
            cout << "Request denied" << endl;
        }
    }

    void returnBook(string isbn) {
        if (account.getBorrowedBooks().find(isbn) != account.getBorrowedBooks().end()) {
            account.getBorrowedBooks().erase(isbn);
            books[stoi(isbn)].setAvailability(true);
        }
    }
};

void displayMainMenu() {
    std::cout << "\n=== Library Management System ===\n"
              << "1. Login\n"
              << "2. Exit\n"
              << "Enter your choice: ";
}

void displayUserMenu(const User* user) {
    std::cout << "\n=== User Menu ===\n";
    if (user->getRole() == "Student" || user->getRole() == "Faculty") {
        std::cout << "1. Search Books\n"
                  << "2. Borrow Book\n"
                  << "3. Return Book\n"
                  << "4. View Borrowed Books\n"
                  << "5. View Fines\n"
                  << "6. Pay Fines\n";
    }
    if (user->getRole() == "Librarian") {
        std::cout << "1. Add Book\n"
                  << "2. Remove Book\n"
                  << "3. Add User\n"
                  << "4. Remove User\n"
                  << "5. View All Books\n"
                  << "6. View All Users\n";
    }
    std::cout << "7. Logout\n"
              << "Enter your choice: ";
}

void initializeLibrary(Library& library) {
    // Add initial books
    library.addBook(std::make_unique<Book>("Introduction to C++", "Bjarne Stroustrup", "Addison-Wesley", 2013, "978-0321714114"));
    library.addBook(std::make_unique<Book>("Design Patterns", "Erich Gamma", "Addison-Wesley", 1994, "978-0201633610"));
    library.addBook(std::make_unique<Book>("Clean Code", "Robert Martin", "Prentice Hall", 2008, "978-0132350884"));
    library.addBook(std::make_unique<Book>("The Art of Computer Programming", "Donald Knuth", "Addison-Wesley", 1968, "978-0201038019"));
    library.addBook(std::make_unique<Book>("Introduction to Algorithms", "Thomas Cormen", "MIT Press", 2009, "978-0262033848"));
    library.addBook(std::make_unique<Book>("Database System Concepts", "Abraham Silberschatz", "McGraw-Hill", 2010, "978-0073523323"));
    library.addBook(std::make_unique<Book>("Operating System Concepts", "Abraham Silberschatz", "Wiley", 2012, "978-1118063330"));
    library.addBook(std::make_unique<Book>("Computer Networks", "Andrew Tanenbaum", "Pearson", 2010, "978-0132126953"));
    library.addBook(std::make_unique<Book>("Artificial Intelligence", "Stuart Russell", "Pearson", 2009, "978-0136042594"));
    library.addBook(std::make_unique<Book>("Software Engineering", "Ian Sommerville", "Pearson", 2010, "978-0137035151"));

    // Add users
    // Students
    library.addUser(std::make_unique<Student>("S001", "John Doe", "pass123"));
    library.addUser(std::make_unique<Student>("S002", "Jane Smith", "pass123"));
    library.addUser(std::make_unique<Student>("S003", "Bob Wilson", "pass123"));
    library.addUser(std::make_unique<Student>("S004", "Alice Brown", "pass123"));
    library.addUser(std::make_unique<Student>("S005", "Charlie Davis", "pass123"));

    // Faculty
    library.addUser(std::make_unique<Faculty>("F001", "Dr. James Wilson", "pass123"));
    library.addUser(std::make_unique<Faculty>("F002", "Prof. Sarah Johnson", "pass123"));
    library.addUser(std::make_unique<Faculty>("F003", "Dr. Michael Brown", "pass123"));

    // Librarian
    library.addUser(std::make_unique<Librarian>("L001", "Admin", "admin123"));
}

int main() {
    Library library;
    initializeLibrary(library);

    while (true) {
        if (!library.getCurrentUser()) {
            displayMainMenu();
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                std::string userId, password;
                std::cout << "Enter User ID: ";
                std::cin >> userId;
                std::cout << "Enter Password: ";
                std::cin >> password;

                if (!library.login(userId, password)) {
                    std::cout << "Invalid credentials!\n";
                    continue;
                }
            } else if (choice == 2) {
                break;
            }
        } else {
            const User* currentUser = library.getCurrentUser();
            displayUserMenu(currentUser);
            
            int choice;
            std::cin >> choice;

            if (choice == 7) {
                library.logout();
                continue;
            }

            // Handle user menu choices based on role
            if (currentUser->getRole() == "Student" || currentUser->getRole() == "Faculty") {
                switch (choice) {
                    case 1: {
                        std::string query;
                        std::cout << "Enter search query: ";
                        std::cin.ignore();
                        std::getline(std::cin, query);
                        auto books = library.searchBooks(query);
                        for (const auto* book : books) {
                            std::cout << book->getTitle() << " by " << book->getAuthor() << " (ISBN: " << book->getISBN() << ")\n";
                        }
                        break;
                    }
                    case 2: {
                        std::string isbn;
                        std::cout << "Enter book ISBN: ";
                        std::cin >> isbn;
                        if (library.borrowBook(isbn)) {
                            std::cout << "Book borrowed successfully!\n";
                        } else {
                            std::cout << "Failed to borrow book.\n";
                        }
                        break;
                    }
                    case 3: {
                        std::string isbn;
                        std::cout << "Enter book ISBN: ";
                        std::cin >> isbn;
                        if (library.returnBook(isbn)) {
                            std::cout << "Book returned successfully!\n";
                        } else {
                            std::cout << "Failed to return book.\n";
                        }
                        break;
                    }
                    case 4:
                        library.displayBorrowedBooks(currentUser->getUserId());
                        break;
                    case 5: {
                        double fine = library.calculateFine(currentUser->getUserId());
                        std::cout << "Outstanding fine: Rs. " << fine << std::endl;
                        break;
                    }
                    case 6:
                        if (library.payFine(currentUser->getUserId())) {
                            std::cout << "Fine paid successfully!\n";
                        } else {
                            std::cout << "No fine to pay.\n";
                        }
                        break;
                }
            } else if (currentUser->getRole() == "Librarian") {
                switch (choice) {
                    case 1: {
                        std::string title, author, publisher, isbn;
                        int year;
                        std::cout << "Enter book details:\n";
                        std::cout << "Title: ";
                        std::cin.ignore();
                        std::getline(std::cin, title);
                        std::cout << "Author: ";
                        std::getline(std::cin, author);
                        std::cout << "Publisher: ";
                        std::getline(std::cin, publisher);
                        std::cout << "Year: ";
                        std::cin >> year;
                        std::cout << "ISBN: ";
                        std::cin >> isbn;
                        
                        auto book = std::make_unique<Book>(title, author, publisher, year, isbn);
                        if (library.addBook(std::move(book))) {
                            std::cout << "Book added successfully!\n";
                        } else {
                            std::cout << "Failed to add book.\n";
                        }
                        break;
                    }
                    case 2: {
                        std::string isbn;
                        std::cout << "Enter book ISBN to remove: ";
                        std::cin >> isbn;
                        if (library.removeBook(isbn)) {
                            std::cout << "Book removed successfully!\n";
                        } else {
                            std::cout << "Failed to remove book.\n";
                        }
                        break;
                    }
                    case 3: {
                        std::string userId, name, password, role;
                        std::cout << "Enter user details:\n";
                        std::cout << "User ID: ";
                        std::cin >> userId;
                        std::cout << "Name: ";
                        std::cin.ignore();
                        std::getline(std::cin, name);
                        std::cout << "Password: ";
                        std::cin >> password;
                        std::cout << "Role (Student/Faculty): ";
                        std::cin >> role;

                        std::unique_ptr<User> newUser;
                        if (role == "Student") {
                            newUser = std::make_unique<Student>(name, stoi(userId));
                        } else if (role == "Faculty") {
                            newUser = std::make_unique<Faculty>(name, stoi(userId));
                        }

                        if (newUser && library.addUser(std::move(newUser))) {
                            std::cout << "User added successfully!\n";
                        } else {
                            std::cout << "Failed to add user.\n";
                        }
                        break;
                    }
                    case 4: {
                        std::string userId;
                        std::cout << "Enter user ID to remove: ";
                        std::cin >> userId;
                        if (library.removeUser(stoi(userId))) {
                            std::cout << "User removed successfully!\n";
                        } else {
                            std::cout << "Failed to remove user.\n";
                        }
                        break;
                    }
                    case 5:
                        library.displayBooks();
                        break;
                    case 6:
                        library.displayUsers();
                        break;
                }
            }
        }
    }

    return 0;
} 