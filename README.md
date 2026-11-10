# Crypto and Bitcoin Hardware Inventory

## What the program does

This is a C++ console program for a small shop that sells hardware wallets and accessories. It shows a product catalog, searches and sorts products, manages a cart and wishlist, and processes checkout orders.

The catalog uses Ledger, Trezor, BitBox, and Coldcard as a sample partner roster. Product details, prices, and stock counts are examples for the demo. Confirm your group's approved brands and product data before submitting.

## Main menu

| Option | What it does |
| --- | --- |
| 1 | Show the full catalog and its current array capacity. |
| 2 | Show products from one brand. |
| 3 | Search by keyword or product ID. |
| 4 | Find products inside a PHP price range. |
| 5 | Add items to the cart or wishlist, browse them, and queue a cart. |
| 6 | Show the checkout queue and process its next order. |
| 7 | Undo or redo the latest inventory change. |
| 8 | Cycle through the sample brand banners. |
| 9 | Display inventory using a selected sort. |
| 10 | Exit. |

## Where to find each lab

| Lab | Code to look for | What it demonstrates |
| --- | --- | --- |
| 1: Dynamic allocation | `Catalog`, especially `grow()` | A product array starts small and doubles its capacity when it fills up. |
| 2: Linked lists | `ShoppingCart`, `Wishlist` | Cart nodes link forward and backward. Wishlist nodes link forward. |
| 3: Circular list | `BrandShowcase` | The last brand node points back to the first. |
| 4: Stack | `CheckoutStack`, `undoLastInventoryChange()`, `redoLastInventoryChange()` | Undo and Redo use LIFO order. |
| 5: Queue | `CheckoutQueue` | Orders are processed in FIFO order. |
| 6: Binary search tree | `ProductIdTree` | Product IDs are found by moving left or right through a balanced tree. |
| 7: Searching | `filterByKeyword()`, `filterByPriceRange()` | Keyword search scans products. Price search uses binary search on a price-sorted copy. |
| 8: Sorting | `insertionSort()`, `quickSort()` | The program sorts by price, stock, and brand. |

## Run it in VS Code

The `.vscode` folder has a build task and a debugger setup for this computer. The Microsoft C/C++ extension is already installed here.

1. Open the `DSA_ECommerce_Capstone` folder in VS Code.
2. If VS Code shows **Restricted Mode**, choose **Manage → Trust this folder** to enable build and debug tasks.
3. Press **Ctrl+Shift+B** to build.
4. Press **F5** to build and start debugging.

The settings look for GCC and GDB in `%USERPROFILE%\Documents\Codex\toolchains\w64devkit\bin`, so the path works with different Windows account names. If you put the toolchain somewhere else, update the paths in `.vscode/tasks.json`, `.vscode/launch.json`, and `.vscode/settings.json`. The build creates `ecommerce.exe` in the project folder.

## Demo path

1. Choose **1** to show the catalog and the array capacity.
2. Choose **3**, then **2**, and search for ID `1003` to show the product ID tree.
3. Choose **4** and enter `500` and `1000` to show the price-range search.
4. Choose **5**. In its submenu, add product `1001` with quantity `2`, then product `1003` with quantity `1`. Navigate the cart, queue it with submenu option **5**, and choose **0** to return to the main menu.
5. Choose **6** to process the queued order. Choose **7**, then **1**, to undo it. Choose **7**, then **2**, to redo it.
6. Choose **8** to show the circular brand list. Choose **9** to view the sorting options.

## Details to remember

- The catalog is built once at startup. The product ID tree is balanced from those IDs and stays in sync because the menu does not add or remove products afterward.
- The price search sorts a copy of the catalog, so it does not change the catalog's stored order.
- Adding an order to the queue does not change stock. Stock changes when the order is processed.
- Each linked structure and tree releases its allocated nodes in its destructor. `Catalog` releases its array with `delete[]`.
- Prices and quantities are illustrative PHP values.
