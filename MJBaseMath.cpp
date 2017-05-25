#include "MJBaseMath.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>


const int SCMJ_HU_FANS[] = {0,6,6,6,6,5,5,5,5,5,4,4,3,3,3,3,2,1};

int JudgeHu(list<CMjCard> &handCard, list < CMjCardPile > & pCardList,ResultTypeDef & resultType)
{
	int handCards[3][10] = {0};
	for (list<CMjCard>::iterator it = handCard.begin(); it != handCard.end(); ++it) {
		++handCards[it->Type()][it->Value()];
		++handCards[it->Type()][0];
	}

	return JudgeHu(handCards, pCardList, resultType);
}

//ljt 
//note: handCards -- only in hand
//note: pCardList -- cards peng gang in mj game
//note: resultType -- include quemen, lastCard 
int JudgeHu(int handCards[3][10], std::list<CMjCardPile> &pCardList, ResultTypeDef &resultType)
{
	int allPai[3][10] = {0};

 	for (int i = 0; i != 3; ++i){
		for (int j = 1; j != 10; ++j) {
			allPai[i][j] = handCards[i][j];
			allPai[i][0] += handCards[i][j];
		}
	}
	if (resultType.lastCard.iValue && resultType.lastCard.bOther) { //是其他人的牌才加入，是自己的牌已经在摸牌的时候加入了
		++allPai[resultType.lastCard.iType][resultType.lastCard.iValue];
		++allPai[resultType.lastCard.iType][0];
	}

	if (allPai[resultType.cQueMen][0] != 0)
		return 0;
	if ((allPai[0][0]+allPai[1][0]+allPai[2][0])%3 != 2)
		return 0;
	
	for (std::list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
		if (it->GetPileCard(0).Type() == resultType.cQueMen)
			return 0;
	}

	if (JudgeSpecialHu(allPai, pCardList))
		return 2; //七对胡
		
	if (pCardList.size() <= 4) {
		#if 0
		for (std::list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
			allPai[it->GetPileCard(0).Type()][it->GetPileCard(0).Value()] += 3; //代码中可见是这样用的，所以这里也这样写
			allPai[it->GetPileCard(0).Type()][0] += 3;
		}
		#endif
		if (JudgeNormalHu(allPai, resultType))
			return 1;  //一般的胡
		else
			return 0;
		
	} else {
		printf("pCardList.size is too big %d\n", pCardList.size());
		return 0;
	}
	
	return 0;
}

//ljt note as above
//judge 7dui 
bool JudgeSpecialHu(int handCards[3][10], std::list<CMjCardPile> &pCardList)
{
	//判断手牌是否是七对，不能有碰杠
	if (0 != pCardList.size())
		return false;
	if (handCards[0][0]+handCards[1][0]+handCards[2][0] != 14)
		return false;

	int duiNum = 0;
	for (int i = 0; i != 3; ++i)
		for (int j = 1; j != 10; ++j) {
			if (handCards[i][j]) {
				if (handCards[i][j]%2 == 0) {
					if (2 == handCards[i][j]) {
						++duiNum;
					} else if (4 == handCards[i][j]) {
						duiNum += 2;
					}
				} else {
					return false;
				}
			}
		}
	return (7 == duiNum);
}

bool dai19(int allPai[3][10])
{
	for (int i = 0; i != 3; ++i) {
		if (0 != allPai[i][4] || 0 != allPai[i][5] || 0 != allPai[i][6])
			return false;
	}

	bool bHaveJiang = false;
	for (int i = 0; i != 3; ++i) {
		if (allPai[i][1] < allPai[i][2])
			return false;
		if (allPai[i][2] != allPai[i][3])
			return false;
		if (allPai[i][1] > allPai[i][2]) {
			if (allPai[i][1] - allPai[i][2] == 1) {
				return false;
			}
			if (allPai[i][1] - allPai[i][2] == 2) {
				if (bHaveJiang)
					return false;
				else
					bHaveJiang = true;
			}
		}

		if (allPai[i][9] < allPai[i][8])
			return false;
		if (allPai[i][8] != allPai[i][7])
			return false;
		if (allPai[i][9] > allPai[i][8]) {
			if (allPai[i][9] - allPai[i][7] == 1)
				return false;
			if (allPai[i][9] - allPai[i][7] == 2) {
				if (bHaveJiang)
					return false;
				else
					bHaveJiang = true;
			}
		}
	}
	if (bHaveJiang)
		return true;
	else
		return false;
}

