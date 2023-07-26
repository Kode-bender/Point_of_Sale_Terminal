#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <string>
#include <chrono>
#include <map>
#include <thread>
#include <unordered_map>
#include <random>

const std::string shopAddress = "Arries Cosmetics Ltd \n P.O. Box 23 \nNairobi\n";
std::string loggedInUser;
std::string role;

class Product {
public:
    std::string id;
    std::string name;
    int quantity;
    double price;
    std::string type;

    std::string getId() const { return id; }

    // Function to retrieve product information
    std::string getProductInfo() const {
        return id + "," + name + "," + type;
    }

public:
    Product(std::string id, const std::string& name, int quantity, double price, const std::string& type)
        : id(id), name(name), quantity(quantity), price(price), type(type) {}

};

// Base class - User
class User {
protected:
    std::string username;
    std::string password;

public:
    User(const std::string& username, const std::string& password) : username(username), password(password) {}

    std::string getUsername() const {
        return username;
    }

    std::string getPassword() const {
        return password;
    }

    void setUsername(const std::string& newUsername) {
        username = newUsername;
    }

    void setPassword(const std::string& newPassword) {
        password = newPassword;
    }

    virtual ~User() {}
};

// Derived class - Admin
class Admin : public User {
public:
    Admin(const std::string& username, const std::string& password) : User(username, password) {}

    void displayRole() const {
        std::cout << "Role: Admin" << std::endl;
    }
};

// Derived class - Employee
class Employee : public User {
public:
    Employee(const std::string& username, const std::string& password) : User(username, password) {}

    void displayRole() const {
        std::cout << "Role: Employee" << std::endl;
    }
};

void adminMenu(std::vector<User*>& accounts);
void employeeMenu(std::vector<User*>& accounts);
bool login(std::vector<User*>& accounts);
void createAccount(std::vector<User*>& accounts);
void changePassword(std::vector<User*>& accounts);
void displayProducts(const std::vector<Product>& products);
void addProduct(std::vector<Product>& vector);

//Function to validate and get an integer input from the user
int getIntInput() {
    int input;
    while (true) {
        if (std::cin >> input) {
            // Check if there are any trailing characters
            if (std::cin.peek() == '\n') {
                std::cin.ignore();  // Consume the newline character
                break;  // Valid input, break out of the loop
            }
        }

        // Clear the input stream and ignore any remaining characters
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid input. Please enter a number: ";
    }
    return input;
}

// Function to validate and get a double input from the user
double getDoubleInput() {
    double input;
    while (true) {
        if (std::cin >> input) {
            // Check if there are any trailing characters
            if (std::cin.peek() == '\n') {
                break;  // Valid input, break out of the loop
            }
        }

        // Clear the input stream and ignore any remaining characters
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid input. Please enter a number: ";
    }
    return input;
}

void printLine(char lineChar = '*', int lineLength = 30) {
    std::cout << std::string(lineLength, lineChar) << std::endl;
}

void printHeader(const std::string& headerText) {
    printLine();
    std::cout << "  " << headerText << std::endl;
    printLine();
}

// Function to validate and get a string input from the user
std::string getStringInput() {
    std::string input;
    while (true) {
        if (getline(std::cin, input)) {
            break;  // Valid input, break out of the loop
        }

        // Clear the input stream
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Invalid input. Please enter a string: ";
    }
    return input;
}

void clearScreen(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
#ifdef _WIN32
    system("cls"); // for Windows
#else
    system("clear"); // for Linux and macOS
#endif

}

bool isValidPassword(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }

    bool hasSpecialCharacter = false;
    for (char c : password) {
        if (!isalnum(c)) {
            hasSpecialCharacter = true;
            break;
        }
    }

    return hasSpecialCharacter;
}


