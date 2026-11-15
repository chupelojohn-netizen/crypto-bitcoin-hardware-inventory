#include "catalog.hpp"
#include "checkout_actions.hpp"
#include "input.hpp"
#include "inventory_features.hpp"
#include "structures.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace cryptid {
using namespace std;

// -------------------- Demo catalog and program entry point --------------------

vector<Product> makeSampleProducts() {
    return {
        {1001, "Ledger Nano X", "Hardware Wallet", "Ledger", 8990.00, 14},
        {1002, "Ledger Recovery Kit", "Backup Accessory", "Ledger", 1490.00, 25},
        {1003, "Trezor Safe 5", "Hardware Wallet", "Trezor", 10990.00, 11},
        {1004, "Trezor Metal Backup", "Backup Accessory", "Trezor", 2290.00, 18},
        {1005, "BitBox02 Bitcoin Edition", "Hardware Wallet", "BitBox", 7990.00, 9},
        {1006, "BitBox USB-C Adapter", "Wallet Accessory", "BitBox", 650.00, 30},
        {1007, "Coldcard Mk4", "Hardware Wallet", "Coldcard", 8990.00, 8},
        {1008, "Coldcard Backup Plate", "Backup Accessory", "Coldcard", 1750.00, 16},
        {1009, "Ledger USB-C Cable", "Wallet Accessory", "Ledger", 490.00, 40},
        {1010, "Trezor USB-C Cable", "Wallet Accessory", "Trezor", 550.00, 32},
        {1011, "BitBox Steel Backup", "Backup Accessory", "BitBox", 1990.00, 13},
        {1012, "Coldcard Power Adapter", "Wallet Accessory", "Coldcard", 890.00, 21}
    };
}

void printMainMenu() {
    cout << "\n============================================================\n"
         << " CRYPTO AND BITCOIN HARDWARE INVENTORY SYSTEM\n"
         << "============================================================\n"
         << "1. View Complete Catalog (Brand Affiliations)\n"
         << "2. Filter Products by Official Brand\n"
         << "3. Search Products (Keyword Linear Search / Product ID BST)\n"
         << "4. Filter Products by Price Range (Binary Search)\n"
         << "5. Add Item to Shopping Cart (Doubly Linked List Navigation)\n"
         << "6. Process Customer Checkout Queue (FIFO Order Queue)\n"
         << "7. Perform Admin Undo/Redo (LIFO Stacks)\n"
         << "8. Display Partner Brand Banner Showcase (Circular List Cycle)\n"
         << "9. Sort Inventory Display (Price, Stock, or Brand)\n"
         << "10. Exit\n";
}

} // namespace cryptid

int main() {
    using namespace cryptid;
    Catalog catalog;
    for (const Product& product : makeSampleProducts()) {
        catalog.addProduct(product);
    }

    const vector<string> partnerBrands = {"Ledger", "Trezor", "BitBox", "Coldcard"};
    ProductIdTree productIndex(catalog);
    ShoppingCart cart;
    Wishlist wishlist;
    CheckoutQueue checkoutQueue;
    CheckoutStack undoStack;
    CheckoutStack redoStack;
    BrandShowcase brandShowcase(partnerBrands);
    int nextOrderNumber = 5001;

    cout << "Demo partner roster features Ledger, Trezor, BitBox, and Coldcard hardware products.\n"
         << "Prices and stock are illustrative; this project does not imply brand sponsorship.\n";

    while (true) {
        printMainMenu();
        const int choice = readInt("Choose an option: ");

        if (choice == 1) {
            catalog.printAll();
        } else if (choice == 2) {
            const string brand = readLine("Enter a brand name: ");
            vector<Product> matches;
            for (const Product& product : catalog.snapshot()) {
                if (lowerCopy(product.brand) == lowerCopy(brand)) {
                    matches.push_back(product);
                }
            }
            catalog.printProducts(matches, "Brand filter results");
        } else if (choice == 3) {
            searchMenu(catalog, productIndex);
        } else if (choice == 4) {
            filterByPriceRange(catalog);
        } else if (choice == 5) {
            manageCart(catalog, cart, wishlist, checkoutQueue, nextOrderNumber);
        } else if (choice == 6) {
            checkoutQueue.print();
            processNextCheckout(catalog, checkoutQueue, undoStack, redoStack);
        } else if (choice == 7) {
            manageInventoryHistory(catalog, undoStack, redoStack);
        } else if (choice == 8) {
            brandShowcase.displayCycles(2);
        } else if (choice == 9) {
            sortInventory(catalog);
        } else if (choice == 10 || cin.eof()) {
            cout << "Goodbye.\n";
            break;
        } else {
            cout << "Choose a number from 1 to 10.\n";
        }
    }

    return 0;
}
