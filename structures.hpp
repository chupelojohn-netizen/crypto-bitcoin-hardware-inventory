#pragma once

#include "catalog.hpp"
#include "input.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace cryptid {
using namespace std;

// -------------------- Lab 2: doubly linked shopping cart --------------------

class ShoppingCart {
private:
    struct Node {
        CartLine line;
        Node* previous;
        Node* next;

        Node(int productId, int quantity)
            : line{productId, quantity}, previous(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;

public:
    ShoppingCart() : head_(nullptr), tail_(nullptr) {}

    ~ShoppingCart() {
        clear();
    }

    ShoppingCart(const ShoppingCart&) = delete;
    ShoppingCart& operator=(const ShoppingCart&) = delete;

    bool empty() const {
        return head_ == nullptr;
    }

    int quantityFor(int productId) const {
        for (Node* current = head_; current != nullptr; current = current->next) {
            if (current->line.productId == productId) {
                return current->line.quantity;
            }
        }
        return 0;
    }

    void addOrUpdate(int productId, int quantity) {
        for (Node* current = head_; current != nullptr; current = current->next) {
            if (current->line.productId == productId) {
                current->line.quantity += quantity;
                return;
            }
        }

        Node* node = new Node(productId, quantity);
        node->previous = tail_;
        if (tail_ != nullptr) {
            tail_->next = node;
        } else {
            head_ = node;
        }
        tail_ = node;
    }

    vector<CartLine> lines() const {
        vector<CartLine> result;
        for (Node* current = head_; current != nullptr; current = current->next) {
            result.push_back(current->line);
        }
        return result;
    }

    void clear() {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
    }

    void print(const Catalog& catalog) const {
        cout << "\nShopping cart\n";
        if (empty()) {
            cout << "The cart is empty.\n";
            return;
        }

        double total = 0.0;
        for (Node* current = head_; current != nullptr; current = current->next) {
            const Product* product = catalog.findById(current->line.productId);
            if (product == nullptr) {
                continue;
            }
            const double lineTotal = product->price * current->line.quantity;
            total += lineTotal;
            cout << "ID " << product->id << " | " << product->name
                 << " | quantity " << current->line.quantity
                 << " | line total PHP " << fixed << setprecision(2) << lineTotal << "\n";
        }
        cout << "Cart total: PHP " << fixed << setprecision(2) << total << "\n";
    }

    void navigate(const Catalog& catalog) const {
        if (empty()) {
            cout << "The cart is empty.\n";
            return;
        }

        Node* current = head_;
        cout << "\nCart navigation: n = next, p = previous, q = quit.\n";
        while (current != nullptr) {
            const Product* product = catalog.findById(current->line.productId);
            if (product != nullptr) {
                cout << "\n[" << product->id << "] " << product->name
                     << " | " << current->line.quantity << " item(s)"
                     << " | PHP " << fixed << setprecision(2) << product->price << " each\n";
            }

            const char action = readChar("Navigation: ");
            if (action == 'q') {
                break;
            }
            if (action == 'n') {
                if (current->next == nullptr) {
                    cout << "You are at the last cart item.\n";
                } else {
                    current = current->next;
                }
            } else if (action == 'p') {
                if (current->previous == nullptr) {
                    cout << "You are at the first cart item.\n";
                } else {
                    current = current->previous;
                }
            } else {
                cout << "Choose n, p, or q.\n";
            }
        }
    }
};

// Lab 2: singly linked wishlist.
class Wishlist {
private:
    struct Node {
        int productId;
        Node* next;

        explicit Node(int id) : productId(id), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;

public:
    Wishlist() : head_(nullptr), tail_(nullptr) {}

    ~Wishlist() {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    Wishlist(const Wishlist&) = delete;
    Wishlist& operator=(const Wishlist&) = delete;

    bool add(int productId) {
        for (Node* current = head_; current != nullptr; current = current->next) {
            if (current->productId == productId) {
                return false;
            }
        }

        Node* node = new Node(productId);
        if (tail_ == nullptr) {
            head_ = node;
        } else {
            tail_->next = node;
        }
        tail_ = node;
        return true;
    }

    bool empty() const {
        return head_ == nullptr;
    }

    void navigate(const Catalog& catalog) const {
        if (empty()) {
            cout << "The wishlist is empty.\n";
            return;
        }

        Node* current = head_;
        cout << "\nWishlist navigation: n = next, q = quit.\n";
        while (current != nullptr) {
            const Product* product = catalog.findById(current->productId);
            if (product != nullptr) {
                cout << "\n[" << product->id << "] " << product->name
                     << " | " << product->brand
                     << " | PHP " << fixed << setprecision(2) << product->price << "\n";
            }

            const char action = readChar("Navigation: ");
            if (action == 'q') {
                break;
            }
            if (action == 'n') {
                if (current->next == nullptr) {
                    cout << "You are at the last wishlist item.\n";
                } else {
                    current = current->next;
                }
            } else {
                cout << "Choose n or q.\n";
            }
        }
    }

    void print(const Catalog& catalog) const {
        cout << "\nWishlist\n";
        if (empty()) {
            cout << "The wishlist is empty.\n";
            return;
        }

        for (Node* current = head_; current != nullptr; current = current->next) {
            const Product* product = catalog.findById(current->productId);
            if (product != nullptr) {
                cout << "ID " << product->id << " | " << product->name
                     << " | " << product->brand << "\n";
            }
        }
    }
};

// -------------------- Lab 5: FIFO checkout queue --------------------

class CheckoutQueue {
private:
    struct Node {
        CheckoutOrder order;
        Node* next;