// Function to analyze sales data
void analyzeSales() {
    clearScreen(1);
    std::ifstream salesFile("sales.txt");

    if (!salesFile) {
        std::cout << "Sales file not found." << std::endl;
        return;
    }

    std::string line;
    int totalSales = 0;
    double totalRevenue = 0.0;
    int totalQuantity = 0;
    double highestPrice = 0.0;
    std::string highestPriceProduct;
    double totalAmountSold = 0.0;
    std::map<std::string, int> productQuantityMap;

    while (getline(salesFile, line)) {
        std::istringstream iss(line);
        std::string productName;
        int quantity;
        double subtotal;

        if (iss >> productName >> quantity >> subtotal) {
            totalSales++;
            totalRevenue += subtotal;
            totalQuantity += quantity;
            totalAmountSold += subtotal;

            if (subtotal > highestPrice) {
                highestPrice = subtotal;
                highestPriceProduct = productName;
            }

            // Update product quantity sold
            if (productQuantityMap.find(productName) == productQuantityMap.end()) {
                productQuantityMap[productName] = quantity;
            }
            else {
                productQuantityMap[productName] += quantity;
            }
        }
    }

    salesFile.close();

    std::cout << "Sales Analysis" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Total Sales: " << totalSales << std::endl;
    std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
    std::cout << "Total Quantity Sold: " << totalQuantity << std::endl;
    std::cout << "Product with Highest Price: " << highestPriceProduct << " ($" << std::fixed << std::setprecision(2) << highestPrice << ")" << std::endl;
    std::cout << "Total Amount Sold So Far: $" << std::fixed << std::setprecision(2) << totalAmountSold << std::endl;

    std::cout << std::endl << "Top 5 Most Sold Products:" << std::endl;
    std::cout << "-----------------------------" << std::endl;

    // Sort products based on quantity sold
    std::vector<std::pair<std::string, int>> productQuantityVec(productQuantityMap.begin(), productQuantityMap.end());
    std::sort(productQuantityVec.begin(), productQuantityVec.end(), [](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs) {
        return lhs.second > rhs.second;
        });


    // Display top 5 most sold products
    int count = 0;
    for (const auto& product : productQuantityVec) {
        std::cout << product.first << ": " << product.second << " units" << std::endl;
        count++;
        if (count == 5) {
            break;
        }
    }

    // Ask if the user wants to continue analyzing sales data
    std::cout << "Press any key to exit: ";
    std::string continueAnalysis = getStringInput();
    if (continueAnalysis != "y" || continueAnalysis != "Y") {
        return;
    }
}


// Function to create an account
void createAccount(std::vector<User*>& accounts) {
    clearScreen(1);
    std::string username, password, role;

    bool quit = false;
    while (!quit) {
        while (true) {
            std::cout << "Enter username: ";
            username = getStringInput();
            if (username == "q") {
                quit = true;
                break;
            }

            // Check if the username already exists
            bool usernameExists = false;
            for (const auto& account : accounts) {
                if (account->getUsername() == username) {
                    std::cout << "Username already exists. Please choose another username." << std::endl;
                    usernameExists = true;
                    break;
                }
            }

            if (!usernameExists) {
                break;
            }
        }

        if (quit) {
            break;
        }

        std::cout << "Enter password: ";
        password = getStringInput();

        // Validate the password
        if (!isValidPassword(password)) {
            std::cout << "Invalid password. Password must have a minimum length of 8 characters and contain at least one special character." << std::endl;
            continue;
        }

        while (true) {
            std::cout << "Enter role (admin/employee): ";
            role = getStringInput();

            if (role == "admin" || role == "employee") {
                break;
            }
            else {
                std::cout << "Invalid role. Please enter 'admin' or 'employee'." << std::endl;
            }
        }

        if (role == "admin") {
            Admin* admin = new Admin(username, password);
            accounts.push_back(admin);
            std::cout << "Admin account created successfully." << std::endl;
            std::cout << "Username: " << admin->getUsername() << ", Password: " << password << std::endl;
            admin->displayRole();
        }
        else if (role == "employee") {
            Employee* employee = new Employee(username, password);
            accounts.push_back(employee);
            std::cout << "Employee account created successfully." << std::endl;
            std::cout << "Username: " << employee->getUsername() << ", Password: " << password << std::endl;
            employee->displayRole();

        }

        // Write account information to a file
        std::ofstream outputFile("accounts.txt", std::ios::app);
        if (outputFile.is_open()) {
            outputFile << username << "," << password << "," << role << std::endl;
            outputFile.close();
        }
        else {
            std::cout << "Failed to write account information to file." << std::endl;
        }

        // Ask if the user wants to create another account
        std::cout << "Do you want to create another account? (y/n): ";
        std::string createAnother = getStringInput();
        if (createAnother != "y") {
            break;
        }
    }
}