bool Qing1Se(int allPai[3][10]) 
{
	if (allPai[0][0] != 0) {
		return (allPai[1][0] == 0 && allPai[2][0] == 0);
	}
	if (allPai[1][0] != 0) {
		return (allPai[0][0] == 0 && allPai[2][0] == 0);
	}
	if (allPai[2][0] != 0) {
		return (allPai[0][0] == 0 && allPai[1][0] == 0);
	}
}


bool Qing19(int allPai[3][10])
{
	return Qing1Se(allPai) && dai19(allPai);
}

bool duiduihu(int allPai[3][10])
{
	int _3Count = 0;
	bool bJiang = false;
	for (int i = 0; i != 3; ++i) {
		for (int j = 1; j != 10; ++j) {
			if (allPai[i][j] != 0) {
				if (2 == allPai[i][j]) {
					if (bJiang)
						return false;
					else 
						bJiang = true;
				} else if (3 == allPai[i][j]) {
					++_3Count;
				} else
					return false;
			}
		}
	}
	
	if (4 == _3Count)
		return true;
	return false;
}

bool QingDui(int allPai[3][10])
{
	return duiduihu(allPai) && Qing1Se(allPai);
}

bool JiangDui(int allPai[3][10])
{
	if (duiduihu(allPai)) {
		for (int i = 0; i != 3; ++i)
			for (int j = 1; j != 10; ++j) {
				if (allPai[i][j]) {
					if (!(2 == j || 5 == j || 8 == j))
						return false;
				}
			}
		return true;
	} else
		return false;
}

//判断是否胡牌的函数
bool JudgeNormalHu(int allPai[3][10],ResultTypeDef &resultType)
{
	//printf("goto judge normal hu\n");
	int i,j,m;
	int jiangPos;//“将”的位置
	int yuShu;//余数
	bool jiangExisted=false;
	//是否满足3，3，3，3，2模型
	for(i=0;i<3;i++)
	{
		//yuShu=(int)fmod((float)allPai[i][0],(float)3);
		yuShu = allPai[i][0]%3;
		if (yuShu==1)
		{
			return false;
		}
		if (yuShu==2) 
		{
			if(jiangExisted)
			{
				return false;
			}
			jiangPos=i;
			jiangExisted=true;
		}
	}
	for(i=0;i<3;i++)
	{
		if (i!=jiangPos) 
		{
			if (!Analyze(allPai[i],i,resultType))
			{
				return false;
			}
		}
	}
	//在游戏 中遇到一种牌型ru 111 222 333 444 55 （清大对）即数字相连的牌型
	//用上面的方法会出现 111 22（将） 234 345 345 的牌型（判断为清一色）.
	//该类牌中要包含将，因为要对将进行轮询，效率较低，放在最后
	bool success=false;//指示除掉“将”后能否通过
	for (j = 1; j < 10; j++)
	{
		if(allPai[jiangPos][j]==2)
		{
			//临时定义变量保存，如果判断成功则累积进resultType,不然无视
			ResultTypeDef tempResultType;
			memset(&tempResultType,0,sizeof(ResultTypeDef));
			//除去这2张将牌
			allPai[jiangPos][j]-=2;
			allPai[jiangPos][0]-=2;
			if(Analyze(allPai[jiangPos],jiangPos,tempResultType))
			{
				success=true;


				for(m = 0;m<tempResultType.iChiNum;m++)
				{
					memcpy(&(resultType.chiType[resultType.iChiNum]),&(tempResultType.chiType[m]),sizeof(ChiTypeDef));
					resultType.iChiNum++;
				}
				for(m = 0;m<tempResultType.iPengNum;m++)
				{
					memcpy(&(resultType.pengType[resultType.iPengNum]),&(tempResultType.pengType[m]),sizeof(PengTypeDef));
					resultType.iPengNum++;
				}
			}
			//还原这2张将牌
			allPai[jiangPos][j]+=2;
			allPai[jiangPos][0]+=2;
			if(success) 
			{
				resultType.jiangType.iType = jiangPos;
				resultType.jiangType.iValue = j;
				return success;
			}

		}
	}
	for(j=1;j<10;j++)//对列进行操作，用j表示
	{
		if(allPai[jiangPos][j]>=2)
		{
			//临时定义变量保存，如果判断成功则累积进resultType,不然无视
			ResultTypeDef tempResultType;
			memset(&tempResultType,0,sizeof(ResultTypeDef));
			//除去这2张将牌
			allPai[jiangPos][j]-=2;
			allPai[jiangPos][0]-=2;
			if(Analyze(allPai[jiangPos],jiangPos,tempResultType))
			{
				success=true;

				for(m = 0;m<tempResultType.iChiNum;m++)
				{
					memcpy(&(resultType.chiType[resultType.iChiNum]),&(tempResultType.chiType[m]),sizeof(ChiTypeDef));
					resultType.iChiNum++;
				}
				for(m = 0;m<tempResultType.iPengNum;m++)
				{
					memcpy(&(resultType.pengType[resultType.iPengNum]),&(tempResultType.pengType[m]),sizeof(PengTypeDef));
					resultType.iPengNum++;
				}
			}
			//还原这2张将牌
			allPai[jiangPos][j]+=2;
			allPai[jiangPos][0]+=2;
			if(success) 
			{
				resultType.jiangType.iType = jiangPos;
				resultType.jiangType.iValue = j;
				break;
			}
		}
	}
	return success;
}

