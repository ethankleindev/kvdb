#include "database.h"
#include <cstdint>
#include <iostream>


Database::Database(const std::string& filename)
{
   fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if(!fileStream.is_open())
    {
        fileStream.clear();
        fileStream.open(filename, std::ios::out | std::ios::binary);
        fileStream.close();
        fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}
Database::~Database()
{
    fileStream.close();
}
void Database::put(const std::string& key, const std::string& value)
{
    fileStream.seekp(0, std::ios::end);
    nextOffset = fileStream.tellp();

    uint16_t keySize = key.size();
    fileStream.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));
    fileStream.write(key.data(), key.size());

    int32_t valueSize = value.size();
    fileStream.write(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
    fileStream.write(value.data(), value.size());
    data[key] = nextOffset;
}

std::optional<std::string> Database::get(const std::string& key)
{
    auto iter = data.find(key);
    if (iter == data.end())
    {
        return std::nullopt;
    }
    
    fileStream.seekg(iter->second, std::ios::beg);
    uint16_t keySize = 0;
    fileStream.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
    fileStream.seekg(keySize, std::ios::cur);

    int32_t valueSize = 0;
    fileStream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
    
    if (valueSize == -1) 
    {
        return std::nullopt;
    }
    std::string value(valueSize, '\0');
    fileStream.read(value.data(), valueSize);
    return value;
}

 void Database::erase(const std::string& key)
 {
    fileStream.seekp(0, std::ios::end);
    
    uint16_t keySize = key.size();
    fileStream.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));
    fileStream.write(key.data(), key.size());

    int32_t tombstone = -1;
    fileStream.write(reinterpret_cast<char*>(&tombstone), sizeof(tombstone));
    data.erase(key);
 }