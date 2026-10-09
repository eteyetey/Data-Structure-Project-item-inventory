
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "Item.h"
#include "Inventory.h"
#include "Storage.h"
#include "StorageManager.h"

using namespace std;
namespace fs = std::filesystem;

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


// 특정 아이템의 전체 수량 조회
int getTotalQuantity(Inventory& inventory, int id) {

    int total = 0;
    vector<Item*> items = inventory.findItemsById(id);

    for (Item* item : items) {
        total += item->getQuantity();
    }

    return total;
}


// 파일 존재 여부 확인
bool fileExists(const fs::path& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}


int main() {

    // 기존 프로젝트 파일과 충돌하지 않는 별도의 테스트 경로
    fs::path root = "storage_manager_auto_test_20261009";
    fs::path savePath = root / "storages";
    fs::path invalidPath = root / "invalid";
    fs::path emptyPath = root / "empty";

    // 이전 테스트 파일이 있으면 덮어쓰지 않음
    if (fs::exists(root)) {
        cout << "Test folder already exists." << endl;
        cout << "Remove or rename: " << root.string() << endl;
        return 1;
    }

    fs::create_directories(root);

    cout << "Test folder: " << fs::absolute(root).string() << endl;

    StorageManager manager;
    Inventory player;

    // ========================================
    // TC01. 여러 보관함 생성
    // ========================================
    testTitle("TC01 - Create Storages");

    int id1 = manager.createStorage("Wooden Chest");
    int id2 = manager.createStorage("Iron Chest", 2, 5);
    int id3 = manager.createStorage("Large Chest", 200, 50);

    check("Chest 1 ID = 1", id1 == 1);
    check("Chest 2 ID = 2", id2 == 2);
    check("Chest 3 ID = 3", id3 == 3);
    check("Storage count = 3", manager.getStorageCount() == 3);
    check("Chest 2 max slots = 2",
        manager.getStorage(id2)->getInventory().getMaxSlots() == 2);
    check("Chest 2 max stack = 5",
        manager.getStorage(id2)->getInventory().getMaxStack() == 5);


    // ========================================
    // TC02. 보관함 조회
    // ========================================
    testTitle("TC02 - Find Storages");

    check("Find Chest 1", manager.getStorage(id1) != nullptr);
    check("Find Chest 2", manager.getStorage(id2) != nullptr);
    check("Invalid ID returns nullptr", manager.getStorage(999) == nullptr);


    // ========================================
    // TC03. 이름 변경
    // ========================================
    testTitle("TC03 - Rename Storage");

    check("Rename Chest 1",
        manager.renameStorage(id1, "My Chest"));
    check("Reject invalid name",
        !manager.renameStorage(id1, "Invalid|Name"));
    check("Name remains My Chest",
        manager.getStorage(id1)->getName() == "My Chest");


    // ========================================
    // TC04. 서로 다른 보관함에 아이템 넣기
    // ========================================
    testTitle("TC04 - Deposit Items");

    Item potion(1, "Potion", "Food", 1, ItemType::Consumable);
    potion.setQuantity(12);

    Item sword(2, "Sword", "Weapon", 3, ItemType::Durability, 7, 10);

    player.addItem(potion);
    player.addItem(sword);

    Storage* chest1 = manager.getStorage(id1);
    Storage* chest2 = manager.getStorage(id2);

    AddResult deposit1 = chest1->deposit(player, 0, 7);
    AddResult deposit2 = chest2->deposit(player, 1, 1);

    check("Deposit 7 potions", deposit1.added == 7);
    check("Player has 5 potions", getTotalQuantity(player, 1) == 5);
    check("Chest 1 has 7 potions",
        getTotalQuantity(chest1->getInventory(), 1) == 7);
    check("Deposit sword", deposit2.added == 1);
    check("Chest 2 sword durability = 7",
        chest2->getInventory().getItem(0) != nullptr &&
        chest2->getInventory().getItem(0)->getDurability() == 7);


    // ========================================
    // TC05. 보관함에서 아이템 꺼내기
    // ========================================
    testTitle("TC05 - Withdraw Items");

    AddResult withdraw = chest1->withdraw(player, 0, 3);

    check("Withdraw 3 potions", withdraw.added == 3);
    check("Player has 8 potions", getTotalQuantity(player, 1) == 8);
    check("Chest 1 has 4 potions",
        getTotalQuantity(chest1->getInventory(), 1) == 4);


    // ========================================
    // TC06. 보관함 삭제와 ID 재사용 방지
    // ========================================
    testTitle("TC06 - Remove Storage");

    check("Remove Chest 3", manager.removeStorage(id3));
    check("Chest 3 no longer exists", manager.getStorage(id3) == nullptr);
    check("Storage count = 2", manager.getStorageCount() == 2);

    int id4 = manager.createStorage("New Chest");

    check("New Chest ID = 4", id4 == 4);


    // ========================================
    // TC07. 보관함 파일 저장
    // ========================================
    testTitle("TC07 - Save All");

    bool saved = manager.saveAll(savePath.string());

    check("Save successful", saved);
    check("Storage list file exists",
        fileExists(savePath / "storage_list.txt"));
    check("Storage 1 file exists",
        fileExists(savePath / "storage_1.txt"));
    check("Storage 2 file exists",
        fileExists(savePath / "storage_2.txt"));
    check("Storage 4 file exists",
        fileExists(savePath / "storage_4.txt"));


    // ========================================
    // TC08. 새로운 매니저에서 파일 불러오기
    // ========================================
    testTitle("TC08 - Load All");

    StorageManager loadedManager;

    bool loaded = loadedManager.loadAll(savePath.string());

    check("Load successful", loaded);
    check("Loaded storage count = 3",
        loadedManager.getStorageCount() == 3);

    Storage* loaded1 = loadedManager.getStorage(id1);
    Storage* loaded2 = loadedManager.getStorage(id2);

    check("Loaded Chest 1 name",
        loaded1 != nullptr && loaded1->getName() == "My Chest");
    check("Loaded Chest 1 has 4 potions",
        loaded1 != nullptr &&
        getTotalQuantity(loaded1->getInventory(), 1) == 4);
    check("Loaded Chest 2 sword durability = 7",
        loaded2 != nullptr &&
        loaded2->getInventory().getItem(0) != nullptr &&
        loaded2->getInventory().getItem(0)->getDurability() == 7);


    // ========================================
    // TC09. 불러온 후 ID 유지
    // ========================================
    testTitle("TC09 - Next ID After Load");

    int id5 = loadedManager.createStorage("After Load");

    check("Next ID = 5", id5 == 5);
    check("Storage count = 4", loadedManager.getStorageCount() == 4);


    // ========================================
    // TC10. 개별 파일이 누락된 경우
    // ========================================
    testTitle("TC10 - Missing Storage File");

    bool removedFile = fs::remove(savePath / "storage_2.txt");

    check("Test file removed", removedFile);

    int countBefore = loadedManager.getStorageCount();

    bool missingLoad = loadedManager.loadAll(savePath.string());

    check("Load fails when file missing", !missingLoad);
    check("Existing storages preserved",
        loadedManager.getStorageCount() == countBefore);


    // ========================================
    // TC11. 비정상적인 목록 파일
    // ========================================
    testTitle("TC11 - Invalid Storage List");

    fs::create_directories(invalidPath);

    {
        ofstream file(invalidPath / "storage_list.txt");
        file << "INVALID_DATA\n";
    }

    bool invalidLoad = loadedManager.loadAll(invalidPath.string());

    check("Invalid list rejected", !invalidLoad);
    check("Existing data preserved",
        loadedManager.getStorage(id5) != nullptr);


    // ========================================
    // TC12. 비어 있는 매니저 저장 및 복원
    // ========================================
    testTitle("TC12 - Empty Storage Manager");

    StorageManager emptyManager;

    bool emptySaved = emptyManager.saveAll(emptyPath.string());

    StorageManager restoredEmpty;
    bool emptyLoaded = restoredEmpty.loadAll(emptyPath.string());

    check("Save empty manager", emptySaved);
    check("Load empty manager", emptyLoaded);
    check("Restored manager is empty",
        restoredEmpty.getStorageCount() == 0);


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
