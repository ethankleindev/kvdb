#include "database.h"
#include <iostream>
#include <filesystem>

// Helper: print a get result on one line
void showGet(Database& db, const std::string& key)
{
    std::optional<std::string> result = db.get(key);
    if (result.has_value())
        std::cout << "  " << key << " = " << *result << '\n';
    else
        std::cout << "  " << key << ": key not found\n";
}

// Helper: print the current size of the database file in bytes
void showFileSize(const std::string& path)
{
    if (std::filesystem::exists(path))
        std::cout << "  [file size: " << std::filesystem::file_size(path) << " bytes]\n";
    else
        std::cout << "  [file does not exist]\n";
}

int main()
{
    // Start from a clean slate so the run is reproducible
    std::filesystem::remove("database.db");

    Database db("database.db");

    std::cout << "=== 1. Insert initial records ===\n";
    db.put("cat", "meow");
    db.put("dog", "woof");
    db.put("bird", "tweet");
    db.put("cow", "moo");
    showGet(db, "cat");
    showGet(db, "dog");
    showGet(db, "bird");
    showGet(db, "cow");
    showFileSize("database.db");

    std::cout << "\n=== 2. Update some keys (creates dead records) ===\n";
    db.put("cat", "purr");      // supersedes "meow"
    db.put("dog", "bark");      // supersedes "woof"
    db.put("cat", "hiss");      // supersedes "purr" too
    showGet(db, "cat");         // should be hiss (newest)
    showGet(db, "dog");         // should be bark
    showFileSize("database.db");

    std::cout << "\n=== 3. Delete a key (adds a tombstone) ===\n";
    db.erase("bird");
    showGet(db, "bird");        // should be not found
    showGet(db, "cow");         // should still be moo
    showFileSize("database.db");

    std::cout << "\n=== 4. Test missing key ===\n";
    showGet(db, "fish");        // never inserted

    std::cout << "\n=== 5. Compact (reclaim dead space) ===\n";
    db.compact();
    showFileSize("database.db"); // should be smaller

    std::cout << "\n=== 6. Verify all data survived compaction ===\n";
    showGet(db, "cat");         // hiss
    showGet(db, "dog");         // bark
    showGet(db, "bird");        // not found (was deleted)
    showGet(db, "cow");         // moo
    showGet(db, "fish");        // not found (never existed)

    std::cout << "\n=== 7. Insert after compaction (verify DB still writable) ===\n";
    db.put("fox", "ring-ding");
    showGet(db, "fox");         // ring-ding
    showGet(db, "cat");         // hiss (still there)
    showFileSize("database.db");

    return 0;
}