// Function to read account information from a file
void readAccountsFromFile(std::vector<User*>& accounts) {
    std::ifstream inputFile("accounts.txt");
    if (inputFile.is_open()) {
        std::string line;
        while (getline(inputFile, line)) {
            std::string username, password, role;
            size_t pos = line.find(',');
            if (pos != std::string::npos) {
                username = line.substr(0, pos);
                line.erase(0, pos + 1);

                pos = line.find(',');
                if (pos != std::string::npos) {
                    password = line.substr(0, pos);
                    role = line.substr(pos + 1);

                    if (role == "admin") {
                        Admin* admin = new Admin(username, password);
                        accounts.push_back(admin);
                    }
                    else if (role == "employee") {
                        Employee* employee = new Employee(username, password);
                        accounts.push_back(employee);
                    }
                }
            }
        }
        inputFile.close();
    }
    else {
        std::cout << "Failed to read account information from file." << std::endl;
    }
}

// Function to save products to a file
void saveProductsToFile(const std::string& filename, std::vector<Product> products) {
    std::ofstream file(filename);
    if (file) {
        for (const auto& product : products) {
            file << product.id << "        " << product.name << "          " << product.quantity << "         " << product.price
                << "       " << product.type << std::endl;
        }
        file.close();
    }
    else {
        std::cout << "Error opening file: " << filename << std::endl;
    }
}

// Function to read products from a file
std::vector<Product> readProductsFromFile(const std::string& filename) {
    std::vector<Product> products;
    std::ifstream file(filename);
    if (file) {
        int quantity;
        double price;
        std::string id, name, type;
        while (file >> id >> name >> quantity >> price >> type) {
            Product product(id, name, quantity, price, type);
            products.push_back(product);
        }
        file.close();
    }
    else {
        std::cout << "Error opening file: " << filename << std::endl;
    }
    return products;
}


std::string generateUniqueID() {
    std::string ID;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < 4; ++i) {
        char randomDigit = '0' + std::rand() % 10;
        ID += randomDigit;
    }
    return "#" + ID;
}

// Function to change password and username
void changePassword(std::vector<User*>& accounts) {
    clearScreen(1);
    std::string password;
    std::string username;

    std::cout << "Enter username: ";
    username = getStringInput();

    for (auto& account : accounts) {
        if (account->getUsername() == username) {
            std::cout << "Enter current password: ";
            password = getStringInput();

            // Authenticate user by comparing the entered password with the stored password
            if (password == account->getPassword()) {
                std::string newPassword;

                account->setUsername(username);

                std::cout << "Enter new password: ";
                newPassword = getStringInput();
                account->setPassword(newPassword);

                std::cout << "Password and username updated successfully." << std::endl;

                // Update account information in the file
                std::ofstream outputFile("accounts.txt");
                if (outputFile.is_open()) {
                    for (const auto& acc : accounts) {
                        outputFile << acc->getUsername() << "," << acc->getPassword() << ",";
                        if (dynamic_cast<Admin*>(acc)) {
                            outputFile << "admin";
                        }
                        else if (dynamic_cast<Employee*>(acc)) {
                            outputFile << "employee";
                        }
                        outputFile << std::endl;
                    }
                    outputFile.close();
                }
                else {
                    std::cout << "Failed to update account information in file." << std::endl;
                }

                return;
            }
            else {
                std::cout << "Incorrect password. Authentication failed." << std::endl;
                return;
            }
        }
    }

    std::cout << "Account with username '" << username << "' not found." << std::endl;
}

