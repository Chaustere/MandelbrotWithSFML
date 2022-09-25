#include "CNumber.h"

template <class T>
CNumber<T>::CNumber<T>(T real, T imag)
	:m_real(real), m_imag(imag)
{
	std::cout << "Complex initialisation : " << real << ' ' << imag << std::endl;
}

template <class T>
CNumber<T>::CNumber<T>(const CNumber<T>& c)
	:m_real(c.getReal()), m_imag(c.getImagI())
{
	std::cout << "Complex initialisation : " << c.getReal() << ' ' << c.getImag() << std::endl;
}

template <class T>
T CNumber<T>::getReal()
{
	// Returns the real part of the complex number
	return m_real;
}

template <class T>
T CNumber<T>::getImag()
{
	// Returns the imaginary part of the complex number
	return m_imag;
}

template <class T>
void CNumber<T>::setReal(T real)
{
	// Sets the real part of the complex number
	m_real = real;
}

template <class T>
void CNumber<T>::setImag(T imag)
{
	// Sets the imaginary part of the complex number
	m_imag = imag;
}

template <class T>
T CNumber<T>::mag()
{
	// Returns the magnitude of the complex number
	return pow(pow(m_real, 2) + pow(m_imag, 2), 0.5);
}


template <class T>
CNumber<T> CNumber<T>::operator+(CNumber<T> z)
{
	return CNumber<T>::CNumber<T>(m_real + z.getReal, m_imag + z.getImag());
}

template <class T>
CNumber<T> CNumber<T>::operator-(CNumber<T> z)
{
	return CNumber<T>::CNumber<T>(m_real - z.getReal, m_imag - z.getImag());
}

template <class T>
CNumber<T> CNumber<T>::operator*(CNumber<T> z)
{
	return CNumber<T>::CNumber<T>(m_real * z.getReal() - m_imag * z.getImag(), m_real * z.getImag() + m_imag * z.getReal());
}

template <class T>
CNumber<T> CNumber<T>::operator/(CNumber<T> z)
{
	return CNumber<T>::CNumber<T>((m_real * z.getReal() + m_imag * z.getImag()) / (m_real * m_real + m_imag * m_imag), (-1 * m_real * z.getImag() + z.getReal() * m_imag) / (m_real * m_real + m_imag * m_imag));
}

template <class T>
void CNumber<T>::operator=(CNumber<T> z)
{
	m_real = z.getReal();
	m_imag = z.getImag();
}

template <class T>
void CNumber<T>::operator*=(CNumber<T> z)
{
	m_real = m_real * z.getReal() - m_imag * z.getImag();
	m_imag = m_real * z.getImag() + m_imag * z.getReal();
}

template <class T>
void CNumber<T>::operator+=(CNumber<T> z)
{
	m_real += z.getReal();
	m_imag += z.getImag();
}

template <class T>
CNumber<T> CNumber<T>::cPow(int p)
{
	CNumber<T> c(m_real, m_imag);
	for (size_t i = 1; i < p; i++)
	{
		c *= *this;
	}
	return c;
}


template <class T>
void CNumber<T>::raiseToPow(int p)
{
	CNumber<T> c(m_real, m_imag);
	for (size_t i = 1; i < p; i++)
	{
		*this *= c;
	}
}