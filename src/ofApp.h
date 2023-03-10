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
		void computeSig(int one_freq, vector <float> & sig, vector <float> & notes_phase, int n);
		void computeSigCarre(int one_freq, vector <float> & one_sig, vector <float> & notes_phase, int n);
		void computeSigDent(int one_freq, vector <float> & sig, vector <float> & notes_phase, int n);
		vector <float> computefft(vector <float> sig);
		void addNoise(vector <float> & sig);
		

		ofSoundStream soundStream;

		int		sampleRate;
		float 	volume;
		int 	bufferSize;
		int 	freq; 
		bool	iskeypressed; 
		int		n_harm;
		bool	mode_carre;
		int 	octave;
		bool 	mode_dent;
		int 	n_demo;
		string	wave_mode;
		int 	gap_440;
		float	noise_percent;
		string	playing_note;

		vector <float> sig;
		vector <int> demo;
		vector <bool> notes_played;
		vector <int> gap_vec;
		vector <float> notes_phase;

		//------------------- for the simple sine wave synthesis
		float 	frequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
};
