#include <utility>
#include <stdlib.h>
#include <time.h>
#include "GameModel.h"
#include <QDebug>
#include <iostream>
#include <queue>


using namespace std;


GameModel::GameModel()
{

}

void GameModel::startGame(GameType type)
{
    gameType = type;
    // 初始棋盘
    gameMapVec.clear();
    for (int i = 0; i < kBoardSizeNum; i++)
    {
        std::vector<int> lineBoard;
        for (int j = 0; j < kBoardSizeNum; j++)
            lineBoard.push_back(0);
        gameMapVec.push_back(lineBoard);
    }
    playerFlag = true;    // 己方下为true,对方下位false
    // 如果是AI模式，需要初始化评分数组，且AI先行
    if (gameType == BOT)
    {
        scoreMapVec.clear();
        for (int i = 0; i < kBoardSizeNum; i++)
        {
            std::vector<int> lineScores;
            for (int j = 0; j < kBoardSizeNum; j++)
                lineScores.push_back(0);
            scoreMapVec.push_back(lineScores);
        }
        playerFlag = false;
        actionByAI();
    }





}

void GameModel::updateGameMap(int row, int col)
{
    if (playerFlag)
        gameMapVec[row][col] = 1;
    else
        gameMapVec[row][col] = -1;

    latestrow=row;
    latestcol=col;
    // 换手
    playerFlag = !playerFlag;
}

bool GameModel::isWin(int row, int col)
{
    // 横竖斜四种大情况，每种情况都根据当前落子往后遍历5个棋子，有一种符合就算赢
    // 水平方向
    for (int i = 0; i < 5; i++)
    {
        // 往左5个，往右匹配4个子，20种情况
        if (col - i > 0 &&
            col - i + 4 < kBoardSizeNum &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 4])
            return true;
    }

    // 竖直方向(上下延伸4个)
    for (int i = 0; i < 5; i++)
    {
        if (row - i > 0 &&
            row - i + 4 < kBoardSizeNum &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 4][col])
            return true;
    }

    // 左斜方向
    for (int i = 0; i < 5; i++)
    {
        if (row + i < kBoardSizeNum &&
            row + i - 4 > 0 &&
            col - i > 0 &&
            col - i + 4 < kBoardSizeNum &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4])
            return true;
    }

    // 右斜方向
    for (int i = 0; i < 5; i++)
    {
        if (row - i > 0 &&
            row - i + 4 < kBoardSizeNum &&
            col - i > 0 &&
            col - i + 4 < kBoardSizeNum &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 4][col - i + 4])
            return true;
    }

    return false;
}

bool GameModel::isDeadGame()
{
    // 所有空格全部填满
    for (int i = 1; i < kBoardSizeNum; i++)
        for (int j = 1; j < kBoardSizeNum; j++)
        {
            if (!(gameMapVec[i][j] == 1 || gameMapVec[i][j] == -1))
                return false;
        }
    return true;
}


void GameModel::actionByPerson(int row, int col)
{
    updateGameMap(row, col);
}

int* GameModel::actionByAI()
{

    int* rowAndCol = choose2();          //选择对手
    updateGameMap(rowAndCol[0], rowAndCol[1]);
    return rowAndCol;
}


// 在本函数中完成你的算法。

// 你需要使用到this->gameMapVec变量，它存储了棋盘的状态。空白为0，白子1，黑子-1。务必不要直接对该变量做写操作，这样会破坏程序逻辑。
// this->gameMapVec是一个二维的Vector，其下标对应了你决策位置的行号和列号，如gameMapVec[2][3]存储了棋盘第二行第三列的状态。
// 边界不可下棋。因此gameMapVec的下标范围为[1,kBoardSizeNum-1]。

// 该函数需返回一个长度为2的数组，该数组包含了AI决定落子的行号和列号。如AI决定在第二行第五列落子，则需返回数组{2,5}。
// 以下示例代码展示了AI的一种落子方法，即依次选择行号和列号最小的，且为空白的位置落子。


