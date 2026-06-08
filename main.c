#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#define TABLE_SIZE 101
  
struct member_list {
    char name[64];
    int age;
    struct follow_node *follow_head; // フォローリストの先頭
    struct member_list *next;        
};

struct follow_node {
    struct member_list *user;        // フォロー相手へのポインタ
    struct follow_node *next;
};

struct member_list *hashtable[TABLE_SIZE];

// ハッシュ関数
int hash(char *key) {
    unsigned int hashval = 0;
    while (*key != '\0') {
        hashval += *key;
        key++;
    }
    return hashval % TABLE_SIZE;
}


struct member_list *find_user(char *name) {
    int h = hash(name);
    struct member_list *user = hashtable[h];
    while(user != NULL){
		if(strcmp(user->name, name) == 0){
			return user; 
		}
		user = user->next;
	}
    return NULL;
}

bool is_following(struct member_list *user_from, struct member_list *user_to) {
    struct follow_node *current = user_from->follow_head; 
	struct follow_node *prev = NULL;    
	while (current != NULL) {
		if (current->user == user_to) return true;
		prev = current;
		current = current->next;
	}
    return false;
}

void free_all(void){
    for(int i=0;i<TABLE_SIZE;i++){
        struct member_list *user = hashtable[i];
        while(user != NULL){
            struct member_list *next_user = user->next;
            struct follow_node *f = user->follow_head;
            while(f != NULL){
                struct follow_node *next_f = f->next;
                free(f);
                f = next_f;
            }
            free(user);
            user = next_user;
        }
    }
}

void add(char *name, int age){ //: ユーザー追加
    if(find_user(name)==NULL){
        struct member_list *new_user;
        //メモリ確保
        new_user = (struct member_list *)malloc(sizeof(struct member_list)); 
        //挿入
        new_user->age=age;
        strncpy(new_user->name,name,sizeof(new_user->name)-1);
        new_user->name[sizeof(new_user->name) - 1] = '\0';
        new_user->follow_head =NULL;
        //登録
        int user_hash=hash(new_user->name);
        new_user->next=hashtable[user_hash];//衝突回ひ
        hashtable[user_hash]=new_user;
        return;
    }
    printf("Already existing!\n");
}

void follow(char *from, char *to){ //: 既存ユーザー間のフォロー関係構築
	struct member_list *user_from = find_user(from);
	struct member_list *user_to = find_user(to);
	if(user_from == NULL || user_to == NULL){
		printf("failed\n");	
		return;
	}
    if(user_from == user_to){
    return;
    }
    if (is_following(user_from, user_to)) {
        printf("Already following!\n");
        return;
    }
    struct follow_node *following;
    following = (struct follow_node *)malloc(sizeof(struct follow_node));
    following->user = user_to;
    following->next = user_from->follow_head;
    user_from->follow_head = following;
}

void unfollow(char *from, char *to){ //: フォロー解除（ノードの削除とメモリ解放）
	struct member_list *user_from = find_user(from);
	struct member_list *user_to = find_user(to);
	if (user_from == NULL || user_to == NULL) {
        printf("failed\n");	
		return; 
	} 
	struct follow_node *current = user_from->follow_head; 
	struct follow_node *prev = NULL;    
	while (current != NULL) {
		if (current->user == user_to) {
            if (prev == NULL){
                user_from->follow_head = current->next;
            }
            else{
                prev->next = current->next;
            }
            free(current);
            printf("Success: %s unfollowed %s.\n",from,to);
			return; 
		}
		prev = current;
		current = current->next;
	}
    
    printf("Failed: %s isn`t following %s.\n",from,to);
}


void search(char *name){ //: ユーザー情報とフォロー相手の表示
    struct member_list *user = find_user(name);
    if(user == NULL){
		printf("failed\n");		
		return;
	}
    printf("[User] %s (%d)\n", user->name, user->age);
    printf(" Following:");
    struct follow_node *current = user->follow_head;
    while (current != NULL){
        printf(" %s", current->user->name);
        current = current->next; 
    }
    printf("\n"); 
}

void is_mutual(char *name1, char *name2){ //: 相互フォロー判定（結果は標準出力で良い）
    struct member_list *user1 = find_user(name1);
	struct member_list *user2 = find_user(name2);
	if (user1 == NULL || user2 == NULL) {
        printf("failed\n");	
		return; 
	}
    if(is_following(user1,user2)&&is_following(user2,user1)){
        printf("Yes, %s and %s are mutual friends!\n", name1, name2);
        return;
    }
    printf("No, %s and %s are not mutual friends.\n", name1, name2);
}

