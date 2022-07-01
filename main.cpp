#include "TestTask.hpp"
using namespace TestTask;

int main()
{ 
    
    char buff[123] = "TEXT1TEXT1TEXT1TEXT1TEXT1TEXT1TEXT1TEXT1";
    IVFS fs("./directory");
    File* f1 = fs.Create("/dir1/dir2/dir3/file1.txt");
    File* f2 = fs.Create("/dir1/file3.txt");
    File* f3 = fs.Create("/dir1/dir2/file2.txt");
    char buff2[123];
    std::cout << fs.Write(f1, buff, 5)<<"\t";
    std::cout << fs.Read(f1, buff2, 19)<<"\t";
    std::cout << buff2 << std::endl;
 
    std::cout << fs.Write(f2, buff, 450) << "\t";
    std::cout << fs.Read(f2, buff2, 9) << "\t";
    std::cout << buff2 << std::endl;

    std::cout << fs.Write(f3, buff, 23) << "\t";
    std::cout << fs.Read(f3, buff2, 129) << "\t";
    std::cout << buff2 << std::endl;

    
}