void loading(const std::string& text) {
    for (int i = 0; i <= 5000; i++) {
        std::cout << "\r" << text << i / 50;
    }
}

bool login(std::vector<User*>& accounts) {
    std::string username;
    std::string password;
    const int MAX_ATTEMPTS = 3;
    const int LOCKOUT_DURATION = 10;

    static int failedAttempts = 0;
    static bool lockedOut = false;

    while (failedAttempts < MAX_ATTEMPTS) {
        clearScreen(1);
        std::cout << "Enter your username: ";
        username = getStringInput();
        std::cout << "Enter your password: ";
        password = getStringInput();

        if (lockedOut) {
            std::cout << "Account is currently locked out. Please try again later." << std::endl;
            return false;
        }

        std::ifstream inputFile("accounts.txt");
        if (inputFile.is_open()) {
            std::string line;
            while (getline(inputFile, line)) {
                std::string storedUsername, storedPassword, role;
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    storedUsername = line.substr(0, pos);
                    line.erase(0, pos + 1);

                    pos = line.find(',');
                    if (pos != std::string::npos) {
                        storedPassword = line.substr(0, pos);
                        role = line.substr(pos + 1);

                        if (username == storedUsername && password == storedPassword) {
                            inputFile.close();
                            failedAttempts = 0;
                            // Store the logged-in user's username in the global variable
                            loggedInUser = username;

                            if (role == "admin") {
                                std::cout << "Login successful. Logged in as Admin:" << loggedInUser << std::endl;
                                adminMenu(accounts);
                            }
                            else if (role == "employee") {
                                std::cout << "Login successful. Logged in as Employee:" << loggedInUser << std::endl;
                                employeeMenu(accounts);
                            }
                            return true;
                        }
                    }
                }
            }
            inputFile.close();
        }

        failedAttempts++;
        std::cout << "Login failed. Attempts remaining: " << MAX_ATTEMPTS - failedAttempts << std::endl;

        if (failedAttempts >= MAX_ATTEMPTS) {
            std::cout << "Too many unsuccessful login attempts. Account locked. Please try again after " << LOCKOUT_DURATION << " seconds." << std::endl;
            lockedOut = true;
            std::this_thread::sleep_for(std::chrono::seconds(LOCKOUT_DURATION));
            lockedOut = false;
            failedAttempts = 0;
        }
    }

    return false;
}

void printReceipt() {
    clearScreen(1);

    std::ifstream receiptsFile("receipt.txt");

    if (!receiptsFile) {
        std::cout << "Sales file not found." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(receiptsFile, line)) {
        std::cout << line << std::endl;
    }

    receiptsFile.close();
}

void display(const std::vector<Product>& products) {
    std::cout << "________________________________________PRODUCTS_______________________________________" << std::endl;
    std::cout << "_______________________________________________________________________________________" << std::endl;
    std::cout << "ID\t\tName\t\tQuantity\tPrice\t\tType" << std::endl;
    std::cout << "_______________________________________________________________________________________" << std::endl;
    for (const auto& product : products) {
        std::cout << product.id << "\t\t" << product.name << "\t\t" << product.quantity << "\t\t" << product.price << "\t\t" << product.type << std::endl;
    }
    std::cout << "_______________________________________________________________________________________" << std::endl;


}

