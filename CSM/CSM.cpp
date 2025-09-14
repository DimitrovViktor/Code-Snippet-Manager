#include <iostream>
#include <fstream>
#include <sstream>

int main(const int argc, char* const argv[])
{

    // Create, open, append command line arguments to file
    std::ofstream outputFile("snippets.txt", std::ios::app);
    std::ostringstream os;

    if (outputFile.is_open())
    {
        std::cout << "File opened successfully.\n";
        if (argc > 1)
        {
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

