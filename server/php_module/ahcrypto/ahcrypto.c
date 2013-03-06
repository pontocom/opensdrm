/* include standard header */
/* you will need to include this in all of your php extension projects*/
#include "php.h" 
#include "php_ahcrypto.h"

// external function defined in crypto++.cpp
void internal_rsa_create_keys(
	unsigned int keysize,
	unsigned char * * n_dat, size_t * n_len,
	unsigned char * * e_dat, size_t * e_len,
	unsigned char * * d_dat, size_t * d_len,
	unsigned char * * p_dat, size_t * p_len,
	unsigned char * * q_dat, size_t * q_len
);

void internal_rsa_cipher(
	unsigned char * clear_data, size_t clear_data_len,
	unsigned char * n_dat, size_t n_len,
	unsigned char * e_dat, size_t e_len,
	unsigned char * * cryptogram, size_t * cryptogram_len
);

void internal_rsa_decipher(
	unsigned char * cryptogram, size_t cryptogram_len,
	unsigned char * n_dat, size_t n_len,
	unsigned char * e_dat, size_t e_len,
	unsigned char * d_dat, size_t d_len,
	unsigned char * p_dat, size_t p_len,
	unsigned char * q_dat, size_t q_len,
	unsigned char * * clear_data, size_t * clear_data_len
);

void internal_rsa_sign(
	unsigned char * data, size_t data_len,
	unsigned char * n_dat, size_t n_len,
	unsigned char * e_dat, size_t e_len,
	unsigned char * d_dat, size_t d_len,
	unsigned char * p_dat, size_t p_len,
	unsigned char * q_dat, size_t q_len,
	unsigned char * * signature, size_t * signature_len
);

int internal_rsa_verify(
	unsigned char * data, size_t data_len,
	unsigned char * signature, size_t signature_len,
	unsigned char * n_dat, size_t n_len,
	unsigned char * e_dat, size_t e_len
);

void internal_aes_cipher(
	unsigned char * clear_data, size_t clear_data_len,
	unsigned char * k_dat, size_t k_len,
	unsigned char * * cryptogram, size_t * cryptogram_len
);

void internal_aes_decipher(
	unsigned char * cryptogram, size_t cryptogram_len,
	unsigned char * k_dat, size_t k_len,
	unsigned char * * clear_data, size_t * clear_data_len
);

function_entry ahcrypto_functions[] = {
	PHP_FE(ahrsa_create_keys, NULL)
	PHP_FE(ahrsa_cipher,      NULL)
	PHP_FE(ahrsa_decipher,    NULL)
	PHP_FE(ahrsa_sign,        NULL)
	PHP_FE(ahrsa_verify,      NULL)
	PHP_FE(ahaes_cipher,      NULL)
	PHP_FE(ahaes_decipher,    NULL)
	{NULL, NULL, NULL}
};

zend_module_entry ahcrypto_module_entry = {
	STANDARD_MODULE_HEADER,
	"AHCRYPTO",
	ahcrypto_functions,
	PHP_MINIT(ahcrypto),
	PHP_MSHUTDOWN(ahcrypto),
	NULL,
	NULL,
	PHP_MINFO(ahcrypto),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL_AHCRYPTO
ZEND_GET_MODULE(ahcrypto)
#endif 

PHP_MINIT_FUNCTION(ahcrypto)
{
	return 0;
}

PHP_MSHUTDOWN_FUNCTION(ahcrypto)
{
	return 0;
}

PHP_MINFO_FUNCTION(ahcrypto)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Made by", "ADETTI, 2004");
	php_info_print_table_end();
} 

PHP_FUNCTION(ahrsa_create_keys)
{
	int size;

	unsigned char * n_dat = 0, * e_dat = 0, * d_dat = 0, * p_dat = 0, * q_dat = 0;
	size_t n_len = 0, e_len = 0, d_len = 0, p_len = 0, q_len = 0;

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size) == FAILURE) {
		RETURN_FALSE;
	}

	internal_rsa_create_keys(size, &n_dat, &n_len, &e_dat, &e_len, &d_dat, &d_len,
	                                               &p_dat, &p_len, &q_dat, &q_len);

	array_init(return_value);
	add_assoc_stringl(return_value, "n", n_dat, n_len, 1);
	add_assoc_stringl(return_value, "e", e_dat, e_len, 1);
	add_assoc_stringl(return_value, "d", d_dat, d_len, 1);
	add_assoc_stringl(return_value, "p", p_dat, p_len, 1);
	add_assoc_stringl(return_value, "q", q_dat, q_len, 1);

	efree(n_dat);
	efree(e_dat);
	efree(d_dat);
	efree(p_dat);
	efree(q_dat);
}