//template<> less<Cross>{
//    bool operator()(const Cross &__x, const Cross &__y){
//        reuturn __x.myscore>__y.myscore;
//    }
//};

auto cmp = [](Cross x1, Cross x2) { return x2.score > x1.score; }; //Cross结构体的比较函数


//*******************************************************************************
//*******************************************************************************



//贪心算法
int* GameModel::choose1()
{
    int row, col;
    calculateScore(-1);    //黑子计分
    Cross crosses[kBoardSizeNum][kBoardSizeNum];   //计分板
    for(row=1;row<kBoardSizeNum;row++){  //将计分结果储存在计分板内
        for(col=1;col<kBoardSizeNum;col++){
            crosses[row][col].col=col;
            crosses[row][col].row=row;
            crosses[row][col].type=gameMapVec[row][col] ;
            crosses[row][col].score=scoreMapVec[row][col] ;
    }
    }


    int xrow=kBoardSizeNum/2, xcol=kBoardSizeNum/2, xscore=0;    //若无法比较出结果时，默认的位置
    while(scoreMapVec[xrow][xcol]!=0){
        xrow++; xcol++;
    }

    priority_queue<Cross,vector<Cross> ,decltype(cmp)> Myqueue(cmp);  //所有Cross结构体压入队列
    for (row = 1; row < kBoardSizeNum; row++){
        for (col = 1; col < kBoardSizeNum; col++){
            Myqueue.push(crosses[row][col]);
        }
    }

    //构造结果数组，返回给上层函数以绘制。
    int* result = new int[2];
    if(Myqueue.top().score>xscore)
    {
        result[0]=Myqueue.top().row;
        result[1]=Myqueue.top().col;
    }
    else{
        result[0]= xrow;
        result[1]= xcol;
    }


    return result;
}



//*******************************************************************************
//*******************************************************************************



