#pragma once

#include <complex>

#include "FFT.h"

class NMRIFile
{
public:
	NMRIFile();
	~NMRIFile();

	bool filterHighFreqs;
	bool filterLowFreqs;

	int NrFrames;
	int Width;
	int Height;

	bool Load(const CString& name);

	inline std::complex<double> GetValue(int frame, int posx, int posy) const
	{
		if (frame >= NrFrames) return std::complex<double>(0, 0);

		int pos = Width * Height * frame;

		float re = realData[pos + Width*posy + posx];
		float im = imgData[pos + Width*posy + posx];

		return std::complex<double>(re, im);
	}

	void InverseFFT(int frame)
	{
		const double xCenter = Width / 2.;
		const double yCenter = Height / 2.;

		const double xLowPass = Width / 32.;
		const double yLowPass = Height / 32.;

		const double xLowLowLimit = xCenter - xLowPass;
		const double xHighLowLimit = xCenter + xLowPass;
		const double yLowLowLimit = yCenter - yLowPass;
		const double yHighLowLimit = yCenter + yLowPass;

		const double xHighPass = Width / 16.;
		const double yHighPass = Height / 16.;


		const double xLowHighLimit = xCenter - xHighPass;
		const double xHighHighLimit = xCenter + xHighPass;
		const double yLowHighLimit = yCenter - yHighPass;
		const double yHighHighLimit = yCenter + yHighPass;

		for (int x = 0; x < Width; ++x)
			for (int y = 0; y < Height; ++y)
			{
				if (filterLowFreqs && x > xLowLowLimit && x < xHighLowLimit && y > yLowLowLimit && y < yHighLowLimit)
				{
					srcFrame[y*Width + x] = 0;
					continue;
				}

				if (filterHighFreqs && (x < xLowHighLimit || x > xHighHighLimit || y < yLowHighLimit || y > yHighHighLimit))
				{
					srcFrame[y*Width + x] = 0;
					continue;
				}

				std::complex<double> val = GetValue(frame, x, y);
				srcFrame[y*Width + x] = (themax > 1E-14 ? val / themax : val);
			}

		fft.inv(srcFrame, theFrame, Width, Height);

	    const double thenorm = sqrt(Width*Height);

		for (int x = 0; x < Width; ++x)
			for (int y = 0; y < Height; ++y)
				theFrame[y*Width + x] /= thenorm;
	}

	inline double GetRealValue(int posx, int posy) const
	{
		return std::abs(theFrame[Width*posy + posx]);
	}

	const std::complex<double>* GetRealFrame() const { return theFrame; }
	const std::complex<double>* GetFrame() const { return srcFrame; }

protected:
	double themax;

	float* realData;
	float* imgData;

	std::complex<double>* theFrame;
	std::complex<double>* srcFrame;

	Fourier::FFT fft;
};

