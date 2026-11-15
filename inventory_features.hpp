#pragma once

#include "catalog.hpp"

namespace cryptid {
void filterByKeyword(const Catalog& catalog);
void searchByProductId(const Catalog& catalog, const ProductIdTree& index);
void searchMenu(const Catalog& catalog, const ProductIdTree& index);
void filterByPriceRange(const Catalog& catalog);
void sortInventory(const Catalog& catalog);
} // namespace cryptid