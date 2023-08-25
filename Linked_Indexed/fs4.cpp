#include <iostream>
#include <map>
#include <cstring>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define BLOCK_SIZE 511
#define FILE_NAME_LENGTH 32
#define INITIAL_CONTIGUOUS_AREA_SIZE 1023

struct Block
{
    char data[BLOCK_SIZE + 1];
    struct Block *next;
};

typedef struct Block basic_block;

struct ModifiedContiguousAllocation
{
    char file_name[FILE_NAME_LENGTH];
    int file_size;
    bool overflow;
    char initial_contiguous_area_data[INITIAL_CONTIGUOUS_AREA_SIZE + 1];
    basic_block *head;
};

typedef struct ModifiedContiguousAllocation file_t4;
typedef struct ModifiedContiguousAllocation *file_t4_Ptr;

struct FileInfo
{
    bool isPresent = false;
    file_t4_Ptr file;
};

map<string, FileInfo> filesList; // fileList contains all created files

// appending null character
void appendNullCharacter(char *data, int index)
{
    data[index] = '\0';
}

// formatting filename to limit the filename size
string getFormattedFilename(const char *filename)
{
    int length = strlen(filename);
    char key[FILE_NAME_LENGTH];
    strncpy(key, filename, min(length, FILE_NAME_LENGTH));

    if (length < FILE_NAME_LENGTH)
        appendNullCharacter(key, length);

    return key;
}

// checking if file existing
bool isFileExisting(const char *filename)
{
    return filesList[getFormattedFilename(filename)].isPresent;
}

// getter for file info
file_t4_Ptr getFileInfo(const char *filename)
{
    return filesList[getFormattedFilename(filename)].file;
}

// removing file from file list
void removeFromFileList(const char *filename)
{
    filesList.erase(getFormattedFilename(filename));
}

// creating file obj for new file creation
void createFileObj(file_t4_Ptr *file)
{
    file_t4_Ptr newFile = (file_t4_Ptr)malloc(sizeof(file_t4));

    if (newFile == NULL)
    {
        cout << "\nNo Free Memory" << endl;
        exit(0);
    }

    *file = newFile;
}

// deleting file obj at the time of deletion
void deleteFileObj(file_t4 *file)
{
    free(file);
}

// allocating block incase of overflow
void allocateBlock(basic_block **block)
{
    Block *newBlock = (Block *)malloc(sizeof(Block));

    if (newBlock == NULL)
    {
        cout << "\nNo Free Memory" << endl;
        exit(0);
    }

    newBlock->next = NULL;
    *block = newBlock;
}

