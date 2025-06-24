#include "Librarian.h"
#include <fstream>

Librarian::Librarian() : User() {
    role = "Librarian";
}

Librarian::Librarian(const std::string& userId, const std::string& name, const std::string& password)
    : User(userId, name, password, "Librarian") {}

bool Librarian::addBook(Book* book) {
    // Implementation will be handled by the Library class
    return true;
}

bool Librarian::removeBook(const std::string& isbn) {
    // Implementation will be handled by the Library class
    return true;
}

bool Librarian::updateBook(const std::string& isbn, const Book& updatedBook) {
    // Implementation will be handled by the Library class
    return true;
}

bool Librarian::addUser(User* user) {
    // Implementation will be handled by the Library class
    return true;
}

bool Librarian::removeUser(const std::string& userId) {
    // Implementation will be handled by the Library class
    return true;
}

bool Librarian::updateUser(const std::string& userId, User* updatedUser) {
    // Implementation will be handled by the Library class
    return true;
}

void Librarian::saveToFile(std::ofstream& file) const {
    User::saveToFile(file);
}

Librarian* Librarian::loadFromFile(std::ifstream& file) {
    std::string userId, name, password;
    std::getline(file, userId);
    std::getline(file, name);
    std::getline(file, password);
    
    return new Librarian(userId, name, password);
} 