//对抗算法，有更强大的同级别比较    3级对抗，+剪枝
int* GameModel::choose2()
{
    int row, col;
    calculateScore(-1);    //黑子计分
    Cross crosses[kBoardSizeNum][kBoardSizeNum];   //计分板
    for(row=1;row<kBoardSizeNum;row++){  //将计分结果储存在计分板内
        for(col=1;col<kBoardSizeNum;col++){
            crosses[row][col].col=col;
            crosses[row][col].row=row;
            crosses[row][col].type=gameMapVec[row][col] ;
            crosses[row][col].score=scoreMapVec[row][col] ;
    }
    }


    int xrow=kBoardSizeNum/2, xcol=kBoardSizeNum/2, xscore=10;    //若无法比较出结果时，默认的位置
    while(gameMapVec[xrow][xcol]!=0){
        xrow++; xcol++;
    }

    priority_queue<Cross,vector<Cross> ,decltype(cmp)> Myqueue(cmp);  //所有Cross结构体压入队列

    for (row = 1; row < kBoardSizeNum; row++){   //每个cross，延伸出一个博弈枝
        for (col = 1; col < kBoardSizeNum; col++){
            int totalchildscore=0,childscore[10]={0};
              priority_queue<Cross,vector<Cross> ,decltype(cmp)> Whitequeue(cmp);  //白子情况队列
              if(gameMapVec[row][col]==0&&crosses[row][col].score>0)
                  gameMapVec[row][col]=-1;
              else {           //剪枝，减去分数过少的主干下的枝
                  continue;
              }
              calculateScore(1);   //假设下一步落白子，计分
              Cross imaginationcrosses[kBoardSizeNum][kBoardSizeNum];//白子计分板建立，排出前十个位置，之后展开
              for(int row1=1;row1<kBoardSizeNum;row1++){  //存储白子计分
                  for(int col1=1;col1<kBoardSizeNum;col1++){
                      imaginationcrosses[row1][col1].col=col1;
                      imaginationcrosses[row1][col1].row=row1;
                      imaginationcrosses[row1][col1].type=gameMapVec[row1][col1] ;
                      imaginationcrosses[row1][col1].score=scoreMapVec[row1][col1] ;
                      Whitequeue.push(imaginationcrosses[row1][col1]);   //白子计分情况压入队列，下一步是计算每种情况下黑子的最大分值
              }
              }
              for(int i=0;i<10;i++){  //用i计次，共取10次
                  int whiterow,whitecol,whitescore;
                  whiterow=Whitequeue.top().row;
                  whitecol=Whitequeue.top().col;
                  whitescore=Whitequeue.top().score;
                  Whitequeue.pop();
                  if(gameMapVec[whiterow][whitecol]==0&&imaginationcrosses[whiterow][whitecol].score>0)
                      gameMapVec[whiterow][whitecol]=1;  //假设白子落下，计算最大的黑子分值
                  else {                 //剪枝
                      continue;
                  }
                  calculateScore(-1);   //第三步推演，假设黑子落下，计分，取最大五种情况平均
                  priority_queue<Cross,vector<Cross> ,decltype(cmp)> Blackqueue(cmp);
                  Cross deepimagination[kBoardSizeNum][kBoardSizeNum];
                  for(int row2=1;row2<kBoardSizeNum;row2++){
                      for(int col2=1;col2<kBoardSizeNum;col2++){
                            deepimagination[row2][col2].col=col2;
                            deepimagination[row2][col2].row=row2;
                            deepimagination[row2][col2].score=scoreMapVec[row2][col2];
                            deepimagination[row2][col2].type=gameMapVec[row2][col2];
                            Blackqueue.push(deepimagination[row2][col2]);
                      }
                  }
                  for(int times=0;times<5;times++){    //取前五个的平均值
                      childscore[i]=childscore[i]+Blackqueue.top().score;
                      Blackqueue.pop();
                  }
                  totalchildscore=totalchildscore-whitescore+childscore[i]/50;
                  gameMapVec[whiterow][whitecol]=0;
              }

              gameMapVec[row][col]=0;
              crosses[row][col].score=crosses[row][col].score+totalchildscore/100;
              Myqueue.push(crosses[row][col]);
        }
    }

    //构造结果数组，返回给上层函数以绘制。
    int* result = new int[2];
    if(!Myqueue.empty()&&Myqueue.top().score>xscore)
    {
        result[0]=Myqueue.top().row;
        result[1]=Myqueue.top().col;
    }
    else{
        result[0]= xrow;
        result[1]= xcol;
    }


    return result;
}



//*******************************************************************************
//*******************************************************************************




//爬山算法
int* GameModel::choose3()
{
    int row, col;
    calculateScore(-1);    //黑子计分
    Cross crosses[kBoardSizeNum][kBoardSizeNum];   //计分板
    for(row=1;row<kBoardSizeNum;row++){  //存储黑子计分
        for(col=1;col<kBoardSizeNum;col++){
            crosses[row][col].col=col;
            crosses[row][col].row=row;
            crosses[row][col].type=gameMapVec[row][col] ;
            crosses[row][col].score=scoreMapVec[row][col] ;
    }
    }


    int xrow=kBoardSizeNum/2, xcol=kBoardSizeNum/2, xscore=0;    //若无法比较出结果时，默认的位置
    while(scoreMapVec[xrow][xcol]!=0){
        xrow++; xcol++;
    }

    Cross examples[10];
    for(int i = 0 ; i < 3 ; i++ )
        for(int j = 0 ; j < 3 ; j++)
        {
            examples[3*i+j].row=(i+1)*kBoardSizeNum/4;
            examples[3*i+j].col=(j+1)*kBoardSizeNum/4;
        }
    examples[9].row=latestrow;
    examples[9].col=latestcol;
    priority_queue<Cross,vector<Cross> ,decltype(cmp)> Myqueue(cmp);
    for(int i=0 ; i<10 ; i++){
        priority_queue<Cross,vector<Cross> ,decltype(cmp)> Childqueue(cmp);
        Childqueue.push(crosses[examples[i].row][examples[i].col]);
        bool flat=false;
        while(flat==false){
            flat = true;
            Cross directions[25];
            for(int j=0 ; j<5 ; j++)
                for(int k=0; k<5 ; k++){
                    if(Childqueue.top().row-2+j>0&&Childqueue.top().row-2+j<kBoardSizeNum&&
                            Childqueue.top().col-2+k>0&&Childqueue.top().col-2+k<kBoardSizeNum){
                        directions[j*5+k].row=Childqueue.top().row-2+j;
                        directions[j*5+k].col=Childqueue.top().col-2+k;
                        directions[j*5+k].type=crosses[directions[j*5+k].row][directions[j*5+k].col].type;
                        directions[j*5+k].score=crosses[directions[j*5+k].row][directions[j*5+k].col].score;
                    }
                    else{
                        directions[j*5+k].row=Childqueue.top().row;
                        directions[j*5+k].col=Childqueue.top().col;
                        directions[j*5+k].type=crosses[directions[j*5+k].row][directions[j*5+k].col].type;
                        directions[j*5+k].score=crosses[directions[j*5+k].row][directions[j*5+k].col].score;
                    }
                }
            for(int j=0 ; j<25 ; j++){
                if(directions[j].score>Childqueue.top().score)
                {
                    Childqueue.push(crosses[directions[j].row][directions[j].col]);
                    flat=false;
                }
            }

        }
        Myqueue.push(Childqueue.top());
    }


    //构造结果数组，返回给上层函数以绘制。
    int* result = new int[2];
    if(Myqueue.top().score>xscore)
    {
        result[0]=Myqueue.top().row;
        result[1]=Myqueue.top().col;
    }
    else{
        result[0]= xrow;
        result[1]= xcol;
    }


    return result;
}




