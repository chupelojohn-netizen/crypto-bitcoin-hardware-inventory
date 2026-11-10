# Presentation and Study Guide

Use this to understand the program before presenting it. Explain the ideas in your own words, and change the demo brands or sample data if your group has approved a different roster.

## Short project summary

The program is a small console shop for hardware wallets and accessories. A product has an ID, name, category, brand, price, and stock count. The program uses the data structures from Labs 1–8 to display products, search and sort the catalog, manage shopping lists, and process checkout orders.

## Suggested demo

### Opening summary

You can introduce the project in your own words: it is a console inventory system for Bitcoin hardware and accessories. The demo shows how product data is stored and searched, how cart and checkout actions use linked lists and a queue, and how Undo/Redo changes inventory history. The sample prices and brand list are for demonstration.

### 1. Show the catalog

Choose **1**. Point out the six fields for each product. The message also shows the current catalog size and array capacity. With the sample data, 12 products fit in a capacity of 16.

### 2. Show the product ID tree

Choose **3**, then **2**, and enter `1003`. The program finds Trezor Safe 5 by ID. The tree is balanced when it is built, so each comparison moves to a smaller part of the tree.

### 3. Show both kinds of search

- Choose **3**, then **1**, and enter `bitcoin`. This scans product names, categories, and brands.
- Choose **4** and enter `500` for the minimum and `1000` for the maximum. This searches a price-sorted copy of the catalog.

### 4. Show the cart and wishlist

Choose **5** to open the cart submenu.

1. Choose submenu option **1**, enter product ID `1001`, and quantity `2`.
2. Choose submenu option **1** again, enter product ID `1003`, and quantity `1`.
3. Choose submenu option **2** to browse the cart. Use `n` for next, `p` for previous, and `q` to return.
4. Choose submenu option **3**, enter product ID `1005`, then choose **4** to browse the wishlist.
5. Choose submenu option **5** to queue the cart, then **0** to return to the main menu.

### 5. Show checkout, Undo, and Redo

Choose main menu option **6** to process the queued order. Stock decreases only when the order is processed.

- Choose **7**, then submenu option **1**, to undo the latest checkout and restore stock.
- Choose **7**, then submenu option **2**, to redo it and deduct stock again.

### 6. Show the remaining structures

- Choose **8** to see the brand list cycle through the circular list.
- Choose **9** to try insertion sort by price or stock and quicksort by price or brand.

## How to explain the main ideas

### Array that grows

`Catalog` owns a dynamically allocated array. When it fills, `grow()` makes a larger array, copies the products, and deletes the old array. It starts with capacity 4 and doubles as needed.

### Linked lists

`ShoppingCart` has `previous` and `next` pointers, so it can move in both directions. `Wishlist` has only a `next` pointer, so it moves forward. `BrandShowcase` links its last node back to its first node, which makes the list circular.

### Queue and stacks

`CheckoutQueue` processes orders in the same order they were added: first in, first out. The Undo stack removes the newest completed checkout first: last in, first out. The Redo stack keeps the most recently undone order. A new successful checkout clears Redo history.

### Search and sorting

Keyword search uses a loop because the words are not stored in a searchable order. The price-range search first sorts a copy of the products by price, then uses binary search to find the beginning and end of the range. Sorting the copy keeps the catalog's original order unchanged.

The ID tree is made from the catalog at startup. It sorts the IDs and repeatedly chooses the middle ID as a node, keeping the initial tree balanced. The menu does not add or remove products later, so the tree does not need to be updated.

### Memory cleanup

The catalog deletes its array with `delete[]`. The cart, wishlist, queue, stacks, brand list, and tree each delete their allocated nodes when they are destroyed.

## Questions to practice

Try answering these without reading the guide:

1. Why does the shopping cart use a doubly linked list?
2. What is the difference between how the checkout queue and Undo stack process items?
3. Why does the price search sort a copy before using binary search?
4. When does the program change stock, and how can Undo restore it?
5. Why does the product ID tree start with the middle ID at its root?
6. Where does the program release dynamically allocated memory?

The demo roster uses Ledger, Trezor, BitBox, and Coldcard as sample partner brands. Confirm your group's approved roster before describing these as official partners.
