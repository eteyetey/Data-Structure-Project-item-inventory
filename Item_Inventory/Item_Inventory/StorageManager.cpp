
#include "StorageManager.h"
#include "FileManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <limits>
#include <vector>
#include <utility>

using namespace std;
namespace fs = std::filesystem;


// 생성자가 정상적인 보관함만 생성하도록 검사
int StorageManager::createStorage(const string& name, int maxSlots, int maxStack) {

    //이름 형식이 잘못되었는지 검사
    if (name.empty() || name.find('|') != string::npos ||
        name.find('\n') != string::npos || name.find('\r') != string::npos) {
        return -1;
    }

    //공간 형식이 잘못되었는지 검사
    if (maxSlots <= 0 || maxStack <= 0 || nextId <= 0 ||
        nextId == numeric_limits<int>::max()) {
        return -1;
    }

    int id = nextId;

    // 유니크 포인터 사용, Storage 객체의 복사를 방지하고 메모리를 자동으로 해제하여 메모리 누수 및 이중 해제 방지
    unique_ptr<Storage> storage = make_unique<Storage>(id, name);

    if (!storage->setMaxSlots(maxSlots) || !storage->setMaxStack(maxStack)) {
        return -1;
    }

    //정상적으로 처리된 스토리지를 map에 추가후 id하나 늘림
    storages.emplace(id, move(storage));
    nextId++;

    return id;
}


// 보관함 삭제
bool StorageManager::removeStorage(int id) {
    //map에서 스토리지 삭제
    return storages.erase(id) > 0;
}


// 보관함 조회
Storage* StorageManager::getStorage(int id) {

    auto it = storages.find(id);

    if (it == storages.end()) {
        return nullptr;
    }

    //map에서 스토리지 조회
    return it->second.get();
}


// 보관함 이름 변경
bool StorageManager::renameStorage(int id, const string& name) {

    Storage* storage = getStorage(id);

    if (storage == nullptr || name.empty() ||
        name.find('|') != string::npos ||
        name.find('\n') != string::npos ||
        name.find('\r') != string::npos) {
        return false;
    }

    storage->setName(name);
    return true;
}


// 보관함 개수 반환
int StorageManager::getStorageCount() const {
    return static_cast<int>(storages.size());
}


// 보관함 목록 출력
void StorageManager::printAll() const {

    cout << "\n===== Storage List =====" << endl;

    if (storages.empty()) {
        cout << "No storages." << endl;
        return;
    }

    for (const auto& entry : storages) {

        const Storage& storage = *entry.second;

        cout << "ID: " << storage.getId()
            << " | Name: " << storage.getName()
            << endl;
    }
}


// 전체 저장
bool StorageManager::saveAll(const string& folderPath) {

    try {
        fs::path folder(folderPath);

        if (fs::exists(folder) && !fs::is_directory(folder)) {
            return false;
        }

        fs::create_directories(folder);

        // 각 보관함의 아이템 데이터 저장
        for (const auto& entry : storages) {

            int id = entry.first;

            fs::path itemPath = folder / ("storage_" + to_string(id) + ".txt");

            if (!FileManager::saveInventory(itemPath.string(),
                entry.second->getInventory())) {
                return false;
            }
        }

        // 보관함 목록은 아이템 파일 저장 후 작성
        fs::path listPath = folder / "storage_list.txt";
        ofstream file(listPath);

        if (!file.is_open()) {
            return false;
        }

        // 다음 ID 기록
        file << "NEXT_ID|" << nextId << "\n";
        file << "ID|NAME|MAX_SLOTS|MAX_STACK\n";

        for (const auto& entry : storages) {

            Storage& storage = *entry.second;
            Inventory& inventory = storage.getInventory();

            file << storage.getId() << "|"
                << storage.getName() << "|"
                << inventory.getMaxSlots() << "|"
                << inventory.getMaxStack() << "\n";
        }

        file.close();
        return !file.fail();
    }
    catch (const fs::filesystem_error&) {
        return false;
    }
}


// 전체 불러오기
bool StorageManager::loadAll(const string& folderPath) {

    try {
        fs::path folder(folderPath);
        ifstream file(folder / "storage_list.txt");

        if (!file.is_open()) {
            return false;
        }

        string line;

        if (!getline(file, line)) {
            return false;
        }

        // NEXT_ID 파싱
        size_t pos = line.find('|');

        if (pos == string::npos || line.substr(0, pos) != "NEXT_ID") {
            return false;
        }

        int loadedNextId;
        size_t parsed = 0;

        try {
            loadedNextId = stoi(line.substr(pos + 1), &parsed);

            if (parsed != line.size() - pos - 1 || loadedNextId <= 0) {
                return false;
            }
        }
        catch (const exception&) {
            return false;
        }

        // 헤더 읽기
        if (!getline(file, line) ||
            line != "ID|NAME|MAX_SLOTS|MAX_STACK") {
            return false;
        }

        map<int, unique_ptr<Storage>> loadedStorages;
        int maxId = 0;

        while (getline(file, line)) {

            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            vector<string> fields;
            string field;

            while (getline(ss, field, '|')) {
                fields.push_back(field);
            }

            if (fields.size() != 4) {
                return false;
            }

            int id, maxSlots, maxStack;

            try {
                size_t p1 = 0, p2 = 0, p3 = 0;

                id = stoi(fields[0], &p1);
                maxSlots = stoi(fields[2], &p2);
                maxStack = stoi(fields[3], &p3);

                if (p1 != fields[0].size() ||
                    p2 != fields[2].size() ||
                    p3 != fields[3].size()) {
                    return false;
                }
            }
            catch (const exception&) {
                return false;
            }

            if (id <= 0 || fields[1].empty() ||
                maxSlots <= 0 || maxStack <= 0 ||
                loadedStorages.count(id) > 0) {
                return false;
            }

            unique_ptr<Storage> storage = make_unique<Storage>(id, fields[1]);

            if (!storage->setMaxSlots(maxSlots) ||
                !storage->setMaxStack(maxStack)) {
                return false;
            }

            fs::path itemPath = folder / ("storage_" + to_string(id) + ".txt");

            if (!FileManager::loadInventory(itemPath.string(),
                storage->getInventory())) {
                return false;
            }

            loadedStorages.emplace(id, move(storage));

            if (id > maxId) {
                maxId = id;
            }
        }

        if (file.bad() || loadedNextId <= maxId) {
            return false;
        }

        // 모든 데이터가 정상일 때만 기존 보관함 교체
        storages.swap(loadedStorages);
        nextId = loadedNextId;

        return true;
    }
    catch (const fs::filesystem_error&) {
        return false;
    }
}
