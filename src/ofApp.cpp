#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	bufferSize			= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 1.0f;
	phaseAdderTarget 	= 1.0f;
	volume				= 0.5f;

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

	vector <float> fft;
	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);

	
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
			fft = computefft(sig);
			for (unsigned int i = 0; i < fft.size(); i++){
				float x =  ofMap(i, 0, fft.size(), 0, 900, true);
				ofVertex(x, fft[i]);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
		
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys";
	if( !bNoise ){
		reportString += "sine wave (" + ofToString(freq, 2) + "hz) modify with mouse y";
	}else{
		reportString += "noise";	
	}
	ofDrawBitmapString(reportString, 32, 579);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == '-' || key == '_'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} 
	
	else if ((key == '+' || key == '=') & (volume < 0.5f)){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if( key == 's' || key == 'd' || key == 'f' || key == 'g' || key == 'h' || key == 'j' || key == 'k' || key == 'l'){
		iskeypressed = true;
	
		if( key == 's' ){
			freq = 262;
		}
		
		else if( key == 'd' ){
			freq = 294;
		}
		
		else if( key == 'f' ){
			freq = 330;
		}

		else if( key == 'g' ){
			freq = 349;
		}

		else if( key == 'h' ){
			freq = 392;
		}

		else if( key == 'j' ){
			freq = 440;
		}

		else if( key == 'k' ){
			freq = 494;
		}
		else if( key == 'l' ){
			freq = 523;
		}
	} 
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	iskeypressed = false;
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

	if ( iskeypressed ){
		float scale = 2.0f;

		computeSig(sig) ;
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			buffer[i*buffer.getNumChannels()    ] = sig[i] * scale;
		}
	}

}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::computeSig(vector <float> & sig){
	phaseAdder = (freq / (float) sampleRate) * TWO_PI;
    
	while (phase > TWO_PI){
			phase -= TWO_PI;
		}

	for (size_t i = 0; i < sig.size(); i++){
		phase += phaseAdder;
		float sample = sin(phase);
		sig[i] = sample * volume;
	}
}

vector <float> ofApp::computefft(vector <float> sig){

	CArray x(sig.size());
	vector <float> data;
	data.assign(sig.size(), 0);

	for (int k=0; k < sig.size(); k++){
		x[k] = Complex(sig[k], 0);
	}
	
	
 	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}

	
    for (int i = 0; i < x.size(); i++){
		data[i] = abs(x[i]);

    }

	return data;

}