
#include "Action.h"

#include <iostream>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

// 메뉴를 다시 출력하기 전에 이전 화면 지우기
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

// 작업 결과를 확인할 수 있도록 입력 대기
static void waitForEnter() {
    cout << "\nPress Enter to continue...";
    string line;
    if (!getline(cin, line)) {
        throw runtime_error("Input stream closed");
    }
}

// 정수 입력(숫자가 아닌 값이 들어오면 다시 입력받음)
int Action::readInt(const string& message) {
    string line;
    int value;
    char extra;

    while (true) {
        cout << message;

        if (!getline(cin, line)) {
            throw runtime_error("Input stream closed");
        }

        // 정수 외에 다른 문자가 섞여있으면 잘못된 입력으로 처리
        stringstream ss(line);

        if ((ss >> value) && !(ss >> extra)) {
            return value;
        }

        cout << "Invalid input. Enter an integer.\n";
    }
}

// 메인 메뉴
void Action::showMenu() {
    cout << "\n========== ITEM INVENTORY ==========\n";
    cout << "1. Manage Inventory\n";
    cout << "2. Manage Storage\n";
    cout << "3. Item Catalog\n";
    cout << "4. Save\n";
    cout << "5. Load\n";
    cout << "0. Exit\n";
    cout << "====================================\n";
}

void Action::run() {
    // 프로그램 시작시 아이템 목록을 먼저 불러옴
    if (!FileManager::loadItems(itemFile, itemCatalog)) {
        cout << "Failed to load item catalog: " << itemFile << "\n";
        cout << "Item acquisition is unavailable.\n";
    }

    while (true) {
        try {
            clearScreen();
            showMenu();

            int choice = readInt("Select > ");
            clearScreen();

            if (choice == 0) {
                cout << "Exiting program.\n";
                break;
            }

            handleInput(choice);

            // 하위 메뉴에서는 자체적으로 입력 대기를 처리함
            if (choice != 1 && choice != 2) {
                waitForEnter();
            }
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
        manageInventory();
        break;

    case 2:
        manageStorage();
        break;

    case 3:
        showItemCatalog();
        break;

    case 4:
        saveData();
        break;

    case 5:
        loadData();
        break;

    default:
        cout << "Invalid menu option.\n";
    }
}

// 인벤토리를 표시한 상태에서 기능 선택
void Action::manageInventory() {
    while (true) {
        clearScreen();
        showInventory();

        cout << "\n===== MANAGE INVENTORY =====\n";
        cout << "1. Add Item\n";
        cout << "2. Use Item\n";
        cout << "3. Remove Item\n";
        cout << "4. Transfer to Storage\n";
        cout << "0. Back\n";

        int choice = readInt("Select > ");

        if (choice == 0) {
            return;
        }

        clearScreen();
        showInventory();
        cout << "\n";

        switch (choice) {
        case 1:
            addItem();
            break;

        case 2:
            useItem();
            break;

        case 3:
            removeItem();
            break;

        case 4:
            transferToStorage();
            break;

        default:
            cout << "Invalid menu option.\n";
        }

        waitForEnter();
    }
}

// 인벤토리 메뉴에서 바로 보관함으로 이동
void Action::transferToStorage() {
    if (inventory.isEmpty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    cout << "\n===== SELECT STORAGE =====\n";
    storageManager.printAll();

    int id = readInt("Storage ID (0=Cancel): ");

    if (id == 0) {
        cout << "Transfer cancelled.\n";
        return;
    }

    Storage* storage = storageManager.getStorage(id);

    if (storage == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    clearScreen();
    showInventory();

    cout << "\n===== " << storage->getName() << " =====\n";
    storage->printAll();

    int slot = readInt("Slot to deposit: ");
    int quantity = readInt("Quantity to deposit: ");

    if (quantity <= 0 || inventory.getItem(slot) == nullptr) {
        cout << "Invalid slot or quantity.\n";
        return;
    }

    AddResult result = storage->deposit(inventory, slot, quantity);

    cout << "Items transferred: " << result.added << "\n";
    cout << "Items not transferred: " << result.remaining << "\n";
}

// 인벤토리 출력
void Action::showInventory() {
    cout << "\n===== INVENTORY =====\n";

    inventory.printAll();

    cout << "Used slots: "
        << inventory.getItemCount()
        << " / "
        << inventory.getMaxSlots()
        << "\n";
}

// 아이템 목록 출력
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

// 아이템 추가
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

        clearScreen();
        showInventory();
        showItemCatalog();

        cout << "Items added: "
            << result.added << "\n";

        cout << "Items not added: "
            << result.remaining << "\n";

        return;
    }

    cout << "Item ID not found.\n";
}

// 아이템 사용
void Action::useItem() {
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

    // 슬롯 번호가 아니라 같은 ID 아이템 중 몇번째인지 찾아서 사용해야함
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


// 아이템 삭제
void Action::removeItem() {
    if (inventory.isEmpty()) {
        cout << "Inventory is empty.\n";
        return;
    }

    int slot = readInt("Slot to remove: ");

    Item* item = inventory.getItem(slot);

    if (item == nullptr) {
        cout << "Invalid slot.\n";
        return;
    }

    int currentQuantity = item->getQuantity();

    cout << "Selected: " << item->getName() << "\n";
    cout << "Current quantity: " << currentQuantity << "\n";

    int quantity = readInt("Quantity to remove: ");

    if (quantity < 0 || quantity > currentQuantity) {
        cout << "Invalid quantity.\n";
        return;
    }

    // 슬롯 전체를 삭제하는 경우 확인을 받음
    if (quantity >= currentQuantity) {
        int confirm = readInt(
            "Remove entire slot? (1=Yes, 0=No): "
        );

        if (confirm != 1) {
            cout << "Removal cancelled.\n";
            return;
        }
    }

    int removed = quantity == 0 ? currentQuantity : quantity;

    if (inventory.removeItemQuantity(slot, quantity)) {
        cout << "Items removed: " << removed << "\n";

        Item* remaining = inventory.getItem(slot);

        // 슬롯이 삭제되면 다음 아이템이 당겨질 수 있으므로 기존 포인터는 사용하지 않음
        if (removed == currentQuantity) {
            cout << "Slot removed successfully.\n";
        }
        else if (remaining != nullptr) {
            cout << "Remaining quantity: "
                << remaining->getQuantity() << "\n";
        }
    }
    else {
        cout << "Failed to remove item.\n";
    }
}


// 보관함 메뉴
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
        clearScreen();
        showStorageMenu();

        int choice = readInt("Select > ");
        clearScreen();

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
            continue;

        default:
            cout << "Invalid menu option.\n";
        }

        waitForEnter();
    }
}

