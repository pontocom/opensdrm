#ifndef PHP_AHCRYPTO_H
#define PHP_AHCRYPTO_H

#ifdef PHP_WIN32
# undef PHP_AHCRYPTO_API
# ifdef AHCRYPTO_EXPORTS
#  define PHP_AHCRYPTO_API __declspec(dllexport)
# else
#  define PHP_AHCRYPTO_API __declspec(dllimport)
# endif
#else
# undef PHP_AHCRYPTO_API
# define PHP_AHCRYPTO_API /* nothing special */
#endif

extern zend_module_entry ahcrypto_module_entry;
#define phpext_ahcrypto_ptr &ahcrypto_module_entry

PHP_MINIT_FUNCTION(ahcrypto);
PHP_MSHUTDOWN_FUNCTION(ahcrypto);
PHP_MINFO_FUNCTION(ahcrypto);

PHP_FUNCTION(ahrsa_create_keys);
PHP_FUNCTION(ahrsa_cipher);
PHP_FUNCTION(ahrsa_decipher);
PHP_FUNCTION(ahrsa_sign);
PHP_FUNCTION(ahrsa_verify);

PHP_FUNCTION(ahaes_cipher);
PHP_FUNCTION(ahaes_decipher);

#endif
