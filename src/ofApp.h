#pragma once

#include "ofMain.h"
#include <valarray>
#include <complex>

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioOut(ofSoundBuffer & buffer);
		void computeSig(vector <float> & sig);
		vector <float> computefft(vector <float> sig);
		
		ofSoundStream soundStream;

		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		int 	bufferSize;

		vector <float> Audio;
		vector <float> sig;

		//------------------- for the simple sine wave synthesis
		float 	frequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
};
