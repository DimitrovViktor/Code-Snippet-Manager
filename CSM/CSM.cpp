#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "sqlite/sqlite3.h"

std::string langInput(std::string& languageInput); // gets language from user

std::string tagInput(std::vector<std::string>& tagList, std::string& tagsCombined); // gets tags from user

std::string db_escape(const std::string& escapeText); // escapes db format

void db_search(std::ostringstream& sqlQueryStream);

void db_delete(std::ostringstream& sqlQueryStream);

void db_edit(std::ostringstream& sqlQueryStream);

int main()
{
    std::ostringstream sqlQueryStream;
    std::string sqlQueryString;

    std::string languageInput;
    std::vector<std::string> tagList;
    std::string tagsCombined;
    std::string codeLine;
    std::string codeSnippet;

    int menuChoice;

    char* errorMessage;
    sqlite3* db;
    sqlite3_stmt* statement;
    sqlite3_open("snippets.db", &db);
    int resultCode = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS SNIPPETS(id INTEGER PRIMARY KEY AUTOINCREMENT,code varchar(10000), language varchar(20), tags varchar(100));", NULL, NULL, &errorMessage);
    if (resultCode != SQLITE_OK)
    {
        std::cout << "error:" << errorMessage << std::endl;
    }
    else
    {
        std::cout << "File opened successfully.\n"
            << "Pick an option from the menu:\n"
            << "[1] Add snippet\n"
            << "[2] Search snippet\n"
            << "[3] Delete snippet\n"
            << "[4] Edit snippet\n"
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
                langInput(languageInput);

                tagInput(tagList, tagsCombined);

                sqlQueryStream.str("");
                sqlQueryStream << "INSERT INTO Snippets(code, language, tags) VALUES ('" << db_escape(codeSnippet) << "', '" << db_escape(languageInput) << "', '" << db_escape(tagsCombined) << "' )";
                sqlQueryString = sqlQueryStream.str();
                resultCode = sqlite3_exec(db, sqlQueryString.c_str(), NULL, NULL, &errorMessage);

            }


            return 0;
            break;

        case 2: // OPTION 2 (SEARCH SNIPPET)
            db_search(sqlQueryStream);
            sqlQueryString = sqlQueryStream.str();
            resultCode = sqlite3_prepare_v2(db, sqlQueryString.c_str(), -1, &statement, nullptr);
            while (sqlite3_step(statement) == SQLITE_ROW) {
                // Get column values
                const unsigned char* id = sqlite3_column_text(statement, 0);
                const unsigned char* code = sqlite3_column_text(statement, 1);
                const unsigned char* language = sqlite3_column_text(statement, 2);
                const unsigned char* tags = sqlite3_column_text(statement, 3);

                // Print results
                std::cout << "----------------------------- ID [" << (id ? reinterpret_cast<const char*>(id) : "") << "] -----------------------------\n";
                std::cout << "Snippet:\n" << (code ? reinterpret_cast<const char*>(code) : "") << "\n";
                std::cout << "Language: " << (language ? reinterpret_cast<const char*>(language) : "") << "\n";
                std::cout << "Tags: " << (tags ? reinterpret_cast<const char*>(tags) : "") << "\n";
            }

            break;

        case 3: // OPTION 3 (DELETE SNIPPET)
            db_delete(sqlQueryStream);
            sqlQueryString = sqlQueryStream.str();
            resultCode = sqlite3_exec(db, sqlQueryString.c_str(), NULL, NULL, nullptr);

            break;

        case 4: // OPTION 4 (EDIT SNIPPET)
            db_edit(sqlQueryStream);
            sqlQueryString = sqlQueryStream.str();
            resultCode = sqlite3_exec(db, sqlQueryString.c_str(), NULL, NULL, nullptr);

            break;

        }

    }
    sqlite3_close(db);
    std::cout << "\nAll complete, database closed." << std::endl;

}

std::string langInput(std::string& languageInput)
{
    std::cout << "Enter the programming language (e.g., C++, Python, Java): ";
    std::cin >> languageInput;
    return languageInput;
}

