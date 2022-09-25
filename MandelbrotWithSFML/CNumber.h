#ifndef BALL_H
#define BALL_H
#include <iostream>
template <class T>
class CNumber
{
private:
	T m_real;
	T m_imag;

public:
	CNumber<T>(T real, T imag);
	CNumber<T>(CNumber<T>& c);
	CNumber<T>(const CNumber<T>& c);
	T getReal() const;
	T getImag() const;
	void setReal(T real);
	void setImag(T imag);
	T mag();

	CNumber<T> operator+(CNumber<T> z);
	CNumber<T> operator-(CNumber<T> z);
	CNumber<T> operator*(CNumber<T> z);
	CNumber<T> operator/(CNumber<T> z);
	void operator=(CNumber<T> z);
	void operator*=(CNumber<T> z);
	void operator+=(CNumber<T> z);
	CNumber<T> cPow(int p);
	void raiseToPow(int p);
};

template <class T>
CNumber<T>::CNumber<T>(T real, T imag)
	:m_real(real), m_imag(imag)
{
}

template <class T>
CNumber<T>::CNumber<T>(CNumber<T>& c)
	:m_real(c.getReal()), m_imag(c.getImag())
{
}

template <class T>
CNumber<T>::CNumber<T>(const CNumber<T>& c)
	:m_real(c.getReal()), m_imag(c.getImag())
{
}

template <class T>
T CNumber<T>::getReal() const
{
	// Returns the real part of the complex number
	return m_real;
}

template <class T>
T CNumber<T>::getImag() const
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
	return CNumber<T>::CNumber<T>(m_real + z.getReal(), m_imag + z.getImag());
}

template <class T>
CNumber<T> CNumber<T>::operator-(CNumber<T> z)
{
	return CNumber<T>::CNumber<T>(m_real - z.getReal(), m_imag - z.getImag());
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
	T tmp_real = m_real * z.getReal() - m_imag * z.getImag();
	m_imag = m_real * z.getImag() + m_imag * z.getReal();
	m_real = tmp_real;
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
#endif //!BALL_H