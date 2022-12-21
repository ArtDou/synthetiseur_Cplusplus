#include "ofApp.h"
#include <cmath>

const float r = 1.05946;
//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	bufferSize			= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 1.0f;
	phaseAdderTarget 	= 1.0f;
	volume				= 0.2f;
	n_harm				= 1;
	octave 				= 0;
	wave_mode 			= "Sinusoide";
	n_demo 				= 0;
	noise_percent		= 0.0f;

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
		440 // A
	});
	notes_played.assign(12, false);
	gap_vec.assign({-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2});
	sig.assign(bufferSize, 0.0);
	notes_phase.assign(12, 0.0);
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
	const int offset_haut = 10; //184 -> pour centrer les rectangles par rapport à la fenètre
	const int all_pick_enven_the_wrong_ones = 2; //1-> pour avoir tous les picks, 2 -> pour limiter à 2*Fmax, 5-> pour s'arreter à 6000Hz
	const int white_tile_width = 40;
	const int white_tile_height = 150;
	const int black_tile_width = 30;
	const int black_tile_height = 100;
	const int R_fill_color = 245;
	const int G_fill_color = 58;
	const int B_fill_color = 135;
	const int volume_width = 40;
	const int volume_height = 16;
	const int line_size = 30;
	const int sep = 60;
	const int noise_height = 36;
 	ofSetWindowTitle("Coucou, tu veux voir mon synthetiseur");
	
	ofSetColor(225);
	ofNoFill();
	
	// draw the Original sound Channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(offset_droite, offset_haut + 20, 0);
			
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
		
		
			// Information sur les touches blanches
			ofSetColor(10);
			ofDrawBitmapString("S", white_tile_width*0 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("D", white_tile_width*1 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("F", white_tile_width*2 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("G", white_tile_width*3 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("H", white_tile_width*4 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("J", white_tile_width*5 + white_tile_width/2 -4, white_tile_height - 10);
			ofDrawBitmapString("K", white_tile_width*6 + white_tile_width/2 -4, white_tile_height - 10);

			// Information sur les touches noires
			ofSetColor(255);
			ofDrawBitmapString("E", white_tile_width*1 - 4, black_tile_height - 10);
			ofDrawBitmapString("R", white_tile_width*2 - 4, black_tile_height - 10);
			ofDrawBitmapString("Y", white_tile_width*4 - 4, black_tile_height - 10);
			ofDrawBitmapString("U", white_tile_width*5 - 4, black_tile_height - 10);
			ofDrawBitmapString("I", white_tile_width*6 - 4, black_tile_height - 10);


			// Information sur les octaves
			ofSetColor(255);
			ofDrawBitmapString("Octave " + ofToString(octave/12 + 3), white_tile_width*2 + white_tile_width/2, - 10);
			ofSetColor(255);
			ofDrawBitmapString("Note " + playing_note + ofToString(octave/12 + 3) + "\nFrequence = " + ofToString(freq, 2) + "Hz", white_tile_width*8 + 10 , 0);



		ofPopMatrix();
	ofPopStyle();

	ofPushStyle();
		ofPushMatrix();
			ofTranslate(900, offset_haut + hauteur_rectangle * 2 + 100, 0);
			ofDrawBitmapString("Volume", volume_width/2 - 20, 0);
			// Case de volume
			ofFill();
			ofSetColor(255, 255, 255);
			if (volume > 0.05){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*10, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.1){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*9, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.15){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*8, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.20){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*7, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.25){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*6, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.30){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*5, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.35){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*4, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.40){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*3, volume_width, volume_height);

			ofSetColor(255, 255, 255);
			if (volume > 0.45){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, volume_height*2, volume_width, volume_height);
			
			//Contour du volume
			ofNoFill();  
			ofSetLineWidth(1);	
			ofSetColor(10, 10, 10);
			ofDrawRectangle(0, volume_height*10, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*9, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*8, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*7, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*6, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*5, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*4, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*3, volume_width, volume_height);
			ofDrawRectangle(0, volume_height*2, volume_width, volume_height);

			// Afficher + et -
			ofSetColor(255);
			ofDrawBitmapString("-", volume_width/2 - 2, volume_height*11 + 15);
			ofDrawBitmapString("+", volume_width/2 - 2, volume_height*2 - 5);

		ofPopMatrix();
	ofPopStyle();

	ofPushStyle();
		ofPushMatrix();
			ofTranslate(0, offset_haut + hauteur_rectangle * 2 + 150, 0);
			ofSetLineWidth(2);

			ofSetColor(255);
			ofDrawBitmapString("Signal", offset_droite +30, -70);
			// signal sinusoïdale
			ofSetColor(255);
			ofDrawBitmapString("1", offset_droite - 20, -15);
			if (wave_mode == "Sinusoide"){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofBeginShape();
			for (unsigned int i = 20; i < 80; i++){
				ofVertex(i + offset_droite, 20 * sin(i / TWO_PI)-20);
			}
			ofEndShape(false);

			// signal carré
			ofSetColor(255);
			ofDrawBitmapString("2", offset_droite - 20, sep - 15);
			if (wave_mode == "Carre"){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawLine(offset_droite, 0 + sep, offset_droite + line_size, 0 + sep);
			ofDrawLine(offset_droite + line_size, 0 + sep, offset_droite + line_size, -line_size + sep);
			ofDrawLine(offset_droite + line_size, -line_size + sep, offset_droite + 2*line_size, -line_size + sep);
			ofDrawLine(offset_droite + 2*line_size, -line_size + sep, offset_droite + 2*line_size, 0 + sep);
			ofDrawLine(offset_droite + 2*line_size, 0 + sep, offset_droite + 3*line_size, 0 + sep);

			// signal triangle
			ofSetColor(255);
			ofDrawBitmapString("3", offset_droite - 20, 2*sep - 15);
			if (wave_mode == "Dent de scie"){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawLine(offset_droite + line_size, 0 + 2*sep, offset_droite + line_size, -line_size + 2*sep);
			ofDrawLine(offset_droite, 0 + 2*sep, offset_droite + line_size, 0 + 2*sep);
			ofDrawLine(offset_droite + line_size, -line_size + 2*sep, offset_droite + 2*line_size, 0 + 2*sep);
			ofDrawLine(offset_droite + 2*line_size, 0 + 2*sep, offset_droite + 3*line_size, 0 + 2*sep);

			// Harmonique
			ofSetColor(255);
			ofDrawBitmapString("Nunber of harmonic = " + ofToString(n_harm - 1) + "\nincrease: *\ndecrease: /\nreset: 0", offset_droite, 2* sep + 35);

		ofPopMatrix();
	ofPopStyle();

	ofPushStyle();
		ofPushMatrix();
			ofTranslate(820, offset_haut + hauteur_rectangle * 2 + 100, 0);
			ofDrawBitmapString("Noise", volume_width/2 - 20, 0);

			// CAse du bruit
			ofFill();
			ofSetColor(255);
			if (noise_percent > 0){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, noise_height*4, volume_width, noise_height);
			ofSetColor(255);
			if (noise_percent > 0.25){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, noise_height*3, volume_width, noise_height);
			ofSetColor(255);
			if (noise_percent > 0.5){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawRectangle(0, noise_height*1, volume_width, noise_height*2);



			//Contour des case du bruit
			ofNoFill();  
			ofSetLineWidth(1);	
			ofSetColor(10, 10, 10);
			ofDrawRectangle(0, noise_height*4, volume_width, noise_height);
			ofDrawRectangle(0, noise_height*3, volume_width, noise_height);
			ofDrawRectangle(0, noise_height*1, volume_width, noise_height*2);


			ofSetColor(10);
			ofDrawBitmapString("100%", volume_width/2 - 14, noise_height*1.5 + noise_height/2 + 4);
			ofDrawBitmapString("50%", volume_width/2 - 11, noise_height*3 + noise_height/2 + 4);
			ofDrawBitmapString("25%", volume_width/2 - 11, noise_height*4 + noise_height/2 + 4);
			ofSetColor(255);
			if (noise_percent == 0.0){
				ofSetColor(R_fill_color, G_fill_color, B_fill_color);
			}
			ofDrawBitmapString("0%", volume_width/2 - 7, noise_height*5 + noise_height/2 + 4);

			ofSetColor(255);
			ofDrawBitmapString("V", -20, noise_height*1.5 + noise_height/2 + 4);
			ofDrawBitmapString("C", -20, noise_height*3 + noise_height/2 + 4);
			ofDrawBitmapString("X", -20, noise_height*4 + noise_height/2 + 4);
			ofDrawBitmapString("W", -20, noise_height*5 + noise_height/2 + 4);
		
		ofPopMatrix();
	ofPopStyle();

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


	// Définition de l'octave
	if (key == OF_KEY_UP & octave < 12*4){
		octave += 12;
	}

	else if (key == OF_KEY_DOWN & octave > -12*4){
		octave -= 12;
	}
	//Calul de la différence entre la key et le la3(440Hz)
	switch(key){
		case 's':
			gap_440 = -9;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Do";
			notes_played[0]=true;
			break;
		case 'e':
			gap_440 = -8;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Do#";
			notes_played[1]=true;
			break;
		case 'd':
			gap_440 = -7;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Re";
			notes_played[2]=true;
			break;
		case 'r':
			gap_440 = -6;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Re#";
			notes_played[3]=true;
			break;
		case 'f':
			gap_440 = -5;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Mi";
			notes_played[4]=true;
			break;
		case 'g':
			gap_440 = -4;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Fa";
			notes_played[5]=true;
			break;
		case 'y':
			gap_440 = -3;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Fa#";
			notes_played[6]=true;
			break;
		case 'h':
			gap_440 = -2;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Sol";
			notes_played[7]=true;
			break;
		case 'u':
			gap_440 = -1;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Sol#";
			notes_played[8]=true;
			break;
		case 'j':
			gap_440 = 0;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "La";
			notes_played[9]=true;
			break;
		case 'i':
			gap_440 = 1;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "La#";
			notes_played[10]=true;
			break;
		case 'k':
			gap_440 = 2;
			iskeypressed = true;
			freq = 440 * pow(r, gap_440 + octave);
			playing_note = "Si";
			notes_played[11]=true;
			break;
			// automate mode
		case 'n' : 
			iskeypressed = true;
			freq = demo[n_demo];
			// cout<<demo[n_demo]<<endl;
			n_demo += 1;
			if (n_demo == demo.size())
				n_demo = 0;
			break;
	}

	if ( key == '*' & n_harm < 256) {
		n_harm += 1;
	}

	if ( key == '/' ) {
		if (n_harm > 1){
			n_harm -= 1;}
	}

	if ( key == '3' ) {
		mode_dent = true;
		mode_carre = false;
		wave_mode = "Dent de scie";
	}
	else if ( key == '2' ) {
		mode_carre = true;
		mode_dent = false;
		wave_mode = "Carre";
	}

	// reset filters and modes
	if ( key == '1' ) {
		mode_carre = false;
		mode_dent = false;

		wave_mode = "Sinusoide";
	}

	if ( key == '0' ) {
		n_harm = 1;

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
	switch(key){
		case 's':
			notes_played[0]=false;
			break;
		case 'e':
			notes_played[1]=false;
			break;
		case 'd':
			notes_played[2]=false;
			break;
		case 'r':
			notes_played[3]=false;
			break;
		case 'f':
			notes_played[4]=false;
			break;
		case 'g':
			notes_played[5]=false;
			break;
		case 'y':
			notes_played[6]=false;
			break;
		case 'h':
			notes_played[7]=false;
			break;
		case 'u':
			notes_played[8]=false;
			break;
		case 'j':
			notes_played[9]=false;
			break;
		case 'i':
			notes_played[10]=false;
			break;
		case 'k':
			notes_played[11]=false;
			break;
			// automate mode
		case 'n' : 
			iskeypressed = false;
			break;
	}
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
	float scale = 2.0f;
	vector <float> one_sig;
	int freq_note;
	sig.assign(bufferSize, 0.0);
	float n_note = 1.0;

	for (int n = 0; n < notes_played.size(); n++){
		if (notes_played[n]){
			n_note += 1.0;
			one_sig.assign(bufferSize, 0.0);
			freq_note = 440 * pow(r, gap_vec[n] + octave);

				if (mode_carre){
					computeSigCarre(freq_note, one_sig, notes_phase, n) ;
					addNoise(one_sig) ;
				}
				else if (mode_dent){
					computeSigDent(freq_note, one_sig, notes_phase, n);
					addNoise(one_sig) ;
				}
				else{
					computeSig(freq_note, one_sig, notes_phase, n) ;
					addNoise(one_sig) ;
				}
			for (int j = 0; j < one_sig.size(); j++) {
				sig[j] += one_sig[j];
			}
		}
	}
	// Normalize
	if (n_note > 1.0) n_note -= 1.0;
	for (int j = 0; j < one_sig.size(); j++) {
		sig[j] += one_sig[j] / n_note;
	}
	// for (int a=0;a<notes_phase.size();a++)
	// 	cout<<notes_phase[a]<<" ";
	// cout<<endl;
	for (size_t i = 0; i < buffer.getNumFrames(); i++){
		buffer[i*buffer.getNumChannels()    ] = sig[i] * scale;
	}

}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::computeSigCarre(int one_freq, vector <float> & one_sig, vector <float> & notes_phase, int n){
	phaseAdder = (one_freq / (float) sampleRate) * TWO_PI;
	float one_phase = notes_phase[n];

	while (one_phase > TWO_PI){
			one_phase -= TWO_PI;
	}

	for (size_t i = 0; i < one_sig.size(); i++){
		one_phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 0; k < n_harm +1; k++) {
			sample += (sin((2 * k + 1) * one_phase)) / (2*k	+ 1);
		}
		one_sig[i] = sample * 4 / PI * volume;
	}
	notes_phase[n] = one_phase;
}

void ofApp::computeSigDent(int one_freq, vector <float> & one_sig, vector <float> & notes_phase, int n){
	phaseAdder = (one_freq / (float) sampleRate) * TWO_PI;
	float one_phase = notes_phase[n];

	while (one_phase > TWO_PI){
			one_phase -= TWO_PI;
	}

	for (size_t i = 0; i < one_sig.size(); i++){
		one_phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 1; k < n_harm +1; k++) {
			sample += pow((-1),k) * (sin(k * one_phase)/k);
		}
		one_sig[i] = sample * 2 / PI * volume;
	}
	notes_phase[n] = one_phase;
}

void ofApp::computeSig(int one_freq, vector <float> & one_sig, vector <float> & notes_phase, int n){
	phaseAdder = (one_freq / (float) sampleRate) * TWO_PI;
    float one_phase = notes_phase[n];

	while (one_phase > TWO_PI){
			one_phase -= TWO_PI;
	}

	for (size_t i = 0; i < one_sig.size(); i++){
		one_phase += phaseAdder;
		float sample = 0.0f;
		for (int k = 1; k < n_harm +1; k++) {
			sample += sin(k * one_phase) ;
		}
		one_sig[i] = sample * volume / n_harm;
	}
	notes_phase[n] = one_phase;
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