PHP_FUNCTION(ahrsa_cipher)
{
	unsigned char * clear_data;
	unsigned char * n_dat, * e_dat;
	size_t clear_data_len, n_len, e_len;

	unsigned char * cryptogram;
	size_t cryptogram_len;

	if(ZEND_NUM_ARGS() != 3) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
			&clear_data, &clear_data_len,
			&n_dat, &n_len,
			&e_dat, &e_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	if (clear_data_len > n_len - 11) return; // message too long

	internal_rsa_cipher(clear_data, clear_data_len, n_dat, n_len, e_dat, e_len, &cryptogram, &cryptogram_len);

	if (cryptogram != NULL && cryptogram_len != 0) {
		RETVAL_STRINGL(cryptogram, cryptogram_len, TRUE);
	}

	if (cryptogram != NULL) efree(cryptogram);
}

PHP_FUNCTION(ahrsa_decipher)
{
	unsigned char * cryptogram;
	unsigned char * n_dat, * e_dat, *d_dat, *p_dat, *q_dat;
	size_t cryptogram_len, n_len, e_len, d_len, p_len, q_len;

	unsigned char * clear_data;
	size_t clear_data_len;

	if(ZEND_NUM_ARGS() != 6) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssss",
			&cryptogram, &cryptogram_len,
			&n_dat, &n_len,
			&e_dat, &e_len,
			&d_dat, &d_len,
			&p_dat, &p_len,
			&q_dat, &q_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	internal_rsa_decipher(
		cryptogram, cryptogram_len,
		n_dat, n_len,
		e_dat, e_len,
		d_dat, d_len,
		p_dat, p_len,
		q_dat, q_len,
		&clear_data, &clear_data_len
	);

	if (clear_data != NULL && clear_data_len != 0) {
		RETVAL_STRINGL(clear_data, clear_data_len, TRUE);
	}

	if (clear_data != NULL) efree(clear_data);
}

PHP_FUNCTION(ahrsa_sign)
{
	unsigned char * data;
	unsigned char * n_dat, * e_dat, *d_dat, *p_dat, *q_dat;
	size_t data_len, n_len, e_len, d_len, p_len, q_len;

	unsigned char * signature;
	size_t signature_len;

	if(ZEND_NUM_ARGS() != 6) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssssss",
			&data, &data_len,
			&n_dat, &n_len,
			&e_dat, &e_len,
			&d_dat, &d_len,
			&p_dat, &p_len,
			&q_dat, &q_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	internal_rsa_sign(
		data, data_len,
		n_dat, n_len,
		e_dat, e_len,
		d_dat, d_len,
		p_dat, p_len,
		q_dat, q_len,
		&signature, &signature_len
	);

	if (signature != NULL && signature_len != 0) {
		RETVAL_STRINGL(signature, signature_len, TRUE);
	}

	if (signature != NULL) efree(signature);
}

PHP_FUNCTION(ahrsa_verify)
{
	unsigned char * data, * signature;
	unsigned char * n_dat, * e_dat;
	size_t data_len, signature_len, n_len, e_len;

	if(ZEND_NUM_ARGS() != 4) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss",
			&data, &data_len,
			&signature, &signature_len,
			&n_dat, &n_len,
			&e_dat, &e_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	if (internal_rsa_verify(
			data, data_len,
			signature, signature_len,
			n_dat, n_len,
			e_dat, e_len
		)) RETVAL_TRUE;
}

PHP_FUNCTION(ahaes_cipher)
{
	unsigned char * clear_data;
	unsigned char * k_dat;
	size_t clear_data_len, k_len;

	unsigned char * cryptogram;
	size_t cryptogram_len;

	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			&clear_data, &clear_data_len,
			&k_dat, &k_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	if (k_len != 16 && k_len != 24 && k_len != 32) return; // invalid key

	internal_aes_cipher(clear_data, clear_data_len, k_dat, k_len, &cryptogram, &cryptogram_len);

	if (cryptogram != NULL && cryptogram_len != 0) {
		RETVAL_STRINGL(cryptogram, cryptogram_len, TRUE);
	}

	if (cryptogram != NULL) efree(cryptogram);
}

PHP_FUNCTION(ahaes_decipher)
{
	unsigned char * cryptogram;
	unsigned char * k_dat;
	size_t cryptogram_len, k_len;

	unsigned char * clear_data;
	size_t clear_data_len;

	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;

	/* Extract parameters */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			&cryptogram, &cryptogram_len,
			&k_dat, &k_len) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	if (k_len != 16 && k_len != 24 && k_len != 32) return; // invalid key

	internal_aes_decipher(
		cryptogram, cryptogram_len,
		k_dat, k_len,
		&clear_data, &clear_data_len
	);

	if (clear_data != NULL && clear_data_len != 0) {
		RETVAL_STRINGL(clear_data, clear_data_len, TRUE);
	}

	if (clear_data != NULL) efree(clear_data);
}

