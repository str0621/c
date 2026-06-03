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

void add(char *name, int age){ //: ユーザー追加
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
	
}

void follow(char *from, char *to){ //: 既存ユーザー間のフォロー関係構築
	struct member_list *user_from = find_user(from);
	struct member_list *user_to = find_user(to);
	if(user_from == NULL || user_to == NULL){
		printf("failed in\n%s %s\n", from, to);	
		return;
	}
    struct follow_node *following;
    following = (struct follow_node *)malloc(sizeof(struct follow_node));
    following->user = user_to;
    following->next = user_from->follow_head;
    user_from->follow_head = following;
    printf("Success: %s followed %s.",from,to);
}

void unfollow(char *from, char *to){ //: フォロー解除（ノードの削除とメモリ解放）
	struct member_list *user_from = find_user(from);
	struct member_list *user_to = find_user(to);
	if (user_from == NULL || user_to == NULL) {
        printf("failed in\n%s %s\n", from, to);	
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
            printf("Success: %s unfollowed %s.",from,to);
			return; 
		}
		prev = current;
		current = current->next;
	}
    
    printf("Failed: %s isn`t following %s.",from,to);
}


void search(char *name){ //: ユーザー情報とフォロー相手の表示
    struct member_list *user = find_user(name);
    if(user == NULL){
		printf("failed in\n%s\n", name);	
		return;
	}
    printf("[User] %s (%d)\n", user->name, user->age);
    printf(" Following:");
    struct follow_node *current = user->follow_head;
    while (current != NULL){
        printf(" %s", current->user->name);
        current = current->next; 
    }

    printf("\n"); // 最後に改行を出力して見た目を整える
}

void is_mutual(char *name1, char *name2){ //: 相互フォロー判定（結果は標準出力で良い）
    struct member_list *user1 = find_user(name1);
	struct member_list *user2 = find_user(name2);
	if (user1 == NULL || user2 == NULL) {
        printf("failed in\n%s %s\n", name1, name2);	
		return; 
	}
    struct follow_node *current = user1->follow_head;
    bool mutual = false;
    while (current != NULL){
        if(current->user==user2){
            mutual=true;
            break;
        }
        current = current->next;
    }
    if(mutual){
        current = user2->follow_head;
        while (current != NULL){
            if(current->user==user1){
                printf("Yes, %s and %s are mutual friends!\n", name1, name2);
                return;
            }
            current = current->next;
        }
    }
    printf("No, %s and %s are not mutual friends.\n", name1, name2);
}

void recommend(char *name){ //: 友人の友人を推薦する（相互フォローの場合は自分を表示しないようにする）
    struct member_list *user = find_user(name);
    if(user == NULL){
        printf("failed in\n%s\n", name);
        return;
    }
    printf("Recommendations for %s:\n", name);

    // 重複を防ぐための「すでにおすすめした人」を記録する一時的なメモ帳（配列）
    struct member_list *recommended_list[100]; 
    int rec_count = 0;
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
                bool already_recommended = false;
                for(int i = 0; i < rec_count; i++){
                    if(recommended_list[i] == recommender){
                        already_recommended = true;
                        break;
                    }
                }
                if(already_followed == false && already_recommended == false){
                    printf("- %s (because %s follows them)\n", friend2->user->name, friend1->user->name);
                    recommended_list[rec_count] = recommender;
                    rec_count++;
                    
                }
            }
            friend2 = friend2->next;
        }
        friend1 = friend1->next;
    }
    printf("\n");
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
        sscanf(line, "%s", command);
        if (strcmp(command, "exit") == 0) {
            printf("Goodbye! \n")
            break; 
        } 
        else if (strcmp(command, "search") == 0){
            sscanf(line, "%*s %s", name);
            search(name);
        }
        else if (strcmp(command, "add") == 0){
            sscanf(line, "%*s %s %d", name, &age);
            add(name,age);
        }
        else if (strcmp(command, "follow") == 0){
            sscanf(line, "%*s %s %s", from, to);
            follow(from,to);
        }
        else if (strcmp(command, "unfollow") == 0){
            sscanf(line, "%*s %s %s", from, to);
            unfollow(from,to);
        }
        else if (strcmp(command, "is_mutual") == 0){
            sscanf(line, "%*s %s %s", name1, name2);
            is_mutual(name1,name2);
        }
        else if (strcmp(command, "recommend") == 0){
            sscanf(line, "%*s %s", name);
            recommend(name);
        }
    }

    return 0;
}