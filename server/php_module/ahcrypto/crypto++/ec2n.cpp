// ec2n.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "ec2n.h"
#include "asn.h"
#include "nbtheory.h"	// for primeTable

#include "algebra.cpp"
#include "eprecomp.cpp"

NAMESPACE_BEGIN(CryptoPP)

EC2N::EC2N(BufferedTransformation &bt)
	: m_field(BERDecodeGF2NP(bt))
{
	BERSequenceDecoder seq(bt);
	m_field->BERDecodeElement(seq, m_a);
	m_field->BERDecodeElement(seq, m_b);
	// skip optional seed
	if (!seq.EndReached())
		BERDecodeOctetString(seq, g_bitBucket);
	seq.MessageEnd();
}

void EC2N::DEREncode(BufferedTransformation &bt) const
{
	m_field->DEREncode(bt);
	DERSequenceEncoder seq(bt);
	m_field->DEREncodeElement(seq, m_a);
	m_field->DEREncodeElement(seq, m_b);
	seq.MessageEnd();
}

bool EC2N::DecodePoint(EC2N::Point &P, const byte *encodedPoint, unsigned int encodedPointLen) const
{
	StringStore store(encodedPoint, encodedPointLen);
	return DecodePoint(P, store, encodedPointLen);
}

bool EC2N::DecodePoint(EC2N::Point &P, BufferedTransformation &bt, unsigned int encodedPointLen) const
{
	byte type;
	if (encodedPointLen < 1 || !bt.Get(type))
		return false;

	switch (type)
	{
	case 0:
		P.identity = true;
		return true;
	case 2:
	case 3:
	{
		if (encodedPointLen != EncodedPointSize(true))
			return false;

		P.identity = false;
		P.x.Decode(bt, m_field->MaxElementByteLength()); 

		if (P.x.IsZero())
		{
			P.y = m_field->SquareRoot(m_b);
			return true;
		}

		FieldElement z = m_field->Square(P.x);
		assert(P.x == m_field->SquareRoot(z));
		P.y = m_field->Divide(m_field->Add(m_field->Multiply(z, m_field->Add(P.x, m_a)), m_b), z);
		assert(P.x == m_field->Subtract(m_field->Divide(m_field->Subtract(m_field->Multiply(P.y, z), m_b), z), m_a));
		z = m_field->SolveQuadraticEquation(P.y);
		assert(m_field->Add(m_field->Square(z), z) == P.y);
		z.SetCoefficient(0, type & 1);

		P.y = m_field->Multiply(z, P.x);
		return true;
	}
	case 4:
	{
		if (encodedPointLen != EncodedPointSize(false))
			return false;

		unsigned int len = m_field->MaxElementByteLength();
		P.identity = false;
		P.x.Decode(bt, len);
		P.y.Decode(bt, len);
		return true;
	}
	default:
		return false;
	}
}

void EC2N::EncodePoint(byte *encodedPoint, const Point &P, bool compressed) const
{
	if (P.identity)
		memset(encodedPoint, 0, EncodedPointSize(compressed));
	else if (compressed)
	{
		encodedPoint[0] = 2 + (!P.x ? 0 : m_field->Divide(P.y, P.x).GetBit(0));
		P.x.Encode(encodedPoint+1, m_field->MaxElementByteLength());
	}
	else
	{
		unsigned int len = m_field->MaxElementByteLength();
		encodedPoint[0] = 4;	// uncompressed
		P.x.Encode(encodedPoint+1, len);
		P.y.Encode(encodedPoint+1+len, len);
	}
}

EC2N::Point EC2N::BERDecodePoint(BufferedTransformation &bt) const
{
	SecByteBlock str;
	BERDecodeOctetString(bt, str);
	Point P;
	if (!DecodePoint(P, str, str.size))
		BERDecodeError();
	return P;
}

void EC2N::DEREncodePoint(BufferedTransformation &bt, const Point &P, bool compressed) const
{
	SecByteBlock str(EncodedPointSize(compressed));
	EncodePoint(str, P, compressed);
	DEREncodeOctetString(bt, str);
}

bool EC2N::ValidateParameters(RandomNumberGenerator &rng) const
{
	return m_field->GetModulus().IsIrreducible()
		&& m_a.CoefficientCount() <= m_field->MaxElementBitLength()
		&& m_b.CoefficientCount() <= m_field->MaxElementBitLength() && !!m_b;
}

