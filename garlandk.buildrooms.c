#include <stdio.h>          
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

//create type bool
typedef int bool;
enum {false, true};

//taken from reading 2.1
struct room
{
    char *name;
    //type instead of ID
    char *roomType;
    int numOutboundConnections;
    //max connections for each room is 6
    struct room *outboundConnections[6];
};


//Function prototypes
bool IsGraphFull(struct room *room[]);
void AddRandomConnection(struct room *room[]);
struct room *GetRandomRoom(struct room *room[]);
bool CanAddConnectionFrom(struct room *room1);
bool ConnectionAlreadyExists(struct room *room1, struct room *room2);
void ConnectRoom(struct room *room1, struct room *room2); 
bool IsSameRoom(struct room *room1, struct room *room2);
//char *randomRoomName(char *rooms[]);
void init(struct room *room[], char *roomType[], char *roomName[]);
void ShuffleRoomNames(char ** possibleRoomNames);



/*
    General outline from specs:
    The first thing your rooms program must do is create a directory called 
    "<YOUR STUDENT ONID USERNAME>.rooms.<PROCESS ID OF ROOMS PROGRAM>". 
    Next, it must generate 7 different room files, which will contain 
    one room per file, in the directory just created. You may use any filenames 
    you want for these 7 room files, and these names should be hard-coded into your 
    program.

    Rooms: Telsa, Maxwell, Curie, Volta, Watt, Newton, Euler, Coulomb, Einstein
           Ampere
*/



int main()
{   //create directory with onid name and pid
    int i, j;
    char dirName[40];
    srand(time(NULL));
    memset(dirName, '\0', 40);
    sprintf(dirName, "garlandk.rooms.%d", getpid());
    mkdir(dirName, 0755);

    //hard coded room
    //named after my favorite scientists/engineers
    char *roomNames[10] = {"Tesla", "Maxwell", "Curie", "Volta", "Newton", "Euler", "Coulomb", "Einstein", "Ampere", "Watt"};
    char* roomType[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};
    //array of room pointers to hold all 7 rooms

    //roomArray is 7 rooms out of 10 chosen at random
    struct room *roomArray[7];
    for (j = 0; j < 6; j++)
    {
        roomArray[j] = malloc (sizeof(struct room));
    }
    //randomly assign 7 of these 10 rooms
    ShuffleRoomNames(roomNames);
    init(roomArray, roomType, roomNames);

    while(IsGraphFull(roomArray) == false)
    {
        AddRandomConnection(roomArray);
    }

    chdir(dirName);
    for (i = 0; i < 7; i++)
    {
        char roomFileName[40];
        sprintf(roomFileName, "%s_room", roomNames[i]);
        FILE *infile = fopen(roomFileName, "w+");
        //fprintf(infile, "%s_room", roomNames[i]);
        fclose(infile);
    }   
    

    //generate 7 different room files

    //
    return 0;
}


// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
// loop through all rooms to see if they have 3-6 connections. Need room pointer
bool IsGraphFull(struct room *room[])  
{
    int index;
    //7 rooms max, starting at index 0
    for (index = 0; index < 6; index++)
    {
        //checking array of outbound connections for each room
        if (room[index]->numOutboundConnections < 3)
        {
            return false;
        }
    }
    //returns true if we make through the loop and didn't return false
    return true;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct room *room[])  
{
  struct room *A; 
  struct room *B;

  while(true)
  {
    A = GetRandomRoom(room);

    if (CanAddConnectionFrom(A) == true)
        break;
  }

  do
  {
    B = GetRandomRoom(room);
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

  ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
  ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

// Returns a random Room, does NOT validate if connection can be added
struct room *GetRandomRoom(struct room *room[])
{
    int i = rand() % 7; //random integer from 0 to 6
    return room[i];
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct room *room) 
{
    if (room->numOutboundConnections < 6)
    {
        return true;
    }
    else
    {
        return false;
    }
}
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct room *room1, struct room *room2)
{
    int i;
    for(i = 0; i < 6; i++)
    {
        if (room1->outboundConnections[i] == NULL)
        {
            return false;
        }
        else if (strcmp(room2->name, room1->outboundConnections[i]->name) == 0)
        {
            return true;
        }
    }
    return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room *room1, struct room *room2) 
{
    room1->outboundConnections[room1->numOutboundConnections] = room2;
    room1->numOutboundConnections++;

    room2->outboundConnections[room2->numOutboundConnections] = room1;
    room2->numOutboundConnections++;
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct room *room1, struct room *room2) 
{
    //strcmp returns 0 if the compared strings are the same
    if (strcmp(room1->name, room2->name) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
char *randomRoomName(char *rooms[])
{
    int found = 1;
    int i;
    int alreadyseen[7] = {0,0,0,0,0,0,0};

    while(found)
    {
        i = rand() % 10; //random integer from 0 to 10
        if (alreadyseen[i] == 0)
        {
            found = 0;
            alreadyseen[i] = 1;
            return rooms[i];
        }
    }
}*/


void ShuffleRoomNames(char ** possibleRoomNames)
{
    int i, j;
    char * temp;

    for(i = 10 - 1; i > 0; i--){
        j = rand() % (i+1);
        temp = possibleRoomNames[i];
        possibleRoomNames[i] = possibleRoomNames[j];
        possibleRoomNames[j] = temp;
    }
}

void init(struct room *rooms[], char *roomType[], char *roomName[])
{
    int i;
    for (i = 0; i < 6; i++)
    {
        //rooms[i] = malloc(sizeof(struct room));
        rooms[i]->name = roomName[i];

        if (i == 0)
        {
            //roomType[0] = START_ROOM
            rooms[i]->roomType = roomType[0];
        }
        else if (i == 6)
        {
            //roomType[1] = END_ROOM
            rooms[i]->roomType = roomType[1];
        }
        else
        {
            //roomType[2] = MID_ROOM
            rooms[i]->roomType = roomType[2];
        }
    }
}
