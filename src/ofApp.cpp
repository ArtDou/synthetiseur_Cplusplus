#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	targetFrequency 	= 512.0f;
	phaseAdder 			= 1.0f;
	phaseAdderTarget 	= 1.0f;
	volume				= 0.5f;

	Audio.assign(bufferSize, 0.0);
	sig.assign(bufferSize, 0.0);
	
	soundStream.printDeviceList();

	ofSoundStreamSettings settings;

#ifdef TARGET_LINUX
	// Latest linux versions default to the HDMI output
	// this usually fixes that. Also check the list of available
	// devices if sound doesn't work
	auto devices = soundStream.getMatchingDevices("default");
	if(!devices.empty()){
		settings.setOutDevice(devices[0]);
	}
#endif

	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 1;
	settings.numInputChannels = 0;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);

	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\npress 'e' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the Original sound Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Original sound Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < sig.size(); i++){
				float x =  ofMap(i, 0, sig.size(), 0, 900, true);
				ofVertex(x, 100 -sig[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the Fourier transform Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Fourier transform Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < Audio.size(); i++){
				float x =  ofMap(i, 0, Audio.size(), 0, 900, true);
				ofVertex(x, 100 -Audio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
		
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys";
	if( !bNoise ){
		reportString += "sine wave (" + ofToString(targetFrequency, 2) + "hz) modify with mouse y";
	}else{
		reportString += "noise";	
	}
	ofDrawBitmapString(reportString, 32, 579);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
	computeSig(sig);
	// float scale = 0.5f;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	// while (phase > TWO_PI){
	// 	phase -= TWO_PI;
	// }

	// if (bNoise == true){
	// 	// ---------------------- noise --------------
	// 	for (size_t i = 0; i < buffer.getNumFrames(); i++){
	// 		Audio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume * scale;
	// 	}
	// } else {
	// 	phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
	// 	for (size_t i = 0; i < buffer.getNumFrames(); i++){
	// 		phase += phaseAdder;
	// 		float sample = sin(phase);
	// 		Audio[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume * scale;
	// 	}
	// }

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::computeSig(vector <float> & sig){
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;

	while (phase > TWO_PI){
			phase -= TWO_PI;
		}

	for (size_t i = 0; i < sig.size(); i++){
		phase += phaseAdder;
		float sample = sin(phase);
		sig[i] = sample * volume;
	}
}