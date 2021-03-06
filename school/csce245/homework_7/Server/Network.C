#include "Network.h"
#include "ServerConn.h"
#include <iostream>
#include <errno.h>     /* for ETIMEDOUT */
#include <algorithm>   /* for find() */
#include <assert.h>
#include <Errors.h>
using namespace std;

Network::Network(int portno) : _si(sockbuf::sock_stream)
{
    try {
	if (portno>0) {		                  // If constructor got a port number
	    sockinetaddr a(static_cast<long unsigned int>(INADDR_ANY), portno); 
	    _si.bind(a);                          // Make wildcard address w/port and go for it
	} else _si.bind();                        // Else pick any port number
    } catch (const sockerr & e) {

	string s = "ConnectionError: Network Initialization:  Could NOT bind port: ";
	throw ConnectionError(s.c_str()); 
    }
    _si.recvtimeout(0);		                  // Don't block on accept
    _si.listen();                                 // Start listening for connection attempts

    // INITIALIZING    
    gameRunning = false;
    count = 0;
    
    startingX[0] = "5";
    startingY[0] = "5";
    startingX[1] = "12";
    startingY[1] = "10";
    startingX[2] = "21";
    startingY[2] = "18";
    startingX[3] = "30";
    startingY[3] = "4";
    startingX[4] = "39";
    startingY[4] = "11";
    startingX[5] = "45";
    startingY[5] = "2";
    startingX[6] = "56";
    startingY[6] = "9";
    startingX[7] = "68";
    startingY[7] = "17";

    boundX = 70;
    boundY = 14;
    
    string temp(boundX, ' ');
    string temp2(30, '<');
    string temp3(30, '>');
    string temp4(10, ' ');
    for(int i = 0; i < boundY; i++){
       if(i != 9)
           map += temp;
       else
           map = map + temp2 + temp4 + temp3;
    }
}

Network::~Network()
{
    if (!_tempNet.empty())              //puts temps on _us list just before network is killed
	mergeTemps();

    for (Lcxn::iterator i = _us.begin(); i != _us.end(); ) {
	Lcxn::iterator j = i;                     // Be careful because *i is about to die
	++i;                                      // So increment i now
//	(*j)->Finalize();                         // Then let it go
    }
}

bool Network::Accept()                            // Go for (up to one) new connection
{
    if (!_si.is_readready(0,100000))              // Bail if nobody shows within .1 sec
	return false;                             // (BTW, that sets the Run loop idle rate)

    sockinetaddr peer;                            // A place to build incoming address
    iosockinet * i = new iosockinet(_si.accept(peer)); // Accept it
    s = new ServerConn(*this,i,peer);                 // Build the connection
    return true;
}

void Network::Add(ServerConn * c) {  _us.push_back(c);  }   //add a conn to _net

void Network::addTemp(ServerConn * c)                       // Adds a conn to _tempNet
{  
    _tempNet.push_back(c); 
//    newConn(c);
}

void Network::receiveTempMsgs(){   // Receive messages from temp user
    string tmp, tmp2;
    int serverMsgLength, clientMsgLength, msgNum;
    for (Lcxn::iterator i = _tempNet.begin(); i != _tempNet.end(); ++i){
        try{ 
            tmp = (*i)->receive();
        }catch(...){}  // ADD USER REMOVAL IF EXCEPTION THROWN HERE
        while(tmp != ""){  // HANDLES ALL MESSAGES IN STREAM
            serverMsgLength = atoi(tmp.substr(0, tmp.find(":")).c_str());
            cout << tmp.substr(0, serverMsgLength + tmp.find(":") + 1) << endl;
            tmp = tmp.substr(tmp.find(":") + 1);
            msgNum = atoi(tmp.substr(0, tmp.find(" ")).c_str());
            serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
            tmp = tmp.substr(tmp.find(" ") + 1);
 
            if(tmp.substr(0, tmp.find(" ")) == "HELO"){
                tmp2 = tmp.substr(0, serverMsgLength);
                count++;
                tmp2 = intToString(count) + " " + tmp2;
                clientMsgLength = tmp2.size();
                (*i)->setName(tmp2.substr(tmp2.rfind(" ") + 1));
                (*i)->send(intToString(clientMsgLength) + ":" + tmp2);
            }
            else if(tmp.substr(0, 4) == "MAPC"){
                count++;
                tmp2 = intToString(count) + " MAPS " + intToString(boundX) + " " + intToString(boundY) + " " + map;  // NEEDS TO BE FIXED LATER
                clientMsgLength = tmp2.size();  
                (*i)->send(intToString(clientMsgLength) + ":" + tmp2);   
                (*i)->readyToPlay = true;
            }
            tmp = tmp.substr(serverMsgLength);
        }
    }
}

