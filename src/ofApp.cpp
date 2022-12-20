#include "ofApp.h"
#include <cmath>


//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	bufferSize			= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 1.0f;
	phaseAdderTarget 	= 1.0f;
	volume				= 0.5f;
	n_harm				= 1;
	octave 				= 0;


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
	vector <float> fft_inv;
	const int hauteur_rectangle = 200;
	const int largeur_rectangle = 900;
	const int offset_droite = 62;
	const int offset_haut = 150; //184 -> pour centrer les rectangles par rapport à la fenètre
	const int all_pick_enven_the_wrong_ones = 2; //1-> pour avoir tous les picks, 2 -> pour limiter à 2*Fmax, 5-> pour s'arreter à 6000Hz
	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);

	
	ofNoFill();
	
	// draw the Original sound Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(offset_droite, offset_haut, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Original sound Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, largeur_rectangle, hauteur_rectangle);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < sig.size(); i++){
				float x =  ofMap(i, 0, sig.size(), 0, largeur_rectangle, true);
				ofVertex(x, 100 -sig[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the Fourier transform Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(offset_droite, offset_haut + hauteur_rectangle, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Fourier transform Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, largeur_rectangle, hauteur_rectangle);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			fft = computefft(sig);
			fft_inv.assign(fft.size(), 0);
			for (int j = 0; j < fft.size(); j++){
				fft_inv[j] = - fft[j];
			}
			for (unsigned int i = 0; i < fft_inv.size()/all_pick_enven_the_wrong_ones; i++){
				float x =  ofMap(i, 0, fft_inv.size()/all_pick_enven_the_wrong_ones, 0, largeur_rectangle, true);
				ofVertex(x, fft_inv[i]+hauteur_rectangle);
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

	const float r = 1.05946;
	int n;

	if (key == '-' || key == '_'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} 
	
	else if ((key == '+' || key == '=') & (volume < 0.5f)){
		volume += 0.05;
		volume = MIN(volume, 1);
	}


	// Définition de l'octave
	if (key == OF_KEY_UP){
		octave += 12;
	}

	else if (key == OF_KEY_DOWN){
		octave -= 12;
	}
	//Calul de la différence entre la key et le la3(440Hz)
	switch(key){
		case 's':
			n = -9;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'e':
			n = -8;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'd':
			n = -7;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'r':
			n = -6;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'f':
			n = -5;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'g':
			n = -4;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'y':
			n = -3;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'h':
			n = -2;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'u':
			n = -1;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'j':
			n = 0;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'i':
			n = 1;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		case 'k':
			n = 2;
			iskeypressed = true;
			freq = 440 * pow(r, n + octave);
			break;
		
	}

	if ( key == '*' ) {
		n_harm += 1;
	}

	if ( key == '/' ) {
		if (n_harm > 1){
			n_harm -= 1;}
	}

	if ( key == '1' ) {
		mode_carre = true;
	}

	// reset filters and modes
	if ( key == '0' ) {
		mode_carre = false;
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
		
		if (mode_carre){
			computeSigCarre(sig) ;
		}else{
			computeSig(sig) ;
		}


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

void ofApp::computeSigCarre(vector <float> & sig){
	phaseAdder = (freq / (float) sampleRate) * TWO_PI;

	while (phase > TWO_PI){
			phase -= TWO_PI;
		}

	for (size_t i = 0; i < sig.size(); i++){
		phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 0; k < n_harm +1; k++) {
			sample += (sin((2*n_harm	+ 1) * TWO_PI * freq * phase)) / (2*n_harm	+ 1);
		}
		sig[i] = sample * 4 / PI * volume;
	}
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