bool EC2N::VerifyPoint(const Point &P) const
{
	const FieldElement &x = P.x, &y = P.y;
	return P.identity || 
		(x.CoefficientCount() <= m_field->MaxElementBitLength()
		&& y.CoefficientCount() <= m_field->MaxElementBitLength()
		&& !(((x+m_a)*x*x+m_b-(x+y)*y)%m_field->GetModulus()));
}

bool EC2N::Equal(const Point &P, const Point &Q) const
{
	if (P.identity && Q.identity)
		return true;

	if (P.identity && !Q.identity)
		return false;

	if (!P.identity && Q.identity)
		return false;

	return (m_field->Equal(P.x,Q.x) && m_field->Equal(P.y,Q.y));
}

const EC2N::Point& EC2N::Inverse(const Point &P) const
{
	if (P.identity)
		return P;
	else
	{
		m_R.identity = false;
		m_R.y = m_field->Add(P.x, P.y);
		m_R.x = P.x;
		return m_R;
	}
}

const EC2N::Point& EC2N::Add(const Point &P, const Point &Q) const
{
	if (P.identity) return Q;
	if (Q.identity) return P;
	if (Equal(P, Q)) return Double(P);
	if (m_field->Equal(P.x, Q.x) && m_field->Equal(P.y, m_field->Add(Q.x, Q.y))) return Zero();

	FieldElement t = m_field->Add(P.y, Q.y);
	t = m_field->Divide(t, m_field->Add(P.x, Q.x));
	FieldElement x = m_field->Square(t);
	m_field->Accumulate(x, t);
	m_field->Accumulate(x, Q.x);
	m_field->Accumulate(x, m_a);
	m_R.y = m_field->Add(P.y, m_field->Multiply(t, x));
	m_field->Accumulate(x, P.x);
	m_field->Accumulate(m_R.y, x);

	m_R.x.swap(x);
	m_R.identity = false;
	return m_R;
}

const EC2N::Point& EC2N::Double(const Point &P) const
{
	if (P.identity) return P;
	if (!m_field->IsUnit(P.x)) return Zero();

	FieldElement t = m_field->Divide(P.y, P.x);
	m_field->Accumulate(t, P.x);
	m_R.y = m_field->Square(P.x);
	m_R.x = m_field->Square(t);
	m_field->Accumulate(m_R.x, t);
	m_field->Accumulate(m_R.x, m_a);
	m_field->Accumulate(m_R.y, m_field->Multiply(t, m_R.x));
	m_field->Accumulate(m_R.y, m_R.x);

	m_R.identity = false;
	return m_R;
}

// ********************************************************

EcPrecomputation<EC2N>& EcPrecomputation<EC2N>::operator=(const EcPrecomputation<EC2N> &rhs)
{
	m_ec = rhs.m_ec;
	m_ep = rhs.m_ep;
	m_ep.m_group = m_ec.get();
	return *this;
}

void EcPrecomputation<EC2N>::SetCurveAndBase(const EC2N &ec, const EC2N::Point &base)
{
	m_ec.reset(new EC2N(ec));
	m_ep.SetGroupAndBase(*m_ec, base);
}

void EcPrecomputation<EC2N>::Precompute(unsigned int maxExpBits, unsigned int storage)
{
	m_ep.Precompute(maxExpBits, storage);
}

void EcPrecomputation<EC2N>::Load(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	word32 version;
	BERDecodeUnsigned<word32>(seq, version, INTEGER, 1, 1);
	m_ep.m_exponentBase.BERDecode(seq);
	m_ep.m_windowSize = m_ep.m_exponentBase.BitCount() - 1;
	m_ep.m_bases.clear();
	while (!seq.EndReached())
		m_ep.m_bases.push_back(m_ec->BERDecodePoint(seq));
	seq.MessageEnd();
}

void EcPrecomputation<EC2N>::Save(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	DEREncodeUnsigned<word32>(seq, 1);	// version
	m_ep.m_exponentBase.DEREncode(seq);
	for (unsigned i=0; i<m_ep.m_bases.size(); i++)
		m_ec->DEREncodePoint(seq, m_ep.m_bases[i]);
	seq.MessageEnd();
}

EC2N::Point EcPrecomputation<EC2N>::Multiply(const Integer &exponent) const
{
	return m_ep.Exponentiate(exponent);
}

EC2N::Point EcPrecomputation<EC2N>::CascadeMultiply(const Integer &exponent, const EcPrecomputation<EC2N> &pc2, const Integer &exponent2) const
{
	return m_ep.CascadeExponentiate(exponent, pc2.m_ep, exponent2);
}

NAMESPACE_END