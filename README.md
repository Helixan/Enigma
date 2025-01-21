# Enigma Password Manager

## Overview
Enigma is a Qt-based desktop application designed for managing passwords, generating secure passwords, and taking encrypted notes. It offers an intuitive graphical user interface (GUI) with features to ensure your sensitive information is stored and managed securely.

---

## Features

### Password Manager
- Store and manage passwords securely.
- Features include service names, URLs, usernames, email, passwords, and TOTP secrets.
- Copy individual password fields to clipboard with one click.

### Password Generator
- Generate strong and customizable passwords.
- Options to include/exclude uppercase, lowercase, numbers, symbols, and custom characters.
- Visual password strength indicator.

### Notepad
- Securely store and manage notes.
- Encrypted notes with a simple interface for adding, editing, and deleting.

### Authentication
- User authentication with hashed passwords (SHA256).
- Register and login functionality.

---

## Installation

### Prerequisites
- **Qt Framework** (>= 5.15): Install using your package manager or download from the [Qt website](https://www.qt.io/download).
- **OpenSSL**: Ensure OpenSSL is installed for encryption.
- **MySQL Server**: Used as the database backend.

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo/enigma.git
   cd enigma
   ```

2. Create a MySQL database and set up the required tables:
   ```sql
   CREATE DATABASE enigma_db;

   USE enigma_db;

   CREATE TABLE users (
       id INT AUTO_INCREMENT PRIMARY KEY,
       username VARCHAR(255) UNIQUE NOT NULL,
       password VARCHAR(255) NOT NULL
   );

   CREATE TABLE passwords (
       id INT AUTO_INCREMENT PRIMARY KEY,
       user_id INT NOT NULL,
       encrypted_service BLOB NOT NULL,
       encrypted_url BLOB,
       encrypted_username BLOB,
       encrypted_email BLOB,
       encrypted_password BLOB NOT NULL,
       encrypted_description BLOB,
       encrypted_totp_secret BLOB,
       FOREIGN KEY (user_id) REFERENCES users(id)
   );

   CREATE TABLE notes (
       id INT AUTO_INCREMENT PRIMARY KEY,
       user_id INT NOT NULL,
       encrypted_title BLOB NOT NULL,
       encrypted_content BLOB NOT NULL,
       FOREIGN KEY (user_id) REFERENCES users(id)
   );
   ```

3. Build the project using CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

4. Run the application:
   ```bash
   ./Enigma
   ```

---

## Configuration

Update the database connection settings in `main.cpp`:
```cpp
QString host = "localhost";
QString dbName = "enigma_db";
QString dbUser = "your_username";
QString dbPassword = "your_password";
```

---

## Technologies Used
- **Programming Language**: C++ (C++20)
- **GUI Framework**: Qt5 (Widgets & SQL)
- **Encryption**: AES-256-CBC with OpenSSL
- **Database**: MySQL
- **Libraries**:
    - OpenSSL
    - Qt5

---

## Security Features
- **Encryption**: Sensitive data is encrypted using AES-256 before storage.
- **TOTP Integration**: Generate secure codes for two-factor authentication.
- **Hashed Passwords**: User credentials are hashed with SHA256.

---

## Usage

1. Launch the application.
2. Login or register as a new user.
3. Use the sidebar to navigate:
    - **Password Manager**: Add, edit, or delete passwords.
    - **Password Generator**: Create strong passwords based on your criteria.
    - **Notepad**: Store encrypted notes.
4. Logout to end the session securely.

---

## License
This project is licensed under the Attribution-NonCommercial-ShareAlike 4.0 International. See [LICENSE](LICENSE) for more details.
