#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

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

// -------------------- Input helpers --------------------

string lowerCopy(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return value;
}

int readInt(const string& prompt) {
    int value = 0;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) {
            return 0;
        }
        cout << "Please enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(const string& prompt) {
    double value = 0.0;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) {
            return 0.0;
        }
        cout << "Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    if (!getline(cin, value)) {
        return "";
    }
    return value;
}

char readChar(const string& prompt) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty()) {
            if (cin.eof()) {
                return 'q';
            }
            continue;
        }
        return static_cast<char>(tolower(static_cast<unsigned char>(value[0])));
    }
}

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

// -------------------- Lab 8: inventory sorting algorithms --------------------

template <typename Comparator>
void insertionSort(vector<Product>& products, Comparator comesBefore) {
    // Keep the left side sorted and insert each next product into its place.
    for (ptrdiff_t i = 1; i < static_cast<ptrdiff_t>(products.size()); ++i) {
        Product key = products[static_cast<size_t>(i)];
        ptrdiff_t j = i - 1;
        while (j >= 0 && comesBefore(key, products[static_cast<size_t>(j)])) {
            products[static_cast<size_t>(j + 1)] = products[static_cast<size_t>(j)];
            --j;
        }
        products[static_cast<size_t>(j + 1)] = key;
    }
}

template <typename Comparator>
int partitionProducts(vector<Product>& products, int low, int high, Comparator comesBefore) {
    // Use the last product as the pivot for this quicksort partition.
    const Product pivot = products[static_cast<size_t>(high)];
    int boundary = low - 1;
    for (int current = low; current < high; ++current) {
        // Products that belong before the pivot are moved to its left side.
        if (comesBefore(products[static_cast<size_t>(current)], pivot)) {
            ++boundary;
            swap(products[static_cast<size_t>(boundary)], products[static_cast<size_t>(current)]);
        }
    }
    swap(products[static_cast<size_t>(boundary + 1)], products[static_cast<size_t>(high)]);
    return boundary + 1;
}

template <typename Comparator>
void quickSort(vector<Product>& products, int low, int high, Comparator comesBefore) {
    // A range with zero or one product is already sorted.
    if (low >= high) {
        return;
    }
    const int pivotIndex = partitionProducts(products, low, high, comesBefore);
    quickSort(products, low, pivotIndex - 1, comesBefore);
    quickSort(products, pivotIndex + 1, high, comesBefore);
}

// -------------------- Lab 7: search and filter operations --------------------

void filterByKeyword(const Catalog& catalog) {
    const string keyword = lowerCopy(readLine("Enter a product, category, or brand keyword: "));
    if (keyword.empty()) {
        cout << "Enter a keyword to search.\n";
        return;
    }

    // Check each product's name, category, and brand for the keyword.
    vector<Product> matches;
    for (const Product& product : catalog.snapshot()) {
        if (lowerCopy(product.name).find(keyword) != string::npos ||
            lowerCopy(product.category).find(keyword) != string::npos ||
            lowerCopy(product.brand).find(keyword) != string::npos) {
            matches.push_back(product);
        }
    }

    catalog.printProducts(matches, "Linear keyword search results");
}

void searchByProductId(const Catalog& catalog, const ProductIdTree& index) {
    const int id = readInt("Enter product ID (0 to cancel): ");
    if (id == 0) {
        return;
    }

    if (!index.contains(id)) {
        cout << "No product has ID " << id << ".\n";
        return;
    }

    const Product* product = catalog.findById(id);
    if (product != nullptr) {
        catalog.printProducts(vector<Product>{*product}, "BST product ID search result");
    }
}

void searchMenu(const Catalog& catalog, const ProductIdTree& index) {
    cout << "\nSearch options\n"
         << "1. Search by keyword (linear search)\n"
         << "2. Search by product ID (binary search tree)\n";
    const int choice = readInt("Choose an option (0 to return): ");

    if (choice == 1) {
        filterByKeyword(catalog);
    } else if (choice == 2) {
        searchByProductId(catalog, index);
    } else if (choice != 0) {
        cout << "Invalid search option.\n";
    }
}

size_t firstPriceAtLeast(const vector<Product>& sorted, double minimum) {
    // Binary search for the first product whose price is not below the minimum.
    size_t low = 0;
    size_t high = sorted.size();
    while (low < high) {
        const size_t middle = low + (high - low) / 2;
        if (sorted[middle].price < minimum) {
            low = middle + 1;
        } else {
            high = middle;
        }
    }
    return low;
}

