#include "Faculty.h"
#include <fstream>
#include <ctime>

Faculty::Faculty() : User() {
    role = "Faculty";
}

Faculty::Faculty(const std::string& userId, const std::string& name, const std::string& password)
    : User(userId, name, password, "Faculty"), account(userId) {}

bool Faculty::canBorrowMore() const {
    if (account.getNumberOfBorrowedBooks() >= MAX_BOOKS) {
        return false;
    }

    // Check for books overdue more than 60 days
    time_t now = std::time(nullptr);
    const auto& borrows = account.getCurrentBorrows();
    for (const auto& record : borrows) {
        time_t daysOverdue = (now - record.dueDate) / (24 * 60 * 60);
        if (daysOverdue > MAX_OVERDUE_DAYS) {
            return false;
        }
    }
    return true;
}

void Faculty::saveToFile(std::ofstream& file) const {
    User::saveToFile(file);
    account.saveToFile(file);
}

Faculty* Faculty::loadFromFile(std::ifstream& file) {
    std::string userId, name, password;
    std::getline(file, userId);
    std::getline(file, name);
    std::getline(file, password);
    
    auto* faculty = new Faculty(userId, name, password);
    faculty->account = Account::loadFromFile(file);
    return faculty;
} 