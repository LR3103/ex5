/***********
 ID: 215303207
 NAME: LiorRogov
***********/

#define TVSHOW 1
#define SEASON 2
#define EPISODE 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

//to read from user letter
typedef struct Let {
    char let;
    struct Let * nextLet;
} Let;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Episode *episodes, char *name);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();


int isValidTimeFormat(char *str) {
    // 1. Check strict length: Must be exactly 8 characters (e.g., "01:59:30")
    if (strlen(str) != 8) return 0;

    // 2. Check delimiters
    if (str[2] != ':' || str[5] != ':') return 0;

    // 3. Check that all other characters are digits
    for (int i = 0; i < 8; i++) {
        if (i == 2 || i == 5) continue;
        if (str[i] < '0' || str[i] > '9') return 0;
    }

    // 4. Parse values using atoi
    // atoi(str) starts at index 0, reads "HH", and stops automatically at the ':'
    int hours = atoi(str);       
    
    // atoi(str + 3) starts at index 3 (the minutes), reads "MM", stops at ':'
    int minutes = atoi(str + 3); 
    
    // atoi(str + 6) starts at index 6 (the seconds), reads "SS" until null terminator
    int seconds = atoi(str + 6); 

    // 5. Check constraints
    if (hours < 0 || hours > 99) return 0;   
    if (minutes < 0 || minutes > 59) return 0; 
    if (seconds < 0 || seconds > 59) return 0; 

    return 1;
}


char* getEpisodeLength() {
    char tempBuffer[24];
    int valid = 0;

    printf("Enter the length (xx:xx:xx):\n");
    while (!valid) {
        scanf(" %[^\n]", tempBuffer); // Read until newline

        if (isValidTimeFormat(tempBuffer)) {
            valid = 1;
        } else {
            printf("Invalid length, enter again:\n");
        }
    }

    // Allocate memory on Heap and copy
    char *lengthStr = (char *)malloc(strlen(tempBuffer) + 1);
    if (lengthStr == NULL) exit(1);
    
    strcpy(lengthStr, tempBuffer);
    return lengthStr;
}


int getPosition() {
    int num;
    int validInput = 0; // Flag to control the loop

    while (!validInput) {
        printf("Enter the position: \n");
        
        // scanf returns the number of items successfully read
        if (scanf("%d", &num) == 1) {
            if (num >= 0) {
                validInput = 1; // Success! Exit loop.
            } else {
                printf("Invalid! The number must be non-negative.\n");
            }
        } else {
            // If scanf returns 0, it means the user typed text (e.g., "abc")
            printf("Invalid! That is not a number.\n");
            
            // CRITICAL: Clear the input buffer to prevent an infinite loop
            while (getchar() != '\n'); 
        }
    }
    return num;
}

char* getRequestType(int type){
    switch (type)
    {
    case TVSHOW:
        return "show";
    
    case SEASON:
        return "season";

    case EPISODE:
        return "episode";

    default:
        return "unknown";
    }

}


Let * getLetPointer(int count){
    char let;
    int isRead = scanf("%c", &let);
    //if read is successfull
    if (isRead == EOF || let == '\n'){
        return NULL;
    }
    //create a pointer
    Let * ptr = (Let *)malloc(sizeof(Let));
    if (ptr == NULL) exit(1); 
    ptr->let = let;
    ptr->nextLet = getLetPointer(count + 1);
    return ptr;
}

int countLetters(Let* head, int count){
    if (head == NULL){
        return count;
    }
    return countLetters(head->nextLet, count + 1);
}

void getWord(Let * head, char * letPosition){
    if (head == NULL){
        *letPosition = '\0';
        return;
    }

    *letPosition = head->let;
    getWord(head->nextLet, letPosition + 1);
}

void freeLetters(Let* head){
    if (head == NULL)
        return;
    freeLetters(head->nextLet);
    //free current
    free(head);
}

char * getNameFromUser(int requestType){
    printf("Enter the name of the %s: \n", getRequestType(requestType));
    //Read letters one by one
    Let* letters = getLetPointer(0);
    int count = countLetters(letters, 0);
    char *name = (char * )malloc(count + 1);
    if (name == NULL){
        exit(1);
    }
    getWord(letters, name);
    freeLetters(letters);
    return name;
}

TVShow *findShow(char *name){
    if (dbSize == 0)
        return NULL;

    int i, j;
    for (i = 0; i < dbSize; i++){
        for(j = 0; j < dbSize; j++){
            if (database[i][j] == NULL)
                continue;
            if (strcmp(database[i][j]->name, name) == 0){
                return (database[i][j]);
            }
        }
    }
       
    //The TV Show with that name does not exist...
    return NULL;
}

