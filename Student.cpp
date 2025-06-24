#include "Student.h"
#include <fstream>

Student::Student() : User() {
    role = "Student";
}

Student::Student(const std::string& userId, const std::string& name, const std::string& password)
    : User(userId, name, password, "Student"), account(userId) {}

double Student::calculateFine(int daysOverdue) const {
    return daysOverdue * FINE_PER_DAY;
}

void Student::saveToFile(std::ofstream& file) const {
    User::saveToFile(file);
    account.saveToFile(file);
}

Student* Student::loadFromFile(std::ifstream& file) {
    std::string userId, name, password;
    std::getline(file, userId);
    std::getline(file, name);
    std::getline(file, password);
    
    auto* student = new Student(userId, name, password);
    student->account = Account::loadFromFile(file);
    return student;
} 