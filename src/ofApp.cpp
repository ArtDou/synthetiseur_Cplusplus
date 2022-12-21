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
	wave_mode 			= "Sinusoide";
	n_demo 				= 0;

	// Fur Elise
	// demo.assign({783,739,783,739,783,587,698,622,523});
	// Chat-GPT compose
	demo.assign({
659, // E
622, // D#
659, // E
622, // D#
659, // E
493, // B
587, // D
523, // C
440, // A
523, // C
659, // E
440, // A
493, // B
659, // E
415, // G#
493, // B
523, // C
659, // E
659, // E
622, // D#
659, // E
622, // D#
659, // E
493, // B
587, // D
523, // C
440, // A
523, // C
659, // E
440, // A
493, // B
659, // E
523, // C
493, // B
440, // A
493, // B
523, // C
587, // D
659, // E
784, // G
698, // F
659, // E
587, // D
698, // F
659, // E
587, // D
523, // C
659, // E
587, // D
523, // C
493, // B
659, // E
622, // D#
659, // E
622, // D#
659, // E
493, // B
587, // D
523, // C
440, // A
523, // C
659, // E
440, // A
493, // B
659, // E
415, // G#
493, // B
523, // C
659, // E
659, // E
622, // D#
659, // E
622, // D#
659, // E
493, // B
587, // D
523, // C
440, // A
523, // C
659, // E
440, // A
493, // B
659, // E
523, // C
493, // B
440, // A
});
	noise_percent		= 0.0f;

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
	const int offset_haut = 30; //184 -> pour centrer les rectangles par rapport à la fenètre
	const int all_pick_enven_the_wrong_ones = 2; //1-> pour avoir tous les picks, 2 -> pour limiter à 2*Fmax, 5-> pour s'arreter à 6000Hz
	const int white_tile_width = 40;
	const int white_tile_height = 150;
	const int black_tile_width = 30;
	const int black_tile_height = 100;
	const int R_fill_color = 245;
	const int G_fill_color = 58;
	const int B_fill_color = 135;
	
 	ofSetWindowTitle("Coucou, tu veux voir mon synthetiseur");
	
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys";
	reportString += "sine wave (" + ofToString(freq, 2) + "hz) modify with mouse y";

	ofDrawBitmapString(reportString, offset_droite, offset_haut);
	ofDrawBitmapString("wave mode: " + wave_mode + "\nnumber of harmonic: " + ofToString(n_harm), offset_droite, offset_haut + 20);


	ofNoFill();
	
	// draw the Original sound Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(offset_droite, offset_haut + 50, 0);
			
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
		ofTranslate(offset_droite, offset_haut + hauteur_rectangle + 50, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Fourier transform Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, largeur_rectangle, hauteur_rectangle);

		ofSetColor(R_fill_color, G_fill_color, B_fill_color);
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

	ofPushStyle();
		ofPushMatrix();
			ofTranslate((900-280)/2, offset_haut + hauteur_rectangle * 2 + 100, 0);

			// touche blanche
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == -9%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*0, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == -7%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*1, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == -5%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*2, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == -4%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*3, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == -2%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*4, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == 0%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*5, 0, white_tile_width, white_tile_height);
			
			ofFill();
			ofSetColor(255, 255, 255);
			if (gap_440 == 2%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*6, 0, white_tile_width, white_tile_height);



			//Contour touches blanches
			ofNoFill();  
			ofSetLineWidth(1);	
			ofSetColor(10, 10, 10);
			ofDrawRectangle(white_tile_width*0, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*1, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*2, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*3, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*4, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*5, 0, white_tile_width, white_tile_height);
			ofDrawRectangle(white_tile_width*6, 0, white_tile_width, white_tile_height);


			// touche noire
			ofFill();
			ofSetColor(10, 10, 10);
			if (gap_440 == -8%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*1-black_tile_width/2, 0, black_tile_width, black_tile_height);
			
			ofFill();
			ofSetColor(10, 10, 10);
			if (gap_440 == -6%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*2-black_tile_width/2, 0, black_tile_width, black_tile_height);
			
			ofFill();
			ofSetColor(10, 10, 10);
			if (gap_440 == -3%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*4-black_tile_width/2, 0, black_tile_width, black_tile_height);
			
			ofFill();
			ofSetColor(10, 10, 10);
			if (gap_440 == -1%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*5-black_tile_width/2, 0, black_tile_width, black_tile_height);
			
			ofFill();
			ofSetColor(10, 10, 10);
			if (gap_440 == 1%12 & iskeypressed){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(white_tile_width*6-black_tile_width/2, 0, black_tile_width, black_tile_height);


			// contour touche noire
			ofNoFill();  
			ofSetLineWidth(1);	
			ofSetColor(10, 10, 10);
			ofDrawRectangle(white_tile_width*1-black_tile_width/2, 0, black_tile_width, black_tile_height);
			ofDrawRectangle(white_tile_width*2-black_tile_width/2, 0, black_tile_width, black_tile_height);
			ofDrawRectangle(white_tile_width*4-black_tile_width/2, 0, black_tile_width, black_tile_height);
			ofDrawRectangle(white_tile_width*5-black_tile_width/2, 0, black_tile_width, black_tile_height);
			ofDrawRectangle(white_tile_width*6-black_tile_width/2, 0, black_tile_width, black_tile_height);
		
		





		
		
		ofPopMatrix();
	ofPopStyle();

	
		

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	const float r = 1.05946;

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
			gap_440 = -9;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'e':
			gap_440 = -8;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'd':
			gap_440 = -7;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'r':
			gap_440 = -6;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'f':
			gap_440 = -5;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'g':
			gap_440 = -4;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'y':
			gap_440 = -3;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'h':
			gap_440 = -2;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'u':
			gap_440 = -1;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'j':
			gap_440 = 0;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'i':
			gap_440 = 1;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
		case 'k':
			gap_440 = 2;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			break;
			// automate mode
		case 'n' : 
			iskeypressed = true;
			freq = demo[n_demo];
			cout<<demo[n_demo]<<endl;
			n_demo += 1;
			if (n_demo == demo.size())
				n_demo = 0;
			break;
	}

	if ( key == '*' ) {
		n_harm += 1;
	}

	if ( key == '/' ) {
		if (n_harm > 1){
			n_harm -= 1;}
	}

	if ( key == '2' ) {
		mode_dent = true;
		mode_carre = false;
		wave_mode = "Dent de scie";
	}
	else if ( key == '1' ) {
		mode_carre = true;
		mode_dent = false;
		wave_mode = "Carre";
	}

	// reset filters and modes
	if ( key == '0' ) {
		mode_carre = false;
		mode_dent = false;
		n_harm = 1;
		wave_mode = "Sinusoide";
	}


	if (key == 'x'){

		noise_percent = 0.25;

	} else if (key == 'c'){

		noise_percent = 0.50;

	} else if (key == 'v'){

		noise_percent = 1;

	} else if (key == 'w'){

		noise_percent = 0;
		
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
			addNoise(sig) ;
		}
		else if (mode_dent){
			computeSigDent(sig);
			addNoise(sig) ;
		}
		else{
			computeSig(sig) ;
			addNoise(sig) ;
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
			sample += (sin((2 * k + 1) * phase)) / (2*k	+ 1);
		}
		sig[i] = sample * 4 / PI * volume;
	}
}

void ofApp::computeSigDent(vector <float> & sig){
	phaseAdder = (freq / (float) sampleRate) * TWO_PI;

	while (phase > TWO_PI){
			phase -= TWO_PI;
		}

	for (size_t i = 0; i < sig.size(); i++){
		phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 1; k < n_harm +1; k++) {
			sample += pow((-1),k) * (sin(k * phase)/k);
		}
		sig[i] = sample * 2 / PI * volume;
	}
}

void ofApp::computeSig(vector <float> & sig){
	phaseAdder = (freq / (float) sampleRate) * TWO_PI;
    
	while (phase > TWO_PI){
			phase -= TWO_PI;
		}

	for (size_t i = 0; i < sig.size(); i++){
		phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 1; k < n_harm +1; k++) {
			sample += sin(k * phase) ;
		}
		sig[i] = sample * volume / n_harm;
	}
}

void ofApp::addNoise(vector <float> & sig){

	for (int i = 0; i < sig.size(); i++){
		sig[i] = noise_percent * ofRandom(0, 1) + (1 - noise_percent) * sig[i];
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