TVShow ** ravelShows(){
    if (dbSize == 0)
        return NULL;

    TVShow ** raveledShows = (TVShow **)calloc(dbSize * dbSize, sizeof(TVShow *));

    // Check for allocation failure
    if (raveledShows == NULL) return NULL;

    int i, j;
    for (i = 0; i < dbSize; i++){
        for(j = 0; j < dbSize; j++){
            if (database[i][j] == NULL)
                continue;

            raveledShows[i * dbSize + j] = database[i][j];
        }
    }
    return raveledShows;
}

int countShowsOneDimArray(TVShow ** oneDimArray){
    int maxLength = dbSize * dbSize;
    int count;
    for (count = 0; count < maxLength; count++){
        if (oneDimArray[count] == NULL)
            return count;
    }
    return maxLength;
}

void addShow(){
    //lets ask for the show's name...
    char * showName = getNameFromUser(TVSHOW);
    //check if the name of the show already exists (relative to our shows)
    //if it does don't let me add
    //check if show exist
    TVShow * show = findShow(showName);
    if (show != NULL){
        free(showName);
        printf("Show already exists.\n");
        return;
    }

    //create new show 
    TVShow * newShow = (TVShow *)malloc(sizeof(TVShow));
    if (newShow == NULL) {exit(1);}
    newShow->name = showName;
    newShow->seasons = NULL; 

    //first get pointer to a raveled shows array
    TVShow ** oneDimArray = ravelShows(); //length of (dbCount^2)
    int showsCount = countShowsOneDimArray(oneDimArray);
    int newShowsCount = showsCount + 1; //including the new show
    int currentMaxSize = dbSize * dbSize;
    //find the currect index to place new show.
    int i;
    for (i = 0; i < showsCount; i++){
        TVShow *compareToTvShow = oneDimArray[i];
        char*inDbShowName = compareToTvShow->name;
        int compVal = strcmp(inDbShowName, showName);
        // if not equal string check the what supposed to be the index of the new show.
        if (compVal > 0){
            break;
        }
    }
    //we got the index to place our new TV Show    
    if (newShowsCount > currentMaxSize){ //it means we have to migrate or stuff to a bigger 2d array
        //looks crazy but we do hold all the tv shows in the one dim array
        for (int row = 0; row < dbSize; row++){
            free(database[row]);
        }
        free(database); 
        //increase dbsize
        dbSize++;
        database = (TVShow***)calloc(dbSize, sizeof(TVShow**));
        if (database == NULL){
            exit(1);
        }
        for (int row = 0; row < dbSize; row++){
            TVShow **rowPtr = (TVShow **)calloc(dbSize, sizeof(TVShow *));
            if (rowPtr == NULL) {exit(1);}
            database[row] = rowPtr;
        }
    }
    //place all the values until the i value
    int row, col;
    for (int index = 0; index < i; index++){
        row = index / dbSize;
        col = index % dbSize;
        database[row][col] = oneDimArray[index];
    }
    //place the i index
    row = i / dbSize;
    col = i % dbSize;
    database[row][col] = newShow;

    //place all after the i index (shows count is the inex )
    for (int index = i + 1; index < newShowsCount; index++){
        row = index / dbSize;
        col = index % dbSize;
        database[row][col] = oneDimArray[index - 1];
    }

    //data base restored free unneccessary stuff
    free(oneDimArray);
}


Episode *findEpisodeRec(Episode* head, char *name){
    if (head == NULL){
        return NULL;
    }
    //compare names
    int val = strcmp(head->name, name);
    if (val == 0)//if equal
        return head;
    
    //CONTINUE LOOKING
    return findEpisodeRec(head->next, name);
}
Episode *findEpisode(Episode *episodes, char *name){
    //recursive method
    if (episodes == NULL){
        return NULL;
    }
    return findEpisodeRec(episodes, name);
}

Season *findSeasonRec(Season* head, char *name){
    if (head == NULL){
        return NULL;
    }
    //compare names
    int val = strcmp(head->name, name);
    if (val == 0)//if equal
        return head;
    
    //CONTINUE LOOKING
    return findSeasonRec(head->next, name);
}
Season *findSeason(TVShow *show, char *name){
    //recursive method
    if (show == NULL){
        return NULL;
    }
    return findSeasonRec(show->seasons, name);
}

