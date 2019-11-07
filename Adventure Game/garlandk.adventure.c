#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>

struct room {
    char name[16];
    char roomType[16];
    int connections;
    char connected[6][32];
};

struct path {
    int steps;
    int stepsArr[32];
};

void* setTime(void* myMutex);
void* getTime();
struct room* createNewRoom(char* newName, char* newRoomType);
struct path* createNewPath();
void addPath(struct path* curPath, int curRoom);
void addRoomConnection(struct room* room1, char* roomConnection);
int isConnected(struct room* curRoom, char* roomInput);
void setRoomType(struct room* room1, char* newRoomType);
int findRoomInd(struct room** rooms, char* roomName);
void printCurRoom(struct room** rooms, int curRoom);
//char *findMostRecentDir(char ** mostRecent);

void* setTime(void* myMutex) {
    // Takes the current time and writes it to file

    // Attempt to access mutex. Locks if main thread has not released mutex
    pthread_mutex_lock(myMutex);

    FILE* timeFile = fopen("currentTime.txt", "w");
    time_t curTime;
    char* timeStr;

    // Gets current time and formats it
    curTime = time(0);
    timeStr = ctime(&curTime);

    // Writes time to file
    fputs(timeStr, timeFile);
    fclose(timeFile);

    // Releases lock on mutex so main thread can regain control
    pthread_mutex_unlock(myMutex);
    return NULL;
}

void* getTime() {
    // Prints out current time from file
    FILE* timeFile = fopen("currentTime.txt", "r");
    char timestamp[64];
    fgets(timestamp, sizeof(timestamp), timeFile);
    printf("\n%s\n", timestamp);
    fclose(timeFile);
    return NULL;
}

struct room* createNewRoom(char* newName, char* newRoomType) {
    // Returns pointer to a new room, with specified name and room type
    struct room* newRoom = malloc(sizeof(struct room));

    memset(newRoom->name, '\0', sizeof(newRoom->name));
    strcpy(newRoom->name, newName);

    newRoom->connections = 0;

    memset(newRoom->roomType, '\0', sizeof(newRoom->roomType));
    strcpy(newRoom->roomType, newRoomType);

    // connected represents the names of rooms that are connected to this one
    // For initialization, set all of these names to None
    int i = 0;
    for (i = 0; i < 6; i++) {
        memset(newRoom->connected[i], '\0', sizeof(newRoom->connected[i]));
        strcpy(newRoom->connected[i], "None");
    }
    return newRoom;
}

struct path* createNewPath() {
    // A path is a struct that contains the order in which rooms are visited
    // createNewPath() initializes a new path struct and returns a pointer to it
    struct path* newPath = malloc(sizeof(struct path));

    // Initialize with 0 steps
    newPath->steps = 0;

    // Initialize stepsArr with every value of array with sentinel value of -1
    int i;
    for (i = 0; i < 32; i++) {
        newPath->stepsArr[i] = -1;
    }

    return newPath;
}

void addPath(struct path* curPath, int curRoom) {
    // Adds the index of a room in the rooms array to the specified path
    curPath->stepsArr[curPath->steps] = curRoom;
    curPath->steps++;
}

void addRoomConnection(struct room* room1, char* roomConnection) {
    // Adds a connection from the current room to the room with the specified
    // name
    strcpy(room1->connected[room1->connections], roomConnection);
    room1->connections++;
}

int isConnected(struct room* curRoom, char* roomInput) {
    // Returns 1 if the roomInput is connected to curRoom, 0 otherwise
    int i;
    for (i = 0; i < curRoom->connections; i++) {
        if (strcmp(curRoom->connected[i], roomInput) == 0) {
            return 1;
        }
    }
    return 0;
}

void setRoomType(struct room* room1, char* newRoomType) {
    // Sets the room type of the given room to the specified type
    memset(room1->roomType, '\0', sizeof(room1->roomType));
    strcpy(room1->roomType, newRoomType);
}

int findRoomInd(struct room** rooms, char* roomName) {
    // Given a room name, find the index it occupies in rooms array
    int i;
    for (i = 0; i < 7; i++) {
        if (strcmp(rooms[i]->name, roomName) == 0) {
            return i;
        }
    }

    // Returns -1 if not found
    return -1;
}

void printCurRoom(struct room** rooms, int curRoom) {
    // Prints the name of the current room and the rooms it is connected to
    printf("CURRENT LOCATION: %s\n", rooms[curRoom]->name);
    printf("POSSIBLE CONNECTIONS: ");
    int i = 0;
    for (i = 0; i < rooms[curRoom]->connections; i++) {
        printf("%s", rooms[curRoom]->connected[i]);
        if (i != (rooms[curRoom]->connections - 1)) {
            printf(", ");
        }
    }
    printf(".\n");
}