//分解成“刻”“顺”组合
bool Analyze(int aKindPai[],int iType,ResultTypeDef &resultType)
{
	int j;
	if (aKindPai[0]==0)
	{
		return true;
	}
	//寻找第一张牌
	for(j=1;j<10;j++)
	{
		if (aKindPai[j]!=0)
		{
			break;
		}
	}
	bool result;
	if (aKindPai[j]>=3)//作为刻牌
	{
		//除去这3张刻牌
		aKindPai[j]-=3;
		aKindPai[0]-=3;

		resultType.pengType[resultType.iPengNum].iType = iType;
		resultType.pengType[resultType.iPengNum].iValue = j;
		resultType.iPengNum++;


		result=Analyze(aKindPai,iType,resultType);
		//还原这3张刻牌
		aKindPai[j]+=3;
		aKindPai[0]+=3;
		return result;
	}
	//作为顺牌
	if ((j<8)
		&&(aKindPai[j+1]>0)
		&&(aKindPai[j+2]>0))
	{
		//除去这3张顺牌
		aKindPai[j]--;
		aKindPai[j+1]--;
		aKindPai[j+2]--;
		aKindPai[0]-=3;

		resultType.chiType[resultType.iChiNum].iType = iType;
		resultType.chiType[resultType.iChiNum].iFirstValue = j;
		resultType.iChiNum++;

		result=Analyze(aKindPai,iType,resultType);
		//还原这3张顺牌
		aKindPai[j]++;
		aKindPai[j+1]++;
		aKindPai[j+2]++;
		aKindPai[0]+=3;
		return result;
	}
	return false;
}



#define GANG_SHANG_HUA 1
#define GANG_SHANG_PAO 2
#define QIANG_GANG_HU 3