// Change return type from void to Season*
Season* insertSeason(Season *head, Season *newSeason, int pos) {
    // 1. Base Case: List is empty
    // If the list is empty, the new season becomes the head.
    if (head == NULL) {
        return newSeason;
    }
    
    if (pos == 0) {
        newSeason->next = head; 
        return newSeason;       
    }

    head->next = insertSeason(head->next, newSeason, pos - 1);

    //Return the current head unchanged
    return head;
}

void addSeason(){
    //ask for the show name
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }

    //the show is in the data base
    //lets ask for the season name
    char * seasonName = getNameFromUser(SEASON);
    //Does the season exist?
    Season * isDuplicated = findSeason(show, seasonName);
    if (isDuplicated != NULL){
        printf("Season already exists.\n");
        free(seasonName);
        return;
    } 
    //ask user for position (non negative number)
    int pos = getPosition();
    //create a season object
    Season *newSeason = (Season *)malloc(sizeof(Season));
    newSeason->name = seasonName;
    newSeason->episodes = NULL;
    newSeason->next = NULL;
    //insert it into the currect position in seasons   
    show->seasons = insertSeason(show->seasons, newSeason, pos);
}


Episode* insertEpisode(
    Episode * head,
    Episode * newEpisode,
    int pos
){
    if (head == NULL){
        return newEpisode;
    }
    if (pos == 0){
        newEpisode->next = head;
        return newEpisode; 
    }
    head->next = insertEpisode(head -> next, newEpisode, pos - 1);
    return head;
}


void addEpisode(){
    //ask for the show name
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }

    //the show is in the data base
    //lets ask for the season name
    char * seasonName = getNameFromUser(SEASON);
    //Does the season exist?
    Season * season = findSeason(show, seasonName);
    free(seasonName);
    if (season == NULL){
        printf("Season not found.\n");
        return;
    } 

    char * episodeName = getNameFromUser(EPISODE);
    Episode * isEpisode = findEpisode(season->episodes, episodeName);
    if (isEpisode != NULL){ //exists
        printf("Episode already exists.\n");
        free(episodeName);
        return;
    }

    //does not exist create a new episode
    Episode * newEpisode = (Episode *)malloc(sizeof(Episode));
    newEpisode->name = episodeName;
    newEpisode->length = getEpisodeLength();
    newEpisode->next = NULL;

    int pos = getPosition();
    season->episodes = insertEpisode(season->episodes, newEpisode, pos);
}



void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}



void freeEpisode(Episode * head){
    //free name
    if (head == NULL) return; // Add this safety guard!
    free(head->name);
    //free length
    free(head->length);
    free(head);
}


void freeEpisodes(Episode * head){
    if (head == NULL)
        return;
    
    freeEpisodes(head->next);
    
    //if we are here we got to the last
    freeEpisode(head);
}

void freeSeason(Season* season){
    if (season == NULL)
        return;
    //free name
    free(season->name);
    //free episods
    freeEpisodes(season->episodes);
    free(season);
}


void freeSeasons(Season *head){
    if (head == NULL)
        return;
    
    freeSeasons(head->next);
    
    //if we are here we got to the last
    freeSeason(head);
}



void freeShow(TVShow* show){
    if (show == NULL){
        return;
    }
    //free name
    free(show->name);
    //freeseasons
    freeSeasons(show->seasons);
    free(show);
}



void deleteShow(){
    //ask for show name
    if (dbSize == 0){
        //nothing to remove
        return;
    }

    char * showName = getNameFromUser(TVSHOW);
    //if the show exists
    TVShow* show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }
    TVShow ** oneDimArray = ravelShows();
    int showsCount = countShowsOneDimArray(oneDimArray);
    int updatedCount = showsCount - 1;
    int flag = updatedCount == (dbSize - 1) * (dbSize - 1);
    //if can be fit in a smaller array
    if (flag){
        //looks crazy but we do hold all the tv shows in the one dim array
        for (int row = 0; row < dbSize; row++){
            free(database[row]);
        }
        free(database); 
        //increase dbsize
        dbSize--;
        if (dbSize == 0){
            database = NULL;
            free(oneDimArray);
            freeShow(show);
            return;
        }
        database = (TVShow***)calloc(dbSize, sizeof(TVShow**));
        if (database == NULL){
            exit(1);
        }
        for (int row = 0; row < dbSize; row++){
            TVShow **rowPtr = (TVShow **)calloc(dbSize, sizeof(TVShow *));
            if (rowPtr == NULL) {exit(1);}
            database[row] = rowPtr;
        }
    }
    //anyways
    //insert all indexes except of the show we want to remove
    int dest = 0;
    for (int src = 0; src < showsCount; src++){
        if (oneDimArray[src] == show){
            continue;
        }
        int row = dest / dbSize;
        int col = dest % dbSize;
        database[row][col] = oneDimArray[src];
        dest++;
    }
    if (!flag){
        //remove the last value from the database
        int row = (showsCount - 1) / dbSize;
        int col = (showsCount - 1) % dbSize;
        database[row][col] = NULL;
    }
    freeShow(show);
    free(oneDimArray);
}


