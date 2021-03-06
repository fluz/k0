#ifndef K0_SCHEME_KDF_HPP
#define K0_SCHEME_KDF_HPP

#include <libff/common/utils.hpp>
using namespace libff;

namespace k0 {

void kdf(unsigned char key[32], const unsigned char dhsecret[32],
         const unsigned char epk[32], const unsigned char pk_enc[32]);
}

#endif // K0_SCHEME_KDF_HAPP
