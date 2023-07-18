/*
Author: 鄭璟翰(B093040003)
Date: Jan. 3, 2022
Purpose: 以Huffman演算法製作一個檔案壓縮與解壓縮之軟體
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
using namespace std;

//宣告一個Huffman tree的節點
class HuffmanNode {
public:
	//內部存有字母 以及該字母出現的頻率、左右子節點
	//如果左右子節點有內容的話，這不是葉節點 字母以~代替 頻率為左右子節點總和
	char alphabet;
	int freq;
	HuffmanNode* left, * right;
	//constructor
	HuffmanNode(char, int, HuffmanNode*, HuffmanNode*);
	//回傳(包括自身)子樹下的最小字母 判斷左右子使用
	int minAlphabet();
};
//constructor
HuffmanNode::HuffmanNode(char a, int f, HuffmanNode* l, HuffmanNode* r) :alphabet(a), freq(f), left(l), right(r) {}
//尋找子樹下最小自元
int HuffmanNode::minAlphabet() {
	//將自身設為最小
	int smellest = (int)alphabet;
	//如果左右子節點有內容 且比最小的小 則改寫最小值
	if (left != NULL && left->minAlphabet() < smellest)
		smellest = left->minAlphabet();
	if (right != NULL && right->minAlphabet() < smellest)
		smellest = right->minAlphabet();
	//回傳最小值
	return smellest;
}
//比較兩個HuffmandNode的大小順序 讓sort所使用
bool HuffmanCodeCompare(HuffmanNode* a, HuffmanNode* b) {
	//頻率小的優先
	//如果頻率依樣 則比較最小字母
	return a->freq != b->freq ? a->freq < b->freq : a->minAlphabet() < b->minAlphabet();
}
//設定Huffman tree:將各個節點設定成Huffman code
HuffmanNode* HuffmanCode(int symbol[], int freq[], int size) {
	//symbol代表符號 freq 代表頻率 size代表有幾個table

	//HufArray則將上述的資料存入HuffmanNode 存入Vector中
	vector<HuffmanNode*> HufArray;
	for (int i = 0; i < size; ++i)
		HufArray.push_back(new HuffmanNode((char)symbol[i], freq[i], NULL, NULL));

	//將Huffman Array中的資料建成Huffman Tree
	while (HufArray.size() != 1) {
		//如果HufArray的長度不是一則繼續：將所有節點都接到root的下方 後回傳

		//將第一個設為左節點 第二個設為右節點 並移除Array
		HuffmanNode* Left = HufArray[0], * Right = HufArray[1];
		HufArray.erase(HufArray.begin());
		HufArray.erase(HufArray.begin());
		//如果這兩者最小字母的順序錯誤 則對調
		if (Left->minAlphabet() > Right->minAlphabet())
			swap(Left, Right);
		//設立一個新節點 並將牠放到Vector的最後
		HufArray.push_back(new HuffmanNode('~', Left->freq + Right->freq, Left, Right));

		//將這個Vector的順序排序好
		sort(HufArray.begin(), HufArray.begin() + HufArray.size(), HuffmanCodeCompare);
	}
	return HufArray[0];
}
//編碼：回傳一個Vector 每個element帶有char和一個string 傳入root(Huffmantree的根) code:目前編碼到哪裡 top編碼道地幾個數字
vector< pair<char, string> > encode(HuffmanNode* root, string Code, int top) {
	//暫存變數 temp1, temp2
	vector< pair<char, string> > temp1, temp2;
	//如果左節點小孩 則目前字串加上0 然後往下遞迴
	if (root->left) {
		Code += "0";
		temp1 = encode(root->left, Code, top + 1);
		//將遞迴得到的結果接回原本Vector
		temp2.insert(temp2.end(), temp1.begin(), temp1.end());
		//刪掉這層加的數字
		Code.erase(top);
	}
	//和左節點類似 改成加上1
	if (root->right) {
		Code += "1";
		temp1 = encode(root->right, Code, top + 1);
		temp2.insert(temp2.end(), temp1.begin(), temp1.end());
		Code.erase(top);
	}
	//如果為葉節點 則將原本vector存入這個葉節點的編碼
	if (!root->left && !root->right) {
		pair<char, string> buf;
		buf.first = root->alphabet;
		buf.second = Code;
		temp2.push_back(buf);
	}
	//回傳結果
	return temp2;
}

//加壓函數
void compress(char* inputfile, char* outputfile) {
	char buffer = '\0';
	int freq[256] = {}, size = 0;
	//第一次開檔 先計算有幾個字元 加到freq之中
	ifstream forTable(inputfile, ios::binary);
	for (; forTable.get(buffer); ++freq[(int)((unsigned char)buffer)], ++size);
	forTable.close();

	//計算有幾個是非0的結果
	int treeNodeNum = 0;
	for (int i = 0; i < 256; ++i)
		if (freq[i] != 0)
			++treeNodeNum;

	int freqTable[2][300];

	//將非0的放入table之中
	for (int i = 0, j = 0; i < 256; ++i) {
		if (freq[i] != 0) {	//what the fuck?
			freqTable[0][j] = i;
			freqTable[1][j++] = freq[i];
		}
	}
	//將這些pair的順序依照頻率排序
	for (int i = 0; i < treeNodeNum - 1; ++i)
		for (int j = 0; j < treeNodeNum - 1 - i; ++j)
			if (freqTable[1][j] > freqTable[1][j + 1]) {
				swap(freqTable[1][j], freqTable[1][j + 1]);
				swap(freqTable[0][j], freqTable[0][j + 1]);
			}

	//創造結果root
	HuffmanNode* root = HuffmanCode(freqTable[0], freqTable[1], treeNodeNum);

	//依據root編碼 結果存入HufCodeTable
	string Code;
	vector< pair<char, string> > HufCodeTable;
	HufCodeTable = encode(root, Code, 0);
	char combination8 = 0;

	//index為放入一個字元所需要的順序
	int index = 7, ErrorBit = 0;

	//zeroOne代表要輸出的字串
	string zeroOne;
	char buf2 = '\0';
	//開檔
	ifstream input(inputfile, ios::binary);
	ofstream output(outputfile, ios::binary);
	while (input.get(buf2)) {
		//讀入一個字元
		//根據Huffman table進行壓縮
		for (size_t i = 0; i < HufCodeTable.size(); ++i) {
			//尋找和Table中相同的字元
			if (HufCodeTable[i].first == buf2) {
				//如果找到 則將編碼表依序放入combination8之中
				for (size_t j = 0; j < HufCodeTable[i].second.size(); ++j, --index) {
					combination8 |= HufCodeTable[i].second[j] - '0' << index;
					//如果已經滿八個數字 則輸出結果
					if (!index) {
						zeroOne += combination8;
						index = 8;
						combination8 = 0;
					}
				}
				break;
			}
		}
	}

	//如果沒有最後幾個位數奏不到八個一組 則將剩下的結果輸入 並記錄有幾個bit是錯誤的
	if (index != 7) {
		zeroOne += combination8;
		ErrorBit = index + 1;
	}
	
	//計算壓縮率 24表示 compression ratio: %這行的長度
	size_t outputfileSize = 24;
	outputfileSize += zeroOne.size();	//加上輸出結果的長度
	for (size_t i = 0; i < HufCodeTable.size(); ++i)
		outputfileSize += 2 + HufCodeTable[i].second.size();
	//輸出壓縮率
	output << "compression ratio:" << size * 100 / (int)outputfileSize << "%" << endl;
	cout << "compression ratio:" << size * 100 / (int)outputfileSize << "%" << endl;
	output << HufCodeTable.size() << " " << ErrorBit << endl;
	//輸出table
	for (size_t i = 0; i < HufCodeTable.size(); ++i)
		output << HufCodeTable[i].first << " " << HufCodeTable[i].second << endl;
	//輸出結果至檔案中
	output << zeroOne;
	input.close();
	output.close();
}
//解壓縮
void uncompress(char* inputfile, char* outputfile) {
	//站存變數
	char buf100[100], temp;
	int tableSize = 0, ErrorBit;
	pair<char, string> buf;
	//開檔
	vector< pair<char, string> >HufmanTable;
	ifstream input(inputfile, ios::binary);
	ofstream output(outputfile, ios::binary);
	//將壓縮率那行忽略
	input.getline(buf100, 100);

	//讀入有幾行table以及最後有幾個Error bit
	input >> tableSize;
	input.get();
	input >> ErrorBit;

	//讀入Huffman table 依照輸出格式判斷
	for (int i = 0; i < tableSize; ++i) {
		input.get();	//remove /n
		input.get(buf.first);	//read char
		input.get();	//remove  ' '
		input >> buf.second;
		HufmanTable.push_back(buf);
	}
	//跳過換行
	input.get();

	for (size_t i = 0; i < HufmanTable.size(); ++i) {
		cout << "i:" << i << " " << HufmanTable[i].first << " " << HufmanTable[i].second << endl;
	}
	//讀入加壓後字元 依序將結果加入zeroOne的字串中
	string zeroOne;
	while (input.get(temp)) {
		for (int i = 0; i < 8; ++i) {
			zeroOne += to_string(temp < 0);	//判斷是否為正數 以判斷第一個字0 或 1(2'complenent)
			temp = temp << 1;
		}
	}
	input.close();

	//移除最後幾位Errorbit
	while (ErrorBit--) {
		zeroOne.erase(zeroOne.end() - 1);
	}

	//解壓縮 從 0 ~ zeroOne.size()
	for (size_t start = 0; start < zeroOne.size(); ) {
		//判斷屬於哪一個編碼
		for (size_t i = 0; i < HufmanTable.size(); ++i) {
			if (zeroOne.substr(start, HufmanTable[i].second.size()) == HufmanTable[i].second) {
				output << HufmanTable[i].first;
				start += HufmanTable[i].second.size();	//改變幾個位數
				break;
			}
		}
	}

	output.close();
	//關檔
}

int main(int argc, char* argv[]) {
	//command line不符合
	if (argc != 7 || strcmp(argv[1], "huffman") || strcmp(argv[3], "-i") || strcmp(argv[5], "-o") || argv[2][0] != '-') {
		cout << "Error in command line:"
			<< "Not 'huffman -c -i infile -o outfile' or 'huffman -u -i infile -o outfile'\n";
		return -1;
	}
	//判斷壓縮或解壓縮 並將檔案讀入
	switch (argv[2][1]) {
	case 'c':
		compress(argv[4], argv[6]);
		break;
	case 'u':
		uncompress(argv[4], argv[6]);
		break;
	default:
		cout << "Error in command line:\nSecond argument should be '-c' or '-u'" << endl;
	}
	return 0;
}