size_t firstPriceGreaterThan(const vector<Product>& sorted, double maximum) {
    // Binary search for the first product above the maximum.
    size_t low = 0;
    size_t high = sorted.size();
    while (low < high) {
        const size_t middle = low + (high - low) / 2;
        if (sorted[middle].price <= maximum) {
            low = middle + 1;
        } else {
            high = middle;
        }
    }
    return low;
}

void filterByPriceRange(const Catalog& catalog) {
    double minimum = readDouble("Minimum price in PHP: ");
    double maximum = readDouble("Maximum price in PHP: ");
    if (minimum < 0.0 || maximum < minimum) {
        cout << "Enter a non-negative minimum and a maximum at least as large as the minimum.\n";
        return;
    }

    // Binary search needs sorted prices, so sort a copy and leave the catalog alone.
    vector<Product> sorted = catalog.snapshot();
    insertionSort(sorted, [](const Product& left, const Product& right) {
        if (left.price != right.price) {
            return left.price < right.price;
        }
        return left.id < right.id;
    });

    const size_t begin = firstPriceAtLeast(sorted, minimum);
    const size_t end = firstPriceGreaterThan(sorted, maximum);
    vector<Product> matches;
    for (size_t i = begin; i < end; ++i) {
        matches.push_back(sorted[i]);
    }

    catalog.printProducts(matches, "Binary price range search results");
}

void sortInventory(const Catalog& catalog) {
    cout << "\nSort options\n"
         << "1. Price ascending (insertion sort)\n"
         << "2. Price ascending (quick sort)\n"
         << "3. Stock descending (insertion sort)\n"
         << "4. Brand A-Z (quick sort)\n";
    const int choice = readInt("Choose an option (0 to return): ");
    if (choice == 0) {
        return;
    }

    vector<Product> sorted = catalog.snapshot();
    if (choice == 1) {
        insertionSort(sorted, [](const Product& left, const Product& right) {
            if (left.price != right.price) {
                return left.price < right.price;
            }
            return left.id < right.id;
        });
        catalog.printProducts(sorted, "Price ascending - insertion sort");
    } else if (choice == 2) {
        quickSort(sorted, 0, static_cast<int>(sorted.size()) - 1,
                  [](const Product& left, const Product& right) {
                      if (left.price != right.price) {
                          return left.price < right.price;
                      }
                      return left.id < right.id;
                  });
        catalog.printProducts(sorted, "Price ascending - quick sort");
    } else if (choice == 3) {
        insertionSort(sorted, [](const Product& left, const Product& right) {
            if (left.stockQuantity != right.stockQuantity) {
                return left.stockQuantity > right.stockQuantity;
            }
            return left.id < right.id;
        });
        catalog.printProducts(sorted, "Stock descending - insertion sort");
    } else if (choice == 4) {
        quickSort(sorted, 0, static_cast<int>(sorted.size()) - 1,
                  [](const Product& left, const Product& right) {
                      const string leftBrand = lowerCopy(left.brand);
                      const string rightBrand = lowerCopy(right.brand);
                      if (leftBrand != rightBrand) {
                          return leftBrand < rightBrand;
                      }
                      return lowerCopy(left.name) < lowerCopy(right.name);
                  });
        catalog.printProducts(sorted, "Brand A-Z - quick sort");
    } else {
        cout << "Invalid sort option.\n";
    }
}

// -------------------- Menu actions and order processing --------------------

void manageCart(Catalog& catalog,
                ShoppingCart& cart,
                Wishlist& wishlist,
                CheckoutQueue& queue,
                int& nextOrderNumber) {
    while (true) {
        // Keep the current cart visible while the customer uses the submenu.
        cart.print(catalog);
        cout << "\nCart and wishlist menu\n"
             << "1. Add product to cart\n"
             << "2. Navigate cart (doubly linked list)\n"
             << "3. Add product to wishlist\n"
             << "4. Navigate wishlist (singly linked list)\n"
             << "5. Queue cart for checkout\n"
             << "6. View wishlist\n";
        const int choice = readInt("Choose an option (0 to return): ");

        if (choice == 0) {
            return;
        }
        if (choice == 1) {
            catalog.printAll();
            const int id = readInt("Enter product ID (0 to cancel): ");
            if (id == 0) {
                continue;
            }

            Product* product = catalog.findById(id);
            if (product == nullptr) {
                cout << "No product has ID " << id << ".\n";
                continue;
            }

            const int quantity = readInt("Quantity to add: ");
            if (quantity <= 0) {
                cout << "Quantity must be greater than zero.\n";
                continue;
            }

            if (cart.quantityFor(id) + quantity > product->stockQuantity) {
                cout << "That quantity exceeds the current stock available.\n";
                continue;
            }

            cart.addOrUpdate(id, quantity);
            cout << quantity << " item(s) added to the cart.\n";
        } else if (choice == 2) {
            cart.navigate(catalog);
        } else if (choice == 3) {
            catalog.printAll();
            const int id = readInt("Enter product ID to save (0 to cancel): ");
            if (id == 0) {
                continue;
            }
            if (catalog.findById(id) == nullptr) {
                cout << "No product has ID " << id << ".\n";
            } else if (wishlist.add(id)) {
                cout << "Product added to the wishlist.\n";
            } else {
                cout << "That product is already on the wishlist.\n";
            }
        } else if (choice == 4) {
            wishlist.navigate(catalog);
        } else if (choice == 5) {
            vector<CartLine> lines = cart.lines();
            if (lines.empty()) {
                cout << "Add at least one product before queuing checkout.\n";
                continue;
            }

            CheckoutOrder order{nextOrderNumber++, lines};
            queue.enqueue(order);
            cart.clear();
            cout << "Order #" << order.orderNumber << " was added to the checkout queue.\n";
        } else if (choice == 6) {
            wishlist.print(catalog);
        } else {
            cout << "Invalid cart option.\n";
        }
    }
}

