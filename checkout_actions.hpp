#pragma once

#include "structures.hpp"

namespace cryptid {
void manageCart(Catalog& catalog, ShoppingCart& cart, Wishlist& wishlist,
                CheckoutQueue& queue, int& nextOrderNumber);
void processNextCheckout(Catalog& catalog, CheckoutQueue& queue,
                         CheckoutStack& undoStack, CheckoutStack& redoStack);
void undoLastInventoryChange(Catalog& catalog, CheckoutStack& undoStack,
                             CheckoutStack& redoStack);
void redoLastInventoryChange(Catalog& catalog, CheckoutStack& undoStack,
                             CheckoutStack& redoStack);
void manageInventoryHistory(Catalog& catalog, CheckoutStack& undoStack,
                            CheckoutStack& redoStack);
} // namespace cryptid