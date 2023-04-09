#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <limits.h>
#include <algorithm>

int test = 0;
int testF = 1;

using namespace std;
enum port_state {
    FREE = 0,
    BUSY
};

static int timer;
static int portN;

class Port{
public:
    Port(int id, int size) : state(FREE), portID(id), maxSize(size), 
                    resSize(size), lastTime(0), curQueSize(0),waitQueSize(0){};

    port_state state;
    int portID;
    int maxSize;

    int resSize;
    int lastTime;   // 上一次更新的时间
    int curQueSize;
    int waitQueSize;
    vector<int> resTime; 
    vector<vector<int>> curList = {};
    queue <vector<int>> waitQue = {}; 
};

bool cmp(vector<int> &a, vector<int> &b);
bool cmpPort(vector<int> &a, vector<int> &b);
vector<vector<int>> readfile(string &addr, int seek);

void updataTable(Port *portTable[]);

int findMInPort(vector<int> &flow, Port *portTable[]){
    int n = portN;
    int flowSize = flow[1];
    for(int i = 0; i < n; i++){
        if(portTable[i]->resSize >= flowSize){

            portTable[i]->resSize -= flowSize;
            portTable[i]->curList.push_back(flow);
            portTable[i]->resTime.push_back(flow[3]);
            return portTable[i]->portID;
        }
    }

    int minQueSize = INT_MAX;
    int minQueIdx = 0;
    for(int j = 1; j < n; j++){
        if(portTable[j]->maxSize >= flow[1]){
            if(portTable[j]->waitQueSize < minQueSize){
                minQueSize = min(minQueSize, portTable[j]->waitQueSize);
                minQueIdx = j;
            }
        }
    }

    portTable[minQueIdx]->waitQue.push(flow);
    portTable[minQueIdx]->waitQueSize++;
    return portTable[minQueIdx]->portID;
}


int main(){

    int it = 0;
    while(1){
    vector<vector<int>> flowtest;
    vector<vector<int>> ports;

    // string flowfile = "../data/" + to_string(it) + "/flow.txt";
    // string portfile = "../data/" + to_string(it) + "/port.txt";
    // ofstream outtest("../data/" + to_string(it) + "/result.txt", ios::trunc);
    string flowfile = "E:/work/ZTEsuanfa/data/" + to_string(it) + "/flow.txt";
    string portfile = "E:/work/ZTEsuanfa/data/" + to_string(it) + "/port.txt";
    ofstream outtest("E:/work/ZTEsuanfa/data/" + to_string(it) + "/result.txt", ios::trunc);
    it++;
    // string portfile = "E:/work/ZTEsuanfa/data/port.txt";
    // ofstream outtest("E:/work/ZTEsuanfa/data/result.txt", ios::trunc);
    flowtest = readfile(flowfile, 47);
    ports = readfile(portfile, 22);

    if(flowtest.empty())
        break;

    portN = ports.size();
    Port* portTable[portN];

    sort(ports.begin(), ports.end(), cmpPort);

    for(int i = 0; i < ports.size(); i++){
        Port* portPtr = new Port(ports[i][0], ports[i][1]);
        portTable[i] = portPtr;
    }


    sort(flowtest.begin(), flowtest.end(), cmp);

    int maxtime = (*(flowtest.end()-1))[2];
    int flowIdx = 0;

    while(timer <= maxtime){
        updataTable(portTable);

        if(timer == flowtest[flowIdx][2]){
            // 当前时刻有流到达
            // vector<vector<int>> curTimeFlow;
            while(flowIdx < flowtest.size() &&  timer == flowtest[flowIdx][2]){
                // curTimeFlow.push_back(flowtest[flowIdx]);

                int port_flow = findMInPort(flowtest[flowIdx], portTable);

                outtest << flowtest[flowIdx][0] <<',' << port_flow << ',' << flowtest[flowIdx][2] << endl;

                flowIdx++;
            }
        }
        
        timer++;
    }
    timer = 0;

    }
    return 0;

}

vector<vector<int>> readfile(string &addr, int seek){
    ifstream infile(addr);
    infile.seekg(seek, ios::beg);

    // ofstream outfile("E:/work/ZTEsuanfa/data/outtest.txt", ios::trunc);

    vector<vector<int>> data;
    while(infile){
        string s;
        if(!getline(infile, s))
            break;
        istringstream ss(s);
        vector<int> record;

        while(ss){
            string s;
            if(!getline(ss, s, ','))
                break;
            record.push_back(atoi(s.c_str()));
        }
        data.push_back(record);
    }
    return data;
}
bool cmp(vector<int> &a, vector<int> &b){
    if(a[2] < b[2]){
        return true;
    }
    else{
        if(a[2] ==  b[2]){
            return a[1] > b[1];
        }
    }
    return false;
}

bool cmpPort(vector<int> &a, vector<int> &b){
    if(a[1] < b[1])
        return true;
    return false;
}

void updataTable(Port *portTable[]){
    int n = portN;
    for(int i = 0; i < n; i++){
        Port* curPortPtr = portTable[i];
        if(curPortPtr->lastTime == timer)
            continue;
        int numCur =  curPortPtr->curQueSize;

        for(int i = 0; i < numCur; i++){
            if(timer - curPortPtr->lastTime < curPortPtr->resTime[i]){
                // 当前流未完全发送，更新剩余时间
                curPortPtr->resTime[i] = timer - curPortPtr->lastTime;
            }
            else{
                // 当前流发送完毕，从当前队列中删除
                // 检查等待队列的队头是否可发送， 可发送则更新加入当前发送队列并记录剩余时间
                curPortPtr->resSize += curPortPtr->curList[i][1];
                curPortPtr->resTime.erase(i + curPortPtr->resTime.begin());
                curPortPtr->curList.erase(i + curPortPtr->curList.begin());
                if(curPortPtr->resSize >= curPortPtr->waitQue.front()[1]){
                    curPortPtr->curList.push_back(curPortPtr->waitQue.front());
                    curPortPtr->resTime.push_back(curPortPtr->waitQue.front()[3]);
                    curPortPtr->waitQueSize--;
                }
            }
        }
        // 更新时间
        curPortPtr->lastTime = timer;
    }
}