std::string tagInput(std::vector<std::string>& tagList, std::string& tagsCombined)
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
                tagList.push_back(temp);
            }
        }

    } while (tagCount < 1 || tagCount > tagMax);

    for (size_t k = 0; k < tagList.size(); k++) // Add tags
    {
        if (k == tagList.size() - 1)
        {
            tagsCombined += tagList[k];
        }
        else
        {
            tagsCombined += tagList[k] + ",";
        }
    }

    return tagsCombined;
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

void db_search(std::ostringstream& sqlQueryStream)
{

    std::string searchTerm;

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
        std::cin >> searchTerm;

        sqlQueryStream.str("");
        sqlQueryStream << " SELECT * FROM Snippets WHERE language LIKE '%" << searchTerm << "%'";

        break;

    case 2:
        std::cout << "You chose [2] Tag Search\n Pick a tag: ";
        std::cin >> searchTerm;

        sqlQueryStream.str("");
        sqlQueryStream << " SELECT * FROM Snippets WHERE tags LIKE '%" << searchTerm << "%'";

        break;

    case 3:
        std::cout << "You chose [3] Word Search\n Pick a word: ";
        std::cin >> searchTerm;

        sqlQueryStream.str("");
        sqlQueryStream << " SELECT * FROM Snippets WHERE code LIKE '%" << searchTerm << "%'";

        break;

    case 4:
        std::cout << "You chose [4] ID(number) search\n Pick an ID number: ";
        std::cin >> searchTerm;

        sqlQueryStream.str("");
        sqlQueryStream << " SELECT * FROM Snippets WHERE id =" << searchTerm;

        break;

    case 5:
        std::cout << "[5] Show all snippets\n";

        sqlQueryStream.str("");
        sqlQueryStream << "SELECT * FROM Snippets";

        break;
    }
}

void db_delete(std::ostringstream& sqlQueryStream)
{

    std::string deleteId;

    std::cout << "You chose to DELETE a snippet.\n Pick an ID number: ";
    std::cin >> deleteId;

    sqlQueryStream.str("");
    sqlQueryStream << " DELETE FROM Snippets WHERE id =" << deleteId;

}

void db_edit(std::ostringstream& sqlQueryStream)
{

    std::string editId;
    int editOption;
    std::string newCodeLine;
    std::string newCodeSnippet = "";
    std::vector<std::string> newTags;
    std::string editTags;
    std::string newLanguage;

    std::cout << "You chose to EDIT a snippet.\n Pick an ID number: ";
    std::cin >> editId;
    std::cout << "What would you like to edit?\n[1] Code replace\n[2] Tags replace\n[3] Language replace \nYour choice : ";
    std::cin >> editOption;

    sqlQueryStream.str("");
    sqlQueryStream << " UPDATE Snippets";
    switch (editOption) {
    case 1:
        std::cout << "You chose to REPLACE the CODE of snippet #" << editId << ".\n";
        std::cout << "Enter the new code snippet below (use doneEdit to finish editing):\n";

        while (std::getline(std::cin, newCodeLine))
        {
            std::string quitWord = "doneEdit";
            bool finishInputFound = newCodeLine.find(quitWord) != std::string::npos;


            if (finishInputFound)
            {
                break;
            }
            else
            {
                newCodeSnippet += newCodeLine;
                newCodeSnippet += "\n";
            }

        }
        sqlQueryStream << " SET code ='" << db_escape(newCodeSnippet) << "'";

        sqlQueryStream << " WHERE id ='" << editId << "'";
        break;

    case 2:
        std::cout << "You chose to REPLACE the TAGS of snippet #" << editId << ".\n";
        std::cout << "The tags will be reset and you can pick new ones now.\n";
        tagInput(newTags, editTags);

        sqlQueryStream << " SET tags ='" << db_escape(editTags) << "'";

        sqlQueryStream << " WHERE id ='" << editId << "'";

        break;

    case 3:
        std::cout << "You chose to REPLACE the LANGUAGE of snippet #" << editId << ".\n";
        langInput(newLanguage);

        sqlQueryStream << " SET language ='" << db_escape(newLanguage) << "'";
        sqlQueryStream << " WHERE id ='" << editId << "'";
        break;
    }

}
