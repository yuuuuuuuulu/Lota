//?????????????
//????????
//?????
//??��??????
//??????��?????cd
//??????????????????????????????????
//?????��??????????
//?��????
//?????????��???
//???????????????��????��??
//????��????????????????
//????????????????????
//????????????
//?????????????move???????????
//50914 ??????��??
//????????��???????? ?????????????
//????????????
//????????????
//???????????????
// 51785
//51924  ?????????��
//????????????????????
// 51941 279round
//52318 198 ???????
//52235 527????????
//52649 447 ???????? pushingTower??fightingstrategy ??????????
#include "sdk.h"
#include<ctime>
#include<iostream>
#include<stdio.h>
#include<fstream>
#define ReinforceDistance 900
#define DefendTowerDistance 1600
#define PushingLevel 22
#define Maxint 2100000000
#define W *10000
#define RainyValue 12500
#define TowerAttackValue 12500
#define MinHpValue 5
#define ArcherStepValue 1000
#define FarAwayValue 100
#define AvoidArrowRainSwitch true
using namespace std;

ofstream fout("newout.txt",ios::app);
string LvUp[5][10] = {
"PALU", "PALU", "PALU", "ARLU", "ARLU", "ARLU",  "YellowPoint", "YellowPoint", "YellowPoint", "YellowPoint",
"ARLU", "ARLU", "ARLU", "PALU", "PALU", "PALU",  "YellowPoint", "YellowPoint", "YellowPoint", "YellowPoint",
"PALU", "PALU", "PALU", "ARLU", "ARLU", "ARLU",  "YellowPoint", "YellowPoint", "YellowPoint", "YellowPoint",
"ARLU", "ARLU", "ARLU", "PALU", "PALU", "PALU",  "YellowPoint", "YellowPoint", "YellowPoint", "YellowPoint",
"PALU", "PALU", "PALU", "ARLU", "ARLU", "ARLU",  "YellowPoint", "YellowPoint", "YellowPoint", "YellowPoint"};
const Pos EMPTYPOS = Pos(), DEFAULFPOS = Pos(75, 75);
const int EMPTYID = 0, HIGHLEVEL = 0, MAXLEVEL = 10, MAXID = 100;

static int fixId[MAXID];
static Pos fixPos[MAXID];
static bool towerDes[2],OurTowerDes[2];
typedef vector<const PUnit*> PUnits;
typedef vector<const PArea*> PAreas;
int reviving[50];
int StimulateHp[50];
Pos StimulatePos[50];
Pos OurHeroPos[5];
Pos MonsterBornPos[50];
double OurHeroInDizzy[5];
bool HeroHadBeenProtected[5];
int LastRoundsGetInfo[50];
PUnits OurHero,ours, others, nearBy,EnemyHero;
bool EnemyCanEscape[5];
bool EnemyIntheRain[5];
bool OurHeroIntheRain[5];
PAreas OurArea,EnemyArea;
const Pos* EnemyTower,*FriendlyTower;
const double EPS = 1e-7;
static vector<Pos> basic_blocks;
static vector<Pos> blocks;
static vector<Pos> blocks_without_Tower;
static vector<Pos> blocks_without_Tower_Rainy;
static vector<Pos> blocks_without_Rainy;
const PMap* MAP;
const PPlayerInfo* INFO;
Pos SpringPos;
bool Pushed[5] = {0};
int unitEvaluation(string name);
double StrengthCaculate(const Pos &centre, int range, int camp);
void GetBlocks(){
    blocks.clear();
    blocks_without_Tower.clear();
    blocks_without_Rainy.clear();
    blocks_without_Tower_Rainy.clear();
    for (int i=0; i<INFO->units.size(); ++i){
        basic_blocks.push_back(INFO->units[i].pos);
        blocks.push_back(INFO->units[i].pos);
        blocks_without_Tower.push_back(INFO->units[i].pos);
        blocks_without_Rainy.push_back(INFO->units[i].pos);
        blocks_without_Tower_Rainy.push_back(INFO->units[i].pos);
    }
    for (int i = 0; i < 2; ++i) if (!towerDes[i]){
        blocks.push_back(EnemyTower[i]);
        for (int x = -12;x <= 12;x++)
            for (int y = -12;y <= 12;y++)
                if (x*x+y*y<=144){
                    blocks.push_back(EnemyTower[i] + Pos(x,y));
                    blocks_without_Rainy.push_back(EnemyTower[i] + Pos(x,y));
                }
    }
    for (int i = 0; i < EnemyArea.size();i++)
        for (int x = -5;x <= 5;x++)
            for (int y = -5;y <= 5;y++)
            if (x*x+y*y<=25){
                blocks.push_back(EnemyArea[i]->center + Pos(x,y));
                blocks_without_Tower.push_back(EnemyArea[i]->center + Pos(x,y));
            }

}
ostream& operator <<(ostream& O,Pos p){
    O << "(" << p.x << ',' << p.y << ")\n";
    return O;
}
bool operator <(const Pos& a,const Pos b){
    return a.y < b.y;
}
double MIN(double a,double b){
    if (a < b)
        return a;
    else
        return b;
}
double MAX(double a,double b){
    if (a > b)
        return a;
    else
        return b;
}
//-------------------------------------------------------------------------------From Monster.cc
#define Maxid 100

int MonsterPlayer = 2;
static int ang[Maxid][Maxid] = {0};
void calAng() //??????????????????
{
    PUnits ourUnits;
    INFO->findUnitByPlayer(MonsterPlayer,ourUnits);
    for (int i = 0; i < ourUnits.size(); ++i)
    {
        const PArg* arg = (*ourUnits[i])["LastHit"];
        if (arg)
        {
            for(int j=0; j<Maxid && j<arg->val.size(); ++j)
                if (arg->val[j] >= INFO->round-1)
                    ang[ourUnits[i]->id][j] += 5; //??????????��?????+5
        }
    }
    for (int i = 0; i < OurHero.size(); ++i)
    {
        const PUnit* otherUnit = OurHero[i];
        for (int j = 0; j < ourUnits.size(); ++j)
        {
            if (dis2(otherUnit->pos,ourUnits[j]->pos) <= 8)
                ang[ourUnits[j]->id][otherUnit->id] += 1;
            if (dis2(otherUnit->pos,ourUnits[j]->pos) > 144)
                ang[ourUnits[j]->id][otherUnit->id] = 0;
            if (ourUnits[j]->findBuff("Reviving"))
                ang[ourUnits[j]->id][otherUnit->id] = 0;
        }
    }
}
//-------------------------------------------------------------------------------------fromsdk.cc
class ShortPath{
    int vst[151][151];
    Pos Q[151 * 151 + 1], Q_prior[151][151];
    int countF;
public:
    ShortPath():countF(0){}
    void findShortestPath(const PMap &map, Pos start, Pos dest, const std::vector<Pos> &blocks, std::vector<Pos> &_path)
    //????��???��???????��??start??dest?????????��????blocks?��?��?��???????????????????��??????????????????????
    {
        int _sz = blocks.size();

        ++countF;
        _path.clear();
        for (int i=0; i<_sz; ++i)
        {
            if (checkPos(blocks[i]))
                vst[blocks[i].x][blocks[i].y] = countF;
        }
        if (!checkPos(start) || !checkPos(dest))
        {
            _path.push_back(Pos());
            return;
        }
        vst[start.x][start.y] = vst[dest.x][dest.y] = countF - 1;

        Pos _0, _1;

        int dir_ord[4];
        Q[0] = start;
        vst[start.x][start.y] = countF;
        for (int front=0, rear=0; front<=rear; ++front) {
            _0 = Q[front];
            for (int i=0; i<4; ++i) dir_ord[i] = (i + front + countF) % 4;

            for (int i=0; i<4; ++i) {
                _1 = _0 + Pos(dir[dir_ord[i]][0], dir[dir_ord[i]][1]);
                if (checkPos(_1) && abs(map.height[_0.x][_0.y] - map.height[_1.x][_1.y]) <= 1 && vst[_1.x][_1.y] != countF) {
                    vst[_1.x][_1.y] = countF;
                    Q[++rear] = _1;
                    Q_prior[_1.x][_1.y] = _0;
                }
            }
        }

        if (vst[dest.x][dest.y] == countF) {
            for (_0 = dest; _0 != start; _0 = Q_prior[_0.x][_0.y])
                _path.push_back(_0);
            _path.push_back(_0);
            reverse(_path.begin(), _path.end());
        } else _path.push_back(Pos());
    }
}SelfShortPath;

//---------------------------------------------------------------------------------------------------
const PUnit* belongs(int id, PUnits& units) //???unit?????vector?��??????id??unit
{
    for (int i=0;i<units.size();++i)
        if (units[i]->id == id)
            return units[i];
    return NULL;
}
int Skill(string skillname){
    for (int i = 0; i < 20;i++)
        if (skillname == Skill_name[i])
            return i;
    fout << "??????"<<skillname<<endl;
    return -1;
}

