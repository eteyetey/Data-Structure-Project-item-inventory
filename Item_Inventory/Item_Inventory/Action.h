
#pragma once

#include <string>
#include <vector>

#include "Inventory.h"
#include "StorageManager.h"
#include "FileManager.h"

class Action {
private:
    Inventory inventory;
    StorageManager storageManager;
    std::vector<Item> itemCatalog;

    const std::string itemFile = "items.txt";
    const std::string inventoryFile = "inventory.txt";
    const std::string storageFolder = "storages";

    int readInt(const std::string& message);

    void showMenu();
    void handleInput(int choice);

    void manageInventory();
    void transferToStorage();
    void showInventory();
    void showItemCatalog();
    void addItem();
    void useItem();
    void removeItem();

    void manageStorage();
    void showStorageMenu();
    void createStorage();
    void removeStorage();
    void renameStorage();
    void openStorage();
    void depositItem(Storage& storage);
    void withdrawItem(Storage& storage);

    void saveData();
    void loadData();

    void sortInventory();
public:
    void run();
};