void processNextCheckout(Catalog& catalog,
                         CheckoutQueue& queue,
                         CheckoutStack& undoStack,
                         CheckoutStack& redoStack) {
    CheckoutOrder order{};
    if (!queue.dequeue(order)) {
        cout << "The checkout queue is empty. Add items to the cart and queue an order first.\n";
        return;
    }

    // Check again here because stock may have changed while the order waited.
    bool canComplete = true;
    for (const CartLine& line : order.lines) {
        const Product* product = catalog.findById(line.productId);
        if (product == nullptr || product->stockQuantity < line.quantity) {
            canComplete = false;
            break;
        }
    }

    if (!canComplete) {
        cout << "Order #" << order.orderNumber
             << " could not be completed because stock changed. It was removed from the queue.\n";
        return;
    }

    // A new completed checkout starts a new history branch.
    redoStack.clear();
    // Change stock only after every line in the order passes the check above.
    double total = 0.0;
    for (const CartLine& line : order.lines) {
        Product* product = catalog.findById(line.productId);
        product->stockQuantity -= line.quantity;
        total += product->price * line.quantity;
    }
    undoStack.push(order);

    cout << "Order #" << order.orderNumber << " processed successfully.\n"
         << "Inventory was updated. Total: PHP " << fixed << setprecision(2) << total << "\n";
}

void undoLastInventoryChange(Catalog& catalog,
                             CheckoutStack& undoStack,
                             CheckoutStack& redoStack) {
    CheckoutOrder order{};
    if (!undoStack.pop(order)) {
        cout << "There is no completed checkout to undo.\n";
        return;
    }

    // Restore the stock, then save the order so Redo can apply it again.
    for (const CartLine& line : order.lines) {
        Product* product = catalog.findById(line.productId);
        if (product != nullptr) {
            product->stockQuantity += line.quantity;
        }
    }
    redoStack.push(order);

    cout << "Undo complete: inventory for order #" << order.orderNumber
         << " was restored. The order is available to redo.\n";
}

void redoLastInventoryChange(Catalog& catalog,
                             CheckoutStack& undoStack,
                             CheckoutStack& redoStack) {
    CheckoutOrder order{};
    if (!redoStack.pop(order)) {
        cout << "There is no undone checkout to redo.\n";
        return;
    }

    // Validate the whole order before changing any product's stock.
    for (const CartLine& line : order.lines) {
        const Product* product = catalog.findById(line.productId);
        if (product == nullptr || product->stockQuantity < line.quantity) {
            redoStack.push(order);
            cout << "Redo could not be completed because stock is no longer sufficient. "
                 << "The order remains available to retry.\n";
            return;
        }
    }

    for (const CartLine& line : order.lines) {
        Product* product = catalog.findById(line.productId);
        product->stockQuantity -= line.quantity;
    }
    undoStack.push(order);

    cout << "Redo complete: inventory for order #" << order.orderNumber
         << " was deducted again.\n";
}

void manageInventoryHistory(Catalog& catalog,
                            CheckoutStack& undoStack,
                            CheckoutStack& redoStack) {
    cout << "\nInventory history (LIFO stacks)\n"
         << "1. Undo the most recent checkout\n"
         << "2. Redo the most recently undone checkout\n";
    const int choice = readInt("Choose an option (0 to return): ");
    if (choice == 1) {
        undoLastInventoryChange(catalog, undoStack, redoStack);
    } else if (choice == 2) {
        redoLastInventoryChange(catalog, undoStack, redoStack);
    } else if (choice != 0) {
        cout << "Invalid history option.\n";
    }
}

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

int main() {
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

