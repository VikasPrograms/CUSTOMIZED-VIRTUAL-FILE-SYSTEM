// CVFC

//---------------------------------------------------------------------------------------
//
// --------------------- Costomised Virtual/Dynamic File System --------------------------
//
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//
//  Header Files
//
//---------------------------------------------------------------------------------------


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
// #include<io.h>


//---------------------------------------------------------------------------------------
// ----------------------->> Defining The Macros <<----------------------------------
//---------------------------------------------------------------------------------------

#define MAXINODE 50          // Maximum Files To Be Created 50

// Permissition Read and write
#define READ 1      // File Read means 1
#define WRITE 2     // File Write means 2      // Read + Write means 3

#define MAXFILESIZE 1024        // File Size - 1024 byte = 1 KB     // Freedom any size you can allocate  --->  512 byte, 1024 byte, 2048 byte

#define REGULAR 1       // filetype - Regular file
#define SPECIAL 2       // filetype - .c, .py file

#define START 0         // File Offset(lseek)
#define CURRENT 1
#define END 2           // Hole In The File ie potential gap


//---------------------------------------------------------------------------------------
//--------------------->> Creating SUPERBLOCK Structure <<--------------------------
//---------------------------------------------------------------------------------------

typedef struct superblock
{
    int TotalInodes;        //  50 Inodes
    int FreeInode;          
}SUPERBLOCK, *PSUPERBLOCK;


//---------------------------------------------------------------------------------------
//----------------------->> Creating INODE Structure <<----------------------------
//---------------------------------------------------------------------------------------

typedef struct inode
{
    char FileName[50];      // File Name sathi 50 Bytes memory allocate keli        // 50 Bytes
    int InodeNumber; 		//  inode number
    int FileSize; 			//  1024 byte 
    int FileActualSize;   	//  allocated when we write into it ie 10 bytes of data
    int FileType; 			//  type of file
    char *Buffer;           //  On Windows It Stores Block Number But In This Code It Stores 1024 Bytes                                                      // 8 Bytes  pointer
    int LinkCount;  		//  linking count
    int ReferenceCount; 	//  reference count
    int permission;         //  read write permission  ( 1   2   3 )
    struct inode *next;	    //  self referential structure           // 8 Bytes  pointer
}INODE, *PINODE, **PPINODE;                                                 // Total : 94 Bytes Memory Allocating for INODE in Harddisk


//---------------------------------------------------------------------------------------
//--------------------->> Creating FileTable Structure <<--------------------------
//---------------------------------------------------------------------------------------
typedef struct filetable        // filetable tayar hoto
{
    int readoffset;    		//  From Where To Read 
    int writeoffset;    	//  From Where To Write      
    int count;              //  Remains 1 Throught The Code
    int mode;     		    //  mode of file
    PINODE ptrinode;          
}FILETABLE, *PFILETABLE;        // Total : 24 Bytes Memory Allocating for FILETABLE

//---------------------------------------------------------------------------------------
//----------------------->> Creating UFDT Structure <<-----------------------------
//---------------------------------------------------------------------------------------
typedef struct ufdt
{
    PFILETABLE ptrfiletable;           //create ufdt structure, // Pointer Which Points To File Table    
}UFDT;

UFDT UFDTArr[50];			    // Create Array Of Structure i.e Array Of Pointer
SUPERBLOCK SUPERBLOCKobj;		// global variable
PINODE head = NULL;				// global pointer 

//----------------------------------------------------------------------



//---------------------------------------------------------------------------------------
//
//	Function Name	: 	man
//	Input			: 	char *
//	Output			: 	None
//	Description 	: 	It Display The Description For Each Commands
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


