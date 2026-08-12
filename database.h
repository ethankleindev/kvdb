#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
#include <fstream>


class Database
{
private:
    std::map<std::string, std::streamoff> data;
    std::streamoff nextOffset;
    std::fstream fileStream;
public:
    Database(const std::string&);
    ~Database();
    void put(const std::string&, const std::string&);
    std::string get(const std::string&);
    void erase(const std::string&);
};

#endif