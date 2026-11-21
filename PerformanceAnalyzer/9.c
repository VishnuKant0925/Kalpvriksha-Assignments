#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Players_data.h"

typedef enum{
    Batsman = 1,
    Bowler = 2,
    AllRounder = 3
} Role;

typedef struct PlayerRecord{
    int playerId;
    char name[50];
    char teamName[50];
    Role playerRole;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
} PlayerRecord;

typedef struct PlayerNode{
    PlayerRecord player;
    struct PlayerNode *next;
} PlayerNode;

typedef struct Team{
    int teamId;
    char name[50];
    int totalPlayers;
    float averageBattingStrikeRate;

    float totalBattingStrikeRate;
    int battingPlayerCount;

    PlayerNode *batsmen;
    PlayerNode *bowlers;
    PlayerNode *allRounders;

} Team;

Team teamsArray[10];

Role getRole(const char *role){
    if (strcmp(role, "Batsman") == 0){
        return Batsman;
    }
    if(strcmp(role,"Bowler")==0){
        return Bowler;
    }
    return AllRounder;
}

void initializeTeams(){
    for(int i=0;i<teamCount;i++){
        teamsArray[i].teamId = i+1;
        strcpy(teamsArray[i].name , teams[i]);

        teamsArray[i].totalPlayers = 0;
        teamsArray[i].averageBattingStrikeRate = 0.0;
        teamsArray[i].totalBattingStrikeRate = 0.0;
        teamsArray[i].battingPlayerCount = 0;

        teamsArray[i].batsmen = NULL;
        teamsArray[i].bowlers = NULL;
        teamsArray[i].allRounders = NULL;
    }
}

PlayerNode* createPlayerNode(PlayerRecord player){
    PlayerNode * newNode = malloc(sizeof(PlayerNode));
    if(newNode == NULL){
        printf("Memory Allocation failed :\n");
        return NULL;
    }

    newNode->next = NULL;
    newNode->player = player;

    return newNode;
}

void insertPlayerSorted(PlayerNode **head , PlayerRecord player){
    PlayerNode * newNode = createPlayerNode(player);
    if(newNode==NULL){
        return;
    }

    if(*head == NULL){
        *head = newNode;
        return;
    }
    PlayerNode * temp = *head;

    if(newNode->player.performanceIndex >= temp->player.performanceIndex){
        newNode->next = temp;
        *head = newNode;
        return;
    }

    while(temp->next != NULL && temp->next->player.performanceIndex > newNode->player.performanceIndex){
        temp = temp->next;
    }
    newNode->next = temp->next;
    temp->next = newNode;

}

