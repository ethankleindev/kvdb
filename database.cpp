#include "database.h"
#include <iostream>
#include <cstdint>

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

std::string Database::get(const std::string&)
{
    return "";
}

 void Database::erase(const std::string&)
 {

 }