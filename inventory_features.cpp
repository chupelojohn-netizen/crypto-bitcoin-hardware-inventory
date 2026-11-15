#include "inventory_features.hpp"
#include "input.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace cryptid {
using namespace std;

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

} // namespace cryptid