int TeamIndex(const char *teamName){
    int low = 0;
    int high = 9;
    while(low<=high){
        int mid = (low+high)/2;
        int res = strcmp(teamName, teamsArray[mid].name);
        if (res == 0){
            return mid;
        }
        else if(res > 0 ){
            low = mid +1;
        }
        else{
            high = mid-1;
        }
    }
    return -1;
}
float PerformanceIndex(PlayerRecord *p)
{
    if (p->playerRole == Batsman)
    {
        return (p->battingAverage * p->strikeRate) / 100.0;
    }
    if (p->playerRole == Bowler)
    {
        return ((p->wickets * 2.0) + (100.0 - p->economyRate));
    }

    return (((p->battingAverage * p->strikeRate) / 100.0) + (p->wickets * 2.0));
}
void initialisePlayer(){
    for(int i=0;i<playerCount;i++){

        PlayerRecord newPlayer;

        newPlayer.playerId = players[i].id;
        strcpy(newPlayer.name ,players[i].name);
        strcpy(newPlayer.teamName , players[i].team);
        newPlayer.playerRole = getRole(players[i].role);
        newPlayer.totalRuns = players[i].totalRuns;
        newPlayer.battingAverage = players[i].battingAverage;
        newPlayer.strikeRate = players[i].strikeRate;
        newPlayer.wickets = players[i].wickets;
        newPlayer.economyRate = players[i].economyRate;

        float perIndex = PerformanceIndex(&newPlayer);

        newPlayer.performanceIndex = perIndex;

        int teamIdx = TeamIndex(players[i].team);

        if(teamIdx == -1){
            continue;
        }
        
       
        if(newPlayer.playerRole == Batsman || newPlayer.playerRole == AllRounder){
            teamsArray[teamIdx].totalBattingStrikeRate += newPlayer.strikeRate;
            teamsArray[teamIdx].battingPlayerCount++;
        }

        if(teamsArray[teamIdx].battingPlayerCount > 0){
            teamsArray[teamIdx].averageBattingStrikeRate = teamsArray[teamIdx].totalBattingStrikeRate / teamsArray[teamIdx].battingPlayerCount;
        }
        

        if(newPlayer.playerRole == Batsman){
            PlayerNode **head = &teamsArray[teamIdx].batsmen;
            insertPlayerSorted( head , newPlayer);
        }
        else if(newPlayer.playerRole == Bowler){
            PlayerNode **head = &teamsArray[teamIdx].bowlers;
            insertPlayerSorted(head,newPlayer);
        }
        else{
            PlayerNode **head = &teamsArray[teamIdx].allRounders;
            insertPlayerSorted(head,newPlayer);
        }

        teamsArray[teamIdx].totalPlayers++;
    }
}
const char * roleName(Role role){
    if(role == Batsman){
        return "Batsman";
    }
    if(role == Bowler){
        return "Bowler";
    }
    return "All-Rounder";
}
void displayPlayer(PlayerNode * head){
    PlayerNode * temp = head;
    while(temp != NULL){
        printf("%-5d %-25s %-15s %-10d %-10.2f %-10.2f %-10d %-8.2f %-10.2f\n",
               temp->player.playerId,
               temp->player.name,
               roleName(temp->player.playerRole),
               temp->player.totalRuns,
               temp->player.battingAverage,
               temp->player.strikeRate,
               temp->player.wickets,
               temp->player.economyRate,
               temp->player.performanceIndex);

        temp = temp->next;
    }
}
void displayTeam(int id){
    if(id <1 || id >10){
        printf("Invalid Team ID\n");
        return;
    }
    printf("Players of Team %s:\n", teamsArray[id-1].name);
    printf("=====================================================================================================\n");
    printf("%-5s %-25s %-15s %-10s %-10s %-10s %-8s %-10s %-8s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER" ,"Perf.Index");
    displayPlayer(teamsArray[id-1].batsmen);
    displayPlayer(teamsArray[id-1].bowlers);
    displayPlayer(teamsArray[id-1].allRounders);

    printf("=====================================================================================================\n");
    printf("Total Players: %d\n", teamsArray[id-1].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamsArray[id-1].averageBattingStrikeRate);
}

void displayAvgSR(){
    printf("Teams Sorted by Average Batting Strike Rate:\n");
    printf("========================================================================================================\n");
    printf("%-5s %-25s %-10s %-10s\n", "ID", "Team Name", "Avg SR" , "Total Players");

    printf("==========================================================================================================\n");
    int tempArr[10];
    for(int i=0;i<10;i++){
        tempArr[i]=i;
    }
    for(int i=0;i<9;i++){
        for(int j=0;j<9-i-1;j++){
            int idx1 = tempArr[j];
            int idx2 = tempArr[j + 1];
            if(teamsArray[idx1].averageBattingStrikeRate < teamsArray[idx2].averageBattingStrikeRate)
            {
                int temp = tempArr[j];
                tempArr[j] = tempArr[j+1];
                tempArr[j+1] = temp;
            }
        }
    }
    for(int i=0;i<10;i++){
        int idx = tempArr[i];
        printf("%-5d %-25s %-10.2f %-10d\n", teamsArray[idx].teamId, teamsArray[idx].name, teamsArray[idx].averageBattingStrikeRate, teamsArray[idx].totalPlayers);
    }
}