void man(char *name)
{
    if(name == NULL) 
    {
        return;
    }

    if(strcmp(name, "create") == 0)             // create Demo.txt 3
    {
        printf("Description : Used tocreate new regular file \n");
        printf("Usage : create File name Permission \n");
    }
    else if(strcmp(name, "read") == 0)          // read Demo.txt 4      (India is data and it allocating the 5 bytes memory , command madhil 4 he 4 byte memory read kara means Indi )
    {
        printf("Description : Used to read data from regular file \n");
        printf("Usage : read File_name No_Of_Bytes_To_Read \n");
    }
    else if(strcmp(name, "write") == 0)         // write Demo.txt       \n India     (India is the written data)
    {
        printf("Description : Used to write into regular file \n");
        printf("Usage : write File_name \n After this enter the data that we want to write \n");
    }
    else if(strcmp(name, "ls") == 0)            // ls
    {
        printf("Description : Used to list all information of files \n");
        printf("Usage : ls \n");
    }
    else if(strcmp(name, "stat") == 0)          // stat Demo.txt
    {
        printf("Description : Used to display information of file \n");
        printf("Usage : stat File_name \n");
    }
    else if(strcmp(name, "fstat") == 0)         // fstat 6       // this descriptor number creating the file create time        // fstat function is fast as compare stat. (student chya "name (means stat)"" peksha "Roll Number(means fstat)" ne tyach data check karan sop jat Teacher la)
    {
        printf("Description : Used to display information of file \n");
        printf("Usage : fstat File_Discriptor \n");
    }
    else if(strcmp(name, "truncate") == 0)      // truncate Demo.txt
    {
        printf("Description : Used to remove data from file \n");
        printf("Usage : truncate File_name \n");
    }
    else if(strcmp(name, "open") == 0)          // open Demo.txt
    {
        printf("Description : Used to open existing file \n");
        printf("Usage : open File_name mode \n");
    }
    else if(strcmp(name, "close") == 0)         // close Demo.txt
    {
        printf("Description : Used to close opened file \n");
        printf("Usage : close File_name \n");
    }
    else if(strcmp(name, "closeall") == 0)      // closeall
    {
        printf("Description : Used to close all opened file \n");
        printf("Usage : closeall \n");
    }
    else if(strcmp(name, "lseek") == 0)         // lseek Demo.txt
    {
        printf("Description : Used to change file offset \n");
        printf("Usage : lseek File_Name ChangeInOffset StartPoint \n");
    }
    else if(strcmp(name, "rm") == 0)            // rm Demo.txt      // rm ha link count 0 karato
    {
        printf("Description : Used to delete the file \n");
        printf("Usage : rm File_Name \n");
    }    
    else if(strcmp(name,"backup") == 0)
    {
        printf("Description : Used To Take Backup Of All Files Created\n");
        printf("Usage : backup\n");
    }
    else
    {
        printf("ERROR : No manual entry available. \n");
    }
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	DisplayHelp
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	It Display All List / Operations About This Application
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


void DisplayHelp()
{
    printf("ls        :   To List out all files \n");
    printf("clear     :   To clear console \n");
    printf("open      :   To open the file \n");
    printf("close     :   To close the file \n");
    printf("closeall  :   To close all opened files \n");
    printf("read      :   To Read the contents from file \n");
    printf("write     :   To Write contents into file \n");
    printf("exit      :   To Terminate file system \n");
    printf("stat      :   To Display information of file using name \n");
    printf("fstat     :   To Display information of file using file descriptor \n");
    printf("truncate  :   To Remove all data from file \n");
    printf("rm        :   To Delete the file \n");
    printf("backup    :   To Take Backup Of All Created Files\n");
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	GetFDFromName
//	Input			: 	char*
//	Output			: 	Integer
//	Description 	: 	Get File Descriptor Value
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


int GetFDFromName(char *name)
{
    int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
            {
                break;
            }
        }
        i++;
    }

    if(i == 50)
    {
        return -1;
    }
    else
    {
        return i;
    }
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	Get_Inode
//	Input			: 	char*
//	Output			: 	PINODE
//	Description 	: 	Return Inode Value Of File
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


PINODE Get_Inode(char *name)        // file ch name ithe aale
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
    {
        return NULL;
    }

    while(temp != NULL)
    {
        if(strcmp(name, temp->FileName) == 0)       // file name check  // sapadali tr break
        {
            break;
        }
        temp = temp -> next;
    }

    return temp;        // return value is depends means file is allready exist or file nasel tr NULL
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	CreateDILB
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	It Creates The DILB When Program Starts 
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


void CreateDILB()       // Disk inode list block        // Inode create karate
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while(i <= MAXINODE)        // loop iterate 5 times
    {
        newn = (PINODE)malloc(sizeof(INODE));       // Dynamic memory allocation
    
        newn -> LinkCount = 0;
        newn -> ReferenceCount = 0;
        newn -> FileType = 0;
        newn -> FileSize = 0;

        newn -> Buffer = NULL;
        newn -> next = NULL;

        newn -> InodeNumber = i;        // i =  1, 2, 3, 4, 5  automatic numbering hote sadhhya 5 ahe     // ha inode link list tayar karun dete maximum inode he MAXINODE asatil

        if(temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp -> next = newn;
            temp = temp -> next;
        }
        i++;
    }
    printf("DILB created successfully \n");
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	InitialiseSuperBlock
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	Initialize Inode Values 
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


void InitialiseSuperBlock()
{
    int i = 0;

    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}


//---------------------------------------------------------------------------------------
//
//	Function Name	: 	CreateFile
//	Input			: 	char*, Integer
//	Output			: 	None
//	Description 	: 	Create New Files
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


// CreateFile (  Demo.txt        3       )
int CreateFile(char *name, int permission)
{
    int i = 3;      // int i = 3;       means pahile 3 permission reserve ahet

    PINODE temp = head;

    if((name == NULL) || (permission == 0) || (permission > 3))     // Filter 1     // permission (eg. 3 < peksha mothi ) or name wrong dile tr means incorrect parameters
    {
        return -1;
    }

    if(SUPERBLOCKobj.FreeInode == 0)     // Filter 2        // There is no inodes    // Inodes sample tr
    {
        return -2;
    }

    (SUPERBLOCKobj.FreeInode)--;     // Filter 3

    if(Get_Inode(name) != NULL)     // ya function la file ch name pathavato apan
    {
        return -3;
    }

    while(temp != NULL)
    {
        if(temp -> FileType == 0)       // apalya project madhhe filetype 0 nahiye mhanun break honar nahi
        {
            break;
        }
        temp = temp -> next;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));        // UFDT Array madhhe file dili

    //---------------------->> This is FileTable Block <<------------------

    UFDTArr[i].ptrfiletable -> count = 1;               // count value is always 1
    UFDTArr[i].ptrfiletable -> mode = permission;       //
    UFDTArr[i].ptrfiletable -> readoffset = 0;          //
    UFDTArr[i].ptrfiletable -> writeoffset = 0;         //

    UFDTArr[i].ptrfiletable -> ptrinode = temp;

    //---------------------------------------------------------------------

    //---------------------->> This is INODE Block <<----------------------

    strcpy(UFDTArr[i].ptrfiletable -> ptrinode -> FileName, name);      // File name paryant janyacha marg ahe

    UFDTArr[i].ptrfiletable -> ptrinode -> FileType = REGULAR;
    UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount = 1;
    UFDTArr[i].ptrfiletable -> ptrinode -> LinkCount = 1;
    UFDTArr[i].ptrfiletable -> ptrinode -> FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable -> ptrinode -> FileActualSize = 0;
    UFDTArr[i].ptrfiletable -> ptrinode -> permission = permission;
    UFDTArr[i].ptrfiletable -> ptrinode -> Buffer = (char *)malloc(MAXFILESIZE);
    
    //---------------------------------------------------------------------

    return i;       // UFDT index 3 , this is permission
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	rm_File
//	Input			: 	char*
//	Output			: 	Integer
//	Description 	: 	Remove Created Files
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int rm_File(char *name)     // rm_File("Demo.txt")
{
    int fd = 0;             // fd = file descriptor

    fd = GetFDFromName(name);

    if(fd == -1)
    {
        return -1;
    }

    (UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount)--;

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount = 0)
    {
        UFDTArr[fd].ptrfiletable -> ptrinode -> FileType = 0;
        // free(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer)
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;

	printf("File Successfully Deleted\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	ReadFile
//	Input			: 	Integer, char*, Integer
//	Output			: 	Integer
//	Description 	: 	Read Data From File
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable -> mode != READ + WRITE)
    {
        return -2;
    }

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ && UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ + WRITE)
    {
        return -2;
    }

    if(UFDTArr[fd].ptrfiletable -> readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)
    {
        return -3;
    }

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> FileType != REGULAR)
    {
        return -4;
    }

    read_size = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) - (UFDTArr[fd].ptrfiletable -> readoffset);
    
    if(read_size < isize)
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), read_size);
        (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + isize;
    }
    else
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), isize);
        (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + isize;
    }
    return isize;
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	WriteFile
//	Input			: 	Integer, char*, Integer
//	Output			: 	Integer
//	Description 	: 	Write Data Into The File
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int WriteFile(int fd, char *arr, int isize)     // write Demo.txt
{
    if(((UFDTArr[fd].ptrfiletable -> mode) != WRITE) && ((UFDTArr[fd].ptrfiletable -> mode) != READ + WRITE))
    {
        return -1;
    }

    if(((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != WRITE) && ((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != READ + WRITE))
    {
        return -1;
    }

    if((UFDTArr[fd].ptrfiletable -> writeoffset) == MAXFILESIZE)
    {
        return -2;
    }
    
    if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileType) != REGULAR)
    {
        return -3;
    }
    
    strncpy((UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + isize;

    return isize;
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	OpenFile
//	Input			: 	char*, Integer
//	Output			: 	Integer
//	Description 	: 	Open An Existing File
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int OpenFile(char *name, int mode)
{
    int i = 0;

    PINODE temp = NULL;

    if(name == NULL || mode <= 0)
    {
        return -1;
    }

    temp = Get_Inode(name);
    if(temp == NULL)
    {
        return -2;
    }

    if(temp -> permission < mode)
    {
        return -3;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = mode;

    if(mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }

    UFDTArr[i].ptrfiletable -> ptrinode = temp;
    (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)++;

    return i;
  	printf("File Opened Successfully\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	CloseFileByName
//	Input			: 	Integer
//	Output			: 	None
//	Description 	: 	Close Existing File By By Its File Descriptor
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    (UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount)--;
	printf("File Closed Succesfully\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	CloseFileByName
//	Input			: 	Char
//	Output			: 	Integer
//	Description 	: 	Close Existing File By Its Name
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int CloseFileByName(char *name)
{
    int i = 0;

    i = GetFDFromName(name);

    if(i == -1)
    {
        printf("All Open Files are Closed \n");
        return -1;
    }

    if((UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount) == 0)
    {
        return -2;
    }

    UFDTArr[i].ptrfiletable -> readoffset = 0;
    UFDTArr[i].ptrfiletable -> writeoffset = 0;
    (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)--;
	printf("File Closed Succesfully\n");

    return 0;
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	CloseAllFile
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	Close All Existing Files
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

void CloseAllFile()
{
    int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable -> readoffset = 0;
            UFDTArr[i].ptrfiletable -> writeoffset = 0;
            (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)--;
            break;
        }
        i++;
    }
    printf("All Files Are Closed Successfully\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	LseekFile
//	Input			: 	Integer, Integer, Integer
//	Output			: 	Integer
//	Description 	: 	Write Data Into The File From Perticular Position
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int LseekFile(int fd, int size, int from)
{
    if((fd < 0) || (from > 2))
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount == 0)
    {
        return -2;
    }

    if((UFDTArr[fd].ptrfiletable -> mode == READ) || (UFDTArr[fd].ptrfiletable -> mode == READ + WRITE))
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) > UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                return -1;
            }

            if(size < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable -> readoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
        }
    }
    else if(UFDTArr[fd].ptrfiletable -> mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0)
            {
                return -1;
            }            

            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
            }

            (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
        }
        else if(from == START)
        {
            if(size > MAXFILESIZE)
            {
                return -1;
            }

            if(size < 0)
            {
                return -1;
            }

            if(size > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = size;
            }

            (UFDTArr[fd].ptrfiletable -> writeoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
        }
    }
    printf("Successfully Changed\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	ls_file
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	List Out All Existing Files Name
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

void ls_file()
{
    int i = 0;

    PINODE temp = head;

    if(SUPERBLOCKobj.FreeInode == MAXINODE)     // Filter    // Inodes free kiti
    {
        printf("ERROR : There are no files \n");
        return;
    }

    //----------------------------------------> ls command <------------------------------------------------

    printf("\nFile Name\tInode Number\tFile Size\tLink Count\n");       // Inode madhil block
    printf("---------------------------------------------------------------------------\n");

    while(temp != NULL)
    {
        if(temp -> FileType != 0)       // this loop iterate (Maxinode - freeinode) times
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp -> FileName, temp -> InodeNumber, temp -> FileActualSize, temp -> LinkCount);
        }
        temp = temp -> next;
    }
    printf("---------------------------------------------------------------------------\n");

    //-------------------------------------------------------------------------------------------------------
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	ls_file
//	Input			: 	Integer
//	Output			: 	Integer
//	Description 	: 	Display Statistical Information Of The File By Using File Descriptor
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int fstat_file(int fd)
{
    PINODE temp = head;

    int i = 0;

    if(fd < 0)
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -2;
    }

    temp = UFDTArr[fd].ptrfiletable -> ptrinode;

    printf("\n---------------->> Statistical Information about file <<----------------\n");
    printf("File Name        : %s\n", temp -> FileName);
    printf("Inode Number     : %d\n", temp -> InodeNumber);
    printf("File Size        : %d\n", temp -> FileSize);
    printf("Actual File Size : %d\n", temp -> FileActualSize);
    printf("Link Count       : %d\n", temp -> LinkCount);
    printf("Reference Count  : %d\n", temp -> ReferenceCount);

    if(temp -> permission == 1)
    {
        printf("File Permission : Read Only \n");
    }
    else if(temp -> permission == 2)
    {
        printf("File Permission : Write \n");
    }
    else if(temp -> permission == 3)
    {
        printf("File Permission : Read & Write \n");
    }
    printf("---------------------------------------------------------------------------\n\n");

    return 0;
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	stat_file
//	Input			: 	Char*
//	Output			: 	Integer
//	Description 	: 	Display Statistical Information Of The File By Using File Name
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
    {
        return -1;
    }

    while(temp != NULL)
    {
        if(strcmp(name, temp -> FileName) == 0)
        {
            break;
        }
        temp = temp ->next;
    }

    if(temp == NULL)
    {
        return -2;
    }

    printf("\n---------------->> Statistical Information about file <<----------------\n");
    printf("File Name        : %s\n", temp -> FileName);
    printf("Inode Number     : %d\n", temp -> InodeNumber);
    printf("File Size        : %d\n", temp -> FileSize);
    printf("Actual File Size : %d\n", temp -> FileActualSize);
    printf("Link Count       : %d\n", temp -> LinkCount);
    printf("Reference Count  : %d\n", temp -> ReferenceCount);

    if(temp -> permission == 1)
    {
        printf("File Permission : Read Only \n");
    }
    else if(temp -> permission == 2)
    {
        printf("File Permission : Write \n");
    }
    else if(temp -> permission == 3)
    {
        printf("File Permission : Read & Write \n");
    }
    printf("---------------------------------------------------------------------------\n\n");

    return 0;
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	truncate_File
//	Input			: 	Char*
//	Output			: 	Integer
//	Description 	: 	Delete All Data From The File
//	Author			: 	Vikas Bade
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    
    if(fd == -1)
    {
        return -1;
    }

    memset(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer, 0, 1024);
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize = 0;
	printf("Data Succesfully Removed\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	Backup
//	Input			: 	None
//	Output			: 	None
//	Description 	: 	Take Backup Of All Created Files Into Hard-Disk
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------


void backup()
{
    PINODE temp = head;

    int fd = 0;

    while(temp != NULL)
    {
        if(temp -> FileType != 0)
        {
            fd = creat(temp -> FileName, 0777);
            write(fd, temp -> Buffer, temp -> FileActualSize);
        }
        temp = temp -> next;
    }
    printf("Successfully Get The Backup Of All Created Files...\n");
}

//---------------------------------------------------------------------------------------
//
//	Function Name	: 	main
//	Input			: 	None
//	Output			: 	Integer
//	Description 	: 	Entry Point Function
//	Author			: 	Prasad Dangare
//	Date			:	28 June 2021
//
//---------------------------------------------------------------------------------------

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while(1)
    {
        fflush(stdin);
        strcpy(str, "");

        printf("\nMy VFS : >    ");

        fgets(str, 80, stdin);  // scanf("%[^'\n']s", str);

        // create demo.txt read

        count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);
        // str token

        if(count == 1)      // Command cha Actual Word
        {
            if(strcmp(command[0], "ls") == 0)
            {
                ls_file();      // User defined functions ----------- means varati yach logic ahe
            }
            else if(strcmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully \n");
                continue;
            }
            else if(strcmp(command[0], "clear") == 0)
            {
                system("cls");      // for linux used "clear" this command platform dependent // this command is pass out the terminal
                continue;
            }
            else if(strcmp(command[0], "help") == 0)
            {
                DisplayHelp();
                continue;       // used Continue next iteration
            }
            else if(strcmp(command[0], "exit") == 0)
            {
                printf("Terminating the My Virtual File System \n");
                break;      // shell stop // Deallocate all DS // Loop cha break
            }
            else
            {
                printf("\nERROR : Command not found  !!!! \n");     // dileli command jar 'if' loop madhhe nasel tr command not found dakhavat
                continue;
            }
        }
        else if(count == 2)     // Command che Parameters
        {
            if(strcmp(command[0], "stat") == 0)
            {
                ret = stat_file(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters \n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no such file \n");
                }

                continue;
            }
            else if(strcmp(command[0], "fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));
                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters \n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no such file \n");
                }

                continue;
            }
            else if(strcmp(command[0], "close") == 0)
            {
                ret = CloseFileByName(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters \n");
                }

                continue;
            }
            else if(strcmp(command[0], "rm") == 0)
            {
                ret = rm_File(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : There is no such file \n");
                }

                continue;
            }
            else if(strcmp(command[0], "man") == 0)
            {
                man(command[1]);
                continue;
            }
            else if(strcmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)        // Filter
                {
                    printf("ERROR : Incorrect parameter \n");
                    continue;
                }

                if(UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount == 0)
                {
                    printf("ERROR : File is Not Opened\n");
                }
                else
                {
                    printf("Enter the data : \n");
				    scanf("%[^\n]", arr); 
                }
				//fflush(stdin); // empty input buffer

                ret = strlen(arr);
                if(ret == 0)
                {
                    printf("ERROR : Incorrect parameter \n");
                    continue;
                }

                ret = WriteFile(fd, arr, ret);
                if(ret == -1)
                {
                    printf("ERROR : Permission Denied \n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no sufficient memory to write \n");
                }

                if(ret == -3)
                {
                    printf("ERROR : It is not regular file \n");
                }
            }
            else if(strcmp(command[0], "truncate") == 0)
            {
                ret = truncate_File(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameter \n");
                }

                continue;
            }
            else
            {
                printf("\nERROR : Command not found !!!!\n");
                continue;
            }
        }
        else if(count == 3)     // Command che Parameters
        {
            if(strcmp(command[0], "create") == 0)       
            {
                // create    Demo.txt      3    --- 3 is indicates Read&write
                // ret = CreateFile(Demo.txt, 3);
                ret = CreateFile(command[1], atoi(command[2]));    // ASCII to Integer // command[1] is file name,command[2] is indication means read=1 & write=2 or read&write=3
                if(ret >= 0)
                {
                    printf("File is successfully created with file descriptor : %d\n",ret);
                }

                if(ret == -1)
                {
                    printf("ERROR : Incorrect paramenters\n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no inodes\n");
                }

                if(ret == -3)
                {
                    printf("ERROR : File already exists\n");
                }

                if(ret == -4)
                {
                    printf("ERROR : Memory allocation Failure \n");
                }

                continue;
            }
            else if(strcmp(command[0], "open") == 0)
            {
                ret = OpenFile(command[1], atoi(command[2]));
                if(ret >= 0)
                {
                    printf("File is successfully opened with file descriptor : %d\n",ret);
                }

                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters \n");
                }

                if(ret == -2)
                {
                    printf("ERROR : File not present \n");
                }

                if(ret == -3)
                {
                    printf("ERROR : Permission Denied \n");
                }

                continue;
            }
            else if(strcmp(command[0], "read") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect parameter \n");
                    continue;
                }

                ptr = (char *)malloc(sizeof(atoi(command[2]))+1);

                if(ptr == NULL)
                {
                    printf("ERROR : Memory Allocation Failure \n");
                    continue;
                }

                ret = ReadFile(fd, ptr, atoi(command[2]));
                if(ret == -1)
                {
                    printf("ERROR : File not existing \n");
                }

                if(ret == -2)
                {
                    printf("ERROR : Permission Denied \n");
                }

                if(ret == -3)
                {
                    printf("ERROR : Reached at end of file \n");
                }

                if(ret == -4)
                {
                    printf("ERROR : It is not regular file \n");
                }

                if(ret == 0)
                {
                    printf("ERROR : File Empty \n");
                }

                if(ret > 0)
                {
                    write(2, ptr, ret);
                }

                continue;
            }
            else
            {
                printf("\nERROR : Command not found !!!! \n");
                continue;
            }
        }
        else if(count == 4)     // Command che Parameters
        {
            if(strcmp(command[0], "lseek") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameter \n");
                    continue;
                }

                ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                if(ret == -1)
                {
                    printf("ERROR : Unable to perform lseek \n");
                }
            }
            else
            {
                printf("\nERROR : Command not found !!!! \n");
                continue;
            }
        }
        else
        {
            printf("\nERROR : Command not found !!!! \n");
            continue;
        }
    }
    return 0;
}