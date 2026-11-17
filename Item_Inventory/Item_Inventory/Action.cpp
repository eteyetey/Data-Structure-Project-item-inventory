
#include "Action.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

// ==============================
// INPUT
// ==============================

int Action::readInt(const string& message) {
    string line;
    int value;
    char extra;

    while (true) {
        cout << message;

        if (!getline(cin, line)) {
            throw runtime_error("Input stream closed");
        }

        stringstream ss(line);

        if ((ss >> value) && !(ss >> extra)) {
            return value;
        }

        cout << "Invalid input. Enter an integer.\n";
    }
}

// ==============================
// MAIN MENU
// ==============================

void Action::showMenu() {
    cout << "\n========== ITEM INVENTORY ==========\n";
    cout << "1. View Inventory\n";
    cout << "2. View Item Catalog\n";
    cout << "3. Add Item\n";
    cout << "4. Use Item\n";
    cout << "5. Remove Item\n";
    cout << "6. Manage Storages\n";
    cout << "7. Save Data\n";
    cout << "8. Load Data\n";
    cout << "0. Exit\n";
    cout << "====================================\n";
}

void Action::run() {
    if (!FileManager::loadItems(itemFile, itemCatalog)) {
        cout << "Failed to load item catalog: " << itemFile << "\n";
        cout << "Item acquisition is unavailable.\n";
    }

    while (true) {
        try {
            showMenu();

            int choice = readInt("Select > ");

            if (choice == 0) {
                cout << "Exiting program.\n";
                break;
            }

            handleInput(choice);
        }
        catch (const exception& e) {
            cout << "Program error: " << e.what() << "\n";
            break;
        }
    }
}

void Action::handleInput(int choice) {
    switch (choice) {
    case 1:
        showInventory();
        break;

    case 2:
        showItemCatalog();
        break;

    case 3:
        addItem();
        break;

    case 4:
        useItem();
        break;

    case 5:
        removeItem();
        break;

    case 6:
        manageStorage();
        break;

    case 7:
        saveData();
        break;

    case 8:
        loadData();
        break;

    default:
        cout << "Invalid menu option.\n";
    }
}

// ==============================
// INVENTORY
// ==============================

void Action::showInventory() {
    cout << "\n===== INVENTORY =====\n";

    inventory.printAll();

    cout << "Used slots: "
        << inventory.getItemCount()
        << " / "
        << inventory.getMaxSlots()
        << "\n";
}

// ==============================
// ITEM CATALOG
// ==============================

void Action::showItemCatalog() {
    cout << "\n===== ITEM CATALOG =====\n";

    if (itemCatalog.empty()) {
        cout << "No items available.\n";
        return;
    }

    for (const Item& item : itemCatalog) {
        cout << "ID: " << item.getId()
            << " | Name: " << item.getName()
            << " | Category: " << item.getCategory()
            << " | Rarity: " << item.getRarity()
            << "\n";
    }
}

// ==============================
// ADD ITEM
// ==============================

void Action::addItem() {
    if (itemCatalog.empty()) {
        cout << "Item catalog is empty.\n";
        return;
    }

    showItemCatalog();

    int id = readInt("Item ID: ");
    int quantity = readInt("Quantity: ");

    if (quantity <= 0) {
        cout << "Quantity must be positive.\n";
        return;
    }

    for (const Item& item : itemCatalog) {
        if (item.getId() != id) {
            continue;
        }

        Item newItem = item;
        newItem.setQuantity(quantity);

        AddResult result = inventory.addItem(newItem);

        cout << "Items added: "
            << result.added << "\n";

        cout << "Items not added: "
            << result.remaining << "\n";

        return;
    }

    cout << "Item ID not found.\n";
}

// ==============================
// USE ITEM
// ==============================

void Action::useItem() {
    showInventory();

    if (inventory.isEmpty()) {
        return;
    }

    int slot = readInt("Slot to use: ");

    Item* target = inventory.getItem(slot);

    if (target == nullptr) {
        cout << "Invalid slot.\n";
        return;
    }

    int id = target->getId();

    vector<Item*> sameItems = inventory.findItemsById(id);

    for (int i = 0; i < static_cast<int>(sameItems.size()); i++) {
        if (sameItems[i] == target) {
            if (inventory.useItem(id, i)) {
                cout << "Item used successfully.\n";
            }
            else {
                cout << "Failed to use item.\n";
            }

            return;
        }
    }

    cout << "Failed to find item.\n";
}

// ==============================
// REMOVE ITEM
// ==============================

void Action::removeItem() {
    showInventory();

    if (inventory.isEmpty()) {
        return;
    }

    int slot = readInt("Slot to remove: ");

    if (inventory.getItem(slot) == nullptr) {
        cout << "Invalid slot.\n";
        return;
    }

    int confirm = readInt(
        "Remove entire slot? (1=Yes, 0=No): "
    );

    if (confirm != 1) {
        cout << "Removal cancelled.\n";
        return;
    }

    if (inventory.removeItemAt(slot)) {
        cout << "Item removed successfully.\n";
    }
    else {
        cout << "Failed to remove item.\n";
    }
}

// ==============================
// STORAGE MENU
// ==============================

void Action::showStorageMenu() {
    cout << "\n===== STORAGE MANAGER =====\n";
    cout << "1. List Storages\n";
    cout << "2. Create Storage\n";
    cout << "3. Remove Storage\n";
    cout << "4. Rename Storage\n";
    cout << "5. Open Storage\n";
    cout << "0. Back\n";
}