// Function to sell a product
void sellProduct(std::vector<Product>& products) {
    clearScreen(1);
    double total = 0;
    double subtotal = 0;
    double discount = 0;
    double cash = 0;
    double balance = 0;
    int quantityToSell = 0;

    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }
    std::string sellAnother = "y";
    while (sellAnother == "y" || sellAnother == "Y") {
        auto it = products.end();;
        do {
            display(products);
            std::string id;
            std::cout << "Enter product ID: ";
            id = getStringInput();

            it = find_if(products.begin(), products.end(), [&](const Product& p) {
                return p.id == id;
                });

            if (it == products.end()) {
                clearScreen(0);
                std::cout << "Product not found. Please enter a ID from Products menu above." << std::endl;
            }

        } while (it == products.end());

        if (it != products.end()) {

            std::cout << "Enter quantity to sell: ";
            quantityToSell = getIntInput();

            if (quantityToSell > it->quantity) {
                std::cout << "Insufficient quantity available." << std::endl;
                return;
            }
            it->quantity -= quantityToSell;

            if (quantityToSell >= 10) {
                discount = 0.1;
            }

            total = it->price * quantityToSell;
            subtotal = total - (total * discount);

            while (true) {
                std::cout << "  Subtotal: ksh " << subtotal << std::endl;
                std::cout << "Enter Cash Amount: ksh ";
                if (!(std::cin >> cash)) {
                    std::cout << "Invalid input. Please enter a valid cash amount." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if (cash < subtotal) {
                    std::cout << "Insufficient cash amount. Please enter a valid amount." << std::endl;
                }
                else {
                    break;
                }
            }

            balance = cash - subtotal;

            // Store sales information in a file
            std::ofstream receiptsFile("receipt.txt", std::ios::app);
            if (receiptsFile) {

                // Get the current time as a seed for the random number generator
                auto currentTime = std::chrono::system_clock::now();
                std::time_t timeNow = std::chrono::system_clock::to_time_t(currentTime);
                // Format the time as HH:MM:SS
                std::stringstream timeStream;
                timeStream << std::put_time(std::localtime(&timeNow), "%H:%M:%S");
                // Format the date as DD/MM/YYYY
                std::stringstream dateStream;
                dateStream << std::put_time(std::localtime(&timeNow), "%d/%m/%Y");
                // Combine the time, date
                std::string dateTime = timeStream.str() + " " + dateStream.str();


                std::string customerName;
                std::cout << "Balance: ksh " << balance << std::endl;
                std::cout << "Enter customer name: ";
                std::cin.ignore();
                getline(std::cin, customerName);

                std::string receiptID = generateUniqueID();

                receiptsFile << "ReceiptID" << receiptID << std::endl;
                receiptsFile << shopAddress << std::endl;
                receiptsFile << dateTime << std::endl;
                receiptsFile << "Served By: " << loggedInUser << std::endl;
                receiptsFile << "Customer Name: " << customerName << std::endl;
                receiptsFile << "Product Name: " << it->name << std::endl;
                receiptsFile << "Quantity: " << quantityToSell << std::endl;
                receiptsFile << "Total Price: " << total << std::endl;
                receiptsFile << "Discount: " << discount << std::endl;
                receiptsFile << "Subtotal: " << subtotal << std::endl;
                receiptsFile << "Cash Amount : " << cash << std::endl;
                receiptsFile << "Balance: " << balance << std::endl;
                receiptsFile << std::endl;

                // Store sales information in a file
                std::ofstream salesFile("sales.txt", std::ios::app);
                if (salesFile) {
                    salesFile << it->name << " " << quantityToSell << " " << subtotal << std::endl;
                }
                else {
                    std::cout << "Failed to write sales information to file." << std::endl;
                }
                std::cout << "Product sold successfully.Receipt generated successfully" << std::endl;
                printReceipt();
            }
            else {
                std::cout << "Error opening receipts file." << std::endl;
            }
        }
        else {
            std::cout << "Product not found." << std::endl;
        }
        // Ask if the user wants to remove another product
        std::cout << "Do you want to sell another product? (y/n): ";
        sellAnother = getStringInput();
        while (sellAnother != "y" && sellAnother != "n" && sellAnother != "Y" && sellAnother != "N") {
            std::cout << "Invalid input. Please enter 'y' or 'n': ";
            sellAnother = getStringInput();
        }
    }
}

