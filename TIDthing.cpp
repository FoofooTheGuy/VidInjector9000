/*
  C++ program to convert hexadecimal string to decimal for Vidinjector9000
  argv[1] = input string
  input exactly "0" to generate a random hex string from C0000 to EFFFF
*/
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

int main(int argc, char* argv[])
{
    if (argc == 2) {
        std::ofstream outfile("output.txt");
        std::string input(argv[1]);
        int x = std::stoul(input, nullptr, 16);
        if (input == "0") {
            srand(time(0));
            x = rand() % 0x30000 + 0xC0000;//0xC0000 is minimum, 0xEFFFF is maximum (0x30000+0xC0000=F0000)
        }
        outfile << x;
        outfile.close();
    }
    else {
        std::cout << "Hex stuff for Vidinjector9000\nUsage:\nargument 1 = input text.\n";
    }
    return 0;
}