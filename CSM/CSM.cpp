#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "sqlite/sqlite3.h"

std::string langInput(std::string& userLang); // gets language from user

std::vector<std::string> tagInput(std::vector<std::string>& userTags); // gets tags from user

std::string db_escape(const std::string& escapeText); // escapes db format

std::string db_search();

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
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Snippets(code varchar(10000), language varchar(20), tags varchar(100));", NULL, NULL, &err);
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
            db_search();
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

std::string db_search()
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
              << "Your choice: ";

    int searchChoice;
    std::cin >> searchChoice;

    switch (searchChoice)
        {
            case 1:
                std::cout << "You chose [1] Language Search\n Pick a language: ";
                std::cin >> search_term;

                while (getline(readFile, field_one, ',') && !found_record)
                {
                    getline(readFile, field_two, ',');
                    getline(readFile, field_three, '\n');

                    if (field_two == search_term)
                    {
                        found_record = true;
                        record.push_back(field_one);
                        record.push_back(field_two);
                        record.push_back(field_three);
                    }
                }
                std::cout << "Language: " << record[1] << "\n";

                std::cout << "Tag(s): " << record[2] << "\n";

                std::cout << "Snippet: " << record[0] << "\n";
                break;
            case 2:
                std::cout << "You chose [2] Tag Search\n Pick a tag: ";
                std::cin >> search_term;

                while (getline(readFile, field_three, ',') && !found_record)
                {
                    getline(readFile, field_three, '\n');
                    getline(readFile, field_one, ',');
                    getline(readFile, field_two, ',');
                    
                    std::size_t found1 = field_three.find(search_term);

                    if (found1!=std::string::npos)
                    {
                        found_record = true;
                        record.push_back(field_one);
                        record.push_back(field_two);
                        record.push_back(field_three);
                    }
                }
                std::cout << "Language: " << record[1] << "\n";

                std::cout << "Tag(s): " << record[2] << "\n";
                
                std::cout << "Snippet: " << record[0] << "\n";
                
                break;
            case 3:
                std::cout << "You chose [3] Word Search\n Pick a word: ";
                std::cin >> search_term;

                break;
        }
}
