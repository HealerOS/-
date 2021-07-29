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

int user_name=0;//选择的用户
int rand_num;//生成单词数组的随机数下标
int mode_choice;//模式选择的选项
int score = 0;//限时模式的得分
int pass = 0;//这个用来判断是否还要继续计时
int timerest;//限时模式的剩余时间
pthread_mutex_t lock;//线程锁


game_data gameData;//游戏数据
user user1;//选择的用户
user* userp1=&user1;//选择的用户的指针
FILE *fuser= NULL;//用于读取用户数据的文件指针


void init_Game(FILE* p);//初始化游戏，将数据加载到内存
void create_Word();//随机生成单词
void* time_Reckon(void* args);//计时功能
void update();//更新时间和用户输入
void* inputc(void*);//用户输入


void gotoXY(short x, short y);//设置光标位置
game_data Dificulty_level(int choice);//选择游戏难度，生成游戏数据文件
int Mode_select();//游戏模式选择
void setColor(int color);//设置颜色
boolean Normal_game(game_data gameData);//普通模式游戏函数
int Next_level(boolean isPass);//进入下一关或重新开始本关
user Select_user();//选择用户
void exit();//退出游戏保存数据
void Show_list();//显示排行榜
int show();//显示主菜单
boolean time_Limit_Game(game_data gameData);//限时模式游戏函数

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
            //这里要对读取后的换行符进行处理！
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
    //显示正确率
    gotoXY(50, 8);
    printf("快 乐 打 字 机\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 12);
    printf("您的得分是：%d，正确率为：%d%%,时间为%d秒", score, score,end-start);
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
    printf("快 乐 打 字 机\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30,15);
    printf("*******************************************************\n");
    gotoXY(51, 10);
    printf("  剩余时间：%d s", timerest);
    gotoXY(29, 11);
    printf("  单词：%s",pTest.wordname);
    printf("\n\n                         ");  //清空这一行
    gotoXY(29, 13);
    printf("  英文：%s",pUser.wordname);  //重新显示


}
void* inputc(void* p)
{
    int i = 0;
    char ch='0';  //单个输入检查
    while (ch!='\r')  //执行输入
    {
        ch = _getch();  //读取字符


        if (ch == '\b')  //检测是否为退格
        {
            if (i > 0)
            {
                pUser.wordname[--i] = 0;  //是的话将一个字符删掉并向前移动一位
                putchar('\b');
                putchar(' ');
                putchar('\b');  //这三句实现删掉已经输出的字符
            }
        }
        else
        {
            pUser.wordname[i] = ch;  //放进数组
            putchar(ch);
            i++;  //不是则向下移动并输出
        }

        if (ch == '\r')  //检查是否输入完毕
        {
            pUser.wordname[i-1] = '\0';
            if (strcmp(pUser.wordname,pTest.wordname)==0)  //判断输入是否正确
            {
                system("cls");
                // printf("bingo!");
                memset(&pUser, '\0', sizeof(pUser));
                score++;
                pass = 1;
                gotoXY(50, 8);
                printf("快 乐 打 字 机\n");
                gotoXY(30, 10);
                printf("*******************************************************\n");
                gotoXY(30,15);
                printf("*******************************************************\n");
                gotoXY(29, 11);
                printf("  单词：%s",pTest.wordname);
                printf("\n\n                         ");  //清空这一行
                gotoXY(29, 13);
                printf("  英文：%s",pUser.wordname);  //重新显示
                gotoXY(29, 14);
                setColor(2);
                printf("  正确\n");
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
                printf("快 乐 打 字 机\n");
                gotoXY(30, 10);
                printf("*******************************************************\n");
                gotoXY(30,15);
                printf("*******************************************************\n");
                gotoXY(29, 11);
                printf("  单词：%s",pTest.wordname);
                printf("\n\n                         ");  //清空这一行
                gotoXY(29, 13);
                printf("  英文：%s",pUser.wordname);  //重新显示
                gotoXY(29, 14);
                setColor(4);
                printf("  错误\n");
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

    //定义文件指针变量
    FILE* fp;
    //记录单词数量
    int counter = 0;
    //定义字符数组
    char buff[100];
    //定义字符指针记录读文件返回值
    char* status = NULL;
    //定义难度选择参数ch
    int ch = 0;

        gotoXY(50, 8);
        printf("难 度 选 择\n");
        gotoXY(30, 10);
        printf("*******************************************************\n");
        gotoXY(50, 12);
        printf("1.简单难度");
        gotoXY(50, 14);
        printf("2.普通难度");
        gotoXY(50, 16);
        printf("3.困难难度");
        gotoXY(30, 18);
        printf("*******************************************************\n");
        gotoXY(45, 20);
        printf("请输入您的选项（1-3）:");
        scanf("%d",&ch);
        while(!(ch>0&&ch<4)){
        gotoXY(45, 21);
        printf("输入错误，请重新输入");
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

    //获取文件中单词的个数
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
    //定义模式选择参数choice
    int choice = 0;
    //游戏模式选择界面

    gotoXY(50, 8);
    printf("游戏模式选择\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 13);
    printf("1.普通模式");
    gotoXY(50, 15);
    printf("2.限时模式");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(45, 20);
    printf("请输入您的选项（1-2）:");
    scanf("%d",&choice);
    while(!(choice>0&&choice<3)){
        gotoXY(45, 21);
        printf("输入错误，请重新输入");
        scanf("%d",&choice);
    }
    system("cls");
    switch (choice) {
        case '1': {
            //进入普通模式难度选择
            choice = 1;
            break;
        }
        case '2': {
            //进入限时模式难度选择
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

    int count = 0;//文件单词或句子总数
    count=gameData.count;
    int rand_count=0;//随机选择的数组下标
    char answer[100];//用户输入的答案存储入这个字符串
    int length=0;//出现单词或句子的具体长度
    srand(time(0));
    double right=0;//用户输入正确的个数
    int score=0,time_score=50; //默认时间得分满分为50
    time_t start_time,end_time;//游戏开始时间和结束时间
    int used_time=0;//游戏使用时间
    int this_count=user1.level[gameData.difficulty]*2; //本关显示的总单词数
    if(gameData.difficulty==0){
       char chars[26];
       for(int i=0;i<26;i++) {
           chars[i]=i+97;
       }
        start_time= time(0);
        for (int i = 0; i < this_count; ++i) {
            rand_count = rand() % 26;
            gotoXY(50, 8);
            printf("普通模式 关卡：%d",user1.level[0]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(50, 12);
            printf("%c",chars[rand_count]);
            gotoXY(50, 14);
            printf("请输入:   ");

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
                printf("正确\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("错误\n");
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
            printf("普通模式 关卡：%d",user1.level[1]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(50, 12);
            printf("%s", words[rand_count].wordname);
            gotoXY(50, 14);
            printf("请输入:   ");
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
                printf("正确\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("错误\n");
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
            printf("普通模式 关卡：%d",user1.level[2]);
            gotoXY(30, 10);
            printf("*******************************************************\n");
            gotoXY(30, 18);
            printf("*******************************************************\n");
            gotoXY(30, 12);
            printf("%s", sentences[rand_count].sentencename);
            gotoXY(50, 14);
            printf("请输入:   ");
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
                printf("正确\n");
                setColor(7);
                right++;

            } else {
                setColor(4);
                printf("错误\n");
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

    score=right_rate/2+time_score; //用户分数计算
    gotoXY(50, 8);
    printf("通 关 数 据");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("正确率为%.0f%%",right_rate);
    gotoXY(50, 14);
    printf("耗时为%d秒",used_time);
    gotoXY(50, 16);
    printf("分数为%d",score);


    if(score>70){

        user1.right_rate[user1.level[gameData.difficulty]-1][gameData.difficulty]=right_rate;//存储用户的正确率
        user1.time[user1.level[gameData.difficulty]-1][gameData.difficulty]=used_time; //存储用户的耗时
        user1.scores[user1.level[gameData.difficulty]-1][gameData.difficulty]=score;//存储用户的分数
        user1.level[gameData.difficulty]++;//用户相应模式下关卡数加一
        return true;
    }
    else{
        return false;
    }
}

int Next_level(boolean isPass){
    int choice;
    gotoXY(50, 8);
    printf("普通模式 关卡：%d",user1.level[gameData.difficulty]);
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(30, 20);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    if(isPass){
        printf("恭 喜 您 通 过 本 关");
        gotoXY(50, 14);
        printf("请选择  1.进入下一关");
        gotoXY(50, 16);
        printf("        2.退出\n");
        gotoXY(50, 18);
        printf("请输入:");
        scanf("%d",&choice);
        while(choice!=1&&choice!=2){
            choice=0;
            gotoXY(50, 19);
            printf("输入错误，请重新输入");
            scanf("%d",&choice);
        }
        return choice;
    }
    else{
        printf("再接再励");
        gotoXY(50, 14);
        printf("请选择  1.重新开始本关");
        gotoXY(50, 16);
        printf("        2.退出\n");
        gotoXY(50, 18);
        printf("请输入:");
        scanf("%d",&choice);
        while(choice!=1&&choice!=2){
            choice=0;
            gotoXY(50, 19);
            printf("输入错误，请重新输入");
            scanf("%d",&choice);
        }
        return choice;
    }
}
user Select_user(){
    int choice;
    gotoXY(50, 8);
    printf("快 乐 打 字 机\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("请 选 择 用 户");
    gotoXY(50, 14);
    printf("1.用户A");
    gotoXY(50, 16);
    printf("2.用户B");
    gotoXY(50, 18);
    printf("3.用户C");
    gotoXY(30, 20);
    printf("*******************************************************\n");
    gotoXY(45, 22);
    printf("请输入您的选项（1-3）:");


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
                printf("输入错误,请重新输入");
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
            puts("游戏文件存储错误");
            Sleep(3000);
            break;
    }
    user* userp1=&user1;
    if (fuser==NULL){
        puts("文件创建失败");

    }
    else{
        fwrite(userp1,sizeof(user),1,fuser);
        puts("数据存储成功");
    }
    fclose(fuser);
}
void Show_list() {

    //清屏
    system("cls");
    //定义用户输入的模式选择参数
    int ch;
    //定义用户选择的难度
    game_data data;
    //用户选择查看的模式和难度
    ch = Mode_select();
    data= Dificulty_level(ch);
    //清屏
    system("cls");
    //排行榜基本界面

    gotoXY(50, 2);
    printf("快乐打字机天梯榜");
    gotoXY(15, 4);
    printf("------------------------------------------------------------------------------------");
    gotoXY(15, 5);
    printf("玩家****************");
    gotoXY(35, 5);
    printf("得分****************");
    gotoXY(55, 5);
    printf("正确率****************");
    gotoXY(75, 5);
    printf("时间");

    //定义文件指针
    FILE* fp;
    //定义结构体指针
    struct user* p;
    //实例化user来接受读取的文件
    struct user user1,user2,user3;
    //定义指针p的长度大小
    p = (struct user*)malloc(3 * sizeof(struct user));
    //读取玩家A的数据
    fp = fopen(DATA_USER_A, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user1, sizeof(user1), 1, fp);
    fclose(fp);
    p[0] = user1;
    //读取玩家B的数据
    fp = fopen(DATA_USER_B, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user2, sizeof(user2), 1, fp);
    fclose(fp);
    p[1] = user2;
    //读取玩家C的数据
    fp = fopen(DATA_USER_C, "rb");
    if (fp == NULL) {
        printf("open file fail\n");
    }
    fread(&user3, sizeof(user3), 1, fp);
    fclose(fp);
    p[2] = user3;

    //定义用户的平均分
    int average_score = 0;
    //定义用户平均正确率
    double average_right_rate = 0;
    //定义用户平均用时
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
        printf("玩家%c",'A'+i);
        gotoXY(35, 6+i);
        printf("%d", average_score);
        gotoXY(55, 6+i);
        printf("%.0lf%%", average_right_rate);
        gotoXY(75, 6+i);
        printf("%.1lf", average_time);

        //数据清零
        average_score = 0;
        average_time = 0;
        average_right_rate = 0;
    }
    //显示返回功能
    gotoXY(100, 30);
    printf("按任意键退出排行榜");
    getch();
    system("cls");
    return;

}
int show() {
    gotoXY(50, 8);
    printf("快 乐 打 字 机\n");
    gotoXY(30, 10);
    printf("*******************************************************\n");
    gotoXY(50, 12);
    printf("1.开始游戏");
    gotoXY(50, 14);
    printf("2.排行榜");
    gotoXY(50, 16);
    printf("3.退出游戏");
    gotoXY(30, 18);
    printf("*******************************************************\n");
    gotoXY(45, 20);
    printf("请输入您的选项（1-3）:");
    //定义用户输入的选择参数
    int choice = 0;
    //从键盘获取用户输入的参数
    scanf("%d",&choice);
    system("cls");
    switch (choice)
    {
        case 1:
        {
            //生成数据文件

            mode_choice=Mode_select();
            gameData=Dificulty_level(mode_choice);
            return 1;

        }
        case 2:
        {
            //查看排行榜功能
            //system("cls");
            Show_list();
            return 2;

        }

        case 3:
        {
            //退出游戏
            exit();
            return 3;

        }
        default:
            //重新输入
            gotoXY(45, 21);
            printf("输入错误，请重新输入");
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


