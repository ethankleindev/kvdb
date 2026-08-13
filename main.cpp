#include "database.h"
#include <iostream>

int main()
{
    Database db("database.db");
    db.put("cat", "meow");
    std::optional<std::string> catTest = db.get("cat");
    if (catTest.has_value())
    {
        std::cout << *catTest << '\n';
    }
    else
    {
        std::cout << "key not found\n";
    }

    std::optional<std::string> dogTest = db.get("dog");
    if (dogTest.has_value())
    {
        std::cout << *dogTest << '\n';
    }
    if (!dogTest.has_value())
    {
        std::cout << "key not found\n";
    }

    return 0;
}