// Function to add a product
void addProduct(std::vector<Product>& products) {
    clearScreen(1);

    while (true) {
        std::string id;
        std::cout << "Enter product ID: ";
        id = getStringInput();

        auto it = find_if(products.begin(), products.end(), [&](const Product& p) {
            return p.id == id;
            });

        if (it != products.end()) {
            std::cout << "Product with the same ID already exists." << std::endl;
            return;
        }

        std::string name;
        std::cout << "Enter product name: ";
        name = getStringInput();

        int quantity;
        std::cout << "Enter quantity: ";
        quantity = getIntInput();

        double price;
        std::cout << "Enter price: ";
        price = getDoubleInput();

        std::string type;
        std::cout << "Enter product type: ";
        std::cin.ignore();
        getline(std::cin, type);

        Product newProduct(id, name, quantity, price, type);
        products.push_back(newProduct);

        std::ofstream file("products.txt", std::ios::app);  // Open the file in append mode
        if (file.is_open()) {
            file << newProduct.id << "        " << newProduct.name << "           " << newProduct.quantity << "         "
                << newProduct.price << "       " << newProduct.type << std::endl;
            file.close();
            std::cout << "Success." << std::endl;
        }
        else {
            std::cout << "Failed to write product information to file." << std::endl;
        }

        std::cout << "Product added successfully." << std::endl;

        // Ask if the user wants to add another product
        std::cout << "Do you want to add another product? (y/n): ";
        std::string addAnother = getStringInput();
        clearScreen(1);
        if (addAnother != "y" && addAnother != "Y") {
            break;
        }
    }
}

// Function to update the quantity of a product
void updateProductQuantity(std::vector<Product>& products) {
    clearScreen(1);

    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }

    std::string updateAnother = "y";
    while (updateAnother == "y" || updateAnother == "Y") {
        display(products);
        std::cout << "Enter the ID of the product to update the quantity: ";
        std::string id = getStringInput();

        auto it = find_if(products.begin(), products.end(), [&](const Product& p) {
            return p.id == id;
            });

        if (it != products.end()) {
            int newQuantity;
            std::cout << "Enter the new quantity: ";
            newQuantity = getIntInput();

            it->quantity = newQuantity;
            std::cout << "Product quantity updated successfully." << std::endl;
        }
        else {
            std::cout << "Product not found." << std::endl;
        }

        // Ask if the user wants to update the quantity of another product
        std::cout << "Do you want to update the quantity of another product? (y/n): ";
        updateAnother = getStringInput();
        while (updateAnother != "y" && updateAnother != "n" && updateAnother != "Y" && updateAnother != "N") {
            std::cout << "Invalid input. Please enter 'y' or 'n': ";
            updateAnother = getStringInput();
        }
    }
}

// Function to update the quantity of a product
void updateProductPrice(std::vector<Product>& products) {
    clearScreen(1);

    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }

    std::string updateAnother = "y";
    while (updateAnother == "y" || updateAnother == "Y") {
        display(products);
        std::cout << "Enter the ID of the product to update the price: ";
        std::string id = getStringInput();

        auto it = find_if(products.begin(), products.end(), [&](const Product& p) {
            return p.id == id;
            });

        if (it != products.end()) {
            int newPrice;
            std::cout << "Enter the new price: ";
            newPrice = getDoubleInput();

            it->price = newPrice;
            std::cout << "Product price updated successfully." << std::endl;
        }
        else {
            std::cout << "Product not found." << std::endl;
        }

        // Ask if the user wants to update the quantity of another product
        std::cout << "Do you want to update the price of another product? (y/n): ";
        updateAnother = getStringInput();
        while (updateAnother != "y" && updateAnother != "n" && updateAnother != "Y" && updateAnother != "N") {
            std::cout << "Invalid input. Please enter 'y' or 'n': ";
            updateAnother = getStringInput();
        }
    }
}