Season * removeSeason(Season * head, Season * toRemove){
    if (head == NULL)
        return NULL;
    
    if (head == toRemove) {
        Season *nextSeason = head->next;        
        freeSeason(head);
        return nextSeason; 
    }
    head->next = removeSeason(head->next, toRemove);
    return head;
}

void deleteSeason(){
    //ask for the show name
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }

    //the show is in the data base
    //lets ask for the season name
    char * seasonName = getNameFromUser(SEASON);
    //Does the season exist?
    Season * season = findSeason(show, seasonName);
    free(seasonName);
    if (season == NULL){
        printf("Season not found.\n");
        return;
    } 
    show->seasons = removeSeason(show->seasons, season);
}

Episode * removeEpisode(Episode * head, Episode * toRemove){
    if (head == NULL)
        return NULL;
    
    if (head == toRemove) {
        Episode *nextEpisode = head->next;        
        freeEpisode(head);
        return nextEpisode; 
    }
    head->next = removeEpisode(head->next, toRemove);
    return head;
}

void deleteEpisode(){
    //ask for the show name
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }

    //the show is in the data base
    //lets ask for the season name
    char * seasonName = getNameFromUser(SEASON);
    //Does the season exist?
    Season * season = findSeason(show, seasonName);
    free(seasonName);
    if (season == NULL){
        printf("Season not found.\n");
        return;
    } 

    char * episodeName = getNameFromUser(EPISODE);
    Episode * episode = findEpisode(season->episodes, episodeName);
    free(episodeName);
    if (episode == NULL){
        printf("Episode not found.\n");
        return;
    } 

    season->episodes = removeEpisode(season->episodes, episode);
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}


void printEpisodes(Episode * head, int count){
    if (head == NULL){
        return;
    }
    printf("        Episode %d: %s (%s)\n", count, head->name, head->length);
    //print episods

    printEpisodes(head->next, count+1);
}

void printSeasons(Season * head, int count){
    if (head == NULL){
        return;
    }
    printf("    Season %d: %s\n", count, head->name);
    //print episods
    printEpisodes(head->episodes, 0);
    //continue for the next season
    printSeasons(head->next, count+1);
}


void printShow(){
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }
    //print name
    printf("Name: %s\n", show->name);
    //print seasons
    printf("Seasons: \n");
    //print Seasons
    printSeasons(show->seasons, 0);

}

void printEpisode(){
    //ask for the show name
    char * showName = getNameFromUser(TVSHOW);
    //check if show name is in database 
    TVShow * show = findShow(showName);
    free(showName);
    if (show == NULL){
        printf("Show not found.\n");
        return;
    }

    //the show is in the data base
    //lets ask for the season name
    char * seasonName = getNameFromUser(SEASON);
    //Does the season exist?
    Season * season = findSeason(show, seasonName);
    free(seasonName);
    if (season == NULL){
        printf("Season not found.\n");
        return;
    } 

    char * episodeName = getNameFromUser(EPISODE);
    Episode * episode = findEpisode(season->episodes, episodeName);
    free(episodeName);
    if (episode == NULL){
        printf("episode not found.\n");
        return;
    } 

    printf("Name: %s\n", episode->name);
    printf("Length: %s\n", episode->length);

}

void printArray(){
    for (int row = 0; row < dbSize; row++){
        for (int col = 0; col < dbSize; col++){
            // Check the database directly
            if (database[row][col] != NULL) {
                printf("[%s]\t", database[row][col]->name);
            } else {
                printf("[NULL]\t");
            }
        }
        printf("\n");
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

void freeAll(){
    TVShow ** showsArray = ravelShows();
    int countShows = countShowsOneDimArray(showsArray);
    for (int i = 0; i < countShows; i++){
        freeShow(showsArray[i]);
    }
    free(showsArray);
    for (int row = 0; row < dbSize; row++){
        free(database[row]);
    } 
    free(database);
    database = NULL;
    dbSize = 0;
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}