// note: cHuType is ret of JudgeHu
int  getFans(int handCards[3][10], std::list<CMjCardPile> &pCardList, ResultTypeDef &resultType, int cHuType, 
				 bool isZhuang, bool isNotOutCard, HU_FansList &fanList)
{

	if (cHuType != 1 && cHuType != 2)
		return 0;
	
	if (GANG_SHANG_HUA == fanList.gangHuType)
		fanList.gangFans = 1;
	if (GANG_SHANG_PAO == fanList.gangHuType)
		fanList.gangFans = 1;
	if (QIANG_GANG_HU == fanList.gangHuType)
		fanList.gangFans = 1;

	int gens = 0;
	for (int i = 0; i != 3; ++i)
		for (int j = 1; j != 10; ++j)
			if (4 == handCards[i][j])
				++gens;
	fanList.nHaveGen = gens;

	//这里没有计算碰杠之后的番数，在外面判断
	
	if (isZhuang && isNotOutCard && !resultType.lastCard.bOther) { // 天胡
		fanList.huType = HU_TIANHU;
		fanList.huFans = SCMJ_HU_FANS[HU_TIANHU];
		return 1;
	}
	
	if (!isZhuang && isNotOutCard) { // 地胡
		fanList.huType = HU_DIHU;
		fanList.huFans = SCMJ_HU_FANS[HU_DIHU];
		return 1;
	}

	
	int handCardsBak[3][10] = {0}; // all cards
	memcpy(handCardsBak, handCards, sizeof(handCardsBak));
	if (resultType.lastCard.iValue && resultType.lastCard.bOther)
		++handCardsBak[resultType.lastCard.iType][resultType.lastCard.iValue];

	for (list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
		handCardsBak[it->GetPileCard(0).Type()][it->GetPileCard(0).Value()] += 3;
		handCardsBak[it->GetPileCard(0).Type()][0] += 3;
	}
	
	if (2 == cHuType) {
		int typeDui[3] = {0}, typeGangs[3] = {0};
		for (int i = 0; i != 3; ++i) {
			for (int j = 1; j != 10; ++j) {
				if (2 == handCardsBak[i][j]) {
					++typeDui[i];
				} else if (4 == handCardsBak[i][j]) {
					typeDui[i] += 2;
					++typeGangs[i];
				}
			}
		}

		if (Qing1Se(handCardsBak)) {
			if (typeGangs[0]+typeGangs[1]+typeGangs[2] > 0) {  //青龙七对
				fanList.huType = HU_QINGLONG7DUI;
				fanList.huFans = SCMJ_HU_FANS[HU_QINGLONG7DUI];
				--fanList.nHaveGen;
			} else {  //清七对
				fanList.huType = HU_QING7DUI;
				fanList.huFans = SCMJ_HU_FANS[HU_QING7DUI];
			}
		} else if (typeGangs[0]+typeGangs[1]+typeGangs[2] > 0) { //龙七对
			fanList.huType = HU_LONG7DUI;
			fanList.huFans = SCMJ_HU_FANS[HU_LONG7DUI];
			--fanList.nHaveGen;
		} else { //七对
			fanList.huType = HU_7DUI;
			fanList.huFans = SCMJ_HU_FANS[HU_7DUI];
		}
	} else if (1 == cHuType) { //一般的胡

		if (4 == pCardList.size()) { //18罗汉
			bool bAllGang = true;
			for (list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
				if (it->GetPileCardNums() != 4) {
					bAllGang = false;
					break;
				}
			}
			if (bAllGang) {
				fanList.huType = HU_18LUOHAN;
				fanList.huFans = SCMJ_HU_FANS[HU_18LUOHAN];
				return 1;
			}
		}

		if (2 == handCardsBak[resultType.lastCard.iType][resultType.lastCard.iValue] && 4 == pCardList.size()) {// 金钩钓
			if (Qing1Se(handCardsBak)) {  //清金钩钓
				fanList.huType = HU_QINGJINGOUDIAO;
				fanList.huFans = SCMJ_HU_FANS[HU_QINGJINGOUDIAO];
				return 1;
			} else { //将金钩调
				bool isJiang = true;
				if (2 != resultType.lastCard.iValue && 5 != resultType.lastCard.iValue && 8 != resultType.lastCard.iValue)
					isJiang = false;
				if (isJiang) {
					for (list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
						char v = it->GetPileCard(0).Value();
						if (2 != v && 5 != v && 8 != v) {
							isJiang = false;
							break;
						}
					}
					if (isJiang) {
						fanList.huType = HU_JIANGJINGOUDIAO;
						fanList.huFans = SCMJ_HU_FANS[HU_JIANGJINGOUDIAO];
						return 1;
					}
				}
			}
		}

		//清19
		if (Qing19(handCardsBak)) {
			fanList.huType = HU_QING19;
			fanList.huFans = SCMJ_HU_FANS[HU_QING19];
			return 1;
		}

		bool noGang = true;
		for (list<CMjCardPile>::iterator it = pCardList.begin(); it != pCardList.end(); ++it) {
			if (it->GetPileCardNums() == 4) {
				noGang = false;
				break;
			}
		}

		if (noGang) { //对对胡
			bool ballDaDuiZi = true;
			bool bJiangDui = false;
			bool bWrong = false;
			for (int i = 0; i != 3; ++i) {
				for (int j = 1; j != 10; ++j) {
					if (handCardsBak[i][j]) {
						if (handCardsBak[i][j] == 2) {
							if (!bJiangDui)
								bJiangDui = true;
							else {
								bJiangDui = false;
								bWrong = true;
								break;
							}
						} else if (handCardsBak[i][j] == 3) {

						} else {
							ballDaDuiZi = false;
							bWrong = true;
							break;
						}
					}
				}
				if (bWrong)
					break;
			}

			if (!bWrong && ballDaDuiZi && bJiangDui) {
				if (Qing1Se(handCardsBak)) { //清对
					fanList.huType = HU_QINGDUI;
					fanList.huFans = SCMJ_HU_FANS[HU_QINGDUI];
					return 1;
				}
				bool bAll258 = true;
				for (int i = 0; i != 3; ++i)
					for (int j = 1; j != 10; ++j) {
						if (handCardsBak[i][j]) {
							if (2 != handCardsBak[i][j] && 5 != handCardsBak[i][j] && 8 != handCardsBak[i][j]) {
								bAll258 = false;
								break;
							}
						}
					}

				//需求中取消将对的牌型
				//if (bAll258) { //将对
				//	fanList.huType = HU_JIANGDUI;
				//	fanList.huFans = SCMJ_HU_FANS[HU_JIANGDUI];
				//	return 1;
				//}
			}
		}

		if (dai19(handCardsBak)) { //带19
			fanList.huType = HU_DAI19;
			fanList.huFans = SCMJ_HU_FANS[HU_DAI19];
			return 1;
		}

		if (2 == handCardsBak[resultType.lastCard.iType][resultType.lastCard.iValue] && 4 == pCardList.size()) { //金钩钓
			fanList.huType = HU_JINGOUDIAO;
			fanList.huFans = SCMJ_HU_FANS[HU_JINGOUDIAO];
			return 1;
		}

		if (Qing1Se(handCardsBak)) {
			fanList.huType = HU_QING1SE;
			fanList.huFans = SCMJ_HU_FANS[HU_QING1SE];
			return 1;
		}

		if (noGang) {
			bool ballDaDuiZi = true;
			bool bJiangDui = false;
			bool bWrong = false;
			for (int i = 0; i != 3; ++i) {
				for (int j = 1; j != 10; ++j) {
					if (handCardsBak[i][j]) {
						if (handCardsBak[i][j] == 2) {
							if (!bJiangDui)
								bJiangDui = true;
							else {
								bJiangDui = false;
								bWrong = true;
								break;
							}
						} else if (handCardsBak[i][j] == 3) {

						} else {
							ballDaDuiZi = false;
							bWrong = true;
							break;
						}
					}
				}
				if (bWrong)
					break;
			}

			if (!bWrong && ballDaDuiZi && bJiangDui) {
				fanList.huType = HU_DUIDUIHU;
				fanList.huFans = SCMJ_HU_FANS[HU_DUIDUIHU];
				return 1;
			}
		}

		fanList.huType = HU_PINGHU;
		fanList.huFans = SCMJ_HU_FANS[HU_PINGHU];
		return 1;
	}

	return 1;
}

