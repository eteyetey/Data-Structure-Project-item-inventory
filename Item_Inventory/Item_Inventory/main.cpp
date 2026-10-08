
#include <iostream>
#include <vector>
#include <limits>

#include "Item.h"
#include "Inventory.h"
#include "FileManager.h"

using namespace std;


// 콘솔 정수 입력
int inputInt() {

    int value;

    while (!(cin >> value)) {

        // 입력 종료 시 무한 반복 방지
        if (cin.eof()) {
            return -1;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Try again: ";
    }

    return value;
}


// 아이템 원본 목록 출력
void printItemData(const vector<Item>& itemData) {

    cout << "\n===== Item List =====" << endl;

    for (const Item& item : itemData) {

        cout << "ID: " << item.getId()
            << " | Name: " << item.getName()
            << " | Type: " << static_cast<int>(item.getType());

        if (item.getType() == ItemType::Durability) {
            cout << " | Max Durability: " << item.getMaxDurability();
        }

        cout << endl;
    }
}


int main() {

    Inventory inventory;
    vector<Item> itemData;

    // 아이템 원본 정보 불러오기
    if (!FileManager::loadItems("items.txt", itemData)) {
        cout << "Failed to load items.txt" << endl;
        return 1;
    }

    // 저장된 인벤토리 불러오기
    if (!FileManager::loadInventory("inventory.txt", inventory)) {
        cout << "Failed to load inventory.txt" << endl;
        return 1;
    }

    cout << "Inventory loaded successfully!" << endl;

    int choice;

    while (true) {

        cout << "\n========== Inventory Test ==========" << endl;
        cout << "1. Show Item List" << endl;
        cout << "2. Add Item" << endl;
        cout << "3. Show Inventory" << endl;
        cout << "4. Use Item" << endl;
        cout << "5. Remove Item" << endl;
        cout << "6. Save Inventory" << endl;
        cout << "7. Reload Inventory" << endl;
        cout << "0. Exit" << endl;
        cout << "====================================" << endl;
        cout << "Select: ";

        choice = inputInt();

        if (cin.eof()) {
            break;
        }


        // 종료
        if (choice == 0) {
            break;
        }


        // 아이템 원본 목록 출력
        else if (choice == 1) {
            printItemData(itemData);
        }


        // 아이템 추가
        else if (choice == 2) {

            printItemData(itemData);

            cout << "\nEnter Item ID: ";
            int id = inputInt();

            cout << "Enter Quantity: ";
            int quantity = inputInt();

            if (cin.eof()) {
                break;
            }

            if (quantity <= 0) {
                cout << "Quantity must be greater than 0." << endl;
                continue;
            }

            bool found = false;

            for (const Item& item : itemData) {

                if (item.getId() == id) {

                    Item newItem = item;
                    newItem.setQuantity(quantity);

                    AddResult result = inventory.addItem(newItem);

                    cout << "\nRequested: " << quantity << endl;
                    cout << "Added: " << result.added << endl;
                    cout << "Remaining: " << result.remaining << endl;
                    cout << "Used Slots: " << inventory.getItemCount()
                        << " / " << Inventory::MAX_SLOTS << endl;

                    if (result.remaining > 0) {
                        cout << "Inventory capacity exceeded!" << endl;
                    }

                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Item ID not found." << endl;
            }
        }


        // 인벤토리 출력
        else if (choice == 3) {

            cout << "\n===== Current Inventory =====" << endl;

            inventory.printAll();

            cout << "Slots: " << inventory.getItemCount()
                << " / " << Inventory::MAX_SLOTS << endl;
        }


        // 아이템 사용
        else if (choice == 4) {

            inventory.printAll();

            cout << "\nEnter Item ID: ";
            int id = inputInt();

            cout << "Enter Same-ID Index (0-based): ";
            int index = inputInt();

            if (cin.eof()) {
                break;
            }

            if (inventory.useItem(id, index)) {
                cout << "Item used successfully!" << endl;
            }
            else {
                cout << "Failed to use item." << endl;
            }
        }


        // 아이템 삭제
        else if (choice == 5) {

            inventory.printAll();

            cout << "\nEnter Item ID: ";
            int id = inputInt();

            cout << "Enter Same-ID Index (0-based): ";
            int index = inputInt();

            if (cin.eof()) {
                break;
            }

            if (inventory.removeItem(id, index)) {
                cout << "Item removed successfully!" << endl;
            }
            else {
                cout << "Failed to remove item." << endl;
            }
        }


        // 인벤토리 저장
        else if (choice == 6) {

            if (FileManager::saveInventory("inventory.txt", inventory)) {
                cout << "Inventory saved successfully!" << endl;
            }
            else {
                cout << "Failed to save inventory." << endl;
            }
        }


        // 인벤토리 다시 불러오기
        else if (choice == 7) {

            if (FileManager::loadInventory("inventory.txt", inventory)) {
                cout << "Inventory reloaded successfully!" << endl;
                inventory.printAll();
            }
            else {
                cout << "Failed to reload inventory." << endl;
            }
        }


        // 잘못된 메뉴 선택
        else {
            cout << "Invalid menu selection." << endl;
        }
    }

    return 0;
}