void Action::manageStorage() {
    while (true) {
        showStorageMenu();

        int choice = readInt("Select > ");

        switch (choice) {
        case 0:
            return;

        case 1:
            storageManager.printAll();
            break;

        case 2:
            createStorage();
            break;

        case 3:
            removeStorage();
            break;

        case 4:
            renameStorage();
            break;

        case 5:
            openStorage();
            break;

        default:
            cout << "Invalid menu option.\n";
        }
    }
}

// ==============================
// CREATE STORAGE
// ==============================

void Action::createStorage() {
    string name;

    cout << "Storage name: ";

    if (!getline(cin, name)) {
        throw runtime_error("Input stream closed");
    }

    int maxSlots = readInt("Maximum slots: ");
    int maxStack = readInt("Maximum stack size: ");

    int id = storageManager.createStorage(
        name,
        maxSlots,
        maxStack
    );

    if (id == -1) {
        cout << "Failed to create storage.\n";
        return;
    }

    cout << "Storage created successfully.\n";
    cout << "Storage ID: " << id << "\n";
}

// ==============================
// REMOVE STORAGE
// ==============================

void Action::removeStorage() {
    storageManager.printAll();

    int id = readInt("Storage ID to remove: ");

    Storage* storage = storageManager.getStorage(id);

    if (storage == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    if (!storage->getInventory().isEmpty()) {
        cout << "Storage contains items.\n";
        cout << "All stored items will be lost.\n";
    }

    int confirm = readInt(
        "Confirm removal? (1=Yes, 0=No): "
    );

    if (confirm != 1) {
        cout << "Removal cancelled.\n";
        return;
    }

    if (storageManager.removeStorage(id)) {
        cout << "Storage removed successfully.\n";
        cout << "Save data to persist this change.\n";
    }
    else {
        cout << "Failed to remove storage.\n";
    }
}

// ==============================
// RENAME STORAGE
// ==============================

void Action::renameStorage() {
    storageManager.printAll();

    int id = readInt("Storage ID to rename: ");

    if (storageManager.getStorage(id) == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    string name;

    cout << "New name: ";

    if (!getline(cin, name)) {
        throw runtime_error("Input stream closed");
    }

    if (storageManager.renameStorage(id, name)) {
        cout << "Storage renamed successfully.\n";
    }
    else {
        cout << "Failed to rename storage.\n";
    }
}

// ==============================
// OPEN STORAGE
// ==============================

void Action::openStorage() {
    storageManager.printAll();

    int id = readInt("Storage ID to open: ");

    Storage* storage = storageManager.getStorage(id);

    if (storage == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    while (true) {
        cout << "\n===== "
            << storage->getName()
            << " =====\n";

        cout << "1. View Storage\n";
        cout << "2. Deposit Item\n";
        cout << "3. Withdraw Item\n";
        cout << "0. Back\n";

        int choice = readInt("Select > ");

        switch (choice) {
        case 0:
            return;

        case 1:
            storage->printAll();
            break;

        case 2:
            depositItem(*storage);
            break;

        case 3:
            withdrawItem(*storage);
            break;

        default:
            cout << "Invalid menu option.\n";
        }
    }
}

// ==============================
// DEPOSIT ITEM
// ==============================

void Action::depositItem(Storage& storage) {
    cout << "\n===== PLAYER INVENTORY =====\n";

    inventory.printAll();

    if (inventory.isEmpty()) {
        return;
    }

    int slot = readInt("Slot to deposit: ");
    int quantity = readInt("Quantity to deposit: ");

    AddResult result = storage.deposit(
        inventory,
        slot,
        quantity
    );

    cout << "Items transferred: "
        << result.added << "\n";

    cout << "Items not transferred: "
        << result.remaining << "\n";
}

// ==============================
// WITHDRAW ITEM
// ==============================

void Action::withdrawItem(Storage& storage) {
    cout << "\n===== STORAGE INVENTORY =====\n";

    storage.printAll();

    if (storage.getInventory().isEmpty()) {
        return;
    }

    int slot = readInt("Slot to withdraw: ");
    int quantity = readInt("Quantity to withdraw: ");

    AddResult result = storage.withdraw(
        inventory,
        slot,
        quantity
    );

    cout << "Items transferred: "
        << result.added << "\n";

    cout << "Items not transferred: "
        << result.remaining << "\n";
}

// ==============================
// SAVE DATA
// ==============================

void Action::saveData() {
    bool inventorySaved =
        FileManager::saveInventory(
            inventoryFile,
            inventory
        );

    bool storageSaved =
        storageManager.saveAll(storageFolder);

    if (inventorySaved && storageSaved) {
        cout << "All data saved successfully.\n";
    }
    else {
        cout << "Data save failed or partially completed.\n";
    }
}

// ==============================
// LOAD DATA
// ==============================

void Action::loadData() {
    int confirm = readInt(
        "Replace current data with saved data? (1=Yes, 0=No): "
    );

    if (confirm != 1) {
        cout << "Loading cancelled.\n";
        return;
    }

    bool inventoryLoaded =
        FileManager::loadInventory(
            inventoryFile,
            inventory
        );

    bool storageLoaded =
        storageManager.loadAll(storageFolder);

    if (inventoryLoaded && storageLoaded) {
        cout << "All data loaded successfully.\n";
    }
    else {
        cout << "Data load failed or partially completed.\n";
    }
}
