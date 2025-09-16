#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::string langInput(std::string& userLang); // gets language from user

std::vector<std::string> tagInput(std::vector<std::string>& userTags); // gets tags from user

int main(const int argc, char* const argv[])
{

    // Create, open, append command line arguments to file
    std::ofstream outputFile("snippets.csv", std::ios::app);
    std::ostringstream os;

    std::string userLang;
    std::vector<std::string> userTags;


    if (outputFile.is_open())
    {
        std::cout << "File opened successfully.\n";

        langInput(userLang);
        tagInput(userTags);

        if (argc > 1)
        {
            os << userLang << ","; // Add language
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
                << "\"";
            for (int i = 1; i < argc; i++)
            {
                std::cout << "Argument " << i << ": " << argv[i] << "\n";
                os << argv[i]; // join arguments
                if (i < argc - 1) os << " ";
            }
            os << "\"";
        }
        else
        {
            std::cout << "No argument was provided " << "\n Usage: CSM.exe <snippet>" << std::endl;
            outputFile.close();
            return 1;
        }

        outputFile << os.str();
        outputFile << "\n";
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