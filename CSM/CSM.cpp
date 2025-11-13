#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "sqlite/sqlite3.h"

std::string langInput(std::string& userLang); // gets language from user

std::vector<std::string> tagInput(std::vector<std::string>& userTags); // gets tags from user

std::string db_escape(const std::string& escapeText); // escapes db format

std::string db_search(std::ostringstream& testInput);

int main()
{
    std::ostringstream testInput;
    std::string testInsert;

    std::string userLang;
    std::vector<std::string> userTags;
    std::string AllTags;
    std::string codeLine;
    std::string codeSnippet;

    int menuChoice;

    char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("snippets.db", &db);
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS SNIPPETS(id INTEGER PRIMARY KEY AUTOINCREMENT,code varchar(10000), language varchar(20), tags varchar(100));", NULL, NULL, &err);
    if (rc != SQLITE_OK)
    {
        std::cout << "error:" << err << std::endl;
    }
    else
    {
        std::cout << "File opened successfully.\n"
            << "Pick an option from the menu:\n"
            << "[1] Add snippet\n"
            << "[2] Search snippet\n"
            << "Your choice: ";
        std::cin >> menuChoice;

        switch (menuChoice)
        {
        case 1: // OPTION 1 (ADD SNIPPET)
            std::cout << "You chose [1] Add snippet \nEnter code snippet: ";

            while (std::getline(std::cin, codeLine))
            {
                std::string quitWord = "doneSnippet";
                bool finishInputFound = codeLine.find(quitWord) != std::string::npos;


                if (finishInputFound)
                {
                    break;
                }
                else
                {
                    codeSnippet += codeLine;
                    codeSnippet += "\n";
                }

            }


            std::cout << "Your Input: \n" << codeSnippet << "\n";


            if (codeSnippet.empty())
            {
                std::cout << "No argument was provided" << std::endl;
                sqlite3_close(db);
                return 1;
            }
            else
            {
                langInput(userLang);

                tagInput(userTags);
                

                for (size_t k = 0; k < userTags.size(); k++) // Add tags
                {
                    if (k == userTags.size() - 1)
                    {
                        AllTags += userTags[k];
                    }
                    else
                    {
                        AllTags += userTags[k] + ",";
                    }
                }

                testInput.str("");
                testInput << "INSERT INTO Snippets(code, language, tags) VALUES ('" << db_escape(codeSnippet) << "', '" << db_escape(userLang) << "', '" << db_escape(AllTags) << "' )";
                testInsert = testInput.str();
                rc = sqlite3_exec(db, testInsert.c_str(), NULL, NULL, &err);

            }


            return 0;
            break;

        case 2: // OPTION 2 (SEARCH SNIPPET)
            db_search(testInput);
            testInsert = testInput.str();
            rc = sqlite3_prepare_v2(db, testInsert.c_str(), -1, &stmt, nullptr);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // Get column values
                const unsigned char* id = sqlite3_column_text(stmt, 0);
                const unsigned char* code = sqlite3_column_text(stmt, 1);
                const unsigned char* language = sqlite3_column_text(stmt, 2);
                const unsigned char* tags = sqlite3_column_text(stmt, 3);

                // Print results
                std::cout << "----------------------------- ID [" << (id ? reinterpret_cast<const char*>(id) : "") << "] -----------------------------\n";
                std::cout << "Snippet:\n" << (code ? reinterpret_cast<const char*>(code) : "") << "\n";
                std::cout << "Language: " << (language ? reinterpret_cast<const char*>(language) : "") << "\n";
                std::cout << "Tags: " << (tags ? reinterpret_cast<const char*>(tags) : "") << "\n";
            }

            break;

        }

    }
    sqlite3_close(db);
 
}

std::string langInput(std::string& userLang)
{
    std::cout << "Enter the programming language (e.g., C++, Python, Java): ";
    std::cin >> userLang;
    return userLang;
}

std::vector<std::string> tagInput(std::vector<std::string>& userTags)
{
    std::string temp;
    int tagCount{ 0 };
    int tagMax{ 10 };

    std::cout << "How many tags would you like to add?(max " << tagMax << "): ";
    std::cin >> tagCount;

    do
    {

        if (tagCount < 1)
        {
            std::cout << "No tags will be added." << std::endl;
            return userTags;
        }
        else if (tagCount > tagMax)
        {
            std::cout << "Maximum number of tags is: " << tagMax << "\n";
            std::cout << "Enter a number below or equal to " << tagMax << ": ";
            std::cin >> tagCount;
        }
        else
        {
            for (int j = 0; j < tagCount; j++)
            {
                std::cout << "Enter tag #" << j + 1 << " : ";
                std::cin >> temp;
                userTags.push_back(temp);
            }
        }

    } while (tagCount < 1 || tagCount > tagMax);

    return userTags;
}

std::string db_escape(const std::string& escapeText) {
    // If field contains comma, quote, or newline, enclose in quotes
    if (//escapeText.find(',') != std::string::npos ||
        escapeText.find("'") != std::string::npos
        // escapeText.find('\n') != std::string::npos
        ) {

        std::string escaped;
        escaped += "'";

        for (char c : escapeText) {
            if (c == '\'') {
                escaped += "''";
            }
            else {
                escaped += c;
            }
        }

        escaped += '"';
        return escaped;
    }
    return escapeText;
}

std::string db_search(std::ostringstream& testInput)
{

    std::vector<std::string> record;


    std::ifstream readFile("snippets.csv");

    bool found_record = false;

    std::string field_one;
    std::string field_two;
    std::string field_three;

    std::string search_term;

    std::cout << "You chose [2] Search snippet\n"
              << "Would you like to search by: \n"
              << "[1] Language\n"
              << "[2] Tag(s)\n"
              << "[3] Word(inside of code) search\n"
              << "[4] ID(number) search\n"
              << "[5] Show all snippets\n"
              << "Your choice: ";

    int searchChoice;
    std::cin >> searchChoice;

    switch (searchChoice)
        {
            case 1:
                std::cout << "You chose [1] Language Search\n Pick a language: ";
                std::cin >> search_term;

                testInput.str("");
                testInput << " SELECT * FROM Snippets WHERE language LIKE '%" << search_term << "%'";

                break;

            case 2:
                std::cout << "You chose [2] Tag Search\n Pick a tag: ";
                std::cin >> search_term;

                testInput.str("");
                testInput << " SELECT * FROM Snippets WHERE tags LIKE '%" << search_term << "%'";
                
                break;

            case 3:
                std::cout << "You chose [3] Word Search\n Pick a word: ";
                std::cin >> search_term;
                
                testInput.str("");
                testInput << " SELECT * FROM Snippets WHERE code LIKE '%" << search_term << "%'";

                break;

            case 4:
                std::cout << "You chose [4] ID(number) search\n Pick an ID number: ";
                std::cin >> search_term;

                testInput.str("");
                testInput << " SELECT * FROM Snippets WHERE id =" << search_term;

                break;

            case 5:
                std::cout << "[5] Show all snippets\n";

                testInput.str("");
                testInput << "SELECT * FROM Snippets";

                break;
        }
}
