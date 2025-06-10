/**
 * This file is a copy of the rsa.h file from mbedCrypto
 * \file rsa_alt.h
 *
 * \brief This file contains RSA Alternate definitions and functions.
 */

/*
 *  Copyright (C) 2006-2018, Arm Limited (or its affiliates), All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of Mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_RSA_ALT_H
 #define MBEDTLS_RSA_ALT_H

 #include "mbedtls/bignum.h"
 #include "mbedtls/md.h"
 #include "hw_sce_rsa_private.h"
 #include "hw_sce_ra_private.h"

 #ifdef __cplusplus
extern "C" {
 #endif

 #if defined(MBEDTLS_RSA_ALT)

// Alternate implementation
//

/** Return RSA modulus size in bytes from the specified RSA modulus size in bits */
  #define RSA_MODULUS_SIZE_BYTES(RSA_SIZE_BITS)                     ((RSA_SIZE_BITS) / 8U)

/** Return RSA public key size in bytes from the specified RSA modulus size in bits */
  #define RSA_PLAIN_TEXT_PUBLIC_KEY_SIZE_BYTES(RSA_SIZE_BITS)       (((uint32_t) 32 + (uint32_t) RSA_SIZE_BITS) / 8U)

/** Return RSA private key size in bytes from the specified RSA modulus size in bits */
  #define RSA_PLAIN_TEXT_PRIVATE_KEY_SIZE_BYTES(RSA_SIZE_BITS)      (((uint32_t) 2 * (uint32_t) RSA_SIZE_BITS) / 8U)

/** Return RSA wrapped private CRT key size in bytes from the specified RSA modulus size in bits */
  #define RSA_WRAPPPED_PRIVATE_CRT_KEY_SIZE_BYTES(RSA_SIZE_BITS)    ((RSA_PLAIN_TEXT_CRT_KEY_SIZE_BYTES(RSA_SIZE_BITS)) \
                                                                     +                                                  \
                                                                     20U)

  #define SCE_RSA_NUM_TRIES_5120     (5120U)  // NIST FIPS 186-4 recommended value for 1024-bit key generation.
  #define SCE_RSA_NUM_TRIES_10240    (10240U) // NIST FIPS 186-4 recommended value for 2048-bit key generation.
  #define SCE_RSA_NUM_TRIES_20480    (20480U) // Double the "NIST FIPS 186-4 recommended value for 2048-bit key generation" due to empirically observed periodic failure from HW implementation

/** RSA Public Exponent (E) in little endian form  */
  #define RSA_PUBLIC_EXPONENT_LE     (0x01000100)

/** RSA Public Exponent (E) in big endian form  */
  #define RSA_PUBLIC_EXPONENT_BE     (0x00010001)

/** RSA2048 bits */
  #define RSA_2048_BITS              (2048)

/** RSA3072 bits */
  #define RSA_3072_BITS              (3072)

/** RSA4096 bits */
  #define RSA_4096_BITS              (4096)

/**
 * \brief   The RSA context structure.
 *
 * \note    Direct manipulation of the members of this structure
 *          is deprecated. All manipulation should instead be done through
 *          the public interface functions.
 */
typedef struct mbedtls_rsa_context
{
    int    ver;                        /*!<  Always 0. */
    size_t len;                        /*!<  The size of \p N in Bytes. */

    mbedtls_mpi N;                     /*!<  The public modulus. */
    mbedtls_mpi E;                     /*!<  The public exponent. */

    mbedtls_mpi D;                     /*!<  The private exponent. */
    mbedtls_mpi P;                     /*!<  The first prime factor. */
    mbedtls_mpi Q;                     /*!<  The second prime factor. */

    mbedtls_mpi DP;                    /*!<  <code>D % (P - 1)</code>. */
    mbedtls_mpi DQ;                    /*!<  <code>D % (Q - 1)</code>. */
    mbedtls_mpi QP;                    /*!<  <code>1 / (Q % P)</code>. */

    mbedtls_mpi RN;                    /*!<  cached <code>R^2 mod N</code>. */

    mbedtls_mpi RP;                    /*!<  cached <code>R^2 mod P</code>. */
    mbedtls_mpi RQ;                    /*!<  cached <code>R^2 mod Q</code>. */

    mbedtls_mpi Vi;                    /*!<  The cached blinding value. */
    mbedtls_mpi Vf;                    /*!<  The cached un-blinding value. */

    int padding;                       /*!< Selects padding mode:
                                        #MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                        #MBEDTLS_RSA_PKCS_V21 for OAEP or PSS. */
    int hash_id;                       /*!< Hash identifier of mbedtls_md_type_t type,
                                        *   as specified in md.h for use in the MGF
                                        *   mask generating function used in the
                                        *   EME-OAEP and EMSA-PSS encodings. */
    void * vendor_ctx;                 /*!< Vendor defined context. */
  #if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;   /*!<  Thread-safety mutex. */
  #endif
} mbedtls_rsa_context;

 #endif /* MBEDTLS_RSA_ALT */

 #ifdef __cplusplus
}
 #endif

#endif                                 /* MBEDTLS_RSA_ALT_H */