#if 0
int JudgeFan(ResultTypeDef resultType,char *cResult, int& cHuType)
{
	puts("JudgeFan");
	int iTotalFan = 0;
	/*printf("resultType.iChiNum %d resultType.iPengNum %d resultType.jiangType.iValue %d resultType.lastCard.iValue %d\n", resultType.iChiNum, resultType.iPengNum, resultType.jiangType.iValue, resultType.lastCard.iValue);
	if(resultType.iChiNum+resultType.iPengNum != 4 || resultType.jiangType.iValue == 0 || resultType.lastCard.iValue == 0)
	{
		printf("JudgeFan ErrorParameter\n");
		return -1;
	}*/

	//首先对resultType里的chiType和pengType根据iValue进行冒泡排序,从小到大
	int i,j;
	ChiTypeDef tempChiType;
	for(i=0;i<(resultType.iChiNum-1);i++)
	{
		for(j=i+1;j<resultType.iChiNum;j++)
		{
			if(resultType.chiType[i].iFirstValue > resultType.chiType[j].iFirstValue)//前数牌值大于后数牌值
			{
				memcpy(&tempChiType,&(resultType.chiType[i]),sizeof(ChiTypeDef));
				memcpy(&(resultType.chiType[i]),&(resultType.chiType[j]),sizeof(ChiTypeDef));
				memcpy(&(resultType.chiType[j]),&tempChiType,sizeof(ChiTypeDef));
			}	
		}
	}
	PengTypeDef tempPengType;
	for(i=0;i<(resultType.iPengNum-1);i++)
	{
		for(j=i+1;j<resultType.iPengNum;j++)
		{
			if(resultType.pengType[i].iValue > resultType.pengType[j].iValue)//前数牌值大于后数牌值
			{
				memcpy(&tempPengType,&(resultType.pengType[i]),sizeof(PengTypeDef));
				memcpy(&(resultType.pengType[i]),&(resultType.pengType[j]),sizeof(PengTypeDef));
				memcpy(&(resultType.pengType[j]),&tempPengType,sizeof(PengTypeDef));
			}	
		}
	}

	

	////2番，杠上开花 开杠抓进的牌成和牌(不包括补花)不计自摸 
	//if(cResult[WGANGSHANGHUA] != -1)
	//{
	//	if(resultType.lastCard.bGang == true)
	//	{
	//		cResult[GANG_SHANG_KAI_HUA] = 2;
	//		cResult[ZI_MO] = -1;
	//		printf("杠上花\n");
	//		cHuType = WGANGSHANGHUA;
	//		
	//	}
	//}
	//2倍，抢杠和 和别人自抓开明杠的牌。不计和绝张 
	/*if(cResult[QIANG_GANG_HE] != -1)
	{
		if(resultType.lastCard.bQiangGang == true)
		{
			cResult[WQIANGGANGHU] = 2;
			printf("抢杠和\n");
			cHuType = WQIANGGANGHU;
			
		}
	}*/

	if(cResult[QINGDADUI] != -1)
	{
		bool bYiSe = true;
		for(i = 0;i<resultType.iChiNum;i++)
		{
			if(resultType.chiType[i].iType != resultType.jiangType.iType)//判断是否是同一种颜色
			{
				bYiSe = false;
				break;
			}
		}
		for (i =0; i < resultType.iPengNum; i++)
		{
			if (resultType.pengType[i].iType != resultType.jiangType.iType)
			{
				bYiSe = false;
				break;
			}
		}
		if (bYiSe)//如果是同一种颜色，判断是不是大对子，四个搭子均为三张一样的牌。如：111 333 444万222 66筒（2倍）
		{
			printf("是同一种颜色\n");
			if (resultType.iChiNum == 0)
			{
				cResult[QINGDADUI] = 8; //清一色大对子（8倍）
				cResult[QINGYISE] = -1;
				cResult[DADUIZI] = -1;
				cResult[PINGHU] = -1;
				printf("清一色大对子 8 番\n");
				cHuType = QINGDA_DUI;
			}
			else
			{
				cResult[QINGDADUI] = -1; 
				cResult[QINGYISE] = 4;//胡牌时只有一色牌。如：111 234 345 666 88万（4倍）
				cResult[DADUIZI] = -1;
				cResult[PINGHU] = -1;
				printf("清一色 4番\n");
				cHuType = QINGYI_SE;

			}

		}
		else//不是同一种颜色
		{
			printf("不是同一种颜色\n");
			if (resultType.iChiNum == 0)
			{
				cResult[QINGDADUI] = -1; 
				cResult[QINGYISE] = -1;
				cResult[DADUIZI] = 2;//四个搭子均为三张一样的牌。如：111 333 444万222 66筒（2倍）
				cResult[PINGHU] = -1;
				printf("大对子 2 番\n");
				cHuType = DADUI_ZI;
			}
			else
			{
				cResult[QINGDADUI] = -1; 
				cResult[QINGYISE] = -1;
				cResult[DADUIZI] = -1;
				cResult[PINGHU] = 1;//普通的四个搭子一对将。如：123 345 789万 234 66筒（1倍）
				cResult[ZIMO] = -1;
				printf("平胡\n");
				cHuType = HU_DIANPAO;
				if(resultType.lastCard.bOther == false)//自摸
				{
					cResult[ZIMO] = 2;
					cResult[PINGHU] = -1;
					printf("自摸 \n");
					cHuType = HU_ZIMO;
				}

			}
		}

	}

	//计算总番.把所有的-1清成0
	for(i = 1;i<9;i++)
	{
		if(cResult[i] == -1)
		{
			cResult[i] = 0;
		}
		else if(cResult[i] > 0)
		{
			iTotalFan += cResult[i];
		}
	}

	return iTotalFan;
}

