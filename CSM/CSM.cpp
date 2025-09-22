#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::string langInput(std::string& userLang); // gets language from user

std::vector<std::string> tagInput(std::vector<std::string>& userTags); // gets tags from user

std::string csv_escape(const std::string& escapeText); // escapes csv format

int main()
{

    // Create, open, append command line arguments to file
    std::ofstream outputFile("snippets.csv", std::ios::app);
    std::ostringstream os;

    std::string userLang;
    std::vector<std::string> userTags;
    std::string codeLine;
    std::string codeSnippet;


    if (outputFile.is_open())
    {
        std::cout << "File opened successfully.\n";

        std::cout << "Enter code snippet: ";

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
            outputFile.close();
            return 1;
        }
        else
        {
            os << csv_escape(codeSnippet);
            os << ",";

            langInput(userLang);
            os << userLang << ","; // Add language

            tagInput(userTags);
            std::string AllTags;

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

            os << "\"" << AllTags << "\""
                << ","
                << "\""
                << "\n";
        }
        outputFile << os.str();
        outputFile.close();

        return 0;
    }
    else
    {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }



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

std::string csv_escape(const std::string& escapeText) {
    // If field contains comma, quote, or newline, enclose in quotes
    if (escapeText.find(',') != std::string::npos ||
        escapeText.find('"') != std::string::npos ||
        escapeText.find('\n') != std::string::npos
        ) {

        std::string escaped;
        escaped += '"';

        for (char c : escapeText) {
            if (c == '"') {
                escaped += "\"\"";
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