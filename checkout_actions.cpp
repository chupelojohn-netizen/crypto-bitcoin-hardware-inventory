#include "checkout_actions.hpp"
#include "input.hpp"

#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

namespace cryptid {
using namespace std;

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

} // namespace cryptid
