/*
Author: Chinwe Ofonagoro
Program Name: WWF3_Chinwe_Ofonagoro.c
Objective: Create two functions: cleanupWordListNodes() and cleanupMatchListNodes(), build the "bones"
    of displayWorld(), and complete acceptInput() and isDone().

*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


int count = 0;
char *masterWord = NULL;

/*
First data structre which contains wordString of size 30 and a node to point to the next structure.
*/
struct wordListNode
{
    char wordString[30];
    struct wordListNode *next;
};

/*
Second data structure with 30 character string, boolean for if the word has been found, and a pointer to the next node
*/
struct matchListNode{
    char matchString[30];
    bool found;
    struct matchListNode *matchNext;

};

struct wordListNode *root = NULL;
struct matchListNode *matchRoot = NULL;


//Function Declarations

/*
Add each read line into a new structure and move to the next word. 
This function will create a node to begin the structure, create space for memory allocation, read each line from the file, icrements the count,
 and copies the word into the structure created. It points to next, and if there are no more words to be read, it ends.
It returns the count to be used in getRandomWord().
*/
int initialization(){
    srand(time(NULL));
    FILE *fp;
    fp = fopen("2of12.txt", "r");

    char word[128];
    
    //create pointers
    struct wordListNode *first = NULL; //head of the list
    struct wordListNode *added = NULL; //next structure
    
    if(fp != NULL)
    {
        while (fgets(word, sizeof(word), fp))
        {
            for(int i = 0; word[i]; i++){
                word[i] = tolower(word[i]);
            }
            struct wordListNode *newNode = malloc(sizeof(struct wordListNode));
            if (newNode != NULL) 
            {
                strncpy(newNode->wordString, word, 30); //puts current word into the newNode
                newNode->wordString[strcspn(newNode->wordString, "\n")] = 0; //checks for each line and adds the new word into a structure
                newNode->next = NULL;
                count++;

                if(first == NULL)
                {
                    first = newNode;
                }
                else{
                    added->next = newNode;
                }
                added = newNode;
            } 

        }
    }
    root = first;
    //printf("The count: %d", count); //to make sure the count is correct
    fclose(fp);

    return count;
    
}

//Takes in the user's input and iterates through the string. It also creates a new array to create an alphabet assigning a letter to each item at an incremented index. 
void getLetterDistribution(char *input, int dist[26]){
    for (int i = 0; i < 26; i++) {
        dist[i] = 0; // Initialize all values to zero
    }
    while (*input) {
        char ch = *input;
        // Check if the character is an uppercase letter
        /*
        if (ch >= 'A' && ch <= 'Z') {
            int index = ch - 'A';
            dist[index]++;
        }
        */
        // Check if the character is a lowercase letter
        if (ch >= 'a' && ch <= 'z') {
            int index = ch - 'a';
            dist[index]++;
        }
        input++;
    }
}

/*
compareCounts() compares two distributions to determine whther a word can be made from a list of letters, both given. 
Using the function getLetterDistribution, we are able to get the instances of each letter in a word, and compare that to the list of letters.
The for loop checks if the number of times the letters in the given word is in list (wordDist) is greater then the number in the given list of letters.
    If the given word has more letters, then it cannot be formed
*/

bool compareCounts(char *letters, char* word){
    int lettersDist[26]; 
    int wordDist[26];
    getLetterDistribution(letters, lettersDist);
    getLetterDistribution(word, wordDist);
    for (int i = 0; i < 26; i++) {
        if (wordDist[i] > lettersDist[i]) {
            return false;
        }
    }
    return true;
}

/*
Function getRandomWord()
This picks a random number from 0 to the count, walks through the linked list until we reach the ith entry, 
    then searches for a word that is long enough to be the "master word". 
Then, it looks through the linked list and finds a word longer than 6 characters from that range, and uses that word as a master word.
If a word cannot be found in the range, the while loop ensures the index is randomized again until we find a word that satisfies.
*/
char *getRandomWord(){
    
    struct wordListNode *current; //pointer for traversing the linked list
    char *masterWord = NULL; //holds the master word within the range.

    srand(time(0));
    int attempts = 0;

    while (attempts < 4) {
        
        int randomIndex = rand()%count;
        current = root; //start from the head of the list

        for (int i = 0; i < randomIndex && current != NULL; i++) {
            current = current->next;
        }

        //Searches for a word > 6 characters
        if(current == NULL){
            attempts++; 
        }

        while (current != NULL) {
            if (strlen(current->wordString) > 6) {
                //printf("\nindex: %d", randomIndex);
                masterWord = current->wordString;

                return masterWord; //returns the master word to be used later
                break;
            }
            
            current = current->next;
        }
        
    }
    if(attempts >= 4){
        printf("\nCould not find a 6 character word.");
    }
    return NULL;
}