// 보관함 생성
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

// 보관함 삭제
void Action::removeStorage() {
    storageManager.printAll();

    int id = readInt("Storage ID to remove: ");

    Storage* storage = storageManager.getStorage(id);

    if (storage == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    // 아이템이 들어있어도 삭제할 수 있으므로 삭제 전 경고
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

// 보관함 이름 변경
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

// 보관함 열기
void Action::openStorage() {
    storageManager.printAll();

    int id = readInt("Storage ID to open: ");

    Storage* storage = storageManager.getStorage(id);

    if (storage == nullptr) {
        cout << "Storage not found.\n";
        return;
    }

    while (true) {
        clearScreen();

        cout << "\n===== "
            << storage->getName()
            << " =====\n";

        cout << "1. View Storage\n";
        cout << "2. Deposit Item\n";
        cout << "3. Withdraw Item\n";
        cout << "0. Back\n";

        int choice = readInt("Select > ");
        clearScreen();

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

        waitForEnter();
    }
}

// 보관함에 아이템 넣기
void Action::depositItem(Storage& storage) {
    cout << "\n===== PLAYER INVENTORY =====\n";

    inventory.printAll();

    if (inventory.isEmpty()) {
        return;
    }

    int slot = readInt("Slot to deposit: ");
    int quantity = readInt("Quantity to deposit: ");

    // 보관함에 들어간 수량과 못 들어간 수량을 따로 확인
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

// 보관함에서 아이템 꺼내기
void Action::withdrawItem(Storage& storage) {
    cout << "\n===== STORAGE INVENTORY =====\n";

    storage.printAll();

    if (storage.getInventory().isEmpty()) {
        return;
    }

    int slot = readInt("Slot to withdraw: ");
    int quantity = readInt("Quantity to withdraw: ");

    // 인벤토리에 들어간 수량과 못 들어간 수량을 따로 확인
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

// 인벤토리와 보관함 데이터 저장
void Action::saveData() {
    // 인벤토리와 보관함을 각각 저장하고 둘 다 성공했는지 확인
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

// 저장된 데이터 불러오기
void Action::loadData() {
    // 현재 데이터를 덮어쓰게 되므로 불러오기 전에 확인
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
