#include "Account.h"
#include <ctime>
#include <fstream>

Account::Account() : outstandingFine(0.0) {}

Account::Account(const std::string& userId) : userId(userId), outstandingFine(0.0) {}

std::string Account::getUserId() const {
    return userId;
}

const std::vector<BorrowRecord>& Account::getCurrentBorrows() const {
    return currentBorrows;
}

const std::vector<BorrowRecord>& Account::getBorrowHistory() const {
    return borrowHistory;
}

double Account::getOutstandingFine() const {
    return outstandingFine;
}

bool Account::borrowBook(const std::string& bookId, int maxDays) {
    BorrowRecord record;
    record.bookId = bookId;
    record.borrowDate = std::time(nullptr);
    record.dueDate = record.borrowDate + (maxDays * 24 * 60 * 60); // Convert days to seconds
    record.returned = false;
    
    currentBorrows.push_back(record);
    return true;
}

bool Account::returnBook(const std::string& bookId) {
    for (auto it = currentBorrows.begin(); it != currentBorrows.end(); ++it) {
        if (it->bookId == bookId) {
            it->returned = true;
            it->returnDate = std::time(nullptr);
            borrowHistory.push_back(*it);
            currentBorrows.erase(it);
            return true;
        }
    }
    return false;
}

void Account::addFine(double amount) {
    outstandingFine += amount;
}

void Account::payFine() {
    outstandingFine = 0.0;
}

bool Account::hasFine() const {
    return outstandingFine > 0.0;
}

int Account::getNumberOfBorrowedBooks() const {
    return currentBorrows.size();
}

bool Account::hasOverdueBooks() const {
    time_t now = std::time(nullptr);
    for (const auto& record : currentBorrows) {
        if (now > record.dueDate) {
            return true;
        }
    }
    return false;
}

std::vector<BorrowRecord> Account::getOverdueBooks() const {
    std::vector<BorrowRecord> overdueBooks;
    time_t now = std::time(nullptr);
    
    for (const auto& record : currentBorrows) {
        if (now > record.dueDate) {
            overdueBooks.push_back(record);
        }
    }
    
    return overdueBooks;
}

void Account::saveToFile(std::ofstream& file) const {
    file << userId << '\n';
    file << outstandingFine << '\n';
    
    // Save current borrows
    file << currentBorrows.size() << '\n';
    for (const auto& record : currentBorrows) {
        file << record.bookId << '\n'
             << record.borrowDate << '\n'
             << record.dueDate << '\n'
             << record.returned << '\n';
    }
    
    // Save borrow history
    file << borrowHistory.size() << '\n';
    for (const auto& record : borrowHistory) {
        file << record.bookId << '\n'
             << record.borrowDate << '\n'
             << record.dueDate << '\n'
             << record.returned << '\n'
             << record.returnDate << '\n';
    }
}

Account Account::loadFromFile(std::ifstream& file) {
    Account account;
    std::getline(file, account.userId);
    file >> account.outstandingFine;
    file.ignore();
    
    // Load current borrows
    size_t currentBorrowsSize;
    file >> currentBorrowsSize;
    file.ignore();
    
    for (size_t i = 0; i < currentBorrowsSize; ++i) {
        BorrowRecord record;
        std::getline(file, record.bookId);
        file >> record.borrowDate;
        file >> record.dueDate;
        file >> record.returned;
        file.ignore();
        account.currentBorrows.push_back(record);
    }
    
    // Load borrow history
    size_t historySize;
    file >> historySize;
    file.ignore();
    
    for (size_t i = 0; i < historySize; ++i) {
        BorrowRecord record;
        std::getline(file, record.bookId);
        file >> record.borrowDate;
        file >> record.dueDate;
        file >> record.returned;
        file >> record.returnDate;
        file.ignore();
        account.borrowHistory.push_back(record);
    }
    
    return account;
} 