void Network::receiveUserMsgs(){   // Receive messages from users
    string tmp, tmp2, nametmp;
    int serverMsgLength, clientMsgLength, msgNum;
    int tmpX, tmpY;
    for (Lcxn::iterator i = _us.begin(); i != _us.end(); ++i){
        try{
            tmp = (*i)->receive();            
        }catch(...){}  // ADD USER REMOVAL IF EXCEPTION THROWN HERE
        while(tmp != ""){  // HANDLES ALL MESSAGES IN STREAM
            cout << tmp << endl;
            serverMsgLength = atoi(tmp.substr(0, tmp.find(":")).c_str());
            tmp = tmp.substr(tmp.find(":") + 1);
            msgNum = atoi(tmp.substr(0, tmp.find(" ")).c_str());
            serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
            tmp = tmp.substr(tmp.find(" ") + 1);

            if(!gameRunning){   // COMMANDS FOR WHEN GAME IS NOT RUNNING
                if(tmp.substr(0, 6) == "BEGINC" && Count() > 1){
                    sendAll(" BEGINS");
                    int k = 0;
                    for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j, k++){
                        sendAll(" MOVE " + (*j)->getName() + " " + startingX[k] + " " + startingY[k]); // NEED TO COME BACK AND FIX
                        (*j)->x = atoi(startingX[k].c_str());
                        (*j)->y = atoi(startingY[k].c_str());
                        (*j)->alive = true;
                    }
                    gameRunning = true;
                }                
            }
            else if ((*i)->alive){   // COMMANDS FOR WHEN GAME IS RUNNING
                if(tmp.substr(0, tmp.find(" ")) == "MOVE"){
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    nametmp = tmp.substr(0, tmp.find(" "));
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    tmpX = atoi(tmp.substr(0, tmp.find(" ")).c_str());
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    tmpY = atoi(tmp.substr(0, serverMsgLength).c_str());
                    for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){    
                        if((*j)->getName() == nametmp){
                            if(canMove((*j)->x + tmpX, (*j)->y + tmpY)){
                                (*j)->x += tmpX;
                                (*j)->y += tmpY;
                            }
                            sendAll(" MOVE " + (*j)->getName() + " " + intToString((*j)->x) + " " + intToString((*j)->y));
                        }
                    }               
                }
                else if(tmp.substr(0, tmp.find(" ")) == "FIRE"){
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    nametmp = tmp.substr(0, tmp.find(" "));
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    tmpX = atoi(tmp.substr(0, tmp.find(" ")).c_str());
                    serverMsgLength = serverMsgLength - tmp.find(" ") - 1;
                    tmp = tmp.substr(tmp.find(" ") + 1);
                    tmpY = atoi(tmp.substr(0, serverMsgLength).c_str());
                    for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){    
                        if((*j)->getName() == nametmp)
                            Fire((*j), tmpX, tmpY);
                    }
                }
            }
            tmp = tmp.substr(serverMsgLength);
        }
    }
}

void Network::sendAll(string tmp){
   int msgLength;
   string tmp2;
   for (Lcxn::iterator i = _us.begin(); i != _us.end(); ++i){
      count++;
      tmp2 = intToString(count) + tmp;
      msgLength = tmp2.size();
      (*i)->send(intToString(msgLength) + ":" + tmp2);
   }
   for (Lcxn::iterator i = _tempNet.begin(); i != _tempNet.end(); ++i){
      count++;
      tmp2 = intToString(count) + tmp;
      msgLength = tmp2.size();
      (*i)->send(intToString(msgLength) + ":" + tmp2);
   }
}

void Network::admitNewUsers(){
    if(!gameRunning){
        while(!_tempNet.empty() && Count() < 8 && _tempNet.front()->readyToPlay){
           _us.push_back(_tempNet.front());
           _tempNet.pop_front();
        }
    }    
}

string Network::getNames()
{
  string names;
  for (Lcxn::iterator i = _us.begin(); i != _us.end(); ++i)
    {
      names += (*i)->getName();                    //get a name from ServerConn
      names += " ";                                //space out list
    }
  return names;
}


void Network::mergeTemps()
{
    while (!_tempNet.empty())                     //while connections on temp
	_us.push_back(_tempNet.front());          //move to _net for delete
    _tempNet.clear();
}

string Network::intToString(int & num){
    stringstream ss;
    string str;
    ss << num;
    ss >> str; 
    return str;
}

bool Network::canMove(int x1, int y1){
    if(x1 < 0 || x1 > 69 || y1 < 0 || y1 > 13)
        return false;
    if(map[boundX*y1 + x1] != ' ')
        return false;
    for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){
        if((*j)->x == x1 && (*j)->y == y1 && (*j)->alive)
            return false;
    }
    return true;
}


void Network::Fire(ServerConn * sc, int x1, int y1){
    int tmpX = sc->x;
    int tmpY = sc->y;
    while(true){
        tmpX += x1;
        tmpY += y1;
        for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){
            if((*j)->x == tmpX && (*j)->y == tmpY && (*j)->alive){
                sendAll(" KILL " + sc->getName() + " " + (*j)->getName() + " " + intToString(sc->x) + " " + intToString(sc->y) + " " + intToString(tmpX) + " " + intToString(tmpY));
                (*j)->alive = false;
                return;
            }
        }
        if(tmpX  < 0 || tmpX  > 69 || tmpY < 0 || tmpY > 13 || map[boundX*tmpY + tmpX] != ' '){
            sendAll(" FIRE " + sc->getName() + " " + intToString(sc->x) + " " + intToString(sc->y) + " " + intToString(tmpX) + " " + intToString(tmpY));
            return;
        }
    }
}

void Network::checkIfFinished(){
    int tmp = 0;
    for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){
        if((*j)->alive)
            tmp++;
    }
    if(tmp == 1){
        for (Lcxn::iterator j = _us.begin(); j != _us.end(); ++j){
            if((*j)->alive){
                sendAll(" WINNER " + (*j)->getName());
                gameRunning = false;
                (*j)->alive = false;
                return;
            }                
        }
    }
}