// Function to search for a product by ID
void searchProductByID(std::vector<Product>& products) {
    clearScreen(1);

    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }

    std::string id;
    std::cout << "Enter product ID: ";
    id = getStringInput();

    auto it = find_if(products.begin(), products.end(), [&](const Product& p) {
        return p.id == id;
        });


    if (it == products.end()) {
        std::cout << "Product not found" << std::endl;

    }

    if (it != products.end()) {
        std::cout << "Product found: " << std::endl;
        std::cout << "ID: " << it->id << std::endl;
        std::cout << "Name: " << it->name << std::endl;
        std::cout << "Quantity: " << it->quantity << std::endl;
        std::cout << "Price: " << it->price << std::endl;
        std::cout << "Type: " << it->type << std::endl;
        std::string choice;
        std::cout << "Product not found. Do you want to sell it? (y/n): ";
        choice = getStringInput();

        if (choice == "y" || choice == "Y") {
            sellProduct(const_cast<std::vector<Product> &>(products)); // Pass the 'products' vector as an argument
        }
    }
}

// Function to search for a product by name
void searchProductByName(std::vector<Product>& products) {
    clearScreen(1);
    std::string name;
    std::cout << "Enter product name: ";
    name = getStringInput();

    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }

    auto it = find_if(products.begin(), products.end(), [&](const Product& p) {
        return p.name == name;
        });

    if (it == products.end()) {
        std::cout << "Product not found" << std::endl;

    }

    if (it != products.end()) {
        std::cout << "Product found: " << std::endl;
        std::cout << "ID: " << it->id << std::endl;
        std::cout << "Name: " << it->name << std::endl;
        std::cout << "Quantity: " << it->quantity << std::endl;
        std::cout << "Price: " << it->price << std::endl;
        std::cout << "Type: " << it->type << std::endl;
        std::string choice;
        std::cout << "Product not found. Do you want to sell it? (y/n): ";
        choice = getStringInput();

        if (choice == "y" || choice == "Y") {
            sellProduct(const_cast<std::vector<Product>&>(products)); // Pass the 'products' vector as an argument
        }
    }
}

// Function to display the menu
void displayMenu() {

    std::vector<Product> products = readProductsFromFile("products.txt");
    std::vector<User*> accounts;
    // Read existing account information from file
    readAccountsFromFile(accounts);

    clearScreen(1);

    while (true) {
        printHeader("Welcome to Arries Ltd ");
        std::cout << "     1.  Login" << std::endl;
        std::cout << "     2.  Exit" << std::endl;
        printLine();

        int choice;
        std::cout << "Enter your choice: ";
        choice = getIntInput();
        switch (choice) {
        case 1:
            loading("Loading...");
            login(accounts);
        case 2:
            loading("Exiting...");
            saveProductsToFile("products.txt", products);
            exit(0);
        }
        clearScreen(1);
    }
}

void displayProducts(const std::vector<Product>& products) {

    clearScreen(1);
    if (products.empty()) {
        std::cout << "No products available." << std::endl;
        return;
    }
    std::cout << "________________________________________PRODUCTS_______________________________________" << std::endl;
    std::cout << "_______________________________________________________________________________________" << std::endl;
    std::cout << "ID\t\tName\t\tQuantity\tPrice\t\tType" << std::endl;
    std::cout << "_______________________________________________________________________________________" << std::endl;
    for (const auto& product : products) {
        std::cout << product.id << "\t\t" << product.name << "\t\t" << product.quantity << "\t\t" << product.price << "\t\t" << product.type << std::endl;
    }
    std::cout << "_______________________________________________________________________________________" << std::endl;


    std::cout << "Press any key to exit: ";
    std::string continueViewing = getStringInput();
    if (continueViewing != "y" || continueViewing != "Y") {
        return;
    }
}