/*
findWords() function takes in the masterWord as a parameter and gets the distribution of the word. 
Then, it it uses compareCounts() to compare the count of each
    letter in every word in the linked list. If a word can be made, it is added to the linked list "matchListNode". Each word should then be 
    printed out.
*/
void findWords(char *masterWord) {
    struct wordListNode *current = root;
    struct matchListNode *last = NULL;

    //create distribution array for the master word
    int masterDistribution[26] = {0};
    getLetterDistribution(masterWord, masterDistribution);
    
    while (current != NULL) { 
        //array to compare to the current word in the wordString
        int wordDistribution[26] = {0};
        char currentWordCompare[27] = {0};
        strncpy(currentWordCompare, current->wordString, 26);
        getLetterDistribution(currentWordCompare, wordDistribution);

        struct matchListNode *matchNode = malloc(sizeof(struct matchListNode)); //creates the structure that holds the word

            if (compareCounts(masterWord, currentWordCompare)) {
        
                strcpy(matchNode->matchString, currentWordCompare);//makes a copy of the word that can be made from the master word
                //matchNode->matchString;
                matchNode->found = false; 
                matchNode->matchNext = NULL; 

                //if the head has no word, the list is empty. this starts the list. otherwise, add the word to the structure being pointed to (by matchNext)
                if (matchRoot == NULL) {
                    matchRoot = matchNode;
                } else {
                    last->matchNext = matchNode;
                }
                last = matchNode;

            
                //printf("\nMatched Word: %s", matchNode->matchString);
            }
        if (matchNode == NULL) {
            printf("\nCould not allocate memory.");
            break;
        }
        current = current->next;
    }
}

/*
Asks the user for an input, receives the input as a string/an array of letters and prints out the word.
The function compares the input to each word in the matchListNode list and marks found on the words that can be made.
*/
void acceptInput(){
    struct matchListNode *current = matchRoot;

    char input[26];
    printf("\nEnter a guess: ");
    fgets(input, sizeof(input), stdin);

    while(current != NULL){
        int inputDistribution[26] = {0};
        char currentMatchCompare[27] = {0};
        strncpy(currentMatchCompare, current->matchString, 26);
        getLetterDistribution(currentMatchCompare, inputDistribution);

        if(compareCounts(input, currentMatchCompare)){
            //printf("FOUND: %s", currentMatchCompare);
            current->found = true;
        }
        current = current->matchNext;
    }
        
    
}

/*
Function that displays the game
This function generates the sorted letter list (all uppercase) using getletterDistribution() and prints it out.
Then, it goes through matchListNode and for each word, prints the count of underscores equivalent to the length of the word.
*/
void displayWorld(char *masterWord){
    //generate the sorted letter list (words that match the master word) but the linked list
    int masterDistribution[26] = {0};
    getLetterDistribution(masterWord, masterDistribution);
    for (int i = 0; i < 26; i++) {
        if (masterDistribution[i] > 0){
            printf("%c ", 'A' + toupper(i));
        }
        
    }
    
    //print the hyphens
    printf("\n--------------------\n");

    //for each word in gameListNodes, print("Found:XXXX") and the word is from acceptInput()
    struct matchListNode *currentWord = matchRoot;
    

    while (currentWord != NULL) {
        //printf("\nFOUND: %s", currentWord->matchString);
        printf("\n");
        for (int i = 0; i < strlen(currentWord->matchString); i++) {
            printf("__ ");
        }
        printf("\n");
        currentWord = currentWord->matchNext; // Move to the next node
    }
    

}

/*
Function to stop the gameLoop() function
Returns true if all the words in gamelistNodes are found, and false otherwise                        
*/
bool isDone(){
    struct matchListNode *isDoneCurrent = matchRoot;
    while(isDoneCurrent !=NULL){
        if(!isDoneCurrent->found){
            return false;
        }
    }
    return true;
}

//Creates the gameboard and runs the acceptInput function
void gameLoop(){
    while(!isDone()){
        displayWorld(masterWord);
        acceptInput();
    }
}

//Printed at the end of the game
void tearDown(){
    printf("\nAll Done.");
}


/*
Function to clean up the memory associated with the game words found from the masterWord
The function walks through each structure in the wordListNode linked list and uses free() to deallocate the moemory created
*/
void cleanupGameListNodes(struct matchListNode *gameList){
    struct matchListNode *next; //helper pointer
        while(gameList)//while list points to something, point to first element on our list
        {
            next = gameList->matchNext;//next points to the next element in the list
            //printf("Cleaning %s\n", gameList->matchString);
            free(gameList);
            gameList = next; //moves on to the next element to clean
        }

}

/*
Function to clean up the memory associated with the dictionary. 
The function walks through each structure in the wordListNode linked list and uses free() to deallocate the moemory created
*/
void cleanupWordListNodes(struct wordListNode *wordList){
    struct wordListNode *wordNext; //helper pointer
        while(wordList)//while list points to something, point to first element on our list
        {
            wordNext = wordList->next;//next points to the next element in the list
            //printf("Cleaning %s\n", wordList->wordString);
            free(wordList);
            wordList = wordNext; //moves on to the next element to clean
        }
    
}

int main() {
    initialization();
    char *masterWord = getRandomWord();
    findWords(masterWord);
    
    displayWorld(masterWord);
    acceptInput();
    
    //gameLoop();
    //tearDown();

    cleanupGameListNodes(matchRoot);
    cleanupWordListNodes(root);

    return 0;
}
