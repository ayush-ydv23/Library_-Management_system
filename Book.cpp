#include "Book.h"
#include <fstream>

Book::Book() : year(0), status("Available") {}

Book::Book(const std::string& title, const std::string& author, const std::string& publisher,
           int year, const std::string& isbn)
    : title(title), author(author), publisher(publisher), year(year), isbn(isbn), status("Available") {}

// Getters
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
std::string Book::getPublisher() const { return publisher; }
int Book::getYear() const { return year; }
std::string Book::getISBN() const { return isbn; }
std::string Book::getStatus() const { return status; }
std::string Book::getBorrowerId() const { return borrowerId; }

// Setters
void Book::setTitle(const std::string& title) { this->title = title; }
void Book::setAuthor(const std::string& author) { this->author = author; }
void Book::setPublisher(const std::string& publisher) { this->publisher = publisher; }
void Book::setYear(int year) { this->year = year; }
void Book::setISBN(const std::string& isbn) { this->isbn = isbn; }
void Book::setStatus(const std::string& status) { this->status = status; }
void Book::setBorrowerId(const std::string& id) { this->borrowerId = id; }

// Other methods
bool Book::isAvailable() const {
    return status == "Available";
}

void Book::markAsBorrowed(const std::string& borrowerId) {
    this->status = "Borrowed";
    this->borrowerId = borrowerId;
}

void Book::markAsReturned() {
    this->status = "Available";
    this->borrowerId = "";
}

// File operations
void Book::saveToFile(std::ofstream& file) const {
    file << title << '\n'
         << author << '\n'
         << publisher << '\n'
         << year << '\n'
         << isbn << '\n'
         << status << '\n'
         << borrowerId << '\n';
}

Book Book::loadFromFile(std::ifstream& file) {
    Book book;
    std::getline(file, book.title);
    std::getline(file, book.author);
    std::getline(file, book.publisher);
    file >> book.year;
    file.ignore();
    std::getline(file, book.isbn);
    std::getline(file, book.status);
    std::getline(file, book.borrowerId);
    return book;
} 