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
#include "pos.h"


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
            break;
        case 2:
            loading("Loading...");
            search(products);
            break;
        case 3:
            loading("Loading...");
            changePassword(accounts);
            break;
        case 4:
            loading("Logging out...");
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
            break;
        case 4:
            loading("Loading...");
            sellProduct(products);
            break;
        case 5:
            loading("Loading...");
            analyzeSales();
            break;
        case 6:
            loading("Loading...");
            updateProductPrice(products);
            break;
        case 7:
            loading("Loading...");
            updateProductQuantity(products);
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
            displayMenu();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
    }
}

int main() {

    std::vector<User*> accounts;
    // Read existing account information from file
    readAccountsFromFile(accounts);
    // Read existing product information from file
    std::vector<Product> products = readProductsFromFile("products.txt");

    displayMenu();

    return 0;
}