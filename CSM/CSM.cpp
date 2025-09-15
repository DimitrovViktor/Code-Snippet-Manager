#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::string langInput(std::string& userLang); // gets language from user

std::vector<std::string> tagInput(std::vector<std::string>& userTags); // gets tags from user

int main(const int argc, char* const argv[])
{

    // Create, open, append command line arguments to file
    std::ofstream outputFile("snippets.txt", std::ios::app);
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
            os << "\n\nLanguage: " << userLang << "\n"; // Add language and tags to output file
            os << "Tags(" << userTags.size() << "): ";
            for (auto& s : userTags) {
                os << s << " ";
            }
			os << "\n\n";
            for (int i = 1; i < argc; i++)
            {
                std::cout << "Argument " << i << ": " << argv[i] << "\n";
                os << argv[i]; // join arguments
                if (i < argc - 1) os << " ";
            }
        }
        else
        {
            std::cout << "No argument was provided " << "\n Usage: CSM.exe <snippet>" << std::endl;
		    outputFile.close();
            return 1;
        }

        outputFile << os.str();
        outputFile << "\n\n";
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

    std::cout << "How many tags would you like to add? ";
    std::cin >> tagCount;

    for (int j = 0; j < tagCount; j++)
    {
        std::cout << "Enter tag #" << j + 1 << " : ";
        std::cin >> temp;
        userTags.push_back(temp);
    }
    return userTags;
}