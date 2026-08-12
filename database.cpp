#include "database.h"
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

}
void Database::put(const std::string&, const std::string&)
{

}

std::string Database::get(const std::string&)
{
    return "";
}

 void Database::erase(const std::string&)
 {

 }