int main() {
    // Locks mutex with main thread
    int result_code;
    pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&myMutex);

    int newestDirTime = -1; // Modified timestamp of newest subdir examined
    char targetDirPrefix[32] = "garlandk.rooms."; // Prefix we're looking for
    char newestDirName[256]; // Holds the name of the newest dir that contains prefix
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR* dirToCheck; // Holds the directory we're starting in
    struct dirent *fileInDir; // Holds the current subdir of the starting dir
    struct stat dirAttributes; // Holds information we've gained about subdir

    dirToCheck = opendir("."); // Open up the directory this program was run in

    if (dirToCheck > 0) // Make sure the current directory could be opened
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
            {
                //printf("Found the prefex: %s\n", fileInDir->d_name);
                stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

                if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
                {
                newestDirTime = (int)dirAttributes.st_mtime;
                memset(newestDirName, '\0', sizeof(newestDirName));
                strcpy(newestDirName, fileInDir->d_name);
                //printf("Newer subdir: %s, new time: %d\n",
                //       fileInDir->d_name, newestDirTime);
                }
            }
        }
    }

    closedir(dirToCheck); // Close the directory we opened



    DIR* newestDir = opendir(newestDirName);

    // Create an array to store pointers to all rooms
    struct room* rooms[7];

    // Initialize parameters needed to read room files
    FILE* curRoomFile;
    ssize_t nread;
    char readBuffer[512];
    char filenameBuffer[128];
    int i = 0;
    char curRoomName[32];
    char curConnectionName[32];
    char curRoomType[32];

    // Read in all files in rooms directory
    while ((fileInDir = readdir(newestDir)) != NULL) {
        // Opens the file if it is a rooms file
        if (fileInDir->d_type == DT_REG) {
            // Clears char buffers
            memset(readBuffer, '\0', sizeof(readBuffer));
            memset(filenameBuffer, '\0', sizeof(filenameBuffer));
            memset(curRoomName, '\0', sizeof(curRoomName));
            memset(curConnectionName, '\0', sizeof(curConnectionName));
            memset(curRoomType, '\0', sizeof(curRoomType));

            // Constructs room filename to open
            strcpy(filenameBuffer, newestDirName);
            strcat(filenameBuffer, "/");
            strcat(filenameBuffer, fileInDir->d_name);
            curRoomFile = fopen(filenameBuffer, "r");
            if (!curRoomFile) {
                fprintf(stderr, "Could not open room file\n");
                perror("Error opening file");
                exit(1);
            }

            // Get room name and strip trailing newline
            fgets(readBuffer, sizeof(readBuffer), curRoomFile);
            readBuffer[strlen(readBuffer) - 1] = '\0';
            strcpy(curRoomName, &readBuffer[11]);

            // Create new room struct with read-in room name
            rooms[i] = createNewRoom(curRoomName, "MID_ROOM");

            // Get connections and room type
            while (fgets(readBuffer, sizeof(readBuffer), curRoomFile) != NULL) {
                if (strstr(readBuffer, "CONNECTION") != NULL) {
                    readBuffer[strlen(readBuffer) - 1] = '\0';
                    strcpy(curConnectionName, &readBuffer[14]);
                    addRoomConnection(rooms[i], curConnectionName);
                }
                else {
                    readBuffer[strlen(readBuffer) - 1] = '\0';
                    strcpy(curRoomType, &readBuffer[11]);
                    setRoomType(rooms[i], curRoomType);
                }
            }

            // Close current file
            fclose(curRoomFile);
            i++;
        }
    }
    closedir(newestDir);

    // Start the adventure game
    int curRoom = 0;
    size_t bufferSize = 0;
    pthread_t timeThread;
    char* userInput = NULL;
    int charsEntered;
    int breakLoop = 0;
    struct path* gamePath = createNewPath();

    // Run the game until the END_ROOM is reached
    while (strcmp(rooms[curRoom]->roomType, "END_ROOM") != 0) {
        breakLoop = 0;
        printCurRoom(rooms, curRoom);

        // Get user input
        while (breakLoop == 0) {
            printf("WHERE TO? >");
            charsEntered = getline(&userInput, &bufferSize, stdin);
            userInput[charsEntered - 1] = '\0';
            if ((isConnected(rooms[curRoom], userInput) == 0) &&
                (strcmp(userInput, "time") != 0)) {
                // Prompt user for another input if the input is neither a room
                // that the current one is connected to, or "time"
                printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
                free(userInput);
                bufferSize = 0;
                userInput = NULL;
            }
            else if (strcmp(userInput, "time") == 0) {
                // If user inputs "time", then update the currentTime.txt file
                // and print the current time

                // Main thread needs to release mutex
                pthread_mutex_unlock(&myMutex);

                // Creates a new timeThread to update the currentTime.txt file
                // Passes myMutex to function so that timeThread can lock the
                // mutex within the setTime function
                result_code = pthread_create(&timeThread, NULL, setTime, &myMutex);

                // Block until timeThread unlocks and terminates
                pthread_join(timeThread, NULL);

                // Lock mutex with main thread again
                pthread_mutex_lock(&myMutex);

                // Prints the time from within main thread
                getTime();

                free(userInput);
                bufferSize = 0;
                userInput = NULL;
            }
            else {
                // Move player to the specified room and add to path
                curRoom = findRoomInd(rooms, userInput);
                addPath(gamePath, curRoom);

                printf("\n");
                free(userInput);
                bufferSize = 0;
                userInput = NULL;
                breakLoop = 1;
            }
        }
    }
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");

    printf("YOU TOOK %d STEPS. ", gamePath->steps);
    printf("YOUR PATH TO VICTORY WAS:\n");

    for (i = 0; i < gamePath->steps; i++) {
        printf("%s\n", rooms[gamePath->stepsArr[i]]->name);
    }

    // Free dynamically allocated memory
    for (i = 0; i < 7; i++) {
        free(rooms[i]);
    }
    free(gamePath);

    return 0;
}
