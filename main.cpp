#include "database.h"

int main()
{
    Database db("database.db");
    db.put("cat", "meow");
    return 0;
}