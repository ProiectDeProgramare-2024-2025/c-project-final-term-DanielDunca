#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

FILE *matches;
FILE *players;
FILE *temp_file;
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define PURPLE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"


struct players {
  char player1[50];
  char player2[50];
  char score1[50];
  char score2[50];
};
struct player_stats {
  char name[50];
  int wins;
  int losses;
};
void clear_console() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void menu() {
  clear_console();
  printf("-------"GREEN"Table" RESET"" CYAN " Tennis" RESET""PURPLE" Results"RESET"-------\n");
  printf("---------------------------------\n");
  printf("1." MAGENTA " Players" RESET "\n");
  printf("2. " GREEN "Ranking" RESET "\n");
  printf("3. " PURPLE "History" RESET "\n");
  printf("---------------------------------\n");
  printf("0. " RED "Exit" RESET "\n");
}
void while_score1(struct players *player) {
  while (1) {
    scanf("%s", player->score1);
    if (strspn(player->score1, "0123456789") == strlen(player->score1)) {
      int score = atoi(player->score1);
      if (score >= 0 && score <= 21) break;
      else {
        printf("\n");
        printf("Score cannot exceed "RED"21 "RESET"points.\n");
        printf("Enter score again: ");
      }
    } else {
      printf("\n");
      printf("Invalid input!\n");
      printf("Enter "PURPLE"digits only: "RESET);
    }
  }
}

void while_score2(struct players *player) {
  while (1) {
    scanf("%s", player->score2);
    if (strspn(player->score2, "0123456789") == strlen(player->score2)) {
      int score = atoi(player->score2);
      if (score >= 0 && score <= 21) break;
      else {
        printf("\n");
        printf("Score cannot exceed "RED"21 "RESET"points.\n");
        printf("Enter score again: ");
      }
    } else {
      printf("\n");
      printf("Invalid input!\n");
      printf("Enter "PURPLE"digits only: "RESET);
    }
  }
}
void find_player(const char *name, struct player_stats *stats) {
  FILE *pfile = fopen("players", "r");
  if (pfile == NULL) {
    strcpy(stats->name, name);
    stats->wins = 0;
    stats->losses = 0;
    return;
  }

  char line[256];
  char player_name[50];
  int wins, losses;
  int found = 0;

  while (fgets(line, sizeof(line), pfile)) {
    if (sscanf(line, "%s %d/%d\n", player_name, &wins, &losses) == 3) {
      if (strcmp(player_name, name) == 0) {
        strcpy(stats->name, player_name);
        stats->wins = wins;
        stats->losses = losses;
        found = 1;
        break;
      }
    }
  }

  fclose(pfile);

  if (!found) {
    strcpy(stats->name, name);
    stats->wins = 0;
    stats->losses = 0;
  }
}

void update_player_stats(const char *name, int won) {
  struct player_stats stats;
  find_player(name, &stats);

  if (won) {
    stats.wins++;
  } else {
    stats.losses++;
  }

  temp_file = fopen("temp_players", "w");
  players = fopen("players", "r");
  int player_updated = 0;

  if (players != NULL) {
    char line[256];
    char player_name[50];
    int wins, losses;

    while (fgets(line, sizeof(line), players)) {
      if (sscanf(line, "%s %d/%d\n", player_name, &wins, &losses) == 3) {
        if (strcmp(player_name, name) == 0) {
          fprintf(temp_file, "%s %d/%d\n", name, stats.wins, stats.losses);
          player_updated = 1;
        } else {
          fprintf(temp_file, "%s", line);
        }
      } else {
        fprintf(temp_file, "%s", line);
      }
    }
    fclose(players);
  }

  if (!player_updated) {
    fprintf(temp_file, "%s %d/%d\n", name, stats.wins, stats.losses);
  }

  fclose(temp_file);

  remove("players");
  rename("temp_players", "players");
}

void for_option1() {
  int n;
  int score1, score2;
  printf("Enter "PURPLE"number"RESET" of matches"YELLOW"(digits)"RESET": ");
  scanf("%d", &n);
  struct players player;
  for (int i = 0; i < n; i++) {
    printf("\n");
    printf("-------------"CYAN"Match"RESET" "PURPLE"%d"RESET"-------------\n", i+1);
    printf("\n");
    printf("Enter player 1 "MAGENTA"(name)"RESET": ");
    scanf("%s", player.player1);
    printf("\n");
    printf("Enter player 1 score "GREEN"(<=21)"RESET": ");
    while_score1(&player);
    printf("\n");
    printf("Enter player 2 "MAGENTA"(name)"RESET": ");
    scanf("%s", player.player2);
    printf("\n");
    printf("Enter player 2 score "GREEN"(<=21)"RESET": ");
    while_score2(&player);
    fprintf(matches, "[%s] %s / %s [%s]\n", player.player1, player.score1, player.score2, player.player2);
    fflush(matches);
    fprintf(players, "%s\n", player.player1);
    fprintf(players, "%s\n", player.player2);
    score1 = atoi(player.score1);
    score2 = atoi(player.score2);
    if (score1 > score2) {
      update_player_stats(player.player1, 1);
      update_player_stats(player.player2, 0);
    } else if (score2 > score1) {
      update_player_stats(player.player1, 0);
      update_player_stats(player.player2, 1);
    } else {
      update_player_stats(player.player1, 0);
      update_player_stats(player.player2, 0);
    }
  }
}

