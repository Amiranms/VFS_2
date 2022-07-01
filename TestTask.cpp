#include "TestTask.hpp"



std::string TestTask::extractingName(const std::string name )//should search from end of the string 
{
    std::string fileName = name;
    int pos = fileName.find_last_of('/', fileName.size());
    fileName = fileName.substr(pos, fileName.size());
    fileName.erase(0,1);
    return fileName;
} 

std::string TestTask::extractingDirPath(const std::string name)
{
    std::string path = name;
    int pos = path.find_last_of('/', path.size());
    path = path.substr(0, pos);
    return path==""?"/":path;

}

    TestTask::File::File(std::string FullName,int number,std::fstream & fs) :m_Fullname(FullName)
    {
        m_startPos = number * MAX_CHAR;
        m_offset = 0;
        m_numberOfFile = number;
        m_Name = extractingName(FullName);
        time(&m_TimeAccess);
        time(&m_TimeMod);
        m_fcontent=std::string(MAX_CHAR, '#');
        fs.seekp(0,std::ios::end);//сдвинулись в конец, это логично, ведь мы добавляем файл
        fs << m_fcontent;//wrote.
        m_fcontent.clear();//cleared.
        fs.flush();
        m_isOpened = Access::CLOSED;
        m_Access = Access::RaW;
    } 


  //  Dir * parent;//nullptr if it mainfolder 
    TestTask::Dir::Dir()
    {
        time(&m_TimeAccess);
        time(&m_TimeMod);
    }

    TestTask::Dir::Dir(std::string  FullName): m_FullName(FullName)
    {
        m_Name = extractingName(FullName.c_str());
        time(&m_TimeAccess);
        time(&m_TimeMod);
    }

    int TestTask::Dir::ifDirExists(std::string name)
    {
        for (int i = 0; i < m_dirs.size(); i++)
        {
            if (m_dirs[i].m_Name == name)
                return i;
        }
        return -1;
    }

    //creates directory and returns last dir pointer
    TestTask::Dir* TestTask::Dir::addDir(std::string fullPath)
    {
        m_dirs.push_back(Dir{ fullPath });
        return &m_dirs[m_dirs.size() - 1];
    }
    //returns last directory in the path pointer, creates ones if it necesary 
    TestTask::Dir* TestTask::Dir::addAllDirs(std::string fullPath)
    {
        std::string tailPath;
        std::string headPath = fullPath;
        headPath.erase(0, 1);//deleting first " / " ;
        std::string lastDir = extractingName(fullPath);
        int posOfSlash = headPath.find_first_of("/");
        std::string searchableName = posOfSlash == -1 ? headPath : headPath.substr(0, posOfSlash);
        headPath.erase(0, posOfSlash + 1);
        tailPath = "/";
        TestTask::Dir* curDir = this;
        int flag = 0;
        while (tailPath != fullPath)
        {
            flag = curDir->ifDirExists(searchableName);
            if (flag != -1) 
            {//dir exists
                curDir = &curDir->m_dirs[flag];
                posOfSlash = headPath.find_first_of("/");
                searchableName = posOfSlash == -1 ? headPath : headPath.substr(0, posOfSlash);
                headPath.erase(0, posOfSlash + 1);
                tailPath = curDir->m_FullName;
            }
            else
            {
                tailPath = curDir->m_FullName + (curDir!=this?"/":"") + searchableName;
                curDir=curDir->addDir(tailPath);
                posOfSlash = headPath.find_first_of("/");
                searchableName = posOfSlash == -1 ? headPath : headPath.substr(0, posOfSlash);
                headPath.erase(0, posOfSlash + 1);
            }
        }
        return curDir;
    }
    //function findFile could be recursive, but it's not, so it search for file into current directory
    TestTask::File* TestTask::Dir::findFile(const char* name)
    {

        std::string fileName = extractingName(name);
        File* searchableFile = nullptr;

        //so also we should modify this string to find current directory
        //because the full path shouldn't end with slash 
        std::string path = extractingDirPath(name);
        TestTask::Dir* tmpDir = path.empty() ? this : findDir(path);
        
        if(tmpDir!=nullptr)
            for (int i = 0; i < tmpDir->m_files.size(); i++)
            {
                if (tmpDir->m_files[i].m_Name == fileName)
                    searchableFile = &tmpDir->m_files[i];

            }

        return searchableFile;
    }

    TestTask::File* TestTask::Dir::addFile(std::string fileName,int fileNumber,std::fstream & fs) {
        m_files.push_back(File{ fileName,fileNumber,fs});
        return &m_files[m_files.size() - 1];
    }

  
    TestTask::Dir* TestTask::Dir::findDir(std::string  name)//  /dir1/dir2/dir3 --> return pointer to an dir3 object 
    {//also it could be "/dirname" or "/" 

        std::string fullPath = name + "/";
        std::string lastName = extractingName(name);
        fullPath.erase(0, 1);
        int pos = fullPath.find_first_of('/');
        std::string searchableName = pos==-1?fullPath:fullPath.substr(0, pos);
        fullPath.erase(0, pos + 1);//  dir2/dir3...

        Dir* curDir = this;
        for (int i = 0; i < curDir->m_dirs.size(); i++)
        {
            if (curDir->m_dirs[i].m_Name == searchableName) {//searching for directory
                curDir = &curDir->m_dirs[i];
                i = -1;//we need to start from begining 
                pos = fullPath.find_first_of('/');
                searchableName = fullPath.substr(0, pos);
                fullPath.erase(0, pos + 1);
            }
        }
        return searchableName == "" ? curDir : nullptr;
    }




    TestTask::File* TestTask::IVFS::Open(const char* name) //виртуальное открытие файла 
    {
        File* tmpFile = m_root.findFile(name);
        if (tmpFile)//if found
        {
            if (tmpFile->m_isOpened == TestTask::Access::WRITE)
                return nullptr;
            else
                tmpFile->m_isOpened = TestTask::Access::READ;
        }
        return tmpFile;//nullptr if doesent 
    }
    // Открыть файл в readonly режиме. Если нет такого файла или же он открыт во writeonly режиме - вернуть nullptr
    
    TestTask::File* TestTask::IVFS::Create(const char* name) // Открыть или создать файл в writeonly режиме. Если нужно, то создать все нужные поддиректории, упомянутые в пути. Вернуть nullptr, если этот файл уже открыт в readonly режиме.
    {
        //if file exists
        File * f = m_root.findFile(name);
        if (f!=nullptr)
        {
            if (f->m_isOpened == Access::WRITE) {
                return nullptr;
            }
            else {
                f->m_isOpened = Access::WRITE;
                return f;
            }
        }
        std::string fullDirPath = extractingDirPath(name);
        
        std::string fullPath = name;
        std::string SearchableFile = extractingName(name);
        Dir* curDir = m_root.addAllDirs(fullDirPath);
        f = curDir->addFile(name,numberOfFiles,fs);
        f->m_isOpened = Access::RaW;// neobhodimo dlya testirovki , lucshe ubrat after testing
        numberOfFiles++;
        // /dir1/dir2/dir3/file1 imagine situation that dir3 doesent exist 
        
        return f;
        
    }
    
    size_t TestTask::IVFS::Read(TestTask::File* f, char* buff, size_t len)// Прочитать данные из файла. Возвращаемое значение - сколько реально байт удалось прочитать
    {
        size_t i = 0;
        int sizeOfContent = f->m_offset;
        f->m_fcontent.resize(f->m_offset, ' ');
        fs.seekg(f->m_startPos);
        fs.read(&f->m_fcontent[0], f->m_offset);
        if (f->m_isOpened == Access::READ || f->m_isOpened == Access::RaW)
            for (; i < len && i < sizeOfContent; i++)
            {
                buff[i] = f->m_fcontent[i];
            }
        else
            std::cout << "File isn't accesible for reading\n";

        if (buff[i])
            buff[i] = '\0';
        f->m_fcontent.clear();

        return i;
      
    }

    size_t TestTask::IVFS::Write(TestTask::File* f, char* buff, size_t len) { // Записать данные в файл. Возвращаемое значение - сколько реально байт удалось записать
        size_t i = 0;
        fs.seekp(f->m_startPos + f->m_offset);
        int sizeOfContent = f->m_offset;
        if (f->m_isOpened == Access::WRITE||f->m_isOpened==Access::RaW) {
            for (; i < len && buff[i] && sizeOfContent < MAX_CHAR; i++)
            {
                fs.put(buff[i]);
                sizeOfContent++;
            }
            if(i>0)
                time(&f->m_TimeMod);
        }
        else
            std::cout << "File isn't accesible for writing\n";
      
        f->m_offset += i;
        return i;
    }

    void TestTask::IVFS::Close(File* f) // Закрыть файл	
    {
        f->m_isOpened = TestTask::Access::CLOSED;
    }

    TestTask::IVFS::IVFS(const char* folder_path)//folider_path it is a path to a physical folder where will be all our virtual files stored 
    {
       //so now we should create a physical directory into folder where this cpp file exists and also we need two files at the begining;
        std::filesystem::create_directory(folder_path);
        mountDirPath = folder_path;
        m_root.m_FullName = "/";
        m_root.m_Name = "Mount point";
        numberOfFiles = 0;
        DataFileName = mountDirPath + '/' + "info.txt";
        std::string StorageFileName = mountDirPath + '/' + "Storage.txt";
        //trying to open phys file with information about files:
        
        std::ofstream fMake(StorageFileName);//just for file creating 
        fMake.close();
        fs.open(StorageFileName, std::ios::in | std::ios::out | std::ios::ate);
        if (!fs.is_open())
            exit(-1);
         
        time(&m_root.m_TimeAccess);
        time(&m_root.m_TimeAccess);
    }


    TestTask::IVFS::~IVFS()
    {
        char buff[128];
        std::ofstream ofsInfo(DataFileName);
        ofsInfo << numberOfFiles << std::endl;//wrote number of vfiles into phys file
        std::queue<Dir *> dirQ;
        dirQ.push(&m_root);
        Dir* curDir;
        while(!dirQ.empty())//bfs
        {
            curDir = dirQ.front();
            dirQ.pop();
            
            for (File& f : curDir->m_files)
            {
                ofsInfo << f.m_Fullname << std::endl << f.m_numberOfFile << std::endl <<
                    f.m_startPos << std::endl<<f.m_offset<<std::endl;
            }
            for (Dir& d : curDir->m_dirs)
            {
                dirQ.push(&d);
            }

        }
        ofsInfo.close();
        fs.close();
        
    }