void recommend(char *name){ //: 友人の友人を推薦する（相互フォローの場合は自分を表示しないようにする）
    struct member_list *user = find_user(name);
    if(user == NULL){
        printf("failed\n");	
        return;
    }
    printf("Recommendations for %s:\n", name);
    struct member_list *recommended_list[100]; 
    struct member_list *recommenders_list[100][20]; 
    int recommender_count[100]={0};
    int count = 0;
    struct follow_node *friend1 = user->follow_head;
    while(friend1 != NULL) {
        struct follow_node *friend2 = friend1->user->follow_head;
        while(friend2 != NULL){
            struct member_list *recommender = friend2->user; 
            if(recommender != user){
                bool already_followed = false;
                struct follow_node *following = user->follow_head;
                while(following != NULL){
                    if(following->user == recommender){
                        already_followed = true;
                        break;
                    }
                    following = following->next;
                }
                if(already_followed == false) {
                    bool already_recommended = false;
                    int found_index = -1; 
                    for(int i = 0; i < count; i++){ 
                        if(recommended_list[i] == recommender){
                            already_recommended = true;
                            found_index = i;
                            break;
                        }
                    }
                    if(already_recommended == false){
                        recommended_list[count] = recommender;
                        recommenders_list[count][0] = friend1->user;
                        recommender_count[count] = 1;
                        count++;
                    } else {
                        int current_num = recommender_count[found_index];
                        recommenders_list[found_index][current_num] = friend1->user;
                        recommender_count[found_index]++;
                    }
                }
            }
            friend2 = friend2->next;
        }
        friend1 = friend1->next;
    }
    for(int i=0; i<count; i++){
        printf("- %s (because ", recommended_list[i]->name);
        for(int j=0; j<recommender_count[i]; j++){
            printf("%s",recommenders_list[i][j]->name);
            if(j<recommender_count[i]-1){
                printf(", ");
            }
        }
        printf(" follows them)\n");
    }
}
// ファイル読み込み処理
void load_data(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    char line[256];
    int mode = 0; // 0: 初期, 1: USERS, 2: FOLLOWS

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '#' || line[0] == '\0') {
		if (strstr(line, "# USERS")) mode = 1;
            else if (strstr(line, "# FOLLOWS")) mode = 2;
            continue;
        }

        if (mode == 1) { // ユーザー追加
            char name[64];
            int age;
            if (sscanf(line, "%s %d", name, &age) == 2) {
                add(name, age);
            }
        } else if (mode == 2) { // フォロー追加
            char from[64], to[64];
            if (sscanf(line, "%s %s", from, to) == 2) {
                follow(from, to);
            }
        }
    }
    fclose(fp);
    printf("Data loaded successfully.\n");
}




int main (){
    load_data("sns_data.txt");
    printf(
        "--- User Management System ---\n"
        "Commands:\n"
        "search [name] : Show user info and following list\n"
        "add [name] [age] : Register a new user\n"
        "follow [from] [to] : Create a follow relationship\n"
        "unfollow [from] [to] : Remove a follow relationship\n"
        "is_mutual [name1] [name2] : Check mutual follow status\n"
        "recommend [name] : Show 'friend of friends'\n"
        "exit : Quit program\n"
        "-----------------------------\n"
    );
    char line[256];
    char command[64];
    char name[64], name1[64], name2[64], from[64], to[64];
    int age;
    while (1) {
        printf("command > ");
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%s", command) != 1) {
            continue;
        }
        if (strcmp(command, "exit") == 0) {
            free_all();
            printf("Goodbye! \n");
            break; 
        } 
        else if (strcmp(command, "search") == 0){
            if (sscanf(line, "%*s %s", name) == 1) {
                search(name);
            } else {
                printf("Error: Missing arguments. Usage: search [name]\n");
            }
        }
        else if (strcmp(command, "add") == 0){
            if (sscanf(line, "%*s %s %d", name, &age) == 2) {
                add(name, age);
            } else {
                printf("Error: Missing arguments. Usage: add [name] [age]\n");
            }
        }
        else if (strcmp(command, "follow") == 0){
            if (sscanf(line, "%*s %s %s", from, to) == 2) {
                if (strcmp(from, to) == 0) {
                    printf("Error: Cannot follow yourself.\n");
                } else {
                    follow(from, to);                
                }
            } else {
                printf("Error: Missing arguments. Usage: follow [from] [to]\n");
            }
        }
        else if (strcmp(command, "unfollow") == 0){
            if (sscanf(line, "%*s %s %s", from, to) == 2) {
                unfollow(from, to);
            } else {
                printf("Error: Missing arguments. Usage: unfollow [from] [to]\n");
            }
        }
        else if (strcmp(command, "is_mutual") == 0){
            if (sscanf(line, "%*s %s %s", name1, name2) == 2) {
                is_mutual(name1, name2);
            } else {
                printf("Error: Missing arguments. Usage: is_mutual [name1] [name2]\n");
            }
        }
        else if (strcmp(command, "recommend") == 0){
            if (sscanf(line, "%*s %s", name) == 1) {
                recommend(name);
            } else {
                printf("Error: Missing arguments. Usage: recommend [name]\n");
            }
        }
        else {
            printf("Error: Unknown command. Please try again.\n");
        }
    }

    return 0;
}