void option1() {
  players = fopen("players", "a");
  matches = fopen("matches", "a");
  struct players player;
  clear_console();
  for_option1();
  int choice=0;
  while (1) {
    clear_console();
    printf("----------------------------------------\n");
    printf("1. To Enter "GREEN"More"RESET" Matches\n");
    printf("-----------------------------------------\n");
    printf("9. "RED"Main page"RESET"\n");
    scanf("%d", &choice);
    if (choice == 9) {
      fclose(players);
      fclose(matches);
      clear_console();
      menu();
      return;
    }
    if (choice==1){
      option1();
      break;
    }
  }
  fclose(players);
  fclose(matches);
}
int compare_players(const void *a, const void *b) {
  const struct player_stats *playerA = (const struct player_stats *)a;
  const struct player_stats *playerB = (const struct player_stats *)b;

  return playerB->wins - playerA->wins;
}

void option2() {
  clear_console();
  printf("----------------"CYAN"Ranking"RESET"------------------\n");
  printf("-------------"PURPLE"NAME"RESET"-"GREEN"WINS"RESET"/"RED"LOSES"RESET"-------------\n");

  players = fopen("players", "r");
  if (players == NULL) {
    printf("No player data available.\n");
    printf("\n-----------------------------------------\n");
    printf("9. "RED"Main page"RESET"\n");
    int choice;
    scanf("%d", &choice);
    if (choice == 9) {
      clear_console();
      menu();
    }
    return;
  }

  struct player_stats all_players[100];
  int player_count = 0;

  char line[256];
  while (fgets(line, sizeof(line), players) && player_count < 100) {
    if (sscanf(line, "%s %d/%d\n",
               all_players[player_count].name,
               &all_players[player_count].wins,
               &all_players[player_count].losses) == 3) {
      player_count++;
               }
  }
  fclose(players);
  qsort(all_players, player_count, sizeof(struct player_stats), compare_players);

  for (int i = 0; i < player_count; i++) {
    printf("%d. " PURPLE "%s" RESET " " GREEN "%d" RESET "/" RED "%d" RESET "\n",
           i+1,
           all_players[i].name,
           all_players[i].wins,
           all_players[i].losses);
  }


  printf("\n");
  int choice=0;
  while (1) {
    printf("-----------------------------------------\n");
    printf("9. "RED"Main page"RESET"\n");
    scanf("%d", &choice);
    if (choice == 9) {
      clear_console();
      menu();
      return;
    } else {
      clear_console();
      printf("----------------"CYAN"Ranking"RESET"------------------\n");
      printf("-------------"PURPLE"NAME"RESET"-"GREEN"WINS"RESET"/"RED"LOSES"RESET"-------------\n");

      for (int i = 0; i < player_count; i++) {
        printf("%d. " CYAN "%s" RESET " " GREEN "%d" RESET "/" RED "%d" RESET "\n",
               i+1,
               all_players[i].name,
               all_players[i].wins,
               all_players[i].losses);
      }

      printf("\n");
    }
  }
}

void option3() {
  clear_console();
  matches = fopen("matches", "r");
  char buffer[256];
  printf("------------"CYAN"History"RESET"--------------");
  printf("\n");
  while (fgets(buffer, 256, matches) != NULL) {
    char p1[50], p2[50], s1[50], s2[50];
    if (sscanf(buffer, "[%49[^]]] %49s / %49s [%49[^]]]", p1, s1, s2, p2) == 4) {
      printf("[" PURPLE "%s" RESET "] " YELLOW "%s" RESET " / " YELLOW "%s" RESET " [" PURPLE "%s" RESET "]\n",
             p1, s1, s2, p2);
    } else {
      printf("%s", buffer);
    }
  }

  printf("\n");
  int choice=0;
  while (1) {
    printf("-----------------------------------------\n");
    printf("9. "RED"Main page"RESET"\n");
    scanf("%d", &choice);
    if (choice == 9) {
      fclose(matches);
      clear_console();
      menu();
      return;
    }
    else {
      clear_console();
    }
    fclose(matches);
  }
}

void leave() {
  clear_console();
  printf("You "RED"exited"RESET" the program");
}
int main() {
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 500000;
  fopen("matches", "r");
  int a[3]={1,2,3};
  int input=-1;
  menu();
  while (input!=0) {
    scanf("%d",&input);
    if (input==a[input-1] || input == 9) {
      if (input==1) {
        option1();
      }
      if (input==2) {
        option2();
      }
      if (input==3) {
        option3();
      }
    }
    else {
      clear_console();
      printf("This is the "RED"wrong"RESET" option\n");
      select(0, NULL, NULL, NULL, &tv);
      clear_console();
      menu();
    }
  }
  leave();
  return 0;
}