// deleting allocated blocks for a file
void freeAllocatedBlocks(Block *head)
{
    Block *temp;

    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// file creation arguments are filename and data, func return -1 incase of error and 0 if success
int createFile(const char *filename, const char *data)
{
    time_point<high_resolution_clock> start, end;
    duration<double> elapsedSeconds;
    start = high_resolution_clock::now();
    file_t4_Ptr newFile;
    int filenameLen = strlen(filename);
    int fileSize = strlen(data);
    int numOfBlocksToLink;
    int sizeOfSavedData;
    int sizeOfDataToSave;
    Block *temp;
    int i = 0;

    if (isFileExisting(filename))
    {
        cout << "\nFile Aleady Exist" << endl;
        return -1;
    }

    createFileObj(&newFile);
    strncpy(newFile->file_name, filename, min(filenameLen, FILE_NAME_LENGTH));

    if (filenameLen < FILE_NAME_LENGTH)
        appendNullCharacter(newFile->file_name, filenameLen);

    newFile->file_size = fileSize;

    if (fileSize > INITIAL_CONTIGUOUS_AREA_SIZE)
    {
        newFile->overflow = true;
    }
    else
    {
        newFile->overflow = false;
    }

    strncpy(newFile->initial_contiguous_area_data, data, min(fileSize, INITIAL_CONTIGUOUS_AREA_SIZE));
    appendNullCharacter(newFile->initial_contiguous_area_data, min(fileSize, INITIAL_CONTIGUOUS_AREA_SIZE));

    // incase of overflow blocks allocated after the contiguous memory
    if (newFile->overflow)
    {
        numOfBlocksToLink = ceil((fileSize - INITIAL_CONTIGUOUS_AREA_SIZE) / BLOCK_SIZE);
        sizeOfSavedData = INITIAL_CONTIGUOUS_AREA_SIZE;
        sizeOfDataToSave = fileSize - sizeOfSavedData;
        allocateBlock(&(newFile->head));

        if (newFile->head != NULL)
        {
            strncpy(newFile->head->data, data + sizeOfSavedData, min(sizeOfDataToSave, BLOCK_SIZE));
            appendNullCharacter(newFile->head->data, min(sizeOfDataToSave, BLOCK_SIZE));
            sizeOfSavedData += min(sizeOfDataToSave, BLOCK_SIZE);
            i++;
            temp = newFile->head;

            while (i <= numOfBlocksToLink)
            {
                sizeOfDataToSave = fileSize - sizeOfSavedData;
                allocateBlock(&(temp->next));
                temp = temp->next;
                strncpy(temp->data, data + sizeOfSavedData, min(sizeOfDataToSave, BLOCK_SIZE));
                appendNullCharacter(temp->data, min(sizeOfDataToSave, BLOCK_SIZE));
                sizeOfSavedData += min(sizeOfDataToSave, BLOCK_SIZE);
                i++;
            }

            temp->next = NULL;
        }
    }

    filesList[newFile->file_name].file = newFile;
    filesList[newFile->file_name].isPresent = true;

    end = high_resolution_clock::now();
    elapsedSeconds = end - start;
    cout << "\n"
         << filename << " creation elapsed time: " << elapsedSeconds.count() << "s\n";

    return 0;
}

// reading a file argument is filename, func return -1 incase of error and 0 if success
int readFile(const char *filename)
{
    time_point<high_resolution_clock> start, end;
    duration<double> elapsedSeconds;
    start = high_resolution_clock::now();
    file_t4_Ptr file;
    Block *temp;

    if (isFileExisting(filename) == false)
    {
        cout << "\nFile Doesn't Exist" << endl;
        return -1;
    }

    file = getFileInfo(filename);
    cout << "\n"
         << file->file_name << endl;
    cout << "\n"
         << file->initial_contiguous_area_data;

    if (file->overflow)
    {
        temp = file->head;

        while (temp != NULL)
        {
            cout << temp->data;
            temp = temp->next;
        }
    }

    cout << "\n";

    end = high_resolution_clock::now();
    elapsedSeconds = end - start;
    cout << "\n"
         << filename << " reading elapsed time: " << elapsedSeconds.count() << "s\n";

    return 0;
}

// deleting a file argument is filename, func return -1 incase of error and 0 if success
int deleteFile(const char *filename)
{
    time_point<high_resolution_clock> start, end;
    duration<double> elapsedSeconds;
    start = high_resolution_clock::now();
    file_t4_Ptr file;

    if (isFileExisting(filename) == false)
    {
        cout << "\nFile Doesn't Exist" << endl;
        return -1;
    }

    file = getFileInfo(filename);

    if (file->overflow)
        freeAllocatedBlocks(file->head);

    removeFromFileList(filename);
    deleteFileObj(file);
    cout << "\n"
         << filename << " Deleted" << endl;

    end = high_resolution_clock::now();
    elapsedSeconds = end - start;
    cout << "\n"
         << filename << " deletion elapsed time: " << elapsedSeconds.count() << "s\n";

    return 0;
}

// updating a file arguments are filename and data, func return -1 incase of error and 0 if success
int updateFile(const char *filename, const char *data)
{
    time_point<high_resolution_clock> start, end;
    duration<double> elapsedSeconds;
    start = high_resolution_clock::now();
    file_t4_Ptr file;
    int updatedFileSize;
    int dataSize;
    int dataSaved = 0;
    int dataToSave;
    int curBlockSize;
    Block *temp;

    if (isFileExisting(filename) == false)
    {
        cout << "\nFile Doesn't Exist" << endl;
        return -1;
    }

    file = getFileInfo(filename);
    dataSize = strlen(data);
    updatedFileSize = file->file_size + dataSize;

    // checking if contiguous memory is fully utilized
    if (file->file_size < INITIAL_CONTIGUOUS_AREA_SIZE)
    {
        dataToSave = min(INITIAL_CONTIGUOUS_AREA_SIZE - file->file_size, dataSize);
        strncpy(file->initial_contiguous_area_data + file->file_size, data, dataToSave);
        appendNullCharacter(file->initial_contiguous_area_data, file->file_size + dataToSave);
        dataSaved += dataToSave;
    }

    if (dataSaved < dataSize)
    {
        if (file->overflow)
        {
            temp = file->head;

            // finding the last allocated block
            while (temp->next != NULL)
            {
                temp = temp->next;
            }

            curBlockSize = strlen(temp->data);

            // checking if the last allocated block is fully utilized
            if (curBlockSize < BLOCK_SIZE)
            {
                dataToSave = min(BLOCK_SIZE - curBlockSize, dataSize);
                strncpy(temp->data + curBlockSize, data, dataToSave);
                appendNullCharacter(temp->data, curBlockSize + dataToSave);
                dataSaved += dataToSave;
            }
        }
        else
        {
            file->overflow = true;
            allocateBlock(&(file->head));
            dataToSave = dataSize - dataSaved;
            strncpy(file->head->data, data + dataSaved, min(dataToSave, BLOCK_SIZE));
            appendNullCharacter(file->head->data, min(dataToSave, BLOCK_SIZE));
            dataSaved += min(dataToSave, BLOCK_SIZE);
            temp = file->head;
        }
    }

    // allocating new blocks till the entire data is saved to the file.
    while (dataSaved < dataSize)
    {
        allocateBlock(&(temp->next));
        temp = temp->next;
        dataToSave = dataSize - dataSaved;
        strncpy(temp->data, data + dataSaved, min(dataToSave, BLOCK_SIZE));
        appendNullCharacter(temp->data, min(dataToSave, BLOCK_SIZE));
        dataSaved += min(dataToSave, BLOCK_SIZE);
    }

    file->file_size = updatedFileSize;

    end = high_resolution_clock::now();
    elapsedSeconds = end - start;
    cout << "\n"
         << filename << " updation elapsed time: " << elapsedSeconds.count() << "s\n";

    return 0;
}

int main()
{
    createFile("NBA.txt", "The National Basketball Association is a professional basketball league in North America composed of 30 teams. It is one of the major professional sports leagues in the United States and Canada and is considered the premier men's professional basketball league in the world.");
    createFile("Arsenal.txt", "Arsenal Football Club is an English professional football club based in Islington, London. Arsenal plays in the Premier League, the top flight of English football.");
    createFile("Solar-System.txt", "The Solar System[c] is the gravitationally bound system of the Sun and the objects that orbit it. It formed 4.6 billion years ago from the gravitational collapse of a giant interstellar molecular cloud. The vast majority (99.86%) of the system's mass is in the Sun, with most of the remaining mass contained in the planet Jupiter. The planetary system around the Sun contains eight planets. The four inner system planets—Mercury, Venus, Earth and Mars—are terrestrial planets, being composed primarily of rock and metal. The four giant planets of the outer system are substantially larger and more massive than the terrestrials. The two largest, Jupiter and Saturn, are gas giants, being composed mainly of hydrogen and helium; the next two, Uranus and Neptune, are ice giants, being composed mostly of volatile substances with relatively high melting points compared with hydrogen and helium, such as water, ammonia, and methane. All eight planets have nearly circular orbits that lie near the plane of Earth's orbit, called the ecliptic.There are an unknown number of smaller dwarf planets and innumerable small Solar System bodies orbiting the Sun.[d] Six of the major planets, the six largest possible dwarf planets, and many of the smaller bodies are orbited by natural satellites, commonly called after Earth's Moon. Two natural satellites, Jupiter's moon Ganymede and Saturn's moon Titan, are larger than Mercury, the smallest terrestrial planet, though less massive, and Jupiter's moon Callisto is nearly as large. Each of the giant planets and some smaller bodies are encircled by planetary rings of ice, dust and moonlets. The asteroid belt, which lies between the orbits of Mars and Jupiter, contains objects composed of rock, metal and ice. Beyond Neptune's orbit lie the Kuiper belt and scattered disc, which are populations of objects composed mostly of ice and rock.In the outer reaches of the Solar System lies a class of minor planets called detached objects. There is considerable debate as to how many such objects there will prove to be.[9] Some of these objects are large enough to have rounded under their own gravity and thus to be categorized as dwarf planets. Astronomers generally accept about nine objects as dwarf planets: the asteroid Ceres, the Kuiper-belt objects Pluto, Orcus, Haumea, Quaoar, and Makemake, and the scattered-disc objects Gonggong, Eris, and Sedna.[d] Various small-body populations, including comets, centaurs and interplanetary dust clouds, freely travel between the regions of the Solar System.The solar wind, a stream of charged particles flowing outwards from the Sun, creates a bubble-like region of interplanetary medium in the interstellar medium known as the heliosphere. The heliopause is the point at which pressure from the solar wind is equal to the opposing pressure of the interstellar medium; it extends out to the edge of the scattered disc. The Oort cloud, which is thought to be the source for long-period comets, may also exist at a distance roughly a thousand times further than the heliosphere. The Solar System is located 26,000 light-years from the center of the Milky Way galaxy in the Orion Arm, which contains most of the visible stars in the night sky. The nearest stars are within the so-called Local Bubble, with the closest, Proxima Centauri, at 4.2441 light-years.");
    readFile("Solar-System.txt");
    readFile("Arsenal.txt");
    readFile("NBA.txt");
    updateFile("Solar-System.txt", " NB: This is from Wikipedia.");
    readFile("Solar-System.txt");
    deleteFile("Solar-System.txt");
    deleteFile("Arsenal.txt");
    deleteFile("NBA.txt");

    return 0;
}