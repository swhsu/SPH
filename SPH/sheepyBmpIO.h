// sheepy CG land by Shu-Wei Hsu
//-------------------------------------------------------------------------------
///
/// \file		sheepyBmpIO.h 
/// \author		Shu-Wei Hsu
/// \version	1.0
/// \date		Jan 27, 2010
///
/// \brief Read and write bmp files.
///
///
///
/// sheepyBmpIO is a bmp-format image IO class.
///
/// In OpenGL, you can use save the the screen buffer to a bmp image by 
///
/// the following codes:
///
///   GLubyte *pixels = new GLubyte[w*h*3];	
///   glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);
///   sheepyBmpIO::write(filename, pixels, w, h);
///
///
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
#ifndef _SHEEPY_BMPIO_H_
#define _SHEEPY_BMPIO_H_

#include <iostream>
#include <fstream>
#include <string>
#include <GL/freeglut.h>

using namespace std;
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------

/// \class sheepyBmpIO
class sheepyBmpIO
{
public:
	/// \name Constructors
	sheepyBmpIO(void);
	~sheepyBmpIO(void);


	/// \fn Write image method
	/// \param filename outfile name
	/// \param data pixel data packed in BGR sequence
	/// \param w image width
	/// \param d image depth
	/// \note Write in BGR order! Please pack the data in this sequence.
	static void write(string filename, unsigned char* data, int w, int h) {	
		// header
		int pad = (4 - w * 3 % 4) % 4;
		int size = 54+(3*w+pad)*h;
		unsigned char* output = new unsigned char[size];
		
		memset(output, 0, size);
																				// The detail of the bmp format can be found at 
																				// http://en.wikipedia.org/wiki/BMP_file_format
		output[0] = 0x42;										// Magic Number 'B'
		output[1] = 0x4D;										// Magic Number 'M'
		output[2] = size & 0xFF;						// Size of the BMP file
		output[3] = (size >> 8) & 0xFF;
		output[4] = (size >> 16) & 0xFF;
		output[5] = (size >> 24) & 0xFF;
		output[10] = 0x36;									// The offset where the bitmap data (pixels) can be found.
		output[14] = 0x28;									// The number of bytes in the header (from this point).
		output[18] = w & 0xFF;							// The width of the bitmap in pixels 
		output[19] = (w >> 8) & 0xFF; 
		output[20] = (w >> 16) & 0xFF; 
		output[21] = (w >> 24) & 0xFF;
		output[22] = h & 0xFF;							// The height of the bitmap in pixels
		output[23] = (h >> 8) & 0xFF; 
		output[24] = (h >> 16) & 0xFF; 
		output[25] = (h >> 24) & 0xFF;
		output[26] = 0x1;										// Number of color planes being used.
		output[28] = 0x18;									// Bit Depth.

		int data_idx;
		int output_idx = 54;

		for(int i=0; i<h; i++) {												
			for(int j=0; j<w; j++) {
				data_idx = i*w + j;
				output[output_idx++] = data[3*data_idx];		// Note! Write in BGR order
				output[output_idx++] = data[3*data_idx+1];
				output[output_idx++] = data[3*data_idx+2];
			}
			output_idx+=pad;
		}

		ofstream file(filename.c_str(), ios::out|ios::binary);
		if (file.is_open()) {
			file.write((char*)output, size);
			file.close();
		} else {
			cerr << "Unable to write the bmp file." << endl;
		}

		delete output;
	};

	/// \fn Read image method
	/// \param filename outfile name
	/// \return pixels
	/// \note the sequence of the color channels is BGR  
	static unsigned char* read(string filename) {
		ifstream file (filename.c_str(), ios::in|ios::binary|ios::ate);

		if (file.is_open())
		{
			int size = file.tellg();
			unsigned char* pixels = new unsigned char [size-54];
			file.seekg (54, ios::beg);
			file.read ((char*)pixels, size-54);
			file.close();
			return pixels; 
		} else {
			cerr << "Unable to read the bmp file." << endl;
			return NULL;
		}
	};


	static void saveScreen(string filename, int w, int h) {
	  GLubyte *pixels = new GLubyte[w*h*3];	
	  glReadPixels(0, 0, w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	  write(filename, pixels, w, h);
		delete pixels;
	};
};

namespace sheepy {
	typedef sheepyBmpIO BmpIO;
}

#endif
