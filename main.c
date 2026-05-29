#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add(char *name, int age){ //: ユーザー追加
	struct member_list *new_user;
	//メモリ確保
	new_user = (struct member_list *)malloc(sizeof(struct member_list)); 
	//挿入
	new_user->age=age;
	strncpy(new_user->name,name,size_of(new_user->name)-1);
	new_user->name[sizeof(new_user->name) - 1] = '\0';
	new_user->follow_head =NULL;
	//登録
	int user_hash=hash(new_user->name);
	new_user->next=hashtable[user_hash];//衝突回ひ
	hashtable[user_hash]=new_user;
	
}

void follow(char *from, char *to){ //: 既存ユーザー間のフォロー関係構築



}

void unfollow(char *from, char *to){ //: フォロー解除（ノードの削除とメモリ解放）



}


void search(char *name){ //: ユーザー情報とフォロー相手の表示



}

void is_mutual(char *name1, char *name2){ //: 相互フォロー判定（結果は標準出力で良い）



}

void recommend(char *name){ //: 友人の友人を推薦する（相互フォローの場合は自分を表示しないようにする）



}