        explicit Node(const CheckoutOrder& value) : order(value), next(nullptr) {}
    };

    Node* front_;
    Node* rear_;
    size_t size_;

public:
    CheckoutQueue() : front_(nullptr), rear_(nullptr), size_(0) {}

    ~CheckoutQueue() {
        CheckoutOrder ignored{};
        while (dequeue(ignored)) {
        }
    }

    CheckoutQueue(const CheckoutQueue&) = delete;
    CheckoutQueue& operator=(const CheckoutQueue&) = delete;

    bool empty() const {
        return front_ == nullptr;
    }

    void enqueue(const CheckoutOrder& order) {
        Node* node = new Node(order);
        if (rear_ == nullptr) {
            front_ = node;
        } else {
            rear_->next = node;
        }
        rear_ = node;
        ++size_;
    }

    bool dequeue(CheckoutOrder& order) {
        if (front_ == nullptr) {
            return false;
        }

        Node* node = front_;
        order = move(node->order);
        front_ = node->next;
        if (front_ == nullptr) {
            rear_ = nullptr;
        }
        delete node;
        --size_;
        return true;
    }

    void print() const {
        cout << "\nCheckout queue (FIFO), " << size_ << " pending order(s)\n";
        if (empty()) {
            cout << "No orders are waiting.\n";
            return;
        }

        for (Node* current = front_; current != nullptr; current = current->next) {
            cout << "Order #" << current->order.orderNumber
                 << " | " << current->order.lines.size() << " cart line(s)\n";
        }
    }
};

// -------------------- Lab 4: LIFO checkout history stacks --------------------

// The program creates one CheckoutStack for Undo and one for Redo.
class CheckoutStack {
private:
    struct Node {
        CheckoutOrder order;
        Node* next;

        Node(const CheckoutOrder& value, Node* nextNode) : order(value), next(nextNode) {}
    };

    Node* top_;

public:
    CheckoutStack() : top_(nullptr) {}

    ~CheckoutStack() {
        CheckoutOrder ignored{};
        while (pop(ignored)) {
        }
    }

    CheckoutStack(const CheckoutStack&) = delete;
    CheckoutStack& operator=(const CheckoutStack&) = delete;

    bool empty() const {
        return top_ == nullptr;
    }

    void push(const CheckoutOrder& order) {
        top_ = new Node(order, top_);
    }

    bool pop(CheckoutOrder& order) {
        if (top_ == nullptr) {
            return false;
        }

        Node* node = top_;
        order = move(node->order);
        top_ = node->next;
        delete node;
        return true;
    }

    void clear() {
        CheckoutOrder ignored{};
        while (pop(ignored)) {
        }
    }
};

// -------------------- Lab 3: circular partner-brand list --------------------

class BrandShowcase {
private:
    struct Node {
        string brand;
        Node* next;

        explicit Node(const string& value) : brand(value), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;

public:
    explicit BrandShowcase(const vector<string>& brands) : head_(nullptr), tail_(nullptr) {
        for (const string& brand : brands) {
            Node* node = new Node(brand);
            if (head_ == nullptr) {
                head_ = node;
            } else {
                tail_->next = node;
            }
            tail_ = node;
        }
        if (tail_ != nullptr) {
            tail_->next = head_;
        }
    }

    ~BrandShowcase() {
        if (head_ == nullptr) {
            return;
        }
        tail_->next = nullptr;
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    BrandShowcase(const BrandShowcase&) = delete;
    BrandShowcase& operator=(const BrandShowcase&) = delete;

    void displayCycles(size_t cycles) const {
        if (head_ == nullptr) {
            cout << "No partner brands are configured.\n";
            return;
        }

        cout << "\nPartner brand banner showcase\n";
        Node* current = head_;
        const size_t brandCount = countNodes();
        const size_t total = cycles * brandCount;
        for (size_t i = 0; i < total; ++i) {
            cout << "  >> " << current->brand << " <<\n";
            current = current->next;
        }
        cout << "Circular list advanced through " << total << " banner(s).\n";
    }

private:
    size_t countNodes() const {
        if (head_ == nullptr) {
            return 0;
        }
        size_t count = 1;
        for (Node* current = head_->next; current != head_; current = current->next) {
            ++count;
        }
        return count;
    }
};

} // namespace cryptid
