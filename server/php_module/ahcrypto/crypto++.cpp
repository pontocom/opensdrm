#include "crypto++/cryptlib.h"

#include <stdlib.h>
#include <time.h>

#include "crypto++/integer.h"
#include "crypto++/randpool.h"
#include "crypto++/osrng.h"
#include "crypto++/rsa.h"
#include "crypto++/md5.h"

#include "crypto++/rijndael.h"

#include "php.h"

extern "C" {

	void internal_rsa_create_keys(
			unsigned int keysize,
			unsigned char * * n_dat, size_t * n_len,
			unsigned char * * e_dat, size_t * e_len,
			unsigned char * * d_dat, size_t * d_len,
			unsigned char * * p_dat, size_t * p_len,
			unsigned char * * q_dat, size_t * q_len)
	{
		// Initializing the random generator.
		srand( (unsigned)time( NULL ) );
		CryptoPP::RandomPool randPool;
		int seed = rand();
		randPool.Put((byte *)&seed, sizeof(seed));

		// Generating an RSA key-pair.

		CryptoPP::RSAES_PKCS1v15_Decryptor priv(randPool, keysize);
		CryptoPP::RSAES_PKCS1v15_Encryptor pub(priv);

		const CryptoPP::Integer & n = (priv.GetTrapdoorFunction()).GetModulus();
		const CryptoPP::Integer & e = (priv.GetTrapdoorFunction()).GetExponent();
		const CryptoPP::Integer & d = (priv.GetTrapdoorFunction()).GetDecryptionExponent();
		const CryptoPP::Integer & p = (priv.GetTrapdoorFunction()).GetPrime1();
		const CryptoPP::Integer & q = (priv.GetTrapdoorFunction()).GetPrime2();

		*n_dat = (unsigned char *)emalloc(*n_len = n.ByteCount());
		*e_dat = (unsigned char *)emalloc(*e_len = e.ByteCount());
		*d_dat = (unsigned char *)emalloc(*d_len = d.ByteCount());
		*p_dat = (unsigned char *)emalloc(*p_len = p.ByteCount());
		*q_dat = (unsigned char *)emalloc(*q_len = q.ByteCount());

		n.Encode(*n_dat, *n_len);
		e.Encode(*e_dat, *e_len);
		d.Encode(*d_dat, *d_len);
		p.Encode(*p_dat, *p_len);
		q.Encode(*q_dat, *q_len);
	}

	void internal_rsa_cipher(
		unsigned char * clear_data, size_t clear_data_len,
		unsigned char * n_dat, size_t n_len,
		unsigned char * e_dat, size_t e_len,
		unsigned char * * cryptogram, size_t * cryptogram_len)
	{
		CryptoPP::Integer n, e;
		n.Decode(n_dat, n_len);
		e.Decode(e_dat, e_len);

		CryptoPP::RSAES_PKCS1v15_Encryptor pub(n, e);

		// Initializing the random generator.
		srand( (unsigned)time( NULL ) );
		CryptoPP::RandomPool rng;
		int seed = rand();
		rng.Put((byte *)&seed, sizeof(seed));

		*cryptogram_len = pub.CipherTextLength();
		*cryptogram = (unsigned char *)emalloc(*cryptogram_len);

		pub.Encrypt(rng, clear_data, clear_data_len, *cryptogram);
	}

	void internal_rsa_decipher(
		unsigned char * cryptogram, size_t cryptogram_len,
		unsigned char * n_dat, size_t n_len,
		unsigned char * e_dat, size_t e_len,
		unsigned char * d_dat, size_t d_len,
		unsigned char * p_dat, size_t p_len,
		unsigned char * q_dat, size_t q_len,
		unsigned char * * clear_data, size_t * clear_data_len)
	{
		CryptoPP::Integer n, e, d, p, q;
		n.Decode(n_dat, n_len);
		e.Decode(e_dat, e_len);
		d.Decode(d_dat, d_len);
		p.Decode(p_dat, p_len);
		q.Decode(q_dat, q_len);

		CryptoPP::RSAES_PKCS1v15_Decryptor prv(
			n,
			e,
			d,
			p,
			q,
			d.Modulo(p.Minus(CryptoPP::Integer::One())),
			d.Modulo(q.Minus(CryptoPP::Integer::One())),
			q.InverseMod(p)
		);

		srand( (unsigned)time( NULL ) );
		CryptoPP::RandomPool rng;
		int seed = rand();
		rng.Put((byte *)&seed, sizeof(seed));

		*clear_data = (unsigned char *)emalloc(cryptogram_len);
		memset(*clear_data, 0, cryptogram_len);

		*clear_data_len = prv.Decrypt(cryptogram, *clear_data);
	}

	void internal_rsa_sign(
		unsigned char * data, size_t data_len,
		unsigned char * n_dat, size_t n_len,
		unsigned char * e_dat, size_t e_len,
		unsigned char * d_dat, size_t d_len,
		unsigned char * p_dat, size_t p_len,
		unsigned char * q_dat, size_t q_len,
		unsigned char * * signature, size_t * signature_len)
	{
		CryptoPP::Integer n, e, d, p, q;
		n.Decode(n_dat, n_len);
		e.Decode(e_dat, e_len);
		d.Decode(d_dat, d_len);
		p.Decode(p_dat, p_len);
		q.Decode(q_dat, q_len);

		CryptoPP::RSASSA_PKCS1v15_MD5_Signer prv(
			n,
			e,
			d,
			p,
			q,
			d.Modulo(p.Minus(CryptoPP::Integer::One())),
			d.Modulo(q.Minus(CryptoPP::Integer::One())),
			q.InverseMod(p)
		);

		srand( (unsigned)time( NULL ) );
		CryptoPP::RandomPool rng;
		int seed = rand();
		rng.Put((byte *)&seed, sizeof(seed));

		*signature_len = prv.SignatureLength();
		*signature = (unsigned char *)emalloc(*signature_len + 1);
		memset(*signature, 0, *signature_len);

		prv.SignMessage(rng, data, data_len, *signature);

		(*signature)[*signature_len + 1] = '\0';
	}

	int internal_rsa_verify(
		unsigned char * data, size_t data_len,
		unsigned char * signature, size_t signature_len,
		unsigned char * n_dat, size_t n_len,
		unsigned char * e_dat, size_t e_len)
	{
		CryptoPP::Integer n, e;
		n.Decode(n_dat, n_len);
		e.Decode(e_dat, e_len);

		CryptoPP::RSASSA_PKCS1v15_MD5_Verifier pub(n, e);

		return pub.VerifyMessage(data, data_len, signature) ? 1 : 0;
	}

	void internal_aes_cipher(
		unsigned char * clear_data, size_t clear_data_len,
		unsigned char * k_dat, size_t k_len,
		unsigned char * * cryptogram, size_t * cryptogram_len)
	{
		// NOTE: blocks of 16 bytes (128 bits)
		const size_t BLOCK_SIZE = 16;

		// Allocate the return area
		unsigned char padding_len = (BLOCK_SIZE - ((clear_data_len + 1) % BLOCK_SIZE));
		*cryptogram_len = BLOCK_SIZE + clear_data_len + padding_len + 1;
		*cryptogram = (unsigned char *)emalloc(*cryptogram_len);

		// Generate an IV
		unsigned char * pIV = *cryptogram;
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			pIV[i] = (unsigned char)rand();
		}

		// Create the cipher object.
		CryptoPP::RijndaelEncryption aes(k_dat, k_len);

		// Prepare index pointers.
		unsigned char * prev_blk = pIV;
		unsigned char * curr_src = clear_data;
		unsigned char * curr_dst = (*cryptogram) + BLOCK_SIZE;
		unsigned char * curr_blk; 

		// Go.
		size_t remaining_data_len = clear_data_len;
		while (remaining_data_len >= BLOCK_SIZE) {
			curr_blk = curr_dst;
			for (int j = 0; j < BLOCK_SIZE; ++j) {
				*curr_dst++ = *prev_blk++ ^ *curr_src++;
			}
			aes.ProcessBlock(curr_blk);
			remaining_data_len -= BLOCK_SIZE;
		}

		// Last block
		curr_blk = curr_dst;
		for (size_t x = 0; x < remaining_data_len; ++x) {
			*curr_dst++ = *prev_blk++ ^ *curr_src++;
		}
		for (size_t y = 0; y < padding_len; ++y) {
			*curr_dst++ = *prev_blk++ ^ (unsigned char)rand();
		}
		*curr_dst++ = *prev_blk++ ^ padding_len;
		aes.ProcessBlock(curr_blk);
	}

	void internal_aes_decipher(
		unsigned char * cryptogram, size_t cryptogram_len,
		unsigned char * k_dat, size_t k_len,
		unsigned char * * clear_data, size_t * clear_data_len)
	{
		// NOTE: blocks of 16 bytes (128 bits)
		const size_t BLOCK_SIZE = 16;

		// Allocate the return area
		*clear_data_len = cryptogram_len - BLOCK_SIZE;
		*clear_data = (unsigned char *)emalloc(*clear_data_len);

		// Create the decipher object.
		CryptoPP::RijndaelDecryption aes(k_dat, k_len);

		// Prepare index pointers.
		unsigned char * prev_blk = cryptogram;
		unsigned char * curr_src = cryptogram + BLOCK_SIZE;
		unsigned char * curr_dst = *clear_data;
		unsigned char * curr_blk; 

		// Go.
		int remaining_data_len = cryptogram_len - BLOCK_SIZE;
		while (remaining_data_len >= (int)BLOCK_SIZE) {
			curr_blk = curr_dst;
			memcpy(curr_dst, curr_src, BLOCK_SIZE);
			curr_src += BLOCK_SIZE;
			curr_dst += BLOCK_SIZE;
			aes.ProcessBlock(curr_blk);
			for (int j = 0; j < BLOCK_SIZE; ++j) {
				*curr_blk++ ^= *prev_blk++;
			}
			remaining_data_len -= BLOCK_SIZE;
		}
		*clear_data_len -= *(curr_blk - 1) + 1;
	}
}