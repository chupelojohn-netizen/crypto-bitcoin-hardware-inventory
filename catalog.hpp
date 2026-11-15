#pragma once

#include "models.hpp"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace cryptid {
using namespace std;

// -------------------- Lab 1: dynamically resized catalog --------------------

class Catalog {
private:
    Product* items_;
    size_t size_;
    size_t capacity_;

    void grow() {
        // Double the array, copy its products, then release the old storage.
        const size_t newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
        Product* expanded = new Product[newCapacity];
        for (size_t i = 0; i < size_; ++i) {
            expanded[i] = items_[i];
        }
        delete[] items_;
        items_ = expanded;
        capacity_ = newCapacity;
    }

public:
    Catalog() : items_(new Product[4]), size_(0), capacity_(4) {}

    ~Catalog() {
        delete[] items_;
    }

    Catalog(const Catalog&) = delete;
    Catalog& operator=(const Catalog&) = delete;

    bool addProduct(const Product& product) {
        if (findById(product.id) != nullptr) {
            return false;
        }
        if (size_ == capacity_) {
            grow();
        }
        items_[size_++] = product;
        return true;
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    Product* findById(int id) {
        for (size_t i = 0; i < size_; ++i) {
            if (items_[i].id == id) {
                return &items_[i];
            }
        }
        return nullptr;
    }

    const Product* findById(int id) const {
        for (size_t i = 0; i < size_; ++i) {
            if (items_[i].id == id) {
                return &items_[i];
            }
        }
        return nullptr;
    }

    vector<Product> snapshot() const {
        return vector<Product>(items_, items_ + size_);
    }

    void printProducts(const vector<Product>& products, const string& heading) const {
        cout << "\n" << heading << "\n";
        if (products.empty()) {
            cout << "No products to display.\n";
            return;
        }

        cout << left << setw(7) << "ID"
             << setw(31) << "Product"
             << setw(21) << "Category"
             << setw(13) << "Brand"
             << right << setw(13) << "Price PHP"
             << setw(9) << "Stock" << "\n";
        cout << string(94, '-') << "\n";

        for (const Product& product : products) {
            cout << left << setw(7) << product.id
                 << setw(31) << product.name
                 << setw(21) << product.category
                 << setw(13) << product.brand
                 << right << setw(13) << fixed << setprecision(2) << product.price
                 << setw(9) << product.stockQuantity << "\n";
        }
        cout << left;
    }

    void printAll() const {
        cout << "\nCatalog contains " << size_ << " products in a dynamic array with capacity "
             << capacity_ << ".\n";
        printProducts(snapshot(), "Complete product catalog");
    }
};

// -------------------- Lab 6: balanced product ID search tree --------------------

class ProductIdTree {
private:
    struct Node {
        int id;
        Node* left;
        Node* right;

        explicit Node(int value) : id(value), left(nullptr), right(nullptr) {}
    };

    Node* root_;

    static Node* buildBalanced(const vector<int>& sortedIds, size_t first, size_t last) {
        if (first >= last) {
            return nullptr;
        }

        // Put the middle ID at the root so the starting tree stays balanced.
        const size_t middle = first + (last - first) / 2;
        Node* node = new Node(sortedIds[middle]);
        try {
            node->left = buildBalanced(sortedIds, first, middle);
            node->right = buildBalanced(sortedIds, middle + 1, last);
        } catch (...) {
            destroy(node);
            throw;
        }
        return node;
    }

    static bool containsNode(const Node* node, int id) {
        if (node == nullptr) {
            return false;
        }
        if (id == node->id) {
            return true;
        }
        return id < node->id ? containsNode(node->left, id) : containsNode(node->right, id);
    }

    static void destroy(Node* node) {
        if (node == nullptr) {
            return;
        }
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    explicit ProductIdTree(const Catalog& catalog) : root_(nullptr) {
        vector<int> ids;
        for (const Product& product : catalog.snapshot()) {
            ids.push_back(product.id);
        }
        sort(ids.begin(), ids.end());
        // The catalog is fixed after startup, so this index is built once.
        root_ = buildBalanced(ids, 0, ids.size());
    }

    ~ProductIdTree() {
        destroy(root_);
    }

    ProductIdTree(const ProductIdTree&) = delete;
    ProductIdTree& operator=(const ProductIdTree&) = delete;

    bool contains(int id) const {
        return containsNode(root_, id);
    }
};

} // namespace cryptid