//*******************************************************************************
//*******************************************************************************






void GameModel::calculateScore(int xstatus){
    int row, col;
    for (row = 1; row < kBoardSizeNum; row++){
        for (col = 1; col < kBoardSizeNum; col++){
            if(this->gameMapVec[row][col] == 0){
                int score[4];    //四个方向的分数
                bool decided=false;  //已经做出决定
                //横向分数
                //讨论黑
                //tryit++; cout<<endl<<"hi"<<tryit<<endl;
                this->gameMapVec[row][col] = xstatus;//假设落子为黑
                for (int i = 0; i < 5; i++)   //黑子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 4])
                        {
                            score[0]=1000000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //黑子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == xstatus &&
                             gameMapVec[row][col - i+5] == 0)
                            {
                                score[0]=10000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == -xstatus &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == xstatus &&
                             gameMapVec[row][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == xstatus &&
                            gameMapVec[row][col - i+5] == -xstatus) ||
                                (col - i ==0 &&              //边界障碍
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == xstatus &&
                            gameMapVec[row][col - i+5] == 0)||
                                (col - i + 5 == kBoardSizeNum&&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == xstatus ))
                            {
                                score[0]=100;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == xstatus &&
                            gameMapVec[row][col - i+4] == 0 )
                            {
                                score[0]=90;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子二连无障碍情况
                    for (int i = 1; i < 3; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 3 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == xstatus &&
                            gameMapVec[row][col - i+2] == xstatus &&
                            gameMapVec[row][col - i+3] == 0  )
                            {
                                score[0]=10;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子无状况
                    score[0]=0;
                }
                decided=false;

                //讨论白
                this->gameMapVec[row][col] = -xstatus; //假设落子为白、
                for (int i = 0; i < 5; i++)   //白子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
                        gameMapVec[row][col - i] == gameMapVec[row][col - i + 4])
                        {
                            score[0]=score[0]+100000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //白子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == -xstatus &&
                             gameMapVec[row][col - i+5] == 0)
                            {
                                score[0]=score[0]+1000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == xstatus &&
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == -xstatus &&
                             gameMapVec[row][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == -xstatus &&
                            gameMapVec[row][col - i+5] == xstatus) ||
                                (col - i ==0 &&              //边界障碍
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == -xstatus &&
                            gameMapVec[row][col - i+5] == 0)||
                                (col - i + 5 == kBoardSizeNum&&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == -xstatus ))
                            {
                                score[0]=score[0]+93;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            gameMapVec[row][col - i] == 0 &&
                            gameMapVec[row][col - i+1] == -xstatus &&
                            gameMapVec[row][col - i+2] == -xstatus &&
                            gameMapVec[row][col - i+3] == -xstatus &&
                            gameMapVec[row][col - i+4] == 0 )
                            {
                                score[0]=score[0]+88;
                                decided=true;
                                break;
                            }
                    }
                }
                decided=false;   //归位




                // 竖直方向
                //讨论黑
                this->gameMapVec[row][col] = xstatus;//假设落子为黑
                for (int i = 0; i < 5; i++)   //黑子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (row - i > 0 &&
                        row - i + 4 < kBoardSizeNum &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 4][col])
                        {
                            score[1]=1000000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //黑子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == xstatus &&
                             gameMapVec[row - i+5][col] == 0)
                            {
                                score[1]=10000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((row - i > 0 &&            //白子障碍
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == -xstatus &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == xstatus &&
                             gameMapVec[row - i+5][col] == 0)||
                              (row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == xstatus &&
                            gameMapVec[row - i+5][col] == -xstatus) ||
                                (row - i ==0 &&              //边界障碍
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == xstatus &&
                            gameMapVec[row - i+5][col] == 0)||
                                (row - i + 5 == kBoardSizeNum&&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == xstatus ))
                            {
                                score[1]=100;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 4 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == xstatus &&
                            gameMapVec[row - i+4][col] == 0 )
                            {
                                score[1]=90;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子二连无障碍情况
                    for (int i = 1; i < 3; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 3 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == xstatus &&
                            gameMapVec[row - i+2][col] == xstatus &&
                            gameMapVec[row - i+3][col] == 0  )
                            {
                                score[1]=10;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子无状况
                    score[1]=0;
                }
                decided=false;

                //讨论白
                this->gameMapVec[row][col] = -xstatus; //假设落子为白、
                for (int i = 0; i < 5; i++)   //白子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (row - i > 0 &&
                        row - i + 4 < kBoardSizeNum &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
                        gameMapVec[row - i][col] == gameMapVec[row - i + 4][col])
                        {
                            score[1]=score[1]+100000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //白子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == -xstatus &&
                             gameMapVec[row - i+5][col] == 0)
                            {
                                score[1]=score[1]+1000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((row - i > 0 &&            //白子障碍
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == xstatus &&
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == -xstatus &&
                             gameMapVec[row - i+5][col] == 0)||
                              (row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == -xstatus &&
                            gameMapVec[row - i+5][col] == xstatus) ||
                                (row - i ==0 &&              //边界障碍
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == -xstatus &&
                            gameMapVec[row - i+5][col] == 0)||
                                (row - i + 5 == kBoardSizeNum&&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == -xstatus ))
                            {
                                score[1]=score[1]+93;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 4 < kBoardSizeNum &&
                            gameMapVec[row - i][col] == 0 &&
                            gameMapVec[row - i+1][col] == -xstatus &&
                            gameMapVec[row - i+2][col] == -xstatus &&
                            gameMapVec[row - i+3][col] == -xstatus &&
                            gameMapVec[row - i+4][col] == 0 )
                            {
                                score[1]=score[1]+88;
                                decided=true;
                                break;
                            }
                    }
                }
                decided=false;   //归位


                // 左斜方向
                this->gameMapVec[row][col] = xstatus;//假设落子为黑
                for (int i = 0; i < 5; i++)   //黑子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        row + i < kBoardSizeNum &&
                        row + i - 4 > 0 &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4])
                        {
                            score[2]=1000000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //黑子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row + i< kBoardSizeNum &&
                            row + i - 5 > 0  &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)
                            {
                                score[2]=10000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 5 > 0 &&
                            gameMapVec[row + i][col - i] == -xstatus &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 5 > 0 &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == xstatus &&
                            gameMapVec[row + i-5][col - i+5] == -xstatus) ||
                                (((col - i ==0 &&row + i <=kBoardSizeNum&&row+i-5>0)||             //边界障碍
                                 (col - i >=0 &&row + i ==kBoardSizeNum&&col-i+5<kBoardSizeNum)) &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)||
                                (((col - i + 5 == kBoardSizeNum&&
                                 row + i - 5 >=0&&row+i<kBoardSizeNum)||
                                 (col - i + 5 <= kBoardSizeNum&&
                                 row + i - 5 ==0&&col-i>0))&&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == xstatus ))
                            {
                                score[2]=100;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (row + i < kBoardSizeNum &&
                            row + i - 4 > 0 &&
                            col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == xstatus &&
                            gameMapVec[row + i-4][col - i+4] == 0 )
                            {
                                score[2]=90;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子二连无障碍情况
                    for (int i = 1; i < 3; i++)
                    {
                        if (row + i < kBoardSizeNum &&
                            row + i - 3 > 0 &&
                            col - i > 0 &&
                            col - i + 3 < kBoardSizeNum &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == xstatus &&
                            gameMapVec[row + i-2][col - i+2] == xstatus &&
                            gameMapVec[row + i-3][col - i+3] == 0  )
                            {
                                score[2]=10;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子无状况
                    score[2]=0;
                }
                decided=false;

                //讨论白
                this->gameMapVec[row][col] = -xstatus; //假设落子为白、
                for (int i = 0; i < 5; i++)   //白子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        row + i < kBoardSizeNum &&
                        row + i - 4 > 0 &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
                        gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4])
                        {
                            score[2]=score[2]+100000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //白子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 5 > 0 &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == -xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)
                            {
                                score[2]=score[2]+1000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 5 > 0 &&
                            gameMapVec[row + i][col - i] == xstatus &&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == -xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 5 > 0 &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == -xstatus &&
                            gameMapVec[row + i-5][col - i+5] == xstatus) ||
                                (((col - i ==0 &&row + i <=kBoardSizeNum&&row+i-5>0)||              //边界障碍
                                 (col - i >=0 &&row + i ==kBoardSizeNum&&col-i+5<kBoardSizeNum))&&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == -xstatus &&
                            gameMapVec[row + i-5][col - i+5] == 0)||
                                (((col - i + 5 == kBoardSizeNum&&row + i - 5 >= 0&&row+i<kBoardSizeNum)||
                                 (col - i + 5 <= kBoardSizeNum&&row + i - 5 == 0&&col-i>0))&&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == -xstatus ))
                            {
                                score[2]=score[2]+93;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            row + i < kBoardSizeNum &&
                            row + i - 4 > 0 &&
                            gameMapVec[row + i][col - i] == 0 &&
                            gameMapVec[row + i-1][col - i+1] == -xstatus &&
                            gameMapVec[row + i-2][col - i+2] == -xstatus &&
                            gameMapVec[row + i-3][col - i+3] == -xstatus &&
                            gameMapVec[row + i-4][col - i+4] == 0 )
                            {
                                score[2]=score[2]+88;
                                decided=true;
                                break;
                            }
                    }
                }
                decided=false;   //归位

                // 右斜方向
                this->gameMapVec[row][col] = xstatus;//假设落子为黑
                for (int i = 0; i < 5; i++)   //黑子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        row - i > 0 &&
                        row - i + 4 < kBoardSizeNum &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 4][col - i + 4])
                        {
                            score[3]=1000000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //黑子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)
                            {
                                score[3]=10000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == -xstatus &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == xstatus &&
                            gameMapVec[row - i+5][col - i+5] == -xstatus) ||
                                (((col - i ==0 &&row - i >=0&&row-i+5<kBoardSizeNum)||             //边界障碍
                                 (col - i >=0 &&row - i ==0&&col-i+5<kBoardSizeNum)) &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)||
                                (((col - i + 5 == kBoardSizeNum&&
                                 row - i + 5 <=kBoardSizeNum&&row-i>0)||
                                 (col - i + 5 <= kBoardSizeNum&&
                                 row - i + 5 ==kBoardSizeNum&&col-i>0))&&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == xstatus ))
                            {
                                score[3]=100;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 4 < kBoardSizeNum &&
                            col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == xstatus &&
                            gameMapVec[row - i+4][col - i+4] == 0 )
                            {
                                score[3]=90;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子二连无障碍情况
                    for (int i = 1; i < 3; i++)
                    {
                        if (row - i > 0 &&
                            row - i + 3 < kBoardSizeNum &&
                            col - i > 0 &&
                            col - i + 3 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == xstatus &&
                            gameMapVec[row - i+2][col - i+2] == xstatus &&
                            gameMapVec[row - i+3][col - i+3] == 0  )
                            {
                                score[3]=10;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //黑子无状况
                    score[3]=0;
                }
                decided=false;

                //讨论白
                this->gameMapVec[row][col] = -xstatus; //假设落子为白、
                for (int i = 0; i < 5; i++)   //白子五连情况
                {
                    // 往左5个，往右匹配4个子，20种情况
                    if (col - i > 0 &&
                        col - i + 4 < kBoardSizeNum &&
                        row - i > 0 &&
                        row - i + 4 < kBoardSizeNum &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
                        gameMapVec[row - i][col - i] == gameMapVec[row - i + 4][col - i + 4])
                        {
                            score[3]=score[3]+100000;
                            decided=true;
                            break;
                        }
                }
                if(!decided){            //白子四连无障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == -xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)
                            {
                                score[3]=score[3]+1000;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子四连有障碍情况
                    for (int i = 1; i < 5; i++)
                    {
                        if ((col - i > 0 &&            //白子障碍
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == xstatus &&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == -xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)||
                              (col - i > 0 &&
                            col - i + 5 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 5 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == -xstatus &&
                            gameMapVec[row - i+5][col - i+5] == xstatus) ||
                                (((col - i ==0 &&row - i >=0&&row-i+5<kBoardSizeNum)||              //边界障碍
                                 (col - i >=0 &&row - i ==0&&col-i+5<kBoardSizeNum))&&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == -xstatus &&
                            gameMapVec[row - i+5][col - i+5] == 0)||
                                (((col - i + 5 == kBoardSizeNum&&row - i + 5 <= kBoardSizeNum&&row-i>0)||
                                 (col - i + 5 <= kBoardSizeNum&&row - i + 5 == kBoardSizeNum&&col-i>0))&&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == -xstatus ))
                            {
                                score[3]=score[3]+93;
                                decided=true;
                                break;
                            }
                    }
                }
                if(!decided){            //白子三连无障碍情况
                    for (int i = 1; i < 4; i++)
                    {
                        if (col - i > 0 &&
                            col - i + 4 < kBoardSizeNum &&
                            row - i > 0 &&
                            row - i + 4 < kBoardSizeNum &&
                            gameMapVec[row - i][col - i] == 0 &&
                            gameMapVec[row - i+1][col - i+1] == -xstatus &&
                            gameMapVec[row - i+2][col - i+2] == -xstatus &&
                            gameMapVec[row - i+3][col - i+3] == -xstatus &&
                            gameMapVec[row - i+4][col - i+4] == 0 )
                            {
                                score[3]=score[3]+88;
                                decided=true;
                                break;
                            }
                    }
                }
                decided=false;   //归位

                this->gameMapVec[row][col] = 0;
                scoreMapVec[row][col]=score[0]+score[1]+score[2]+score[3];
            }
            else{
                scoreMapVec[row][col]=-1;
            }


            }
        }


}

//int* GameModel::choose()
//{
//    bool find = false; //flag
//    int row, col;
//    for (row = 1; row < kBoardSizeNum; row++){
//        for (col = 1; col < kBoardSizeNum; col++){
//            if (this->gameMapVec[row][col] == 0){ //如果第row行，第col列没有棋子，则记录下该位置，break掉。
//                find = true;break;
//            }
//        }
//        if(find) break;
//    }

//    //构造结果数组，返回给上层函数以绘制。
//    int* result = new int[2];
//    result[0] = row;
//    result[1] = col;
//    return result;
//}



