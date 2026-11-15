#pragma once

#include <string>
#include <vector>

namespace cryptid {
using namespace std;

// One row in the catalog. These are the fields required by the assignment.
struct Product {
    int id;
    string name;
    string category;
    string brand;
    double price;
    int stockQuantity;
};

struct CartLine {
    int productId;
    int quantity;
};

struct CheckoutOrder {
    int orderNumber;
    vector<CartLine> lines;
};

} // namespace cryptid
