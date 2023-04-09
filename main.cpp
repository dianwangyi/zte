#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
enum port_state {
    FREE = 0,
    BUSY
};


class Port{
public:
    Port(int id, int size) : state(FREE), portID(id), maxSize(size), resSize(size), lastTime(0){};

    port_state state;
    int portID;
    int maxSize;

    int resSize;
    int lastTime;   // 上一次更新的时间
    vector<int> resTime; 
    vector<vector<int>> curList = {};
};

bool cmp(vector<int> &a, vector<int> &b);
bool cmpPort(vector<int> &a, vector<int> &b);
vector<vector<int>> readfile(string &addr, int seek);

void updataTable(int cur_t, Port *portTable[], int portN);

// int findMInPort(vector<int> &flow, Port *portTable[], int &startTime, int portN){
//     int n = portN;
//     int flowSize = flow[1];
//     for(int i = 0; i < n; i++){
//         if(portTable[i]->resSize >= flowSize){
//             portTable[i]->resSize -= flowSize;
//             portTable[i]->curList.push_back(flow);
//             portTable[i]->resTime.push_back(flow[3]);
            
//             return i;
//         }
//     }
//     for(int i = 0; i < n; i++){
        
//         }
// }
int findMInPort(vector<int> &flow, Port *portTable[], int &startTime, int portN){
    int n = portN;
    int flowSize = flow[1];
    for(int i = 0; i < n; i++){
        if(portTable[i]->maxSize >= flowSize){
            return i;
        }
    }
    return n-1;
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

    int portNum = ports.size();
    Port* portTable[portNum];

    sort(ports.begin(), ports.end(), cmpPort);

    for(int i = 0; i < ports.size(); i++){
        Port* portPtr = new Port(ports[i][0], ports[i][1]);
        portTable[i] = portPtr;
    }


    // for(int i = 0; i < ports.size(); i++){
    //     // cout << portTable[i]-> << " ";
    //     cout << portTable[i]->maxSize;
    //     cout << endl;
    // }

    sort(flowtest.begin(), flowtest.end(), cmp);
    // for(auto record : flowtest){
    //     for(auto x : record){
    //         // cout << x << " ";
    //         outtest << x << " ";
    //     }
    //     // cout << endl;
    //     outtest << endl;
    // }
    int cur_t = 0;
    for(auto flow : flowtest){
        int cur_t = flow[2];
        
        // updataTable(cur_t, portTable, portNum);     // 更新时间，到达当前时间curt后，端口的变化  经过了t时间，每一个端口中，剩余时间不足t的流结束，当前容量增加；大于t的减少t

        int startTime = -1;
        int port_flow = findMInPort(flow, portTable, startTime, portNum); //对于当前流，找到一个端口放入。某一端口剩余容量大于流大小，放入，端口更新size，list；没有能放的，找最接近的排队
        outtest << flow[0] <<',' << port_flow << ',' << flow[2] << endl;
        // cout << flow[0] <<',' << port_flow << ',' << flow[2] << endl;

    }
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

void updataTable(int cur_t, Port *portTable[], int portN){
    int n = portN;
    for(int i = 0; i < n; i++){
        int listSize = portTable[i]->curList.size();
        for(int j = 0; j < listSize; j++){
            if(cur_t - portTable[i]->lastTime >= portTable[i]->resTime[j]){

                portTable[i]->resSize += portTable[i]->curList[j][1];
                portTable[i]->curList.erase(j + portTable[i]->curList.begin());
                portTable[i]->resTime.erase(j + portTable[i]->resTime.begin());
            }
            else{
                portTable[i]->resTime[j] -= (cur_t - portTable[i]->lastTime);
            }
        }

        portTable[i]->lastTime = cur_t;
    }
}