bool HU( BYTE PAI[],int PaiNums,int& m_iJian,bool bIsLianLiu /*= false*/,int *pIsolatedCardNum /*= NULL*/ )
{
	int nIsolatedCardNum = RemainPai(PAI,PaiNums);//牌的数量
	
	if(nIsolatedCardNum == 14)
	{
		int index = 1;
		bool dan = false;
		
		for(index=1;index<PaiNums;index++)
		{	if(PAI[index]%2==0)
			continue;
			
				dan=true;
				break;
			
		}
		if(!dan)
		{
			return true;
		}
	}
	
	if(nIsolatedCardNum == 0) return true;
	if(pIsolatedCardNum !=NULL && *pIsolatedCardNum > nIsolatedCardNum)
		*pIsolatedCardNum = nIsolatedCardNum;
	
	int i = 0;
	for(i = 1; i< PaiNums; i++ )
	{
	
		if(PAI[i]!=0 )
		{
			
			break;
		}
				
	}  // 找到有牌的地方，i就是当前牌, PAI[i]是个数
	if(i>=PaiNums)
		return false;

	// 2张组合(将牌)
	if ( !(m_iJian) && PAI[i] >= 2 )     // 如果之前没有将牌，且当前牌不少于2张
	{
		m_iJian = 1;                   // 设置将牌标志
		PAI[i] -= 2;                 // 减去2张牌
		
		if( HU(PAI,PaiNums,m_iJian,bIsLianLiu) ) // 如果剩余的牌组合成功，和牌
		{
			
			return true;        
		}                      
		
		PAI[i] += 2;                 // 取消2张组合
		m_iJian = 0;                   // 清除将牌标志
	}

    // 3张组合(大对)
    if ( PAI[i] == 3 )               // 如果当前牌不少于3张
    {
        PAI[i] -= 3;                 // 减去3张牌

        if( HU(PAI,PaiNums,m_iJian,bIsLianLiu) ) return true;   // 如果剩余的牌组合成功，和牌
        PAI[i] += 3;                 // 取消3张组合
    }
	
	if (bIsLianLiu && PAI[i] >= 4)
	{
		
		PAI[i] -= 4;

		if (HU(PAI,PaiNums,m_iJian, bIsLianLiu))
			return true;
		PAI[i] += 4;
	}

	// 顺牌组合，注意是从前往后组合！
	if( i < 30 && i%10 != 8 && i%10 != 9  &&   // 排除数值为8和9的牌
		PAI[i+1] && PAI[i+2] )      // 如果后面有连续两张牌
	{
		PAI[i]--;
		PAI[i+1]--;
		PAI[i+2]--; // 各牌数减1

        if( HU(PAI,PaiNums,m_iJian, bIsLianLiu) ) return true;   // 如果剩余的牌组合成功，和牌
        PAI[i]++;
        PAI[i+1]++;
        PAI[i+2]++;                  // 恢复各牌数


    }
    // 无法全部组合，不和！
	return false;

}

int RemainPai( BYTE PAI[],int PaiNums )
{
	int total = 0;
	for(int i = 0; i < PaiNums; i++)
		total += PAI[i];
	return total;

}

#endif

