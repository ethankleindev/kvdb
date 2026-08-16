#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
#include <fstream>
#include <optional>



class Database
{
private:
    std::string fileName;
    std::map<std::string, std::streamoff> index;
    std::streamoff nextOffset;
    std::fstream fileStream;
public:
    Database(const std::string&);
    ~Database();
    void put(const std::string&, const std::string&);
    std::optional<std::string> get(const std::string&);
    void erase(const std::string&);
    void compact();
};

#endif