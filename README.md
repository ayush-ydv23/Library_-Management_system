# Library Management System
This repository contains a comprehensive C++ based Library Management System that implements Object-Oriented Programming concepts for managing library operations. This is a submission for the C++ Assignment.

## Contents of the ZIP File
The submission zip file contains the following:
1. **Source Code**
   - `library_system.cpp` - Main C++ source code file

2. **Data Files**
   - `accounts.txt` - Stores user account information and borrowing records
   - `books.txt` - Contains book inventory and status information
   - `users.txt` - Maintains user credentials and access levels

3. **Documentation**
   - `README.md` - This file, containing system documentation

4. **Executable**
   - `lms.exe` - Pre-compiled executable for Windows systems
   - Note: If the executable doesn't work on your system, please compile the source code using the instructions below

## Setup Instructions
1. Extract the zip file to your desired location
2. Open terminal/command prompt in the extracted directory
3. Compile the program using:
   ```bash
   g++ library_system.cpp -o library_systemexe
   ```

   library_systemexe is name of executable file(if this name does work try some other names)
4. Run the program:
   ```bash
   library_systemexe
   ```
   




LOOKS LIKE THIS IN TERMINAL




   
C:\Users\ssayu>cd Downloads

C:\Users\ssayu\Downloads> cd c++ Project

C:\Users\ssayu\Downloads\c++ project> g++ library_system.cpp -o lms1

C:\Users\ssayu\Downloads\c++ project>lms1
Starting Library Management System...

Loading previous session data...
Loading all data...
Loading 10 accounts...
Accounts loaded successfully.
Loading 10 books...
Books loaded successfully.
Loading 10 users...
Users loaded successfully.
All data loaded successfully.
Saving all data...
All data saved successfully.
System initialized and data saved.

=== Library Management System ===
Enter your user ID:






## Running the System

### Initial Setup
When you first run the system, it will:
1. Initialize the database
2. Load previous session data (if any)
3. Create necessary data files (accounts.txt, books.txt, users.txt)
4. Display the login prompt

### Login Credentials
The system comes with pre-configured users:

1. **Librarian Access**
   ```
   ID: 100
   Password: admin123
   ```

2. **Faculty Access**
   ```
   IDs: 101, 102, 103
   Password: faculty123
   ```

3. **Student Access**
   ```
   IDs: 201, 202, 203, 204, 205
   Password: student123
   ```

### Sample Operations

1. **Student Operations**
   ```
   Login as Student (e.g., ID: 201, Password: student123)
   → View Available Books (Option 1)
   → Borrow a Book (Option 2)
   → Check Due Date
   → View Account Details (Option 7)
   → Check Fines (Option 5)
   ```

2. **Faculty Operations**
   ```
   Login as Faculty (e.g., ID: 101, Password: faculty123)
   → Borrow up to 5 books (Option 2)
   → 30-day borrowing period
   → No fines system
   → View Borrowing History (Option 5)
   ```

3. **Librarian Operations**
   ```
   Login as Librarian (ID: 100, Password: admin123)
   → Add New Users (Option 1)
   → Add New Books (Option 3)
   → View All Books (Option 6)
   → View All Users (Option 7)
   ```

### Example Session
Here's a typical session for a student:
1. Start the program:
   ```bash
   ./lms
   ```
2. Login with student credentials:
   ```
   Enter your user ID: 201
   Enter your password: student123
   ```
3. Student Menu appears with options:
   ```
   1. View Available Books
   2. Borrow a Book (Max 3 books, 15 days)
   3. Return a Book
   4. View Borrowed Books
   5. View Fine (Current: 0 rupees)
   6. Pay Fine
   7. View Account Details
   8. Exit
   ```
4. Select options by entering the corresponding number
5. Follow the prompts for each operation
6. System automatically saves all changes

### Important Notes for Running
- All changes are saved automatically after each operation
- The system shows current status and confirmation messages
- Fine calculations are done in real-time
- Students can't borrow new books if they have unpaid fines
- Faculty members can borrow more books for longer periods
- The system prevents invalid operations (e.g., borrowing unavailable books)

## Project Structure

### Data Files
The system maintains three data files for persistence:
- `accounts.txt`: Stores user account information, borrowing records, and fine details
- `books.txt`: Contains book inventory and status information
- `users.txt`: Maintains user credentials and access levels

### Classes and Components
- `User` (Base Class):
  - Attributes: UserID, Name, Password
  - Virtual functions for borrowing and returning books
  
- `Student` (Derived from User):
  - Max 3 books for 15 days
  - Fine calculation enabled
  - Functions for borrowing, returning, and fine payment
  
- `Faculty` (Derived from User):
  - Max 5 books for 30 days
  - No fines but 60-day warning
  - Extended borrowing privileges
  
- `Librarian` (Derived from User):
  - Administrative privileges
  - Book and user management functions
  - System monitoring capabilities

- `Book`:
  - Attributes: ISBN, Title, Author, Publisher, Year
  - Status tracking (Available/Borrowed)
  - Reservation management

- `Account`:
  - Borrowing history
  - Fine calculations
  - Due date management

## Initial Data and Users

### Default Users
1. **Librarian**
   - ID: 100
   - Password: admin123
   - Full administrative access

2. **Faculty Members**
   - IDs: 101, 102, 103
   - Password: faculty123
   - 5 books, 30-day limit

3. **Students**
   - IDs: 201-205
   - Password: student123
   - 3 books, 15-day limit

### Fine System

#### Fine Calculation
- **Rate**: 10 rupees per day for overdue books
- **When is a book overdue?**
  - Students: After 15 days from borrowing date
  - Faculty: After 30 days (no fines charged)

#### Fine Payment Process
1. **When returning an overdue book**:
   - System calculates overdue days
   - Shows fine amount (10 rupees × overdue days)
   - Payment must be exact amount
   - Book can't be returned without payment

2. **Paying accumulated fines**:
   - View total fines (Option 5 in Student menu)
   - Pay exact amount shown
   - It will be like a reissue that if user wants  to keep the book but pays the fine then books gets reissued on that that date 
   - All borrowed books get new due dates(this is == date of finepayment + (15 or 30 resp for diff users))
   - Borrowing history updated

#### Fine Restrictions
- Students cannot borrow with unpaid fines
- Faculty exempt from fines but get 60-day warnings
- No partial payments accepted
- Real-time fine calculations

## Assumptions and Design

1. **Borrowing Limits**:
   - Students: Maximum 3 books for 15 days
   - Faculty: Maximum 5 books for 30 days
   - Librarians cannot borrow books

2. **Fine System**:
   - Only applies to students
   - Fixed rate of 10 rupees per day
   - Must be paid in full
   - Calculated in real-time

3. **Data Persistence**:
   - All changes saved automatically
   - Files updated after each operation
   - Session data maintained

4. **Security**:
   - Role-based access control
   - Separate menus for each user type
   - Password protection

## Error Handling
- Invalid login attempts
- Maximum book limit exceeded
- Overdue book restrictions
- Invalid fine payment amounts
- File I/O errors

## Important Notes
- The system automatically saves all changes
- Fines are calculated in real-time
- Faculty members can borrow books for longer periods
- Students must clear all fines before borrowing new books
- The system maintains a complete borrowing history
- Date and time are used for fine calculations

## Support
For any issues or questions, please contact the system administrator. 