void helperKPlayer(PlayerNode *head, int k){
    if(k <= 0){
        printf("Invalid value of k\n");
        return;
    }
    PlayerNode *temp = head;
    int count = 0;
    while (temp != NULL && count < k) {
        printf("%-5d %-25s %-15s %-10d %-10.2f %-10.2f %-10.2d %-8f %-10.2f\n",
               temp->player.playerId,
               temp->player.name,
               roleName(temp->player.playerRole),
               temp->player.totalRuns,
               temp->player.battingAverage,
               temp->player.strikeRate,
               temp->player.wickets,
               temp->player.economyRate,
               temp->player.performanceIndex);
        temp = temp->next;
        count++;
    }
}


void displayTopKPlayers(int teamId, int k, Role role){
    if(teamId <1 || teamId >10){
        printf("Invalid Team ID\n");
        return;
    }

    printf("Top %d Players of Team %s:\n", k, teamsArray[teamId-1].name);
    printf("=====================================================================================================\n");
    printf("%-5s %-25s %-15s %-10s %-10s %-10s %-8s %-10s %-8s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER" ,"Perf.Index");
    printf("=====================================================================================================\n");
    if (role == Batsman){
        helperKPlayer(teamsArray[teamId-1].batsmen,k);
    }
    else if(role == Bowler){
        helperKPlayer(teamsArray[teamId-1].bowlers,k);
    }
    else{
        helperKPlayer(teamsArray[teamId-1].allRounders,k);
    }
}
PlayerNode *copyList(PlayerNode *head)
{
    PlayerNode *newHead = NULL, *tail = NULL;
    while (head != NULL)
    {
        PlayerNode *n = createPlayerNode(head->player);
        if (n == NULL)
            break;
        if (newHead == NULL)
            newHead = n;
        else
            tail->next = n;
        tail = n;
        head = head->next;
    }
    return newHead;
}
PlayerNode *mergeTwoLists(PlayerNode *a, PlayerNode *b)
{
    PlayerNode dummy;
    PlayerNode *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL)
    {
        if (a->player.performanceIndex >= b->player.performanceIndex)
        {
            tail->next = a;
            a = a->next;
        }
        else
        {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

PlayerNode *mergeAllLists(PlayerNode *lists[], int n)
{
    while (n > 1)
    {
        int m = 0;
        for (int i = 0; i < n; i += 2)
        {
            if (i + 1 < n)
                lists[m++] = mergeTwoLists(lists[i], lists[i + 1]);
            else
                lists[m++] = lists[i];
        }
        n = m;
    }
    return (n == 1) ? lists[0] : NULL;
}

void displayAllPlayersRole(Role role){
    printf("%s of all Teams:\n",roleName(role));
    printf("=====================================================================================================\n");
    printf("%-5s %-25s %-15s %-10s %-10s %-10s %-8s %-10s %-8s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER" ,"Perf.Index");
    printf("=====================================================================================================\n");  
    PlayerNode *list[10];
    for(int i=0;i<10;i++){
        if(role == Batsman){
            list[i] = copyList(teamsArray[i].batsmen);
        }
        else if(role == Bowler){
            list[i] = copyList(teamsArray[i].bowlers);
        }
        else{
            list[i] = copyList(teamsArray[i].allRounders);
        }
    }
    
    PlayerNode *mergedList = mergeAllLists(list, 10);
    PlayerNode *temp = mergedList;
    displayPlayer(temp);
    while (temp != NULL)
    {
        PlayerNode *next = temp->next;
        free(temp);
        temp = next;
    }
}

void printMenu(){
    printf("=====================================================================================================\n");
    printf("ICC Player Performance Analyzer\n");
    printf("===================================================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2.Display Players of a Specific Team\n");
    printf("3.Display Teams by Average Batting Strike Rate\n");
    printf("4.Display Top K Players of a Specific Team by Role\n");
    printf("5.Display all Players Of specific role Across All Teams by performance index\n");
    printf("6.Exit\n");

    printf("=====================================================================================================\n");
    printf("Enter your choice: ");
}
void addPlayerToTeam(int teamId){
    if(teamId <1 || teamId >10){
        printf("Invalid Team ID\n");
        return;
    }
    printf("Enter Player Details:\n");
    PlayerRecord newPlayer;
    printf("Enter Player ID: ");
    scanf("%d",&newPlayer.playerId);
    getchar(); 
    printf("Enter Player Name: ");
    fgets(newPlayer.name, sizeof(newPlayer.name), stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0';
    printf("\n");
    printf("Role (1-Batsman, 2-Bowler, 3-All-Rounder): ");
    scanf("%d",&newPlayer.playerRole);
    printf("Total Runs: ");
    scanf("%d",&newPlayer.totalRuns);
    printf("Batting Average: ");
    scanf("%f",&newPlayer.battingAverage);  
    printf("Strike Rate: ");
    scanf("%f",&newPlayer.strikeRate);
    printf("Wickets: ");
    scanf("%d",&newPlayer.wickets);
    printf("Economy Rate: ");
    scanf("%f",&newPlayer.economyRate);

    newPlayer.performanceIndex = PerformanceIndex(&newPlayer);

    if(newPlayer.playerRole == Batsman || newPlayer.playerRole == AllRounder){
        teamsArray[teamId-1].totalBattingStrikeRate += newPlayer.strikeRate;
        teamsArray[teamId-1].battingPlayerCount++;
    }
    if(teamsArray[teamId-1].battingPlayerCount > 0){
        teamsArray[teamId-1].averageBattingStrikeRate = teamsArray[teamId-1].totalBattingStrikeRate / teamsArray[teamId-1].battingPlayerCount;
    }

    if (newPlayer.playerRole == Batsman)
    {
        PlayerNode **head = &teamsArray[teamId-1].batsmen;
        insertPlayerSorted(head, newPlayer);
    }
    else if (newPlayer.playerRole == Bowler)
    {
        PlayerNode **head = &teamsArray[teamId-1].bowlers;
        insertPlayerSorted(head, newPlayer);
    }
    else
    {
        PlayerNode **head = &teamsArray[teamId-1].allRounders;
        insertPlayerSorted(head, newPlayer);
    }

    teamsArray[teamId-1].totalPlayers++;

    printf("Player added successfully to Team %s !\n", teamsArray[teamId-1].name);
}
int main(){
    int choice;
    initializeTeams();
    initialisePlayer();
    while(1){
        printMenu();
        if(scanf("%d",&choice)!=1){
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        if(choice == 1){
            int teamId;
            printf("Enter Team ID to add player: ");   
            if(scanf("%d",&teamId)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            addPlayerToTeam(teamId);
        }
        else if(choice == 2){
            int teamId;
            printf("Enter Team ID (1-10): ");
            if(scanf("%d",&teamId)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            displayTeam(teamId);
        }
        else if(choice == 3){
            displayAvgSR();
        }
        else if(choice == 4){
            int teamId, k, roleChoice;
            printf("Enter Team ID (1-10): ");
            if(scanf("%d",&teamId)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            printf("Enter Role (1-Batsman, 2-Bowler, 3-All-Rounder): ");
            if(scanf("%d",&roleChoice)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            printf("Enter value of K: ");
            if(scanf("%d",&k)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            displayTopKPlayers(teamId, k, (Role)roleChoice);
        }
        else if(choice == 5){
            int roleChoice;
            printf("Enter Role (1-Batsman, 2-Bowler, 3-All-Rounder): ");
            if(scanf("%d",&roleChoice)!=1){
                printf("Invalid input. Please enter a number.\n");
                continue;
            }
            displayAllPlayersRole((Role)roleChoice);
        }
        else if(choice == 6){
            break;
        }
        else{
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}