double StrengthCaculate(const Pos& centre,int range,int camp){
    double ans = 0;
    if (camp == INFO->camp){
        for (int i = 0;i < OurHero.size();i++)
            if (dis2(OurHero[i]->pos,centre) < range)
                ans += (3 + OurHero[i]->level);
    }
    else {
        for (int i = 0;i < EnemyHero.size();i++)
            if (dis2(EnemyHero[i]->pos,centre) < range)
                ans += (3 + EnemyHero[i]->level);
    }
    return ans;
}
double StrengthCompare(const Pos& centre,int range){
    double Our = 0,Enemy = 0;
    for (int i = 0;i < OurHero.size();i++)
        if (dis2(OurHero[i]->pos,centre) < range){
            Our += (3 + OurHero[i]->level) * (OurHero[i]->hp + OurHero[i]->max_hp / 2);
            if (OurHeroInDizzy[OurHero[i]->id % 5] > 0)
                return 1;
        }
    for (int i = 0;i < EnemyHero.size();i++)
        if (dis2(EnemyHero[i]->pos,centre) < range)
            Enemy += (3 + EnemyHero[i]->level) * (EnemyHero[i]->hp + EnemyHero[i]->max_hp / 2);
    if (Enemy == 0)
        return Maxint;
    return Our / Enemy;
}

int PushingTower = -1;
int unitEvaluation(string name) //?????????��???????
{

    if (name == string("Fighter") || name == string("Assassin") || name == string("Archer")) //???????��???��???
        return -1;
    if (PushingTower > 0){
        if (name == string("Tower"))
            return 0;
        else
            return Maxint / 10000;
    }
    if ((name == string("Tower") && PushingTower <= 0) || name == string("Roshan"))
        return Maxint / 10000;
    if (name == string("Wolf") || name == string("StoneMan") || name == string("Bow"))
        return 6;
    if (name == string("Dragon"))
        return 15;
    if (PushingTower <=0 || name == string("Tower"))
        return 0;
    else
        return Maxint / 10000;
}
void levelUp(const PUnit* a, PCommand &cmd) //??��a????????
{
    Operation op;
    op.id = a->id;
    op.type = LvUp[a->id%5][a->level + 1 - (*a)["Exp"]->val[3]];
    cmd.cmds.push_back(op);
}
void chooseHero(const PPlayerInfo &info, PCommand &cmd) //??0?????????
{
    for(int i=0;i<info.units.size();++i)
    {
        Operation op;
        int r=2;
        //if (i >2) r = 1;
        if(r==0)
            op.type="ChooseAssassin";
        else if(r==1)
            op.type="ChooseFighter";
        else if(r==2)
            op.type="ChooseArcher";
        op.targets.clear();
        op.id=info.units[i].id;
        cmd.cmds.push_back(op); //?????????
    }
}
//--------------------------------------------------------------------------------------------------------------value.
class Strategy;
class Advicer;
int TotalTask = 0,TotalAdvicer = 0;
Strategy* HeroTask[100];
Strategy* Task[100];
Advicer* Advicers[100];
//-----------------------------------------------------------------------------------------------------Factor.cc
class BFS;
enum Direct{
    Horizontal = 0,
    Vertical,
    None
};
class Factor{
public:
    virtual int evaluate(Pos pos) = 0;
};
class AvoidInFriendlyTower:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class AvoidEnemyTower:public Factor{
protected:
    const PUnit* myhero;
public:
    AvoidEnemyTower(const PUnit* m):myhero(m){}
    virtual int evaluate(Pos pos);
};
class AvoidMeeleHero:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class Dispersed:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class ArcherEasyAttackEnemy:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class AvoidArrowRain:public Factor{
protected:
    const PUnit* myhero;
public:
    AvoidArrowRain(const PUnit* m):myhero(m){}
public:
    virtual int evaluate(Pos pos);
};
class FarFromTarget:public Factor{
protected:
    const Pos& target;
public:
    FarFromTarget(const Pos& p):target(p){}
    virtual int evaluate(Pos pos);
};
class MoveToEvaluate:public Factor{
protected:
    const Pos& target;
public:
    MoveToEvaluate(const Pos& p):target(p){}
    virtual int evaluate(Pos pos);
};
class FarAwayFromEnemy:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class RunAfterEnemy:public Factor{
public:
    virtual int evaluate(Pos pos);
};
class FarAwayFromOtherMonster:public Factor{
public:
    virtual int evaluate(Pos pos);
};
int FarAwayFromOtherMonster::evaluate(Pos pos){
    PUnits near;
    INFO->findUnitInArea(pos,8,near);
    int valu = 0;
    for (int i = 0 ; i < near.size();i++)
        if (near[i]->id >= 16 && near[i]->id <= 38)
            valu -= 1500;
    return valu;
}

int RunAfterEnemy::evaluate(Pos pos){
    double mindis = Maxint;
    for (int i = 0; i < EnemyHero.size();i++) if (dis(pos,EnemyHero[i]->pos) < mindis) mindis = dis(pos,EnemyHero[i]->pos);
    if (mindis > Maxint - 1)
        return 0;
    else
        return mindis < 3?0:-int((mindis - 3) * 1000);
}
int FarAwayFromEnemy::evaluate(Pos pos){
    int valu = 0;
    for (int i = 0; i < EnemyHero.size();i++){
        valu -= 50*(dis2(EnemyHero[i]->pos,pos) <= 625?abs (15 - dis(EnemyHero[i]->pos,pos)):0);
    }
    return valu;
}

int MoveToEvaluate::evaluate(Pos pos){
    return -int(dis(pos,target) * FarAwayValue);
}
int FarFromTarget::evaluate(Pos pos){
    return int(dis(pos,target) * FarAwayValue);
}
int AvoidInFriendlyTower::evaluate(Pos pos){
    int valu = 0;
    for (int i = 0;i < 2;i++)
        if (!OurTowerDes[i] && dis2(pos,FriendlyTower[i]) <= 51)
            valu += 3500;
        else
            if (!OurTowerDes[i] && dis2(pos,FriendlyTower[i]) <= 400)
                valu += int ((20 - dis(pos,FriendlyTower[i])) * 150);
    return valu;

}
int AvoidEnemyTower::evaluate(Pos pos){
    int Step = 0;
    for (int i = 0;i < 2;i++)
        if (!towerDes[i] && dis2(pos,EnemyTower[i]) <= 100){
            Step =int ((10 - dis(pos,EnemyTower[i])) / (sqrt(myhero->speed) - 0.5) + 1);
        }
    return -Step * TowerAttackValue;
}
int AvoidMeeleHero::evaluate(Pos pos){
    int valu = 0;
    for (int i = 0; i < EnemyHero.size();i++) if (EnemyHero[i]->range < 25){
        valu -= dis2(EnemyHero[i]->pos,pos) < EnemyHero[i]->range?2000:
     int(200*(dis2(EnemyHero[i]->pos,pos) < 47?9 - dis(EnemyHero[i]->pos,pos):0));
    }
    return valu;
}
int Dispersed::evaluate(Pos pos){
    int valu = 0;
    const PUnit* enemy;
    int Dispersed_Rate = 40;
    int MaxPiercingArrow = -1,MaxArrowsRain = -1,MaxYell = -1,MaxSpin = -1;
    for (int i = 0;i < EnemyHero.size();i++){
        enemy = EnemyHero[i];
        if (dis2(enemy->pos,pos) < 170 && enemy->canUseSkill(Skill("PiercingArrow")) && enemy->findSkill("PiercingArrow")->level >  MaxPiercingArrow)
            MaxPiercingArrow = enemy->findSkill("PiercingArrow")->level;
        if (dis2(enemy->pos,pos) < 100 && enemy->canUseSkill(Skill("ArrowsRain")) && enemy->findSkill("ArrowsRain")->level >  MaxPiercingArrow)
            MaxArrowsRain = enemy->findSkill("ArrowsRain")->level;
        if (dis2(enemy->pos,pos) < 58 && enemy->canUseSkill(Skill("Yell")) && enemy->findSkill("Yell")->level >  MaxYell)
            MaxYell= enemy->findSkill("Yell")->level;
        if (dis2(enemy->pos,pos) < 58 && enemy->canUseSkill(Skill("Spin")) && enemy->findSkill("Spin")->level >  MaxSpin)
            MaxSpin = enemy->findSkill("Spin")->level;
    }
    for (int i = 0; i < 5;i++){
        //valu -= dis2(OurHeroPos[i],pos) < 25?int(Dispersed_Rate*(5 - dis(OurHeroPos[i],pos))):0;
        valu -= dis2(OurHeroPos[i],pos) < 9?int((MaxPiercingArrow + 1)*100*(5 - dis(OurHeroPos[i],pos))):0;
        valu -= dis2(OurHeroPos[i],pos) < 25?int((MaxArrowsRain + 1)*200*(6 - dis(OurHeroPos[i],pos))):0;
        valu -= dis2(OurHeroPos[i],pos) < 13?int((MaxYell + 1)*50*(5 - dis(OurHeroPos[i],pos))):0;
        valu -= dis2(OurHeroPos[i],pos) < 13?int((MaxSpin + 1)*100*(5 - dis(OurHeroPos[i],pos))):0;
    }
    return valu;
}
int ArcherEasyAttackEnemy::evaluate(Pos pos){
    int TotalMinHp = Maxint,minhp = Maxint,rank = 0;
    for (int i = 0 ; i < EnemyHero.size();i++) if (StimulateHp[EnemyHero[i]->id] > 0 && dis2(EnemyHero[i]->pos,pos) <= 81 && minhp > StimulateHp[EnemyHero[i]->id]){

    }
    if (minhp == Maxint){
        double speed = 4.5;
        double range = 9;
        int valu = Maxint;
        for (int i = 0 ; i < EnemyHero.size();i++) if (valu > ArcherStepValue * (dis(pos,EnemyHero[i]->pos) - range) / speed + StimulateHp[EnemyHero[i]->id] * MinHpValue)
            valu = int(ArcherStepValue * (dis(pos,EnemyHero[i]->pos) - range) / speed + StimulateHp[EnemyHero[i]->id] * MinHpValue);
        return -valu;
    }
    else {
        return -minhp * MinHpValue;
    }
}
int AvoidArrowRain::evaluate(Pos pos){
    int nowRainyStatus = 1;
    if (!OurHeroIntheRain[myhero->id % 5])
        nowRainyStatus = 0;
    int RainyStep;
    for (int i =0;i <EnemyArea.size();i++)
        if (EnemyArea[i]->containPos(pos)) {
            RainyStep =int ((EnemyArea[i]->radius - dis(pos,EnemyArea[i]->center)) / (sqrt(myhero->speed) - 0.5) + 2);
            RainyStep = MIN(RainyStep,EnemyArea[i]->timeLeft);
            if (RainyStep > nowRainyStatus)
                nowRainyStatus =  RainyStep;
        }
    return -nowRainyStatus * RainyValue;
}
//--------------------------------------------------------------------------------------------TargetEstimate.cc
class TargetEstimate{  //????????
    protected:
        Pos BestTarget;
        int value;
        vector<Factor*> Factors;
        int myheroPosvalue;
        const PUnit* myhero;
    public:
        int PosEvaluate(Pos pos);
        TargetEstimate(const PUnit* m):value(-Maxint),myheroPosvalue(0),myhero(m){}
        int getvalue(){return value;}
        Pos gettarget(){return BestTarget;}
        virtual void compare(Pos pos,BFS* bfs);
};