void search(std::vector<Product>& products) {

    while (true) {
        clearScreen(1);
        printLine();
        std::cout << "   1.    Search by ID" << std::endl;
        std::cout << "   2.    Search by Name" << std::endl;
        std::cout << "   3.    Exit " << std::endl;
        printLine();

        int choice;
        std::cout << "Enter your choice: ";
        choice = getIntInput();

        switch (choice) {
        case 1:
            loading("Loading...");
            searchProductByID(products);
            break;
        case 2:
            loading("Loading...");
            searchProductByName(products);
            break;
        case 3:
            loading("Exiting...");
            return;
        }
    }
}

void employeeMenu(std::vector<User*>& accounts) {

    std::vector<Product> products = readProductsFromFile("products.txt");

    while (true) {
        clearScreen(1);
        printHeader("       Menu");
        printLine();
        std::cout << "     1. Sell Product" << std::endl;
        std::cout << "     2. Search Product" << std::endl;
        std::cout << "     3. Change Password" << std::endl;
        std::cout << "     4. Logout" << std::endl;
        printLine();

        int choice1;
        std::cout << "Enter your choice: ";
        choice1 = getIntInput();
        //cin >> choice;

        switch (choice1) {
        case 1:
            loading("Loading...");
            sellProduct(products);
            saveProductsToFile("products.txt", products);
            break;
        case 2:
            loading("Loading...");
            search(products);
            saveProductsToFile("products.txt", products);
            break;
        case 3:
            loading("Loading...");
            changePassword(accounts);
            break;
        case 4:
            loading("Logging out...");
            saveProductsToFile("products.txt", products);
            displayMenu();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
    }
}

void adminMenu(std::vector<User*>& accounts) {

    std::vector<Product> products = readProductsFromFile("products.txt");

    while (true) {
        clearScreen(1);
        printHeader("       Menu");
        std::cout << "     1.  Add Product" << std::endl;
        std::cout << "     2.  View Products" << std::endl;
        std::cout << "     3.  Search Product" << std::endl;
        std::cout << "     4.  Sell Product" << std::endl;
        std::cout << "     5.  Analyze sales" << std::endl;
        std::cout << "     6.  Update Product Prices" << std::endl;
        std::cout << "     7.  Update Product Quantity" << std::endl;
        std::cout << "     8.  Change password" << std::endl;
        std::cout << "     9.  Create user account" << std::endl;
        std::cout << "     10. Logout" << std::endl;
        printLine();

        int choice1;
        std::cout << "Enter your choice: ";
        choice1 = getIntInput();

        switch (choice1) {
        case 1:
            loading("Loading...");
            addProduct(products);
            break;
        case 2:
            loading("Loading...");
            displayProducts(products);
            break;
        case 3:
            loading("Loading...");
            search(products);
            saveProductsToFile("products.txt", products);
            break;
        case 4:
            loading("Loading...");
            sellProduct(products);
            saveProductsToFile("products.txt", products);
            break;
        case 5:
            loading("Loading...");
            analyzeSales();
            break;
        case 6:
            loading("Loading...");
            updateProductPrice(products);
            saveProductsToFile("products.txt", products);
            break;
        case 7:
            loading("Loading...");
            updateProductQuantity(products);
            saveProductsToFile("products.txt", products);
            break;
        case 8:
            loading("Loading...");
            changePassword(accounts);
            break;
        case 9:
            loading("Loading...");
            createAccount(accounts);
            break;
        case 10:
            loading("Logging out...");
            saveProductsToFile("products.txt", products);
            displayMenu();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
    }
}

int main() {

    std::cout << "This Source code include two file -point of sale.cpp and pos.h- and are dependent to each other" << std::endl;
    std::cout << "Link to my repository: https://github.com/Kode-bender/Point_of_Sale_Terminal/tree/ed97614fb42cbd954198b81b505975ac0330726a" << std::endl;

    std::vector<User*> accounts;
    // Read existing account information from file
    readAccountsFromFile(accounts);
    // Read existing product information from file
    std::vector<Product> products = readProductsFromFile("products.txt");

    displayMenu();

    return 0;
}