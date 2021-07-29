#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<Windows.h>
#include <pthread.h>
#include <conio.h>

#define DATA_USER_A  "../user_A.dat"
#define DATA_USER_B  "../user_B.dat"
#define DATA_USER_C  "../user_C.dat"
#define WORD_FILE       "../words.txt"
#define SENTENCE_FILE  "../sentences.txt"

struct word
{
    char wordname[100];
} *pMemory,pUser,pTest;
struct sentence{
    char sentencename[100];
};
struct user{
    int level[6]={1,1,1,1,1,1};
    int time[10][6];
    int scores[10][6];
    double right_rate[10][6];
};
struct game_data{
    FILE *words;
    int difficulty;
    int count;
};

int user_name=0;//ѡ����û�
int rand_num;//���ɵ��������������±�
int mode_choice;//ģʽѡ���ѡ��
int score = 0;//��ʱģʽ�ĵ÷�
int pass = 0;//��������ж��Ƿ�Ҫ������ʱ
int timerest;//��ʱģʽ��ʣ��ʱ��
pthread_mutex_t lock;//�߳���


game_data gameData;//��Ϸ����
user user1;//ѡ����û�
user* userp1=&user1;//ѡ����û���ָ��
FILE *fuser= NULL;//���ڶ�ȡ�û����ݵ��ļ�ָ��


void init_Game(FILE* p);//��ʼ����Ϸ�������ݼ��ص��ڴ�
void create_Word();//������ɵ���
void* time_Reckon(void* args);//��ʱ����
void update();//����ʱ����û�����
void* inputc(void*);//�û�����


void gotoXY(short x, short y);//���ù��λ��
game_data Dificulty_level(int choice);//ѡ����Ϸ�Ѷȣ�������Ϸ�����ļ�
int Mode_select();//��Ϸģʽѡ��
void setColor(int color);//������ɫ
boolean Normal_game(game_data gameData);//��ͨģʽ��Ϸ����
int Next_level(boolean isPass);//������һ�ػ����¿�ʼ����
user Select_user();//ѡ���û�
void exit();//�˳���Ϸ��������
void Show_list();//��ʾ���а�
int show();//��ʾ���˵�
boolean time_Limit_Game(game_data gameData);//��ʱģʽ��Ϸ����

void init_Game(FILE* p) {
    if (p == NULL) {
        pMemory = (struct word*)malloc(gameData.count * sizeof(struct word));
        for (int  i = 0; i < gameData.count; i++)
        {
            pMemory[i].wordname[0] = 'a' + i;
            pMemory[i].wordname[1] ='\0';
        }
    }
    else {
        FILE* fp2;
        fp2 = fopen((const char*)p, "r");
        int count = 0;
        char chars[100];
        char* status = NULL;
        status = fgets(chars, sizeof(char[100]), fp2);
        while (status) {
            count++;
            status = fgets(chars, sizeof(char[100]), fp2);
        }
        fseek(fp2, 0, SEEK_SET);
        pMemory = (struct word*)malloc(count * sizeof(struct word));
        for (int i = 0; i < count; i++)
        {
            fgets(pMemory[i].wordname, sizeof(char[100]), fp2);
            //����Ҫ�Զ�ȡ��Ļ��з����д���
            strtok(pMemory[i].wordname, "\n");

        }
    }
}
boolean time_Limit_Game(game_data gameData) {
    init_Game(gameData.words);
    int level = 0;
    time_t start = time(NULL);
    while (level < user1.level[gameData.difficulty] * 5)
    {
        pass = 0;
        if(gameData.words!=NULL)
        create_Word();
        else{
                pTest.wordname[0]=rand()%26+97;
                pTest.wordname[1]='\0';

        }
        pthread_t t1;
        pthread_t t2;

        pthread_create(&t1, NULL, inputc, NULL);
        pthread_create(&t2, NULL, time_Reckon, (void*)&pTest);
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        level++;
    }
    time_t end = time(NULL);
    score = score * 1.0 / user1.level[gameData.difficulty]  *20;
    //��ʾ��ȷ��
    gotoXY(50, 8);
    printf("�� �� �� �� ��\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 12);
    printf("���ĵ÷��ǣ�%d����ȷ��Ϊ��%d%%,ʱ��Ϊ%d��", score, score,end-start);
    gotoXY(30, 15);
    printf("*******************************************************\n");
    if (score >= 70)
    {
        user1.right_rate[user1.level[gameData.difficulty]-1][gameData.difficulty] =score;
        user1.scores[user1.level[gameData.difficulty]-1][gameData.difficulty] = score;
        user1.time[user1.level[gameData.difficulty]-1][gameData.difficulty] =pt;
        user1.level[gameData.difficulty]++;
        score=0;
        return true;
    }
    else
    {
        score=0;
        return false;
    }
}
void create_Word() {

    srand(time(NULL));
    rand_num = rand() % gameData.count;
    strcpy(pTest.wordname, pMemory[rand_num].wordname);
}
void* time_Reckon(void* args) {
    int s=0;
    timerest = 15;
    while (timerest > 0 && pass == 0)
    {
        s = score;
        timerest--;
        update();
        Sleep(1000);
        //system("cls");
    }
    return NULL;
}
void update()
{
    gotoXY(50, 8);
    printf("�� �� �� �� ��\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30,15);
    printf("*******************************************************\n");
    gotoXY(51, 10);
    printf("  ʣ��ʱ�䣺%d s", timerest);
    gotoXY(29, 11);
    printf("  ���ʣ�%s",pTest.wordname);
    printf("\n\n                         ");  //�����һ��
    gotoXY(29, 13);
    printf("  Ӣ�ģ�%s",pUser.wordname);  //������ʾ


}
void* inputc(void* p)
{
    int i = 0;
    char ch='0';  //����������
    while (ch!='\r')  //ִ������
    {
        ch = _getch();  //��ȡ�ַ�


        if (ch == '\b')  //����Ƿ�Ϊ�˸�
        {
            if (i > 0)
            {
                pUser.wordname[--i] = 0;  //�ǵĻ���һ���ַ�ɾ������ǰ�ƶ�һλ
                putchar('\b');
                putchar(' ');
                putchar('\b');  //������ʵ��ɾ���Ѿ�������ַ�
            }
        }
        else
        {
            pUser.wordname[i] = ch;  //�Ž�����
            putchar(ch);
            i++;  //�����������ƶ������
        }

        if (ch == '\r')  //����Ƿ��������
        {
            pUser.wordname[i-1] = '\0';
            if (strcmp(pUser.wordname,pTest.wordname)==0)  //�ж������Ƿ���ȷ
            {
                system("cls");
                // printf("bingo!");
                memset(&pUser, '\0', sizeof(pUser));
                score++;
                pass = 1;
                gotoXY(50, 8);
                printf("�� �� �� �� ��\n");
                gotoXY(30, 10);
                printf("*******************************************************\n");
                gotoXY(30,15);
                printf("*******************************************************\n");
                gotoXY(29, 11);
                printf("  ���ʣ�%s",pTest.wordname);
                printf("\n\n                         ");  //�����һ��
                gotoXY(29, 13);
                printf("  Ӣ�ģ�%s",pUser.wordname);  //������ʾ
                gotoXY(29, 14);
                setColor(2);
                printf("  ��ȷ\n");
                setColor(7);
                Sleep(500);
                system("cls");


            }
            else {
                system("cls");
                // printf("error!");
                memset(&pUser, '\0', sizeof(pUser));
                pass = 1;
                gotoXY(50, 8);
                printf("�� �� �� �� ��\n");
                gotoXY(30, 10);
                printf("*******************************************************\n");
                gotoXY(30,15);
                printf("*******************************************************\n");
                gotoXY(29, 11);
                printf("  ���ʣ�%s",pTest.wordname);
                printf("\n\n                         ");  //�����һ��
                gotoXY(29, 13);
                printf("  Ӣ�ģ�%s",pUser.wordname);  //������ʾ
                gotoXY(29, 14);
                setColor(4);
                printf("  ����\n");
                setColor(7);
                Sleep(500);
                system("cls");
            }
            return NULL;
        }
    }
}
void gotoXY(short x, short y) {
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x,y };
    SetConsoleCursorPosition(hOut, pos);
    pthread_mutex_unlock(&lock);
}

game_data Dificulty_level(int choice) {

    //�����ļ�ָ�����
    FILE* fp;
    //��¼��������
    int counter = 0;
    //�����ַ�����
    char buff[100];
    //�����ַ�ָ���¼���ļ�����ֵ
    char* status = NULL;
    //�����Ѷ�ѡ�����ch
    int ch = 0;

        gotoXY(50, 8);
        printf("�� �� ѡ ��\n");
        gotoXY(30, 10);
        printf("*******************************************************\n");
        gotoXY(50, 12);
        printf("1.���Ѷ�");
        gotoXY(50, 14);
        printf("2.��ͨ�Ѷ�");
        gotoXY(50, 16);
        printf("3.�����Ѷ�");
        gotoXY(30, 18);
        printf("*******************************************************\n");
        gotoXY(45, 20);
        printf("����������ѡ�1-3��:");
        scanf("%d",&ch);
        while(!(ch>0&&ch<4)){
        gotoXY(45, 21);
        printf("�����������������");
        scanf("%d",&ch);
        }

    if(choice==1){
        gameData.difficulty = ch-1;
    }
    else if(choice==2) {
        gameData.difficulty = ch+2;
    }

    if(gameData.difficulty==0||gameData.difficulty==3){
        gameData.words=NULL;
        system("cls");
        return gameData;

    }
    if(gameData.difficulty==1||gameData.difficulty==4){
        gameData.words= (FILE *) WORD_FILE;
        fp= fopen( WORD_FILE,"r");
    }
    if(gameData.difficulty==2||gameData.difficulty==5){
        gameData.words= (FILE *) SENTENCE_FILE;
        fp= fopen(SENTENCE_FILE,"r");
    }

    //��ȡ�ļ��е��ʵĸ���
    if (fp == NULL) {
        printf("open file fail\n");
    }
    status = fgets(buff, sizeof(buff), fp);
    while (status) {
        counter++;
        status = fgets(buff, sizeof(buff), fp);
    }
    gameData.count = counter;
    system("cls");
    fclose(fp);
    return gameData;
}
int Mode_select() {
    //����ģʽѡ�����choice
    int choice = 0;
    //��Ϸģʽѡ�����

    gotoXY(50, 8);
    printf("��Ϸģʽѡ��\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 13);
    printf("1.��ͨģʽ");
    gotoXY(50, 15);
    printf("2.��ʱģʽ");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(45, 20);
    printf("����������ѡ�1-2��:");
    scanf("%d",&choice);
    while(!(choice>0&&choice<3)){
        gotoXY(45, 21);
        printf("�����������������");
        scanf("%d",&choice);
    }
    system("cls");
    switch (choice) {
        case '1': {
            //������ͨģʽ�Ѷ�ѡ��
            choice = 1;
            break;
        }
        case '2': {
            //������ʱģʽ�Ѷ�ѡ��
            choice = 2;
            break;
        }
    }
    return choice;
}
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
boolean Normal_game(game_data gameData){

    int count = 0;//�ļ����ʻ��������
    count=gameData.count;
    int rand_count=0;//���ѡ��������±�
    char answer[100];//�û�����Ĵ𰸴洢������ַ���
    int length=0;//���ֵ��ʻ���ӵľ��峤��
    srand(time(0));
    double right=0;//�û�������ȷ�ĸ���
    int score=0,time_score=50; //Ĭ��ʱ��÷�����Ϊ50
    time_t start_time,end_time;//��Ϸ��ʼʱ��ͽ���ʱ��
    int used_time=0;//��Ϸʹ��ʱ��
    int this_count=user1.level[gameData.difficulty]*2; //������ʾ���ܵ�����
    if(gameData.difficulty==0){
       char chars[26];
       for(int i=0;i<26;i++) {
           chars[i]=i+97;
       }
        start_time= time(0);
        for (int i = 0; i < this_count; ++i) {
            rand_count = rand() % 26;
            gotoXY(50, 8);
            printf("��ͨģʽ �ؿ���%d",user1.level[0]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(50, 12);
            printf("%c",chars[rand_count]);
            gotoXY(50, 14);
            printf("������:   ");

            char ans=0,tmp;
            while(ans==0) {
                tmp = getche();
                if (tmp != '\b') {
                    ans = tmp;
                }
            }

            gotoXY(50, 16);
            if (ans==chars[rand_count]) {
                setColor(2);
                printf("��ȷ\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("����\n");
                setColor(7);
            }
            end_time=time(0);
            used_time=end_time-start_time;
            if(used_time>this_count){
                time_score-=used_time-this_count;
            }
            Sleep(500);
            system("cls");
        }
    }
    if(gameData.difficulty==1){
        word *words;
        words = (word *)malloc(count * sizeof(word));
        fuser = fopen(WORD_FILE,"r");
        for (int i = 0; i < count; i++)
        {
            fgets(words[i].wordname, 100, fuser);
                char *find = strchr(words[i].wordname, '\n');
            if (find)
                *find = '\0';
        }
        fclose(fuser);
        start_time= time(0);
        for (int i = 0; i < this_count; ++i) {
            rand_count = rand() %count;
            gotoXY(50, 8);
            printf("��ͨģʽ �ؿ���%d",user1.level[1]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(50, 12);
            printf("%s", words[rand_count].wordname);
            gotoXY(50, 14);
            printf("������:   ");
            length = strlen(words[rand_count].wordname);
            char tmp;
            for (int i = 0; i < length; i++) {
                tmp = getche();
                if (tmp == '\b') {

                    printf(" \b");
                    answer[i] = '\0';
                    i--;
                    i--;
                    continue;
                }

                else{
                    answer[i]=tmp;
                }
            }

            gotoXY(50, 16);
            if (!strncmp(answer, words[rand_count].wordname, length)) {
                setColor(2);
                printf("��ȷ\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("����\n");
                setColor(7);
            }
            end_time=time(0);
            used_time=end_time-start_time;
            if(used_time>this_count*2){
                time_score-=used_time-this_count*2;
            }

            Sleep(500);
            system("cls");

        }

    }

    if(gameData.difficulty==2){
        sentence *sentences;
        sentences = (sentence *)malloc(count * sizeof(sentence));
        fuser = fopen(SENTENCE_FILE,"r");
        for (int i = 0; i < count; i++)
        {
            fgets(sentences[i].sentencename, 100, fuser);
            char *find = strchr(sentences[i].sentencename, '\n');
            if (find)
                *find = '\0';
        }
        fclose(fuser);
        start_time= time(0);
        for (int i = 0; i < this_count; ++i) {
            rand_count = rand() % count;
            gotoXY(50, 8);
            printf("��ͨģʽ �ؿ���%d",user1.level[2]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(30, 12);
            printf("%s", sentences[rand_count].sentencename);
            gotoXY(50, 14);
            printf("������:   ");
            length = strlen(sentences[rand_count].sentencename);

            char tmp;
            for (int i = 0; i < length; i++) {
                tmp = getche();
                if (tmp == '\b') {

                    printf(" \b");
                    answer[i] = '\0';
                    i--;
                    i--;
                    continue;
                }

                else{
                    answer[i]=tmp;
                }
            }
            gotoXY(50, 16);
            if (!strncmp(answer,sentences[rand_count].sentencename, length)) {
                setColor(2);
                printf("��ȷ\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("����\n");
                setColor(7);
            }
            end_time=time(0);
            used_time=end_time-start_time;
            if(used_time>this_count*10){
                time_score-=used_time-this_count*10;
            }

            Sleep(500);
            system("cls");
        }
    }
    double right_rate;
    right_rate=right/this_count*100;

    score=right_rate/2+time_score; //�û���������
    gotoXY(50, 8);
    printf("ͨ �� �� ��");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("��ȷ��Ϊ%.0f%%",right_rate);
    gotoXY(50, 14);
    printf("��ʱΪ%d��",used_time);
    gotoXY(50, 16);
    printf("����Ϊ%d",score);


    if(score>70){

        user1.right_rate[user1.level[gameData.difficulty]-1][gameData.difficulty]=right_rate;//�洢�û�����ȷ��
        user1.time[user1.level[gameData.difficulty]-1][gameData.difficulty]=used_time; //�洢�û��ĺ�ʱ
        user1.scores[user1.level[gameData.difficulty]-1][gameData.difficulty]=score;//�洢�û��ķ���
        user1.level[gameData.difficulty]++;//�û���Ӧģʽ�¹ؿ�����һ
        return true;
    }
    else{
        return false;
    }
}

int Next_level(boolean isPass){
    int choice;
    gotoXY(50, 8);
    printf("��ͨģʽ �ؿ���%d",user1.level[gameData.difficulty]);
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 20);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    if(isPass){
        printf("�� ϲ �� ͨ �� �� ��");
        gotoXY(50, 14);
        printf("��ѡ��  1.������һ��");
        gotoXY(50, 16);
        printf("        2.�˳�\n");
        gotoXY(50, 18);
        printf("������:");
        scanf("%d",&choice);
        while(choice!=1&&choice!=2){
            choice=0;
            gotoXY(50, 19);
            printf("�����������������");
            scanf("%d",&choice);
        }
        return choice;
    }
    else{
        printf("�ٽ�����");
        gotoXY(50, 14);
        printf("��ѡ��  1.���¿�ʼ����");
        gotoXY(50, 16);
        printf("        2.�˳�\n");
        gotoXY(50, 18);
        printf("������:");
        scanf("%d",&choice);
        while(choice!=1&&choice!=2){
            choice=0;
            gotoXY(50, 19);
            printf("�����������������");
            scanf("%d",&choice);
        }
        return choice;
    }
}
user Select_user(){
    int choice;
    gotoXY(50, 8);
    printf("�� �� �� �� ��\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("�� ѡ �� �� ��");
    gotoXY(50, 14);
    printf("1.�û�A");
    gotoXY(50, 16);
    printf("2.�û�B");
    gotoXY(50, 18);
    printf("3.�û�C");
    gotoXY(30, 20);
    printf("*******************************************************\n");
    gotoXY(45, 22);
    printf("����������ѡ�1-3��:");


    while(true) {
        scanf("%d",&choice);
        switch (choice) {
            case 1: {
                fuser = fopen(DATA_USER_A, "rb");
                if (fuser != NULL) {
                    fread(userp1, sizeof(user), 1, fuser);
                }
                user_name=1;
                fclose(fuser);
                system("cls");
                return user1;
            }
            case 2: {
                fuser = fopen(DATA_USER_B, "rb");
                if (fuser != NULL) {
                    fread(userp1, sizeof(user), 1, fuser);
                }
                user_name=2;
                fclose(fuser);
                system("cls");
                return user1;
            }
            case 3: {
                fuser = fopen(DATA_USER_C, "rb");
                if (fuser != NULL) {
                    fread(userp1, sizeof(user), 1, fuser);
                }
                user_name=3;
                fclose(fuser);
                system("cls");
                return user1;
            }
            default:
                gotoXY(45, 24);
                printf("�������,����������");
                fclose(fuser);
                break;

        }
    }

}
void exit(){
    switch (user_name) {
        case 1:fuser= fopen(DATA_USER_A,"wb");
            break;
        case 2:fuser= fopen(DATA_USER_B,"wb");
            break;
        case 3:fuser= fopen(DATA_USER_C,"wb");
            break;
        default:
            puts("��Ϸ�ļ��洢����");
            Sleep(3000);
            break;
    }
    user* userp1=&user1;
    if (fuser==NULL){
        puts("�ļ�����ʧ��");

    }
    else{
        fwrite(userp1,sizeof(user),1,fuser);
        puts("���ݴ洢�ɹ�");
    }
    fclose(fuser);
}
void Show_list() {

    //����
    system("cls");
    //�����û������ģʽѡ�����
    int ch;
    //�����û�ѡ����Ѷ�
    game_data data;
    //�û�ѡ��鿴��ģʽ���Ѷ�
    ch = Mode_select();
    data= Dificulty_level(ch);
    //����
    system("cls");
    //���а��������

    gotoXY(50, 2);
    printf("���ִ��ֻ����ݰ�");
    gotoXY(15, 4);
    printf("------------------------------------------------------------------------------------");
    gotoXY(15, 5);
    printf("���****************");
    gotoXY(35, 5);
    printf("�÷�****************");
    gotoXY(55, 5);
    printf("��ȷ��****************");
    gotoXY(75, 5);
    printf("ʱ��");

    //�����ļ�ָ��
    FILE* fp;
    //����ṹ��ָ��
    struct user* p;
    //ʵ����user�����ܶ�ȡ���ļ�
    struct user user1,user2,user3;
    //����ָ��p�ĳ��ȴ�С
    p = (struct user*)malloc(3 * sizeof(struct user));
    //��ȡ���A������
    fp = fopen(DATA_USER_A, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user1, sizeof(user1), 1, fp);
    fclose(fp);
    p[0] = user1;
    //��ȡ���B������
    fp = fopen(DATA_USER_B, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user2, sizeof(user2), 1, fp);
    fclose(fp);
    p[1] = user2;
    //��ȡ���C������
    fp = fopen(DATA_USER_C, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user3, sizeof(user3), 1, fp);
    fclose(fp);
    p[2] = user3;

    //�����û���ƽ����
    int average_score = 0;
    //�����û�ƽ����ȷ��
    double average_right_rate = 0;
    //�����û�ƽ����ʱ
    double average_time = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < (int)p[i].level[data.difficulty]; j++) {
            average_score += p[i].scores[j][data.difficulty];
            average_right_rate += p[i].right_rate[j][data.difficulty];
            average_time += p[i].time[j][data.difficulty];
        }
        if(p[i].level[data.difficulty]==1){
            average_score =0;
            average_right_rate=0;
            average_time =0;
        }
        else{
            average_score /=  ((int)p[i].level[data.difficulty]-1);
            average_right_rate /=  ((int)p[i].level[data.difficulty]-1);
            average_time /=  ((int)p[i].level[data.difficulty]-1);
        }


        gotoXY(15, 6+i);
        printf("���%c",'A'+i);
        gotoXY(35, 6+i);
        printf("%d", average_score);
        gotoXY(55, 6+i);
        printf("%.0lf%%", average_right_rate);
        gotoXY(75, 6+i);
        printf("%.1lf", average_time);

        //��������
        average_score = 0;
        average_time = 0;
        average_right_rate = 0;
    }
    //��ʾ���ع���
    gotoXY(100, 30);
    printf("��������˳����а�");
    getch();
    system("cls");
    return;

}
int show() {
    gotoXY(50, 8);
    printf("�� �� �� �� ��\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("1.��ʼ��Ϸ");
    gotoXY(50, 14);
    printf("2.���а�");
    gotoXY(50, 16);
    printf("3.�˳���Ϸ");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(45, 20);
    printf("����������ѡ�1-3��:");
    //�����û������ѡ�����
    int choice = 0;
    //�Ӽ��̻�ȡ�û�����Ĳ���
    scanf("%d",&choice);
    system("cls");
    switch (choice)
    {
        case 1:
        {
            //���������ļ�

            mode_choice=Mode_select();
            gameData=Dificulty_level(mode_choice);
            return 1;

        }
        case 2:
        {
            //�鿴���а���
            //system("cls");
            Show_list();
            return 2;

        }

        case 3:
        {
            //�˳���Ϸ
            exit();
            return 3;

        }
        default:
            //��������
            gotoXY(45, 21);
            printf("�����������������");
            return 4;

    }
}


int main() {
   /*for(int i=1;i<4;i++){
       user_name=i;
       exit();
   }*/
    int isGame;
    int menu_choice;
    bool isPass;
    user1=Select_user();
    system("cls");
    while(menu_choice!=3){
        menu_choice=show();
        if (menu_choice==1&&mode_choice==1) {
            isGame=1;
            while (isGame == 1) {
                isPass = Normal_game(gameData);
                Sleep(1500);
                system("cls");
                isGame = Next_level(isPass);
                system("cls");
            }

        } else if(menu_choice==1&&mode_choice==2){
            isGame=1;
            while (isGame == 1) {
                isPass = time_Limit_Game(gameData);
                Sleep(1500);
                system("cls");
                isGame = Next_level(isPass);
                system("cls");
            }

        }
    }
    system("pause");
}


