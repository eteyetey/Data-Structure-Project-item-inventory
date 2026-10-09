
#pragma once

#include <map>
#include <memory>
#include <string>
#include "Storage.h"

using namespace std;

class StorageManager {
private:
    //현재 존재하는 스토리지들(id로 검색가능)
    map<int, unique_ptr<Storage>> storages;
    int nextId = 1;

public:
    // 보관함 생성 및 삭제
    int createStorage(const string& name, int maxSlots = 100, int maxStack = 100);
    bool removeStorage(int id);

    // 보관함 조회 및 이름 변경
    Storage* getStorage(int id);
    bool renameStorage(int id, const string& name);

    // 보관함 개수 및 목록 출력
    int getStorageCount() const;
    void printAll() const;

    // 전체 보관함 저장 및 불러오기
    bool saveAll(const string& folderPath);
    bool loadAll(const string& folderPath);
};
