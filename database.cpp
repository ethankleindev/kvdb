#include "database.h"
#include <cstdint>
#include <filesystem>
#include <iostream>


Database::Database(const std::string& fname)
{
    // file open
    fileName = fname;
    fileStream.open(fname, std::ios::in | std::ios::out | std::ios::binary);
    if(!fileStream.is_open())
    {
        fileStream.clear();
        fileStream.open(fname, std::ios::out | std::ios::binary);
        fileStream.close();
        fileStream.open(fname, std::ios::in | std::ios::out | std::ios::binary);
    }

    // Index rebuild
    fileStream.seekg(0, std::ios::beg);
    uint16_t keySize = 0;
    int32_t valueSize = 0;
    std::streamoff currentOffset = 0;

    while (true)
    {
        currentOffset = fileStream.tellg();
        fileStream.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        if (fileStream.fail())
        {
            break;
        }
        std::string key(keySize, '\0');
        fileStream.read(key.data(), keySize);   

        fileStream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        if (valueSize == -1)
        {
            index.erase(key);
        }
        if (valueSize != -1)
        {
            index[key] = currentOffset;
            fileStream.seekg(valueSize, std::ios::cur);
        }
    }
    fileStream.clear();
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
    index[key] = nextOffset;
}

std::optional<std::string> Database::get(const std::string& key)
{
    auto iter = index.find(key);
    if (iter == index.end())
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
    index.erase(key);
 }

 void Database::compact()
 {
    std::fstream tempFileStream;
    std::string tempFileName = fileName + ".tmp";
    std::streamoff tempOffset = 0;
    std::map<std::string, std::streamoff> tempIndex;
    
    tempFileStream.open(tempFileName, std::ios::out | std::ios::binary);
    auto iter = index.begin();
    while (iter != index.end())
    {
        fileStream.seekg(iter->second, std::ios::beg);
        uint16_t keySize = 0;
        fileStream.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        fileStream.seekg(keySize, std::ios::cur);
        
        int32_t valueSize = 0;
        fileStream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        std::string value(valueSize, '\0');
        fileStream.read(value.data(), valueSize);
        tempOffset = tempFileStream.tellp();
        std::cout << "compact: " << iter->first << " -> " << tempOffset << '\n';

        tempFileStream.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        tempFileStream.write(iter->first.data(), iter->first.size());
        tempFileStream.write(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        tempFileStream.write(value.data(), value.size());
        tempIndex[iter->first] = tempOffset;
        ++iter;
    }

    tempFileStream.close();
    fileStream.close();
    std::filesystem::remove(fileName);
    std::filesystem::rename(tempFileName,fileName);
    fileStream.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    index = tempIndex;
    std::cout << "index size after compact: " << index.size() << '\n';
 }