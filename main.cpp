#include "database.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    Database db("database.db");

    // Default to "get" if no argument is given
    std::string mode = (argc > 1) ? argv[1] : "get";

    if (mode == "put")
    {
        db.put("cat", "meow");
        db.put("dog", "woof");
        std::cout << "Wrote cat and dog to the database.\n";
    }
    else // "get"
    {
        std::optional<std::string> catTest = db.get("cat");
        if (catTest.has_value())
            std::cout << "cat = " << *catTest << '\n';
        else
            std::cout << "cat: key not found\n";

        std::optional<std::string> dogTest = db.get("dog");
        if (dogTest.has_value())
            std::cout << "dog = " << *dogTest << '\n';
        else
            std::cout << "dog: key not found\n";
    }

    return 0;
}