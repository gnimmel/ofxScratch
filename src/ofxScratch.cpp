 /*
 *  ofxScratch.cpp
 *  
 *
 *  Created by mactkg on 10/2/11.
 *  Copyright 2011 Tokyo Tech High School of Science and Technoligy. All rights reserved.
 *
 */

#include "ofxScratch.h"
#include "ofUtils.h"

void ofxScratch::setup() {
	msgTx = "";
	weConnected = tcpClient.setup("127.0.0.1", 42001);
	if(weConnected) std::cout << "connected" << endl;
	if(!weConnected) std::cout << "not connected" << endl;
	connectTime = 0;
	deltaTime = 0;
	tcpClient.setVerbose(true);
}

void ofxScratch::update() {
    if(weConnected) {
		msgTx = "";
		msgRx = "";
        string str = tcpClient.receive();
        if( str.length() > 0 ) {
            msgRx = str;
        }
    } else {
        deltaTime = ofGetElapsedTimeMillis() - connectTime;
        if( deltaTime > 5000) {
            weConnected = tcpClient.setup("127.0.0.1", 42001);
            connectTime = ofGetElapsedTimeMillis();
			std::cout << "connection is lost, reconnecting... " << connectTime << endl;
        }
		if (weConnected) std::cout << "connected" << endl;
    }
}

void ofxScratch::sensorUpdate(string sensor, string value) {
	msgTx = "sensor-update \"" + sensor + "\" " + value;
	if(!broadcastScratch(msgTx)) {
    std::cout << "connection is lost" << endl;
    weConnected = false;
	}
}

void ofxScratch::sendBroadcast(string value) {
	msgTx = "broadcast \"" + value + "\"";
	if(broadcastScratch(msgTx)) {
    std::cout << "connection is lost" << endl;
    weConnected = false;
	}
}

bool ofxScratch::broadcastScratch(string message){
	unsigned char sizeBytes[4];
	int len = message.size();

	sizeBytes[0] =(unsigned char)( len >> 24 );
	sizeBytes[1] =(unsigned char)( (len << 8) >> 24 );
	sizeBytes[2] =(unsigned char)( (len << 16) >> 24 );
	sizeBytes[3] =(unsigned char)( (len << 24) >> 24 );

	if(tcpClient.sendRawBytes((char*)sizeBytes, 4)) {
	   
	} else {
	   return false;
	}
	if(tcpClient.sendRaw(message)) {
	   return true;
	} else {
	   return false;  
	}
}