TargetEstimate* TargetBFS(TargetEstimate* estimate,const PUnit* my_hero,int step,vector<Pos>& _path,vector<Pos>& block,int value_max = Maxint);
class FarAwayFromTarget:public TargetEstimate{
public:
    FarAwayFromTarget(const Pos& p,const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(myhero));
        Factors.push_back(new FarFromTarget(p));
        Factors.push_back(new AvoidInFriendlyTower);
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
class MoveToTarget:public TargetEstimate{
public:
    MoveToTarget(const Pos& p,const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(myhero));
        Factors.push_back(new MoveToEvaluate(p));
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
class FightingMoveEvaluation:public TargetEstimate{
public:
    FightingMoveEvaluation(const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(m));
        Factors.push_back(new AvoidInFriendlyTower);
        Factors.push_back(new AvoidMeeleHero);
        Factors.push_back(new Dispersed);
        Factors.push_back(new ArcherEasyAttackEnemy);
        Factors.push_back(new AvoidEnemyTower(m));
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
class RunAwayFromPowerfulEnemy:public TargetEstimate{
public:
    RunAwayFromPowerfulEnemy(const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(m));
        Factors.push_back(new AvoidInFriendlyTower);
        Factors.push_back(new AvoidMeeleHero);
        Factors.push_back(new Dispersed);
        Factors.push_back(new AvoidEnemyTower(m));
        Factors.push_back(new FarAwayFromEnemy);
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
class RunAfterWeakEnemy:public TargetEstimate{
public:
    RunAfterWeakEnemy(const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(m));
        Factors.push_back(new AvoidMeeleHero);
        Factors.push_back(new Dispersed);
        Factors.push_back(new AvoidEnemyTower(m));
        Factors.push_back(new RunAfterEnemy);
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
class PushingTowerMove:public TargetEstimate{
public :
    PushingTowerMove(const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(m));
        Factors.push_back(new Dispersed);
        Factors.push_back(new AvoidEnemyTower(m));
        Factors.push_back(new AvoidMeeleHero);
        myheroPosvalue = PosEvaluate(m->pos);
    }
    virtual void compare(Pos pos,BFS* bfs);
};
class PushingDispersed:public TargetEstimate{
public :
    PushingDispersed(const PUnit* m):TargetEstimate(m){
        Factors.push_back(new AvoidArrowRain(m));
        Factors.push_back(new AvoidEnemyTower(m));
        Factors.push_back(new Dispersed);
        Factors.push_back(new ArcherEasyAttackEnemy);
        Factors.push_back(new FarAwayFromOtherMonster);
        myheroPosvalue = PosEvaluate(m->pos);
    }
};
void PushingTowerMove::compare(Pos pos,BFS* bfs){
    int valu = PosEvaluate(pos) - myheroPosvalue;
    if (dis2(pos,EnemyTower[0]) > 81 && dis2(pos,EnemyTower[1]) > 81) valu = -Maxint;
    if (valu >= value){
        value = valu;
        BestTarget = pos;
    }
}
int TargetEstimate::PosEvaluate(Pos pos){
    int valu  = 0;
    for (int i = 0;i < Factors.size();i++){
        valu += Factors[i]->evaluate(pos);
        fout << myhero->id << ' ' << valu << ' ' << i << ' ' << pos;
    }
    return valu;
}
void TargetEstimate::compare(Pos pos,BFS* bfs){
    int valu = PosEvaluate(pos) - myheroPosvalue;
    if (valu >= value){
        value = valu;
        BestTarget = pos;
    }
}
//-----------------------------------------------------------------------------------------------------------BFS.cc
struct BFS{
    int head,tail,step;
    const int size_max,centre_fix;
    void enqueue(int x,int y);
    Pos* queue;
    double* spd;
    Direct* dir;
    int *prev,*step_remain;
    bool stone[41][41];
    TargetEstimate* estimate;
    const PUnit* my_hero;
    BFS(TargetEstimate* est,const PUnit* my,int s,vector<Pos>& block);
    int FindBestTarget(vector<Pos>& _path,int = Maxint);
    ~BFS();
};
BFS::BFS(TargetEstimate* est,const PUnit* my,int s,vector<Pos>& block):
size_max(int(sqrt(my->speed) + 1) * s * 2 + 1),centre_fix(int(sqrt(my->speed) + 1) * s),
head(0),tail(0),estimate(est),my_hero(my),step(s)
{
    dir = new Direct[size_max * size_max];
    queue = new Pos[size_max * size_max];
    prev = new int[size_max * size_max];
    step_remain = new int[size_max * size_max];
    spd = new double[size_max * size_max];
    memset(stone,0,sizeof(stone));
    dir[0] = None;
    spd[0] = 0;
    step_remain[0] = step;
    queue[0].x = 0; queue[0].y = 0;
    for (int i = 0;i < block.size();i++){
        if (abs((block[i].x - my_hero->pos).x) < int(sqrt(my->speed) + 1) * s && (abs((block[i] - my_hero->pos).y)) < int(sqrt(my->speed) + 1) * s)
            stone[block[i].x - my_hero->pos.x + centre_fix][block[i].y - my_hero->pos.y + centre_fix] = true;
    }
    stone[centre_fix][centre_fix] = true;

}
BFS::~BFS(){
    delete dir;
    delete queue;
    delete prev;
    delete step_remain;
    delete spd;
}
void BFS::enqueue(int x,int y)
{
    Pos now = queue[head] + my_hero->pos + Pos(x,y);
    if (!checkPos(now) || abs(MAP->height[now.x][now.y] - MAP->height[now.x - x][now.y - y]) > 1) return;
    bool& st = stone[queue[head].x + centre_fix + x][queue[head].y + centre_fix + y];
    double speed_remain = spd[head];
    if (st) return;
    if (((dir[head] == Horizontal && x == 0) || (dir[head] == Vertical && y == 0)) && speed_remain > 0.414 - EPS)
    {
        speed_remain -= 0.414;
        queue[++tail] = queue[head] + Pos(x,y);
        dir[tail] = None;
        spd[tail] = speed_remain;
        step_remain[tail] = step_remain[head];
        prev[tail] = head;
        st = true;
    }
    else
    {
        speed_remain -= 1;
        if (speed_remain > 0 - EPS || step_remain[head] > 0){
            queue[++tail] = queue[head] + Pos(x,y);
            if (y == 0)
                dir[tail] = Horizontal;
            else
                dir[tail] = Vertical;
            if (speed_remain > 0 - EPS){
                spd[tail] = speed_remain;
                step_remain[tail] = step_remain[head];
            }
            else {
                spd[tail] = sqrt(my_hero->speed) - 1;
                step_remain[tail] = step_remain[head] - 1;
            }
            prev[tail] = head;
            st = true;
        }
    }

}
int BFS::FindBestTarget(vector<Pos>& _path,int value_max){
    _path.clear();
    while (head <= tail){
        estimate->compare(queue[head] + my_hero->pos,this);
        if (estimate->getvalue() > value_max) break;
        if (dir[head] == Horizontal){
            enqueue(0,1);
            enqueue(0,-1);
            enqueue(1,0);
            enqueue(-1,0);
        }
        else {
            enqueue(1,0);
            enqueue(-1,0);
            enqueue(0,1);
            enqueue(0,-1);
        }
        head++;
    }
    int best;
    for (int i  = 0; i <= tail;i++) if (queue[i] + my_hero->pos == estimate->gettarget()){
        best = i;
        break;
    }
    while (best != 0){
        _path.push_back(my_hero->pos + queue[best]);
        best = prev[best];
    }
    _path.push_back(my_hero->pos);
    reverse(_path.begin(), _path.end());
    return estimate->getvalue();
}
TargetEstimate* TargetBFS(TargetEstimate* estimate,const PUnit* my_hero,int step,vector<Pos>& _path,vector<Pos>& block,int value_max){
    if (AvoidArrowRainSwitch && !OurHeroIntheRain[my_hero->id % 5])
        for (int i = 0; i < EnemyArea.size();i++)
            for (int x = -5;x <= 5;x++)
                for (int y = -5;y <= 5;y++)
                    if (x*x+y*y<=25)
                        block.push_back(EnemyArea[i]->center + Pos(x,y));
    BFS bfs(estimate,my_hero,step,block);
    bfs.FindBestTarget(_path,value_max);
    if (AvoidArrowRainSwitch && !OurHeroIntheRain[my_hero->id % 5])
        for (int i = 0; i < EnemyArea.size();i++)
            for (int x = -5;x <= 5;x++)
                for (int y = -5;y <= 5;y++)
                    if (x*x+y*y<=25)
                        block.pop_back();
    return estimate;
}
//--------------------------------------------------------------------------------------------------------------strategy.cc
class Strategy{// ?????????��??????????
	protected:        
		int _hero[5];
        const PUnit* myhero;
        int Taskid(){for (int i = 0; i < TotalTask;i++) if (this == Task[i]) return i; return -1;}
        bool FightMonster(PCommand &cmd,const PUnit* ptr);
        void fightingMonster(const PUnit* a, const PUnit* b, PCommand &cmd);
        void Move(const PUnit* myhero,PCommand &cmd,Pos target);
        void Move(const PUnit* myhero,PCommand &cmd,TargetEstimate* estimate,vector<Pos>& block);
        virtual int StepToReach(const PUnit* hero,Pos target);
	public:
        Strategy(string name = "Strategy"):TaskName(name),num(0),Max(5){}
        virtual void command(PCommand &cmd) = 0;
        void append(const PUnit* myhero);
        void erase(const PUnit* myhero);
        void erase(int i);
        int Max,num;//**********************************
        vector<Pos> TaskPos;//**********************************
        const string TaskName;//******************************************
};
class FightMonsterStrategy:public Strategy{// ???--------------------------------------------------------------------------------------
    protected:
        int id[20];
		int TargetSize;
        int now;
        bool TargetCanNotAttack();
	public:
        FightMonsterStrategy(string name = "FightMonsterStrategy"):Strategy(name){}
		virtual void NextTarget() = 0;
        virtual void command(PCommand &cmd);
};
class FightDragonMonsterStrategy:public FightMonsterStrategy{
	public:
        FightDragonMonsterStrategy(string name = "FightDragonMonsterStrategy");
		virtual void NextTarget();
        virtual void command(PCommand &cmd);
};

class FightNormalMonsterStrategy_0:public FightMonsterStrategy{
	protected:
		int _r;
	public:
        FightNormalMonsterStrategy_0(string name = "FightNormalMonsterStrategy_0");
        virtual void NextTarget();
};
class FightNormalMonsterStrategy_1:public FightMonsterStrategy{
	public:
        FightNormalMonsterStrategy_1(string name = "FightNormalMonsterStrategy_1");
        virtual void NextTarget();
};
class AssembleStrategy:public FightMonsterStrategy{
	protected:
		Pos AssemblePos;
		int TowerNum;
        bool NeedToBack(const PUnit* hero);
        virtual int StepToReach(const PUnit* hero,Pos target);
	public:        
        AssembleStrategy(int n,string name = "AssembleStrategy");
		virtual void NextTarget();
        virtual void command(PCommand &cmd);
};
class PushTower:public Strategy{
	protected:
		int TowerNum;
	public:
        PushTower(int n,string name = "PushTower"):Strategy(name),TowerNum(n){Max = 5;TaskPos.push_back(EnemyTower[n]);}
        virtual void command(PCommand &cmd);
};
class FightStrategy:public Strategy{
public:
    void Stimulate(Operation& op);
    FightStrategy(string name = "FightStrategy"):Strategy(name){}
    int BestPiercingArrow(Operation& op,const PUnit* myhero);
    int BestArrowsRain(Operation& op,const PUnit* myhero);
    int BestAttack(Operation& op,const PUnit* myhero);
    int BestMove(Operation& op,const PUnit* myhero,TargetEstimate* estimate);
    double Firerate(int current,const PUnit* enemy);
    virtual void command(PCommand &cmd);
};
int FightStrategy::BestPiercingArrow(Operation& op,const PUnit* myhero){
    if (!myhero->canUseSkill(Skill("PiercingArrow")))
        return 0;
    int maxvalue = 0;
    int damage = PiercingArrow_damage[myhero->findSkill("PiercingArrow")->level];
    double value;
    Pos bestPos(0,0);
    int BasePiercingArrowValue[12] = {8000,1 W,12000,3000};
    for (int i = -9;i <= 9;i++)
        for (int  j = -9;j <= 9;j++)
            if (i*i + j*j <=81 && i*i + j*j >=25){
                value = 0;//1-2
                for (int e = 0; e < EnemyHero.size();e++)
                    if (StimulateHp[EnemyHero[e]->id] > 0 && PiercingArrow_inRange(myhero->pos,myhero->pos + Pos(i,j),EnemyHero[e]->pos)){
                        value += BasePiercingArrowValue[EnemyHero[e]->typeId] *
                                (0.5 + EnemyHero[e]->max_hp / (MAX(StimulateHp[EnemyHero[e]->id],damage) + EnemyHero[e]->max_hp))
                                *(EnemyCanEscape[e]?0.7:1);
                    }
                if (value > maxvalue){
                    maxvalue = int (value);
                    bestPos = myhero->pos + Pos(i,j);
                }

            }
    if (maxvalue > 0){
        maxvalue = maxvalue - 2400 + 1200 * myhero->findSkill("PiercingArrow")->level;
        op.id = myhero->id;
        op.type = "PiercingArrow";
        op.targets.push_back(bestPos);
    }
    return maxvalue;
    //return myhero->findSkill("PiercingArrow")->level * 20 * maxvalue;

}
int FightStrategy::BestArrowsRain(Operation& op,const PUnit* myhero){
    if (!myhero->canUseSkill(Skill("ArrowsRain")))
        return 0;
    int maxvalue = 0;
    double value;
    int damage = Rainy_damage[myhero->findSkill("ArrowsRain")->level];
    Pos bestPos(0,0);
    double BasePiercingArrowValue[12] = {6000,6000,6000,0};
    for (int i = -8;i <= 8;i++)
        for (int  j = -8;j <= 8;j++)
            if (i*i + j*j <=64){
                value = 0;//1-2
                for (int e = 0; e < EnemyHero.size();e++)
                    if (StimulateHp[EnemyHero[e]->id] > 0 && dis2(myhero->pos + Pos(i,j),EnemyHero[e]->pos) <= 25 && !EnemyIntheRain[e]){
                        if (!EnemyHero[e]->findBuff("Dizzy"))
                            value += BasePiercingArrowValue[EnemyHero[e]->typeId] *
                                    (45 - dis2(myhero->pos + Pos(i,j),EnemyHero[e]->pos)) / 25;
                        else
                            value += BasePiercingArrowValue[EnemyHero[e]->typeId] * 2.5;
                    }
                for (int f = 0; f < OurHero.size();f++){
                    if (!HeroHadBeenProtected[OurHero[f]->id % 5] && dis2(myhero->pos + Pos(i,j),OurHero[f]->pos) <= 9 && OurHeroInDizzy[OurHero[f]->id % 5] > 0)
                        value += BasePiercingArrowValue[OurHero[f]->typeId] * 2.5 * OurHeroInDizzy[OurHero[f]->id % 5];
                }
                if (value > maxvalue){
                    maxvalue = int (value);
                    bestPos = myhero->pos + Pos(i,j);
                }
            }
    if (maxvalue > 0){
        maxvalue = maxvalue - 2400 + 1200 * myhero->findSkill("ArrowsRain")->level;
        op.id = myhero->id;
        op.type = "ArrowsRain";
        op.targets.push_back(bestPos);
    }
    return maxvalue;

}
double FightStrategy::Firerate(int current,const PUnit* enemy){
    double ans = 1;
    if (enemy->hp < myhero->atk - enemy->def)
        ans = Maxint;
    else for (int i = current + 1; i < num ;i++)
        if (INFO->findUnitById(_hero[i])->findSkill("Attack")->cd == 0 && dis2(enemy->pos,INFO->findUnitById(_hero[i])->pos) < INFO->findUnitById(_hero[i])->range)
            ans += 0.5;
    return ans;

}
int FightStrategy::BestAttack(Operation& op,const PUnit* myhero){
    if (!myhero->canUseSkill(Skill("Attack")))
        return 0;
    PUnits near;
    INFO->findUnitInArea(myhero->pos, myhero->range, near);
    //{"Fighter", "Archer", "Assassin", "Tower", "Spring", "Chooser", "Dog", "Bow", "Wolf", "StoneMan", "Dragon", "Roshan"};
    for (int i = 0; i < near.size(); ++i)
        if (near[i]->camp == INFO->camp ||near[i]->findBuff("Reviving") || ((*myhero)["OnlyTarget"]->val[0] >=0 && (*myhero)["OnlyTarget"]->val[0] != near[i]->id)){
            near.erase(near.begin()+i);
            --i;
        }
    int IdinGroup;
    for (int i = 0; i < num;i++)
        if (myhero->id == _hero[i])
           IdinGroup = i;
    int BaseAttackValue[12] = {1 W,1 W,1 W,3000};
    int maxvalue = 0;
    const PUnit* BestTarget;
    double value;
    const PUnit* enemy;
    for (int i = 0; i < near.size(); ++i) if (StimulateHp[near[i]->id] > 0){
        enemy = near[i];//1-2
        value = BaseAttackValue[enemy->typeId] * (1 + 0.05 * (10 - StimulateHp[near[i]->id] / (0.0001+ myhero->atk - enemy->def) / Firerate(IdinGroup,enemy)));
        if (value > maxvalue){
            maxvalue = int (value);
            BestTarget = enemy;
        }
    }
    if (maxvalue > 0){
        op.id = myhero->id;
        op.type = "Attack";
        op.targets.push_back(BestTarget->pos);
    }
    return maxvalue;
    return (myhero->atk - BestTarget->def) * maxvalue;

}
int FightStrategy::BestMove(Operation& op,const PUnit* myhero,TargetEstimate* estimate){
    op.id = myhero->id;
    op.type = "Move";
    return TargetBFS(estimate,myhero,1,op.targets,blocks_without_Tower_Rainy)->getvalue();
}
void FightStrategy::Stimulate(Operation& op){
    if (op.type == "Move")
        OurHeroPos[op.id % 5] = op.targets[op.targets.size() - 1];
    if (op.type == "Attack"){
        const PUnit* enemy = INFO->findUnitByPos(op.targets[op.targets.size() - 1]);// possible bug
        StimulateHp[enemy->id]
                -= (myhero->atk - enemy->def);
    }
    if (op.type == "PiercingArrow"){
        Pos target = op.targets[op.targets.size() - 1];
        for (int i = 0;i < EnemyHero.size();i++)
            if (PiercingArrow_inRange(myhero->pos,target,EnemyHero[i]->pos))
               StimulateHp[EnemyHero[i]->id] -= PiercingArrow_damage[myhero->findSkill("PiercingArrow")->level];
    }
    if (op.type == "ArrowsRain"){
        Pos target = op.targets[op.targets.size() - 1];
        OurArea.push_back(new PArea(0,0,INFO->camp,6,target.x,target.y,5));
        for (int i = 0;i < EnemyHero.size();i++)
            if (dis2(EnemyHero[i]->pos,target) <= 25){
                EnemyIntheRain[i] = true;
                StimulateHp[EnemyHero[i]->id] -= Rainy_damage[myhero->findSkill("ArrowsRain")->level];
            }
    }

}

void FightStrategy::command(PCommand &cmd){
    memset(HeroHadBeenProtected,0,sizeof(HeroHadBeenProtected));
    for (int i = 0; i < num;i++){
        myhero = INFO->findUnitById(_hero[i]);
        for (int f = 0; f < OurHero.size();f++){
            for (int j = 0; j < OurArea.size();j++)
                if (dis2(OurArea[j]->center,OurHero[f]->pos) <= 9)
                    HeroHadBeenProtected[OurHero[f]->id % 5] = true;
        }
        int mind = Maxint,minj = -1;
        for (int j = 0;j < EnemyHero.size();j++) if (dis2(myhero->pos,EnemyHero[j]->pos) < mind){
            mind = dis2(myhero->pos,EnemyHero[j]->pos);
            minj = j;
        }
        if (mind > 400)
            fightingMonster(myhero,EnemyHero[minj],cmd);
        else{
            Pos Battlepos((myhero->pos.x + EnemyHero[minj]->pos.x)/2,(myhero->pos.y + EnemyHero[minj]->pos.y)/2);
            double StrengthRate =StrengthCompare(Battlepos,144);
            int value[5] = {0};Operation op[5];
            switch (myhero->typeId){
            case 0: //fighter
                break;
            case 1: //archer
                fout <<_hero[i] <<"evaluate begin\n";
                value[0] = BestPiercingArrow(op[0],myhero);
                fout << "PiercingArrow:" << value[0] << endl;// <<op[0].targets[op[0].targets.size()];
                value[1] = BestArrowsRain(op[1],myhero);
                fout << "ArrowsRain:" << value[1] << endl;// <<op[1].targets[op[1].targets.size()];
                value[2] = 0;
                value[3] = BestAttack(op[3],myhero);
                fout << "Attack:" << value[3] << endl;
                fout << StrengthRate << endl;
                if (StrengthRate <= 2 && StrengthRate >= 0.5)
                    value[4] = BestMove(op[4],myhero,new FightingMoveEvaluation(myhero));
                if (StrengthRate > 2)
                    value[4] = BestMove(op[4],myhero,new RunAfterWeakEnemy(myhero));
                if (StrengthRate < 0.5)
                    value[4] = BestMove(op[4],myhero,new RunAwayFromPowerfulEnemy(myhero));
                fout << "Move:" << value[4] << endl;//  <<op[3].targets[op[3].targets.size()];
                break;
            case 2: // Assassin
                break;
            }
            if (myhero->findBuff("Yelled")){
                value[0] = 0;
                value[1] = 0;
                value[2] = 0;
            }
            if (OurHeroInDizzy[myhero->id % 5] > 0){
                value[4] = int(value[3] * (1 - OurHeroInDizzy[myhero->id % 5]));
                value[3] = int(value[2] * (1 - 0.5 * OurHeroInDizzy[myhero->id % 5]));
            }
            int maxvalue = 0,maxj = 4;
            for (int j = 0; j < 5;j++)
                if (value[j] > maxvalue){
                    maxvalue = value[j];
                    maxj = j;
                }
            fout << maxj << endl;
            cmd.cmds.push_back(op[maxj]);
            Stimulate(op[maxj]);
        }
    }
}
int Strategy::StepToReach(const PUnit* hero,Pos target){
    double speed = sqrt((*hero)["Speed"]->val[1]) - 0.5;
    double range = sqrt(hero->range);
    return int((dis(hero->pos,target) - range) / speed) + 1;

}
void Strategy::Move(const PUnit* myhero,PCommand &cmd,Pos target){
    INFO->findUnitInArea(myhero->pos, myhero->view, nearBy);
    for (int i = 0; i < nearBy.size(); ++i)
        if (nearBy[i]->id > 38 || nearBy[i]->id < 16 || dis2(target,nearBy[i]->pos) <= 8)
        {
            nearBy.erase(nearBy.begin()+i);
            --i;
        }
    for (int i = 0; i < nearBy.size(); ++i){
        bool herooutit = true;
        for (int j =0; j <OurHero.size();j++)
            if (dis2(nearBy[i]->pos,OurHero[j]->pos) <= 8)
                herooutit = false;
        if (herooutit) {
            fout << nearBy[i]->id << "****************************\n";
            for (int x = -2;x <= 2;x++)
                for (int y = -2;y <= 2;y++)
                    if (x*x+y*y<=8){
                        blocks.push_back(nearBy[i]->pos + Pos(x,y));
                        blocks_without_Tower.push_back(nearBy[i]->pos + Pos(x,y));
                        blocks_without_Rainy.push_back(nearBy[i]->pos + Pos(x,y));
                        blocks_without_Tower_Rainy.push_back(nearBy[i]->pos + Pos(x,y));
                    }
        }
    }
    bool TowerBLock = true;
    for (int i = 0; i < 2;i++)
        if (!towerDes[i] && (dis2(myhero->pos,EnemyTower[i]) < 144 || dis2(target,EnemyTower[i]) < 144))
            TowerBLock = false;
    Operation op;
    op.id = myhero->id;
    op.type = "Move";
    if (OurHeroIntheRain[myhero->id % 5]){
        if (TowerBLock)
            TargetBFS(new MoveToTarget(target,myhero),myhero,1,op.targets,blocks_without_Rainy);
        else
            TargetBFS(new MoveToTarget(target,myhero),myhero,1,op.targets,blocks_without_Tower_Rainy);

    }
    else{
        if (TowerBLock)
            SelfShortPath.findShortestPath(*MAP, myhero->pos, target, blocks, op.targets);
        else
            SelfShortPath.findShortestPath(*MAP, myhero->pos, target, blocks_without_Tower, op.targets);
    }
    if (op.targets[0] == Pos(-1,-1))
        SelfShortPath.findShortestPath(*MAP, myhero->pos, target, basic_blocks, op.targets);
    cmd.cmds.push_back(op);
}
void Strategy::Move(const PUnit* myhero,PCommand &cmd,TargetEstimate* estimate,vector<Pos>& block){
    Operation op;
    op.id = myhero->id;
    op.type = "Move";    
    TargetBFS(estimate,myhero,1,op.targets,block);
    cmd.cmds.push_back(op);
}
void Strategy::append(const PUnit* myhero){
   _hero[num++] = myhero->id;
   HeroTask[myhero->id] = this;
}
void Strategy::erase(int i){
    HeroTask[_hero[i]] = 0;//********************
    for (int j = i;j < num - 1;j++){
        _hero[j] = _hero[j + 1];
    }
    num --;
}
void Strategy::erase(const PUnit* myhero){
    for (int i = 0;i < num;i++)
        if (_hero[i] == myhero->id){
            erase(i);
            return;
        }
}
void Strategy::fightingMonster(const PUnit* a, const PUnit* b, PCommand &cmd) //??��a???��b????????
{
    if (b->id >= 16 && b->id <= 36){
        MonsterPlayer = b->player;
    }
    Operation op;
    op.id = a->id;

    vector<const PSkill*> useSkill;
    for (int i = 0; i < a->skills.size(); ++i)
    {
        const PSkill* ptr = &a->skills[i];
        if (ptr->isHeroSkill() && a->canUseSkill(ptr->typeId))
        {
            PUnits ptr_foe;
            infectedBySkill(*INFO, a->id, ptr->typeId, ptr_foe); //?????��???
            if ((belongs(b->id, ptr_foe) && (ptr->maxCd <= 100 || b->isHero()))|| !strcmp(ptr->name,"Hide") || !strcmp(ptr->name,"PowerUp"))
                useSkill.push_back(ptr);
        }
    }
    if (b->typeId == 6 || b->typeId == 7 || (b->typeId == 8 || b->typeId == 3)) useSkill.clear();
    if (useSkill.size() && (b->isHero() || (a->mp > 150))) //???????????��???
    {
        const PSkill* ptr = useSkill[rand()%useSkill.size()];
        op.type = ptr->name;
        if (ptr->needTarget())
            op.targets.push_back(b->pos);
        cmd.cmds.push_back(op);
    } else
        if (dis2(a->pos, b->pos) <= a->range) //?��??��b??????????��??
        {
            if (a->findSkill("Attack")->cd == 1 && b->range < 64 && (b->atk - a->def) * 30 > a->max_hp){
                int max = 0;
                for (int i = 0; i < OurHero.size();i++)
                    if (dis2(OurHero[i]->pos,MonsterBornPos[b->id]) <= 324  && ang[b->id][OurHero[i]->id] > max) max = ang[b->id][OurHero[i]->id];
                if (ang[b->id][a->id] == max && !b->isHero()){
                    Move(a,cmd,new FarAwayFromTarget(b->pos,a),PushingTower <= 0?blocks_without_Rainy:blocks_without_Tower_Rainy);
                 }
                else if (ang[b->id][a->id] < max - 1)
                    Move(a,cmd,b->pos);
            }
            else {
                op.type = "Attack";
                op.targets.push_back(b->pos);
                cmd.cmds.push_back(op);
            }
        } else
        {
            Move(myhero,cmd,b->pos);
        }
}
bool Strategy::FightMonster(PCommand &cmd,const PUnit* ptr){
    if ((*ptr)["Exp"]->val[3]) //????????????
    {
        levelUp(ptr, cmd);
        return true;
    }
    if (ptr->findBuff("Reviving")) {
        return false;
    }
    INFO->findUnitInArea(ptr->pos, ptr->view, nearBy); //??????????
    fixPos[ptr->id] = EMPTYPOS;
    for (int i = 0; i < nearBy.size(); ++i)
    {
        if (unitEvaluation(nearBy[i]->name) * nearBy[i]->hp > nearBy[i]->max_hp * StrengthCaculate(nearBy[i]->pos,901,INFO->camp) ||
                nearBy[i]->camp == ptr->camp ||
                nearBy[i]->pos == Bow_pos[0] || nearBy[i]->pos == Bow_pos[3] ||
                nearBy[i]->findBuff("Reviving"))
            //????????????????????????????????????????????????
        {
            nearBy.erase(nearBy.begin()+i);
            --i;
        }

    }
    if (nearBy.size()) //??????????????��?????????��???
    {
        const PUnit* ptr_foe = fixId[ptr->id]!=EMPTYID ?
                    belongs(fixId[ptr->id], nearBy) : NULL; //?��???????????????
        if (!ptr_foe)
        {
            ptr_foe = nearBy[rand()%nearBy.size()];
            fixId[ptr->id] = ptr_foe->id; //????��???????????????????
        }
        fightingMonster(ptr, ptr_foe, cmd); //?????????
    } else
    {
        //findFoes(ptr, cmd); //????????�֧�???????????????
        return false;
    }
    return true;
}

void PushTower::command(PCommand &cmd){
	if (towerDes[TowerNum]){
        int id = Taskid();
        Task[id] = new AssembleStrategy(1 - TowerNum);
        for (int i = 0;i < num;i++) Task[id]->append(INFO->findUnitById(_hero[i]));
        Task[id]->command(cmd);
		PushingTower = 0;
		return;
	}
	else{
		bool stillFighting = false;
        for (int i = 0;i < num;i++){
            myhero = INFO->findUnitById(_hero[i]);
            if (dis2(myhero->pos,EnemyTower[TowerNum]) <= 3000){
                stillFighting = true;
                break;
            }
        }
        if (!stillFighting){
            int id = Taskid();
            Task[id] = new AssembleStrategy(TowerNum);
            for (int i = 0;i < num;i++) Task[id]->append(INFO->findUnitById(_hero[i]));
            Task[id]->command(cmd);
			PushingTower = 0;
			return;
		}
    }
    //------
    for (int i = 0;i < EnemyHero.size();i++)
        if (dis2(EnemyHero[i]->pos,EnemyTower[TowerNum]) <= 625 && !(INFO->findUnitByPos(EnemyTower[TowerNum]) && INFO->findUnitByPos(EnemyTower[TowerNum])->hp <= 200)){
            for (int j = 0;j < num;j++){
                myhero = INFO->findUnitById(_hero[j]);
                Move(myhero,cmd,new PushingDispersed(myhero),blocks_without_Tower_Rainy);
            }
            for (;num;){
                erase(0);
            }
            PushingTower = 0;
            Task[Taskid()] = new AssembleStrategy(TowerNum);
            return;

        }

    for (int i = 0;i < num;i++){
        myhero = INFO->findUnitById(_hero[i]);
        if ((dis2(myhero->pos,EnemyTower[TowerNum]) <= 81 && myhero->findSkill("Attack")->cd == 1)
                || (dis2(myhero->pos,EnemyTower[TowerNum]) <= 121 && dis2(myhero->pos,EnemyTower[TowerNum]) > 81)){
           TargetEstimate* estimate = new PushingTowerMove(myhero);
           Move(myhero,cmd,estimate,blocks_without_Tower_Rainy);
           OurHeroPos[myhero->id % 5] = estimate->getvalue();
        }
        else {
            if (!FightMonster(cmd,myhero)){
                Move(myhero,cmd,EnemyTower[TowerNum]);
            }
        }
    }
}
void AssembleStrategy::NextTarget(){
	now++;
	if (now >= TargetSize) now = now % TargetSize;
}
bool AssembleStrategy::NeedToBack(const PUnit *hero){
    return (50 - dis(hero->pos,SpringPos) / 4) * hero->max_hp > hero->hp * 100;
}

int AssembleStrategy::StepToReach(const PUnit* hero,Pos target){
    if (NeedToBack(hero))
        return Maxint;
    double speed = sqrt((*hero)["Speed"]->val[1]) - 0.5;
    double range = sqrt(hero->range);
    return int((dis(hero->pos,target) - range) / speed) + 1;

}
void AssembleStrategy::command(PCommand &cmd){
    if (towerDes[TowerNum] && !towerDes[1 - TowerNum]){
        int id = Taskid();
        Task[id] = new AssembleStrategy(1 - TowerNum);
        for (int i = 0;i < num;i++) Task[id]->append(INFO->findUnitById(_hero[i]));
        Task[id]->command(cmd);
		PushingTower = 0;
		return;
    }
    if (!towerDes[1 - TowerNum]){
        int totalHero = 0;
        int UnfitHero = 0;
        int UnfitLevel = 0;
        for (int i = 0; i < OurHero.size();i++) if (!OurHero[i]->findBuff("Reviving")){
            totalHero++;
            if (dis2(OurHero[i]->pos,EnemyTower[1 - TowerNum]) < dis2(OurHero[i]->pos,EnemyTower[TowerNum])){
                UnfitHero++;
                UnfitLevel += (OurHero[i]->level + 2);
            }
        }
        if (UnfitHero * 2 > totalHero && UnfitLevel > PushingLevel)
            TowerNum = 1 - TowerNum;
    }
    vector<Pos> steps;
    for (int i = 0;i < num;i++){
        steps.push_back(Pos(_hero[i],StepToReach(INFO->findUnitById(_hero[i]),EnemyTower[TowerNum])));
    }
    sort(steps.begin(),steps.end());
    int TotalLevel = 0;
    int FarestHero = steps.size() - 1;
    bool CanPush = false;
    for (int i = 0;i < steps.size();i++){
        myhero = INFO->findUnitById(steps[i].x);
        TotalLevel += (myhero->level + 2);
        if (TotalLevel >= PushingLevel){
            CanPush = true;
            FarestHero = i;
             if (!(i+1 < steps.size() && steps[i + 1].y - steps[i].y <= 3))
                break;
        }
    }
    //fout << "stepfarest" <<  steps[FarestHero].y << endl;
    if (CanPush && steps[FarestHero].y < 5){
        PushingTower = 1;
        int id = Taskid();
        Task[id] = new PushTower(TowerNum);
        for (int i = 0;i < num;i++) Task[id]->append(INFO->findUnitById(_hero[i]));
        Task[id]->command(cmd);
        return;
    }
    if (OurTowerDes[1] && id[0] > 24){
        id[0] = 16 + 6 - 4 * INFO->camp;
        id[1] = 24 + 2 + 3 * INFO->camp;
        id[2] = 30 + 3 - 2 * INFO->camp;
        TargetSize = 3;
        now = 0;
    }
    if (TargetCanNotAttack()){
        NextTarget();
    }

    for (int i = 0;i < steps.size();i++){
        myhero = INFO->findUnitById(steps[i].x);
        if (NeedToBack(myhero))
            Move(myhero,cmd,SpringPos);
        else if (!CanPush || steps[i].y < steps[FarestHero].y - 1) {
            if (!FightMonster(cmd,myhero)){
                Move(myhero,cmd,MonsterBornPos[id[now]]);
                /*int Bestid = 0;
                double minstep = Maxint;
                for (int j = 16;j < 34;j++)
                    if (j!= 25 && j != 28 && dis2(MonsterBornPos[j],EnemyTower[TowerNum]) > 900 && MIN(reviving[j],StepToReach(myhero,MonsterBornPos[j])) < minstep){
                        minstep = MIN(reviving[j],StepToReach(myhero,MonsterBornPos[j]));
                        Bestid = j;
                    }
                fout <<"AssembleFightingMonster"<< myhero->id<<' ' <<  Bestid << endl;
                if (Bestid)
                    Move(myhero,cmd,MonsterBornPos[Bestid]);
                else
                    fout << "???????????" << myhero->id << ' ' << Bestid << endl;*/
            }
        }
        else
            Move(myhero,cmd,EnemyTower[TowerNum]);

    }
}
AssembleStrategy::AssembleStrategy(int n,string name):FightMonsterStrategy(name){
    Max = 5;
    TowerNum = n;
    /*if (TowerNum){
        id[0] = 30 + 0 + 2 * INFO->camp;
        id[1] = 16 + 1 + 4 * INFO->camp;
        id[2] = 16 + 0 + 4 * INFO->camp;
        TargetSize = 3;
        now = 0;
    }
    else {
        id[0] = 30 + 0 + 2 * INFO->camp;
        id[1] = 16 + 1 + 4 * INFO->camp;
        id[2] = 16 + 3 + 4 * INFO->camp;
        TargetSize = 3;
        now = 0;
    }*/
    if (OurTowerDes[1]){
        id[0] = 16 + 6 - 4 * INFO->camp;
        id[1] = 24 + 2 + 3 * INFO->camp;
        id[2] = 30 + 3 - 2 * INFO->camp;
        TargetSize = 3;
        now = 0;
    }
    else{
        id[0] = 30 + 2 - 2 * INFO->camp;
        id[1] = 16 + 5 - 4 * INFO->camp;
        id[2] = 16 + 4 - 4 * INFO->camp;
        TargetSize = 3;
        now = 0;
    }
    TaskPos.clear();
    for (int i = 0;i < TargetSize;i++)
        TaskPos.push_back(MonsterBornPos[id[i]]);
}
FightDragonMonsterStrategy::FightDragonMonsterStrategy(string name):FightMonsterStrategy(name){
    Max = 5;
    id[0] = 24 + 2 + 3 * INFO->camp;
    id[1] = 16 + 7 - 4 * INFO->camp;
    id[2] = 30 + 2 - 2 * INFO->camp;
    id[3] = 16 + 5 - 4 * INFO->camp;
    id[4] = 36 + 1 - 1 * INFO->camp;
    TargetSize = 5;
    TaskPos.clear();
    for (int i = 0;i < TargetSize;i++)
        TaskPos.push_back(MonsterBornPos[id[i]]);
	now = 0;
}
void FightDragonMonsterStrategy::NextTarget(){
	now++;
}
FightNormalMonsterStrategy_0::FightNormalMonsterStrategy_0(string name):FightMonsterStrategy(name){
    Max = 3;
    id[0] = 30 + 2 - 2 * INFO->camp;
    id[1] = 16 + 5 - 4 * INFO->camp;
    id[2] = 16 + 4 - 4 * INFO->camp;
    id[3] = 36 + 1 - 1 * INFO->camp;
    TargetSize = 4;
    TaskPos.clear();
    for (int i = 0;i < TargetSize;i++)
        TaskPos.push_back(MonsterBornPos[id[i]]);
	now = 0;
	_r = 0;
}
FightNormalMonsterStrategy_1::FightNormalMonsterStrategy_1(string name):FightMonsterStrategy(name){
    Max = 2;
    id[0] = 16 + 6 - 4 * INFO->camp;
    id[1] = 24 + 2 + 3 * INFO->camp;
    id[2] = 30 + 3 - 2 * INFO->camp;
    TargetSize = 3;
    TaskPos.clear();
    for (int i = 0;i < TargetSize;i++)
        TaskPos.push_back(MonsterBornPos[id[i]]);
    now = 0;
}
void FightNormalMonsterStrategy_0::NextTarget(){
    /*now++;
    if (now == TargetSize - 1){
		_r++;
        if (_r % 2)
			now = 0;		
	}
    if (now > 3) now = now % 4;*/
    now++;
    if (now >= TargetSize) now = now % TargetSize;
}
void FightNormalMonsterStrategy_1::NextTarget(){
	now++;
	if (now >= TargetSize) now = now % TargetSize;
}
void FightDragonMonsterStrategy::command(PCommand &cmd){
    FightMonsterStrategy::command(cmd);
    if (now >= TargetSize){
        int id = Taskid();
        Task[id] = new FightNormalMonsterStrategy_0();
        Task[TotalTask++] = new FightNormalMonsterStrategy_1();
        for (int i = 0; i < 3 && i < num;i++){
            Task[id]->append(INFO->findUnitById(_hero[i]));
        }
        for (int i = 3; i < num;i++){
            Task[TotalTask - 1]->append(INFO->findUnitById(_hero[i]));
        }
        Task[id]->command(cmd);
        return;
    }
}
bool FightMonsterStrategy::TargetCanNotAttack(){
    const PUnit* t= INFO->findUnitById(id[now]);
    int minstep = Maxint;
    for (int i = 0; i < num;i++){
        myhero = INFO->findUnitById(_hero[i]);
        if (StepToReach(myhero,MonsterBornPos[id[now]]) < minstep)
            minstep = StepToReach(myhero,MonsterBornPos[id[now]]);
    }
    return reviving[id[now]] > 3 + minstep|| (t && t->max_hp * StrengthCaculate(t->pos,901,INFO->camp) < t->hp * unitEvaluation(t->name));
}

void FightMonsterStrategy::command(PCommand &cmd){
    if (TargetCanNotAttack()){
		NextTarget();
	}
    if (now >= TargetSize){
		return;
	}
    for (int i = 0; i < num;i++){
        myhero = INFO->findUnitById(_hero[i]);
        if (!FightMonster(cmd,myhero)){
            Move(myhero,cmd,MonsterBornPos[id[now]]);
        }
    }
}
//---------------------------------------------------------------------------------------------------------------advicer.cc
class Advicer{
public:
    virtual bool advice() = 0;
    int Advicersid(){for (int i = 0; i < TotalAdvicer;i++) if (this == Advicers[i]) return i; return -1;}
};
class FirstPushAdvicer: public Advicer{
public:
    virtual bool advice();    
};
class FightAdvicer: public Advicer{
protected:
    bool NeedToFight(Pos,Pos);
public:
    virtual bool advice();
};
class FindTaskAdvicer: public Advicer{
public:
    virtual bool advice();
};
bool FindTaskAdvicer::advice(){
    for (int i = 0; i < OurHero.size();i++)
        if (!OurHero[i]->findBuff("Reviving") && !HeroTask[OurHero[i]->id]){
            int MinDistance = Maxint,minj = -1;
            for (int j = 1; j < TotalTask;j++) if (Task[j] && Task[j]->num < Task[j]->Max){
                for (int k = 0; k < Task[j]->TaskPos.size();k++)
                    if (dis2(Task[j]->TaskPos[k],OurHero[i]->pos) < MinDistance){
                        MinDistance = dis2(Task[j]->TaskPos[k],OurHero[i]->pos);
                        minj = j;
                    }
            }
            if (minj != -1) Task[minj]->append(OurHero[i]);
        }
}
bool FightAdvicer::NeedToFight(Pos MyPos,Pos EnemyPos){
    return dis2(MyPos,EnemyPos) <= ReinforceDistance
            || (dis2(MyPos,EnemyPos) <= DefendTowerDistance &&
                (dis2(FriendlyTower[0],EnemyPos) < 144
                 || dis2(FriendlyTower[1],EnemyPos) < 144));
}
bool FightAdvicer::advice(){
    if (EnemyHero.size() == 0){
        if (Task[0]->num > 0)
            for (int i = 0; i < Task[0]->num;i++){
                Task[0]->erase(i);
                i--;
            }
        return false;
    }
    bool needfight;
    for (int i = 0; i < OurHero.size();i++) {
        needfight = false;
        for (int j = 0; j < EnemyHero.size();j++)
            if (NeedToFight(OurHero[i]->pos,EnemyHero[j]->pos)){
                needfight = true;
                break;
            }
        if (needfight != (HeroTask[OurHero[i]->id] && HeroTask[OurHero[i]->id]->TaskName == "FightStrategy") ){
            if (needfight){
                if (!HeroTask[OurHero[i]->id] || HeroTask[OurHero[i]->id]->TaskName != "PushTower" || dis2(HeroTask[OurHero[i]->id]->TaskPos[0],OurHero[i]->pos) > 100){
                    if (HeroTask[OurHero[i]->id])HeroTask[OurHero[i]->id]->erase(OurHero[i]);
                    Task[0]->append(OurHero[i]);
                }
            }
            else
                Task[0]->erase(OurHero[i]);
        }

    }
    return true;
}
bool FirstPushAdvicer::advice(){
    int totalLevel = 0;
    for (int i = 0;i < OurHero.size();i++) if (HeroTask[OurHero[i]->id] && HeroTask[OurHero[i]->id]->TaskName.find("Monster") != string::npos){
        totalLevel += (OurHero[i]->level + 2);
    }
    if (totalLevel >= PushingLevel){
        Task[TotalTask++] = new AssembleStrategy(0);
        for (int i = 0;i < OurHero.size();i++) if (HeroTask[OurHero[i]->id] && HeroTask[OurHero[i]->id]->TaskName.find("Monster") != string::npos){
            HeroTask[OurHero[i]->id]->erase(OurHero[i]);
            Task[TotalTask - 1]->append(OurHero[i]);
        }
        PushingTower = 0;
        Advicers[Advicersid()] = 0;
        for (int i = 1; i < TotalTask - 1;i++) if (Task[i]->TaskName.find("Monster") != string::npos){
            Task[i] = 0;
        }
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------main.cc
void init() //?��?????????
{
    ofstream f("newout.txt");        f << "";        f.close();
    blocks.clear();
    const Pos* tower = INFO->camp ? Player1_tower_pos : Player0_tower_pos;
    for (int i = 0; i < 2; ++i)
        blocks.push_back(tower[i]); //??????????????
    memset(fixId, EMPTYID, sizeof fixId);
    towerDes[0] = towerDes[1] = false;
    SpringPos = INFO->camp ? Pos(145,5):Pos(5,145);
    for (int i = 0; i < MAXID; ++i) fixPos[i] = EMPTYPOS;
    for (int i = 0;i < 8;i++) MonsterBornPos[i + 16] = Dog_pos[i];
    for (int i = 0;i < 6;i++) MonsterBornPos[i + 24] = Bow_pos[i];
    for (int i = 0;i < 4;i++) MonsterBornPos[i + 30] = Wolf_pos[i];
    for (int i = 0;i < 2;i++) MonsterBornPos[i + 34] = StoneMan_pos[i];
    for (int i = 0;i < 2;i++) MonsterBornPos[i + 36] = Dragon_pos[i];
    for (int i = 0;i < 1;i++) MonsterBornPos[i + 38] = Roshan_pos[i];
    TotalAdvicer = 3;
    Advicers[0] = new FightAdvicer;
    Advicers[1] = new FindTaskAdvicer;
    Advicers[2] = new FirstPushAdvicer;
}
void prepared(){
    OurHero.clear();
    EnemyHero.clear();
    for (int i = 16; i < 50;i++) if (reviving[i]){
        reviving[i]--;
    }
    for (int i = 0; i < INFO->units.size();i++){
        if (INFO->units[i].id >= 16 && INFO->units[i].id <= 48){
            StimulateHp[INFO->units[i].id] = INFO->units[i].hp;
            StimulatePos[INFO->units[i].id] = INFO->units[i].pos;
            if (INFO->units[i].findBuff("Reviving"))
                reviving[INFO->units[i].id] = INFO->units[i].findBuff("Reviving")->timeLeft;
            else
                reviving[INFO->units[i].id] = 0;
            LastRoundsGetInfo[INFO->units[i].id] = INFO->round;
        }
    }
    for (int i = 0; i < 2;i++){
        if (INFO->findUnitByPos(FriendlyTower[i]) && INFO->findUnitByPos(FriendlyTower[i])->typeId == 3)
            OurTowerDes[i] = false;
        else
            OurTowerDes[i] = true;
    }
    INFO->findUnitByPlayer(INFO->player, ours); //??????��?????��
    INFO->findAreaByCamp(INFO->camp,OurArea);
    INFO->findAreaByCamp(1 - INFO->camp,EnemyArea);
    for (int i=0; i<ours.size(); ++i) //???��?????��
    {
        const PUnit* ptr = ours[i];
        if (ptr->isHero()){
            OurHero.push_back(ptr);
            OurHeroPos[ptr->id % 5] = ptr->pos;
            for (int i = 0; i < 2; ++i)
                if (dis2(ptr->pos, EnemyTower[i]) < 100 && !INFO->findUnitByPos(EnemyTower[i]) &&
                        MAP->height[ptr->pos.x][ptr->pos.y] + 2 > MAP->height[EnemyTower[i].x][EnemyTower[i].y])
                    towerDes[i] = true; //????????????
        }
    }
    for (int i = 0; i < INFO->units.size();++i){
        if (INFO->units[i].isHero() && INFO->player != INFO->units[i].player){
            EnemyHero.push_back(&INFO->units[i]);
        }
    }
    for (int i = 0;i < EnemyHero.size();i++)
        EnemyCanEscape[i] = (EnemyHero[i]->canUseSkill(Skill("Charge")) || EnemyHero[i]->canUseSkill(Skill("Blink")));

    if (INFO->round == 1){
        TotalTask = 2;
        Task[0] = new FightStrategy;
        Task[1] = new FightDragonMonsterStrategy;
        for (int i=0; i<OurHero.size(); ++i) Task[1]->append(OurHero[i]);
    }
    calAng();
    for (int i = 0;i < EnemyHero.size();i++){
        EnemyIntheRain[i] = false;
        for (int j = 0;j < OurArea.size();j++)
            if (dis(EnemyHero[i]->pos,OurArea[j]->center) <= OurArea[j]->radius)
                EnemyIntheRain[i] = true;
    }
    for (int i = 0;i < OurHero.size();i++){
        OurHeroIntheRain[OurHero[i]->id % 5] = false;
        OurHeroInDizzy[OurHero[i]->id % 5] = OurHero[i]->findBuff("Dizzy")?1:0;
        for (int j = 0;j < EnemyArea.size();j++)
            if (dis(OurHero[i]->pos,EnemyArea[j]->center) <= EnemyArea[j]->radius)
                OurHeroIntheRain[OurHero[i]->id % 5] = true;
    }
    for (int i = 0;i < EnemyHero.size();i++)
        if (EnemyHero[i]->canUseSkill(Skill("Charge"))){
            int targetnum = 0;
            for (int j = 0;j < OurHero.size();j++)
                if (dis2(EnemyHero[i]->pos,OurHero[j]->pos) <= 81)
                    targetnum++;
            for (int j = 0;j < OurHero.size();j++)
                if (dis2(EnemyHero[i]->pos,OurHero[j]->pos) <= 81)
                    if (OurHeroInDizzy[OurHero[j]->id % 5] < 1){
                        OurHeroInDizzy[OurHero[j]->id % 5] += 1.00/targetnum;
                        if (OurHeroInDizzy[OurHero[j]->id % 5] > 1)
                            OurHeroInDizzy[OurHero[j]->id % 5] = 1;

                    }
        }

    GetBlocks();
}

void player_ai(const PMap &map, const PPlayerInfo &info, PCommand &cmd)
{
    srand(time(0)); //???????????
    cmd.cmds.clear();
    MAP = &map;
    INFO = &info;
    fout << "--------------------------------------------------------Round:" << INFO->round << endl;
    if (info.round == 0){
        chooseHero(info, cmd); //??0?????????
        init(); //?��?????????
        EnemyTower = INFO->camp ? Player0_tower_pos : Player1_tower_pos;
        FriendlyTower = INFO->camp ? Player1_tower_pos : Player0_tower_pos;
    }
    else{
        prepared();
        for (int i = 0; i < TotalAdvicer;i++) if (Advicers[i]){
            if (Advicers[i]->advice()){
                switch (i){
                    case 0 :
                        cout << Task[0]->TaskName << endl;
                        Task[0]->command(cmd);                        
                        break;
                }
            }
        }
        for (int i = 1; i < TotalTask;i++) if (Task[i]){
            cout << Task[i]->TaskName << endl;
            fout << Task[i]->TaskName << endl;
            //cout << EnemyHero.size() << endl;
            if (Task[i]->num > 0)
                Task[i]->command(cmd);
            /*else
                Task[i] = 0;*/
        }
        for (int i = 0; i < 5;i++) if (HeroTask[OurHero[i]->id]) {
            fout <<OurHero[i]->id << ' '<< HeroTask[OurHero[i]->id]->TaskName << endl;
        }
        cout << "done\n";

    }
}
