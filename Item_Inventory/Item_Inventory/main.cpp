
#include <iostream>
#include <string>

#include "Item.h"
#include "Inventory.h"
#include "Storage.h"

using namespace std;

int passed = 0;
int failed = 0;


// 테스트 결과 확인
void check(string name, bool condition) {

    if (condition) {
        cout << "[PASS] " << name << endl;
        passed++;
    }
    else {
        cout << "[FAIL] " << name << endl;
        failed++;
    }
}


// 테스트 제목 출력
void testTitle(string title) {
    cout << "\n========================================" << endl;
    cout << title << endl;
    cout << "========================================" << endl;
}


// 아이템 수량 확인
int getQuantity(Inventory& inventory, int index) {

    Item* item = inventory.getItem(index);

    if (item == nullptr) {
        return 0;
    }

    return item->getQuantity();
}


// 아이템 전체 수량 확인
int getTotalQuantity(Inventory& inventory, int id) {

    int total = 0;

    vector<Item*> items = inventory.findItemsById(id);

    for (Item* item : items) {
        total += item->getQuantity();
    }

    return total;
}


int main() {

    // ========================================
    // TC01. 기본 입고
    // ========================================
    testTitle("TC01 - Basic Deposit");

    {
        Inventory player;
        Storage storage;

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(80);
        player.addItem(potion);

        AddResult result = storage.deposit(player, 0, 60);

        check("Moved 60", result.added == 60);
        check("Remaining request 0", result.remaining == 0);
        check("Player has 20", getQuantity(player, 0) == 20);
        check("Storage has 60", getQuantity(storage.getInventory(), 0) == 60);
    }


    // ========================================
    // TC02. 기본 출고
    // ========================================
    testTitle("TC02 - Basic Withdraw");

    {
        Inventory player;
        Storage storage;

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(60);

        storage.getInventory().addItem(potion);

        AddResult result = storage.withdraw(player, 0, 20);

        check("Withdraw 20", result.added == 20);
        check("Player has 20", getQuantity(player, 0) == 20);
        check("Storage has 40", getQuantity(storage.getInventory(), 0) == 40);
    }


    // ========================================
    // TC03. 중첩 제한에 따른 슬롯 분산
    // ========================================
    testTitle("TC03 - Stack Limit");

    {
        Inventory player;
        Storage storage;

        storage.setMaxStack(50);

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(130);
        player.addItem(potion);

        AddResult result = storage.deposit(player, 0, 130);

        Inventory& box = storage.getInventory();

        check("Moved 130", result.added == 130);
        check("Storage slots = 3", box.getItemCount() == 3);
        check("Slot 0 = 50", getQuantity(box, 0) == 50);
        check("Slot 1 = 50", getQuantity(box, 1) == 50);
        check("Slot 2 = 30", getQuantity(box, 2) == 30);
        check("Player empty", player.isEmpty());
    }


    // ========================================
    // TC04. 공간 부족 시 부분 입고
    // ========================================
    testTitle("TC04 - Partial Deposit");

    {
        Inventory player;
        Storage storage;

        storage.setMaxSlots(1);
        storage.setMaxStack(10);

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);

        potion.setQuantity(8);
        storage.getInventory().addItem(potion);

        potion.setQuantity(5);
        player.addItem(potion);

        AddResult result = storage.deposit(player, 0, 5);

        check("Moved 2", result.added == 2);
        check("Not moved 3", result.remaining == 3);
        check("Storage has 10", getQuantity(storage.getInventory(), 0) == 10);
        check("Player has 3", getQuantity(player, 0) == 3);
    }


    // ========================================
    // TC05. 보관함이 완전히 가득 찬 경우
    // ========================================
    testTitle("TC05 - Full Storage");

    {
        Inventory player;
        Storage storage;

        storage.setMaxSlots(1);
        storage.setMaxStack(10);

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);

        potion.setQuantity(10);
        storage.getInventory().addItem(potion);

        potion.setQuantity(5);
        player.addItem(potion);

        AddResult result = storage.deposit(player, 0, 5);

        check("Moved 0", result.added == 0);
        check("Not moved 5", result.remaining == 5);
        check("Player still has 5", getQuantity(player, 0) == 5);
        check("Storage still has 10", getQuantity(storage.getInventory(), 0) == 10);
    }


    // ========================================
    // TC06. 내구도형 아이템 이동
    // ========================================
    testTitle("TC06 - Durability Item");

    {
        Inventory player;
        Storage storage;

        // 현재 내구도 3 / 최대 내구도 10
        Item sword(2, "Sword", "Weapon", 3,
            ItemType::Durability, 3, 10);

        player.addItem(sword);

        AddResult result = storage.deposit(player, 0, 1);

        Item* moved = storage.getInventory().getItem(0);

        check("Moved 1", result.added == 1);
        check("Player empty", player.isEmpty());
        check("Storage has sword", moved != nullptr);

        if (moved != nullptr) {
            check("Durability = 3", moved->getDurability() == 3);
            check("Max durability = 10", moved->getMaxDurability() == 10);
            check("Quantity = 1", moved->getQuantity() == 1);
        }
        else {
            check("Durability = 3", false);
            check("Max durability = 10", false);
            check("Quantity = 1", false);
        }
    }


    // ========================================
    // TC07. 전체 슬롯 인덱스 이동
    // ========================================
    testTitle("TC07 - Global Slot Index");

    {
        Inventory player;
        Storage storage;

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        Item sword(2, "Sword", "Weapon", 2, ItemType::Durability, 10);
        Item stone(3, "Stone", "Material", 1, ItemType::Unusable);

        player.addItem(potion);
        player.addItem(sword);
        player.addItem(stone);

        // 전체 인벤토리의 1번 슬롯(Sword) 이동
        AddResult result = storage.deposit(player, 1, 1);

        Item* first = player.getItem(0);
        Item* second = player.getItem(1);
        Item* stored = storage.getInventory().getItem(0);

        check("Moved 1", result.added == 1);
        check("Player slots = 2", player.getItemCount() == 2);
        check("First slot is Potion", first != nullptr && first->getId() == 1);
        check("Second slot is Stone", second != nullptr && second->getId() == 3);
        check("Storage has Sword", stored != nullptr && stored->getId() == 2);
    }


    // ========================================
    // TC08. 잘못된 이동 요청
    // ========================================
    testTitle("TC08 - Invalid Requests");

    {
        Inventory player;
        Storage storage;

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(10);
        player.addItem(potion);

        AddResult a = storage.deposit(player, -1, 5);
        AddResult b = storage.deposit(player, 99, 5);
        AddResult c = storage.deposit(player, 0, -5);
        AddResult d = storage.deposit(player, 0, 0);
        AddResult e = storage.deposit(player, 0, 20);

        check("Negative index rejected", a.added == 0);
        check("Invalid index rejected", b.added == 0);
        check("Negative quantity rejected", c.added == 0);
        check("Zero quantity rejected", d.added == 0);
        check("Excess quantity rejected", e.added == 0);

        check("Player still has 10", getQuantity(player, 0) == 10);
        check("Storage empty", storage.getInventory().isEmpty());
    }


    // ========================================
    // TC09. 동일 ID 여러 슬롯에서 전체 이동
    // ========================================
    testTitle("TC09 - Remove Exact Slot");

    {
        Inventory player;
        Storage storage;

        player.setMaxStack(10);

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(25);
        player.addItem(potion);

        // 기존 슬롯: 10, 10, 5
        AddResult result = storage.deposit(player, 1, 10);

        check("Moved 10", result.added == 10);
        check("Player slots = 2", player.getItemCount() == 2);
        check("First slot = 10", getQuantity(player, 0) == 10);
        check("Second slot = 5", getQuantity(player, 1) == 5);
        check("Player total = 15", getTotalQuantity(player, 1) == 15);
        check("Storage total = 10", getTotalQuantity(storage.getInventory(), 1) == 10);
    }


    // ========================================
    // TC10. 용량 설정 유효성 검사
    // ========================================
    testTitle("TC10 - Capacity Settings");

    {
        Inventory player;

        Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
        potion.setQuantity(80);
        player.addItem(potion);

        check("Default slots = 100", player.getMaxSlots() == 100);
        check("Default stack = 100", player.getMaxStack() == 100);

        check("Set max slots to 200", player.setMaxSlots(200));
        check("Set max stack to 50 rejected", !player.setMaxStack(50));

        check("Max slots = 200", player.getMaxSlots() == 200);
        check("Max stack remains 100", player.getMaxStack() == 100);
        check("Invalid slots rejected", !player.setMaxSlots(0));
    }


    // ========================================
    // 최종 테스트 결과
    // ========================================
    cout << "\n========================================" << endl;
    cout << "FINAL TEST RESULT" << endl;
    cout << "========================================" << endl;

    cout << "PASSED : " << passed << endl;
    cout << "FAILED : " << failed << endl;
    cout << "TOTAL  : " << passed + failed << endl;

    if (failed == 0) {
        cout << "ALL TESTS PASSED!" << endl;
    }
    else {
        cout << "SOME TESTS FAILED!" << endl;
    }

    return failed == 0 ? 0 : 1;
}
