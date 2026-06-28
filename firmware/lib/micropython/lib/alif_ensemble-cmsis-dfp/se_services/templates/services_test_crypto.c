/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/**
 * @file  services_test_crypto.c
 * @brief Crypto Services test harness
 * @ingroup services
 * @par
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "services_lib_api.h"
#ifdef A32_LINUX
#include "services_lib_linux.h"
#else
#include "services_lib_interface.h"
#endif

static char s_print_buf[256] = {0};

/**
 * @fn void fill_print_buf(uint8_t*, uint32_t)
 * @brief
 * @param buf
 * @param length
 */
static void fill_print_buf(uint8_t *buf, uint32_t length)
{
  length *= 2; // 1 byte == 2 characters
  s_print_buf[0] = 0;
  for (uint32_t i = 0; i < length; i++)
  {
    char temp_buf[10];
    sprintf(temp_buf, "%02X", buf[i]);
    strcat(s_print_buf, temp_buf);
  }
  s_print_buf[length] = 0;
}

/**
 * @brief Test MBEDTLS AES
 */
static void test_mbedtls_aes(uint32_t services_handle)
{
  // Test vectors
  // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_OFB.pdf
  // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CBC.pdf
  // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CTR.pdf
  // https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_ECB.pdf
  static const uint8_t PLAIN[] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
                                   0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
  static const uint8_t KEY[] = { 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
                                 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
                                 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
                                 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4 };

  // Mode-specific vectors and parameters
  // OFB, CBC
  static const uint8_t IV[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
  // CTR
  // IV is called NONCE, and is passed via the same parameter 'iv'
  // static const uint8_t NONCE[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };
  // ECB
  // the parameter 'iv' is not used

  // Expected ciphertext
  // OFB
  // static const uint8_t CYPHER[] = { 0xDC, 0x7E, 0x84, 0xBF, 0xDA, 0x79, 0x16, 0x4B, 0x7E, 0xCD, 0x84, 0x86, 0x98, 0x5D, 0x38, 0x60 };
  // CBC
  // static const uint8_t CYPHER[] = { 0xF5, 0x8C, 0x4C, 0x04, 0xD6, 0xE5, 0xF1, 0xBA, 0x77, 0x9E, 0xAB, 0xFB, 0x5F, 0x7B, 0xFB, 0xD6 };
  // CTR
  // static const uint8_t CYPHER[] = { 0x60, 0x1E, 0xC3, 0x13, 0x77, 0x57, 0x89, 0xA5, 0xB7, 0xA7, 0xF5, 0x04, 0xBB, 0xF3, 0xD2, 0x28 };
  // ECB
  // static const uint8_t CYPHER[] = { 0xF3, 0xEE, 0xD1, 0xBD, 0xB5, 0xD2, 0xA0, 0x3C, 0x06, 0x4B, 0x5A, 0x7E, 0x3D, 0xB1, 0x81, 0xF8 };

  // The parameter 'mode' is only used in ECB and CBC modes, not used in CTR and OFB

  uint8_t key[MBEDTLS_AES_KEY_256 / 8];
  uint8_t iv[MBEDTLS_AES_BLOCK_SIZE];
  uint8_t buf[MBEDTLS_AES_BLOCK_SIZE];

  uint32_t aes_ctx[24] = {0};

  uint32_t error_code;

  // Multi-part AES tests
  TEST_print(services_handle, "Multi-part AES:\n");

  // Initialize aes engine
  SERVICES_cryptocell_mbedtls_aes_init(services_handle, &error_code,
                                       (uint32_t)aes_ctx);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes_init           service_resp=0x%08X\n",
             error_code);

  memcpy(key, KEY, sizeof(key));
  memcpy(buf, PLAIN, sizeof(buf));

  // encrypt
  memcpy(iv, IV, sizeof(iv));
  // set key into context
  SERVICES_cryptocell_mbedtls_aes_set_key(services_handle, 
                                          &error_code,
                                          (uint32_t)aes_ctx, 
                                          (uint32_t)key, 
                                          MBEDTLS_AES_KEY_256,
                                          MBEDTLS_OP_ENCRYPT);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes_set_key        service_resp=0x%08X\n",
             error_code);

  // perform cryptographic operation
  SERVICES_cryptocell_mbedtls_aes_crypt(services_handle, 
                                        &error_code,
                                        (uint32_t)aes_ctx,
                                        MBEDTLS_AES_CRYPT_OFB,
                                        MBEDTLS_OP_ENCRYPT,     // not used in CTR and OFB modes
                                        sizeof(buf), 
                                        (uint32_t)iv,           // not used in ECB mode
                                        (uint32_t)buf, 
                                        (uint32_t)buf);

  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes_crypt          service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, MBEDTLS_AES_BLOCK_SIZE);
  TEST_print(services_handle, "Encrypted data: %s\n", s_print_buf);

  // decrypt
  memcpy(iv, IV, sizeof(iv));
  // set key into context
  SERVICES_cryptocell_mbedtls_aes_set_key(services_handle, 
                                          &error_code,
                                          (uint32_t)aes_ctx, 
                                          (uint32_t)key, 
                                          MBEDTLS_AES_KEY_256,
                                          MBEDTLS_OP_DECRYPT);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes_set_key        service_resp=0x%08X\n",
             error_code);

  // perform cryptographic operation
  SERVICES_cryptocell_mbedtls_aes_crypt(services_handle, 
                                        &error_code,
                                        (uint32_t)aes_ctx,
                                        MBEDTLS_AES_CRYPT_OFB,
                                        MBEDTLS_OP_DECRYPT,     // not used in CTR and OFB modes
                                        sizeof(buf), 
                                        (uint32_t)iv,           // not used in ECB mode
                                        (uint32_t)buf,
                                        (uint32_t)buf);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes_crypt          service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, MBEDTLS_AES_BLOCK_SIZE);
  TEST_print(services_handle, "Decrypted data: %s\n", s_print_buf);

  // Single-part AES tests
  TEST_print(services_handle, "Singe-part AES:\n");

  // encrypt
  memcpy(iv, IV, sizeof(iv));
  SERVICES_cryptocell_mbedtls_aes(services_handle,
      &error_code,
      (uint32_t)key,
      MBEDTLS_AES_KEY_256,
      MBEDTLS_OP_ENCRYPT,
      MBEDTLS_AES_CRYPT_OFB,
      (uint32_t)iv,
      sizeof(buf),
      (uint32_t)buf,
      (uint32_t)buf);

  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes - encrypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, MBEDTLS_AES_BLOCK_SIZE);
  TEST_print(services_handle, "Encrypted data: %s\n", s_print_buf);

  // decrypt
  memcpy(iv, IV, sizeof(iv));
  SERVICES_cryptocell_mbedtls_aes(services_handle,
      &error_code,
      (uint32_t)key,
      MBEDTLS_AES_KEY_256,
      MBEDTLS_OP_DECRYPT,
      MBEDTLS_AES_CRYPT_OFB,
      (uint32_t)iv,
      sizeof(buf),
      (uint32_t)buf,
      (uint32_t)buf);

  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_aes - decrypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, MBEDTLS_AES_BLOCK_SIZE);
  TEST_print(services_handle, "Decrypted data: %s\n", s_print_buf);
}

/**
 * @brief Test MBEDTLS SHA
 */
static void test_mbedtls_sha(uint32_t services_handle)
{
#define SHA256_BYTES         32  // 256 bits

  char * test_payload = "SHA 256";

  uint32_t sha_ctx[60] = {0};
  uint8_t sha256sum[SHA256_BYTES];
  uint32_t error_code;

  // Multi-part SHA tests
  TEST_print(services_handle, "Multi-part SHA:\n");

  SERVICES_cryptocell_mbedtls_sha_starts(services_handle,
                                         &error_code,
                                         (uint32_t)sha_ctx,
                                         MBEDTLS_HASH_SHA256);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_sha_starts         service_resp=0x%08X\n",
             error_code);

  SERVICES_cryptocell_mbedtls_sha_update(services_handle,
                                         &error_code,
                                         (uint32_t)sha_ctx,
                                         MBEDTLS_HASH_SHA256,
                                         (uint32_t)test_payload,
                                         strlen(test_payload));
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_sha_update         service_resp=0x%08X\n",
             error_code);

  SERVICES_cryptocell_mbedtls_sha_finish(services_handle,
                                         &error_code,
                                         (uint32_t)sha_ctx,
                                         MBEDTLS_HASH_SHA256,
                                         (uint32_t)sha256sum);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_sha_finish         service_resp=0x%08X\n",
             error_code);

  // Expected result: 0d5bad3f01155a5ec3e352d2925eee7700af0225f7891beccd1dc1ddef50393f
  fill_print_buf(sha256sum, SHA256_BYTES);
  TEST_print(services_handle, "SHA256 sum: %s\n", s_print_buf);

  // Single-part SHA tests
  TEST_print(services_handle, "Single-part SHA:\n");

  memset(sha256sum, 0, sizeof(sha256sum));
  SERVICES_cryptocell_mbedtls_sha(services_handle,
                                  &error_code,
                                  MBEDTLS_HASH_SHA256,
                                  (uint32_t)test_payload,
                                  strlen(test_payload),
                                  (uint32_t)sha256sum);


  fill_print_buf(sha256sum, SHA256_BYTES);
  TEST_print(services_handle, "SHA256 sum: %s\n", s_print_buf);
}

/**
 * @brief Test MBEDTLS CMAC
 */
static void test_mbedtls_cmac(uint32_t services_handle)
{
  uint32_t ctx[17] = {0}; // mbedtls_cipher_context_t
  uint32_t ctx_addr = (uint32_t)&ctx;

  uint8_t cmac_key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  uint32_t cmac_key_bits = 128;

  uint8_t cmac_output[MBEDTLS_AES_BLOCK_SIZE];

  uint8_t block1[] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A, 0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C, 0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51, 0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11, 0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF, 0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17, 0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10};
  uint32_t block1_len = 64;

  // Multi-part CMAC tests
  TEST_print(services_handle, "Multi-part CMAC:\n");

  memset(&ctx, 0, sizeof(ctx));

  uint32_t error_code = SERVICES_REQ_SUCCESS;

  SERVICES_cryptocell_mbedtls_cmac_init_setkey(services_handle, &error_code,
      ctx_addr, (uint32_t)cmac_key, cmac_key_bits);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_cmac_init_setkey   service_resp=0x%08X\n",
             error_code);

  SERVICES_cryptocell_mbedtls_cmac_reset(services_handle, &error_code, ctx_addr);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_cmac_reset         service_resp=0x%08X\n",
             error_code);
  SERVICES_cryptocell_mbedtls_cmac_update(services_handle, &error_code,
      ctx_addr, (uint32_t)block1, block1_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_cmac_update        service_resp=0x%08X\n",
             error_code);
  SERVICES_cryptocell_mbedtls_cmac_finish(services_handle, &error_code,
      ctx_addr, (uint32_t)cmac_output);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_cmac_finish        service_resp=0x%08X\n",
             error_code);

  // Expected result: 51F0BEBF7E3B9D92FC49741779363CFE
  fill_print_buf(cmac_output, MBEDTLS_AES_BLOCK_SIZE);
  TEST_print(services_handle, "CMAC: %s\n", s_print_buf);

  memset(cmac_output, 0, sizeof(cmac_output));
  SERVICES_cryptocell_mbedtls_cmac(services_handle, &error_code,
      (uint32_t)cmac_key, cmac_key_bits,
      (uint32_t)block1, block1_len,
      (uint32_t)cmac_output);

  fill_print_buf(cmac_output, MBEDTLS_AES_BLOCK_SIZE);

  // Single-part CMAC tests
  TEST_print(services_handle, "Single-part CMAC:\n");

  TEST_print(services_handle, "CMAC: %s\n", s_print_buf);
}

/**
 * @brief Test CCM Authenticated Encryption
 */
static void test_mbedtls_ccm(uint32_t services_handle)
{
  /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CCM.pdf */
  static const uint8_t KEY[16] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
  static const uint8_t IV[12] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b };
  const uint8_t AD[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13 };
  const uint8_t MSG[24] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  const uint8_t RES[32] = { 0xE3, 0xB2, 0x01, 0xA9, 0xF5, 0xB7, 0x1A, 0x7A, 0x9B, 0x1C, 0xEA, 0xEC, 0xCD, 0x97, 0xE7, 0x0B, 0x61, 0x76, 0xAA, 0xD9, 0xA4, 0x42, 0x8A, 0xA5, 0x48, 0x43, 0x92, 0xFB, 0xC1, 0xB0, 0x99, 0x51 };
  static const size_t TAG_LEN = 8;

#define DATA_LEN  32
  uint8_t output[DATA_LEN];

#define MBEDTLS_CIPHER_ID_AES      2  // mbedtls_cipher_id_t
  uint32_t ccm_ctx[264];              // MBEDTLS_CCM_CONTEXT_SIZE_IN_WORDS
  uint32_t ctx_addr = (uint32_t)ccm_ctx;
  uint32_t error_code = SERVICES_REQ_SUCCESS;

  // Multi-part CCM tests
  TEST_print(services_handle, "Multi-part CCM:\n");

  memset(ccm_ctx, 0, sizeof(ccm_ctx));

  // Encryption
  SERVICES_cryptocell_mbedtls_ccm_gcm_set_key(services_handle, &error_code,
      ctx_addr,
      MBEDTLS_CCM_KEY, MBEDTLS_CIPHER_ID_AES,
      (uint32_t)KEY, 8 * sizeof(KEY));
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_set_key    service_resp=0x%08X\n",
             error_code);
  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_CCM_ENCRYPT_AND_TAG, sizeof(MSG),
      (uint32_t)IV, sizeof(IV), (uint32_t)AD, sizeof(AD),
      (uint32_t)MSG, (uint32_t)output, (uint32_t)(output + sizeof(MSG)), TAG_LEN);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(output, sizeof(output));
  TEST_print(services_handle, "Encrypted data and tag:\n        %s\n", s_print_buf);

  // Decryption
  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_CCM_AUTH_DECRYPT, sizeof(MSG),
      (uint32_t)IV, sizeof(IV), (uint32_t)AD, sizeof(AD),
      (uint32_t)RES, (uint32_t)output, (uint32_t)(output + sizeof(MSG)), TAG_LEN);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(output, sizeof(output));
  TEST_print(services_handle, "Decrypted data and tag:\n        %s\n", s_print_buf);

  // Single-part CCM tests
  TEST_print(services_handle, "Single-part CCM:\n");

  memset(output, 0, sizeof(output));

  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_CCM_ENCRYPT_AND_TAG, (uint32_t)KEY, 8 * sizeof(KEY),
      sizeof(MSG),
      (uint32_t)IV, sizeof(IV), (uint32_t)AD, sizeof(AD),
      (uint32_t)MSG, (uint32_t)output,
      (uint32_t)(output + sizeof(MSG)), TAG_LEN);

  fill_print_buf(output, sizeof(output));
  TEST_print(services_handle, "Encrypted data and tag:\n        %s\n", s_print_buf);

  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_CCM_AUTH_DECRYPT, (uint32_t)KEY, 8 * sizeof(KEY),
      sizeof(MSG),
      (uint32_t)IV, sizeof(IV), (uint32_t)AD, sizeof(AD),
      (uint32_t)RES, (uint32_t)output,
      (uint32_t)(output + sizeof(MSG)), TAG_LEN);

  fill_print_buf(output, sizeof(output));
  TEST_print(services_handle, "Decrypted data and tag:\n        %s\n", s_print_buf);
}


/* AES-CCM* Security levels (ieee-802.15.4-2011, Table 58) */
#define AESCCM_STAR_SECURITY_LEVEL_ENC          4
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_32   5
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_64   6
#define AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_128  7

#define MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES      8
#define MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES 13

static int mbedtls_ccm_get_security_level(uint8_t sizeOfT, uint8_t *pSecurityLevel)
{
    /*
     The security level field for AES-CCM* as defined in ieee-802.15.4-2011, Table 58.
     System spec requirement CCM*-3: The CCM* shall support only the security levels that include encryption (1XX values).
     */
    switch (sizeOfT)
    {
        case 0:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC;
            break;
        case 4:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_32;
            break;
        case 8:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_64;
            break;
        case 16:
            *pSecurityLevel = AESCCM_STAR_SECURITY_LEVEL_ENC_MIC_128;
            break;
        default:
            return -1;
    }

    return 0;
}

static int mbedtls_ccm_star_nonce_generate(unsigned char * src_addr, uint32_t frame_counter, uint8_t size_of_t, unsigned char * nonce_buf)
{
    int rc = 0;
    uint8_t securityLevelField = 0;

    if ((rc = mbedtls_ccm_get_security_level(size_of_t, &securityLevelField)) != 0)
    {
        return rc;
    }

    /*
     The nonce structure for AES-CCM* is defined in ieee-802.15.4-2011, Figure 61:
     Source address (8) | Frame counter (4) | Security lvel (1)
     */

    memmove(nonce_buf, src_addr, MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES);
    memmove(nonce_buf + MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES, &frame_counter, sizeof(uint32_t));
    nonce_buf[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES - 1] = securityLevelField;

    return 0;
}

/**
 * @brief Test CCM Star Authenticated Encryption
 */
static void test_mbedtls_ccm_star(uint32_t services_handle)
{
  /* https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CCM.pdf */
  static const uint8_t KEY[16] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
  const uint8_t AD[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13 };
  const uint8_t MSG[24] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  const uint8_t RES[] = {0x07, 0x11, 0x7e, 0x35, 0x39, 0xb2, 0xed, 0x4c, 0x84, 0x91, 0x66, 0x07, 0x0b, 0x33, 0xe6, 0x39, 0x0c, 0xf8, 0xda, 0xcc, 0x37, 0x44, 0x00, 0x9d, 0xb1, 0xac, 0x43, 0x81, 0x81, 0x8e, 0x17, 0x6d, 0x54, 0x63, 0x78, 0x4b, 0xd7, 0xd0, 0xc5, 0x46, };

  static const size_t TAG_LEN = 16;

  uint32_t FrameCounter = 5;
  unsigned char nonceBuff[MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES];
  uint8_t srcAddr[16] = { 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e };

  uint8_t buf[40];

  uint32_t ccm_ctx[264];              // MBEDTLS_CCM_CONTEXT_SIZE_IN_WORDS
  uint32_t ctx_addr = (uint32_t)ccm_ctx;
  uint32_t error_code = SERVICES_REQ_SUCCESS;

  // Multi-part CCM Star tests
  TEST_print(services_handle, "Multi-part CCM Star:\n");

  // Encryption
  // generate random buffer
  mbedtls_ccm_star_nonce_generate((uint8_t*)srcAddr, FrameCounter, TAG_LEN, nonceBuff);
  // initialize context
  memset(ccm_ctx, 0, sizeof(ccm_ctx));

  // set key into context
  SERVICES_cryptocell_mbedtls_ccm_gcm_set_key(services_handle, &error_code,
      ctx_addr,
      MBEDTLS_CCM_KEY, MBEDTLS_CIPHER_ID_AES,
      (uint32_t)KEY, 8 * sizeof(KEY));

  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_set_key    service_resp=0x%08X\n",
             error_code);

  memset(buf, 0, sizeof(buf));
  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_CCM_STAR_ENCRYPT_AND_TAG, sizeof(MSG),
      (uint32_t)nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, (uint32_t)AD, sizeof(AD),
      (uint32_t)MSG, (uint32_t)buf, (uint32_t)(buf + sizeof(MSG)), TAG_LEN);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, sizeof(buf));
  TEST_print(services_handle, "Encrypted data and tag:\n        %s\n", s_print_buf);

  // Decryption
  memset(buf, 0, 24);
  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_CCM_STAR_AUTH_DECRYPT, sizeof(MSG),
      (uint32_t)nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, (uint32_t)AD, sizeof(AD),
      (uint32_t)RES, (uint32_t)buf, (uint32_t)(buf + sizeof(MSG)), TAG_LEN);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(buf, sizeof(buf));
  TEST_print(services_handle, "Decrypted data and tag:\n        %s\n", s_print_buf);

  // Single-part CCM Star tests
  TEST_print(services_handle, "Single-part CCM Star:\n");

  // Encryption
  memset(buf, 0, sizeof(buf));
  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_CCM_STAR_ENCRYPT_AND_TAG, (uint32_t)KEY, 8 * sizeof(KEY),
      sizeof(MSG),
      (uint32_t)nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, (uint32_t)AD, sizeof(AD),
      (uint32_t)MSG, (uint32_t)buf,
      (uint32_t)(buf + sizeof(MSG)), TAG_LEN);

  fill_print_buf(buf, sizeof(buf));
  TEST_print(services_handle, "Encrypted data and tag:\n        %s\n", s_print_buf);

  // Decryption
  memset(buf, 0, 24);
  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_CCM_STAR_AUTH_DECRYPT, (uint32_t)KEY, 8 * sizeof(KEY),
      sizeof(MSG),
      (uint32_t)nonceBuff, MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES, (uint32_t)AD, sizeof(AD),
      (uint32_t)RES, (uint32_t)buf,
      (uint32_t)(buf + sizeof(MSG)), TAG_LEN);

  fill_print_buf(buf, sizeof(buf));
  TEST_print(services_handle, "Decrypted data and tag:\n        %s\n", s_print_buf);
}

/**
 * @brief Test GCM Authenticated Encryption
 */
void test_mbedtls_gcm(uint32_t services_handle)
{
  static const uint8_t key[] = {0xFE,0xFF,0xE9,0x92,0x86,0x65,0x73,0x1C,0x6D,0x6A,0x8F,0x94,0x67,0x30,0x83,0x08};
  static const uint8_t iv[] = {0xCA,0xFE,0xBA,0xBE,0xFA,0xCE,0xDB,0xAD,0xDE,0xCA,0xF8,0x88};
  static const size_t iv_len = sizeof(iv);
  static const uint8_t additional[] = {0x3A,0xD7,0x7B,0xB4,0x0D,0x7A,0x36,0x60,0xA8,0x9E,0xCA,0xF3,0x24,0x66,0xEF,0x97,0xF5,0xD3,0xD5,0x85,0x03,0xB9,0x69,0x9D,0xE7,0x85,0x89,0x5A,0x96,0xFD,0xBA,0xAF,0x43,0xB1,0xCD,0x7F,0x59,0x8E,0xCE,0x23,0x88,0x1B,0x00,0xE3,0xED,0x03,0x06,0x88,0x7B,0x0C,0x78,0x5E,0x27,0xE8,0xAD,0x3F,0x82,0x23,0x20,0x71,0x04,0x72,0x5D,0xD4};
  static const size_t add_len = sizeof(additional);
  static const uint8_t pt[] = {0xD9,0x31,0x32,0x25,0xF8,0x84,0x06,0xE5,0xA5,0x59,0x09,0xC5,0xAF,0xF5,0x26,0x9A,0x86,0xA7,0xA9,0x53,0x15,0x34,0xF7,0xDA,0x2E,0x4C,0x30,0x3D,0x8A,0x31,0x8A,0x72,0x1C,0x3C,0x0C,0x95,0x95,0x68,0x09,0x53,0x2F,0xCF,0x0E,0x24,0x49,0xA6,0xB5,0x25,0xB1,0x6A,0xED,0xF5,0xAA,0x0D,0xE6,0x57,0xBA,0x63,0x7B,0x39,0x1A,0xAF,0xD2,0x55};
  static const size_t pt_len = sizeof(pt);
  static const uint8_t ct[] = {0x42,0x83,0x1E,0xC2,0x21,0x77,0x74,0x24,0x4B,0x72,0x21,0xB7,0x84,0xD0,0xD4,0x9C,0xE3,0xAA,0x21,0x2F,0x2C,0x02,0xA4,0xE0,0x35,0xC1,0x7E,0x23,0x29,0xAC,0xA1,0x2E,0x21,0xD5,0x14,0xB2,0x54,0x66,0x93,0x1C,0x7D,0x8F,0x6A,0x5A,0xAC,0x84,0xAA,0x05,0x1B,0xA3,0x0B,0x39,0x6A,0x0A,0xAC,0x97,0x3D,0x58,0xE0,0x91,0x47,0x3F,0x59,0x85};
  static const size_t ct_len = sizeof(ct);
  static const uint8_t tag[] = {0x64,0xC0,0x23,0x29,0x04,0xAF,0x39,0x8A,0x5B,0x67,0xC1,0x0B,0x53,0xA5,0x02,0x4D};
  static const size_t tag_len = sizeof(tag);
  static const int key_len = sizeof(key) * 8;

#define MBEDTLS_CIPHER_ID_AES      2  // mbedtls_cipher_id_t
  static uint32_t ctx[40];              // MBEDTLS_GCM_CONTEXT_SIZE_IN_WORDS
  uint32_t ctx_addr = (uint32_t)ctx;
  uint32_t error_code = SERVICES_REQ_SUCCESS;

  // Multi-part GCM tests
  TEST_print(services_handle, "Multi-part GCM:\n");

  memset(ctx, 0, sizeof(ctx));
  uint8_t iv_buf[12];
  memcpy(iv_buf, iv, iv_len);
  uint8_t add_buf[64];
  memcpy(add_buf, additional, add_len);
  uint8_t plain_buf[64];
  memcpy(plain_buf, pt, pt_len);
  uint8_t tag_buf[16];
  memset(tag_buf, 0, sizeof(tag_buf));
  uint8_t cipher_buf[64] = {0};
  memset(cipher_buf, 0, sizeof(cipher_buf));

  // Encryption
  SERVICES_cryptocell_mbedtls_ccm_gcm_set_key(services_handle, &error_code,
      ctx_addr,
      MBEDTLS_GCM_KEY, MBEDTLS_CIPHER_ID_AES,
      (uint32_t)key, key_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_set_key    service_resp=0x%08X\n",
             error_code);
  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_GCM_ENCRYPT_AND_TAG, pt_len,
      (uint32_t)iv_buf, iv_len, (uint32_t)add_buf, add_len,
      (uint32_t)plain_buf, (uint32_t)cipher_buf, (uint32_t)tag_buf, tag_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(cipher_buf, 40);
  TEST_print(services_handle, "Encrypted data (truncated):\n        %s...\n", s_print_buf);
  fill_print_buf(tag_buf, tag_len);
  TEST_print(services_handle, "Tag:  %s\n", s_print_buf);

  // Decryption
  memset(ctx, 0, sizeof(ctx));
  memset(plain_buf, 0, sizeof(plain_buf));
  memcpy(tag_buf, tag, tag_len);
  memcpy(iv_buf, iv, iv_len);
  memcpy(add_buf, additional, add_len);

  SERVICES_cryptocell_mbedtls_ccm_gcm_set_key(services_handle, &error_code,
      ctx_addr,
      MBEDTLS_GCM_KEY, MBEDTLS_CIPHER_ID_AES,
      (uint32_t)key, key_len);

  SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(services_handle, &error_code,
      ctx_addr, MBEDTLS_GCM_AUTH_DECRYPT, pt_len,
      (uint32_t)iv_buf, iv_len, (uint32_t)add_buf, add_len,
      (uint32_t)cipher_buf, (uint32_t)plain_buf, (uint32_t)tag_buf, tag_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm_crypt      service_resp=0x%08X\n",
             error_code);

  fill_print_buf(plain_buf, 40);
  TEST_print(services_handle, "Decrypted data (truncated):\n        %s...\n", s_print_buf);

  // Single-part GCM tests
  TEST_print(services_handle, "Single-part GCM:\n");

  // Encryption
  memcpy(iv_buf, iv, iv_len);
  memcpy(add_buf, additional, add_len);
  memcpy(plain_buf, pt, pt_len);
  memset(tag_buf, 0, sizeof(tag_buf));
  memset(cipher_buf, 0, sizeof(cipher_buf));

  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_GCM_ENCRYPT_AND_TAG, (uint32_t)key, key_len,
      pt_len,
      (uint32_t)iv_buf, iv_len, (uint32_t)add_buf, add_len,
      (uint32_t)plain_buf, (uint32_t)cipher_buf,
      (uint32_t)tag_buf, tag_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm    service_resp=0x%08X\n",
             error_code);

  fill_print_buf(cipher_buf, 40);
  TEST_print(services_handle, "Encrypted data (truncated):\n        %s...\n", s_print_buf);
  fill_print_buf(tag_buf, tag_len);
  TEST_print(services_handle, "Tag:  %s\n", s_print_buf);

  // Decryption
  memset(plain_buf, 0, sizeof(plain_buf));
  memcpy(tag_buf, tag, tag_len);
  memcpy(iv_buf, iv, iv_len);
  memcpy(add_buf, additional, add_len);
  memcpy(cipher_buf, ct, ct_len);

  SERVICES_cryptocell_mbedtls_ccm_gcm(services_handle, &error_code,
      MBEDTLS_GCM_AUTH_DECRYPT, (uint32_t)key, key_len,
      pt_len,
      (uint32_t)iv_buf, iv_len, (uint32_t)add_buf, add_len,
      (uint32_t)cipher_buf, (uint32_t)plain_buf,
      (uint32_t)tag_buf, tag_len);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_ccm_gcm    service_resp=0x%08X\n",
             error_code);

  fill_print_buf(plain_buf, 40);
  TEST_print(services_handle, "Decrypted data (truncated):\n        %s...\n", s_print_buf);
}

/**
 * @brief Test MBEDTLS ChaCha/Poly
 */
static void test_mbedtls_chachapoly(uint32_t services_handle)
{
  uint32_t error_code = SERVICES_REQ_SUCCESS;

  const uint8_t chacha_key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
  static uint8_t chacha_IVCounter[12] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00 };
  uint8_t chacha_dataIn[] = { 0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69, 0x74, 0x2e };
  //static const uint8_t expectedDataOut[] = { 0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80, 0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81, 0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2, 0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b, 0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab, 0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57, 0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab, 0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8, 0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61, 0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e, 0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06, 0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36, 0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6, 0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42, 0x87, 0x4d };
  uint32_t chacha_counter = 1;
  uint8_t chacha_dataOut[114] = {0};

  SERVICES_cryptocell_mbedtls_chacha20_crypt(services_handle, &error_code,
		  (uint32_t)chacha_key, (uint32_t)chacha_IVCounter, chacha_counter,
		  sizeof(chacha_dataIn), (uint32_t)chacha_dataIn, (uint32_t)chacha_dataOut);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_chacha20_crypt     service_resp=0x%08X\n",
             error_code);

  fill_print_buf(chacha_dataOut, 32); // truncated
  TEST_print(services_handle, "ChaCha20 output (truncated):\n        %s...\n", s_print_buf);

  static uint8_t chachapoly_key[] = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f };    //pKey
  uint8_t chachapoly_context[32];
  memcpy(chachapoly_context, chachapoly_key, sizeof(chachapoly_context));

  static const uint8_t chachapoly_dataIn[] = { 0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69, 0x74, 0x2e };
  static const uint8_t chachapoly_addData[] = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };
  static uint8_t chachapoly_nonce[] = { 0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };
  uint8_t chachapoly_mac[16] = {0}; // MBEDTLS_POLY_MAC_SIZE_BYTES
  //static const uint8_t pExpectedDataOut[] = { 0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb, 0x7b, 0x86, 0xaf, 0xbc, 0x53, 0xef, 0x7e, 0xc2, 0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe, 0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6, 0x3d, 0xbe, 0xa4, 0x5e, 0x8c, 0xa9, 0x67, 0x12, 0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b, 0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29, 0x05, 0xd6, 0xa5, 0xb6, 0x7e, 0xcd, 0x3b, 0x36, 0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c, 0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58, 0xfa, 0xb3, 0x24, 0xe4, 0xfa, 0xd6, 0x75, 0x94, 0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc, 0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d, 0xe5, 0x76, 0xd2, 0x65, 0x86, 0xce, 0xc6, 0x4b, 0x61, 0x16 };

  SERVICES_cryptocell_mbedtls_chachapoly_crypt(services_handle, &error_code,
      (uint32_t)chachapoly_context, MBEDTLS_CHACHAPOLY_ENCRYPT_AND_TAG,
      sizeof(chachapoly_dataIn), (uint32_t)chachapoly_nonce,
      (uint32_t)chachapoly_addData, sizeof(chachapoly_addData), (uint32_t)chachapoly_mac,
      (uint32_t)chachapoly_dataIn, (uint32_t)chacha_dataOut);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_chachapoly_crypt   service_resp=0x%08X\n",
             error_code);

  fill_print_buf(chacha_dataOut, 32); // truncated
  TEST_print(services_handle, "ChaCha_Poly output (truncated):\n        %s...\n", s_print_buf);

  uint8_t poly_key[] = {0X85, 0Xd6, 0Xbe, 0X78, 0X57, 0X55, 0X6d, 0X33, 0X7f, 0X44, 0X52, 0Xfe, 0X42, 0Xd5, 0X06, 0Xa8, 0X01, 0X03, 0X80, 0X8a, 0Xfb, 0X0d, 0Xb2, 0Xfd, 0X4a, 0Xbf, 0Xf6, 0Xaf, 0X41, 0X49, 0Xf5, 0X1b};
  uint8_t poly_dataIn[] = {0x43, 0x72, 0X79, 0X70, 0X74, 0X6f, 0X67, 0X72, 0X61, 0X70, 0X68, 0X69, 0X63, 0X20, 0X46, 0X6f, 0x72, 0x75, 0X6d, 0X20, 0X52, 0X65, 0X73, 0X65, 0X61, 0X72, 0X63, 0X68, 0X20, 0X47, 0X72, 0X6f, 0x75, 0x70};
  uint8_t poly_macBuf[16]; // MBEDTLS_POLY_MAC_SIZE_BYTES
  //uint8_t EXPECTED_MAC[] = {0xa8, 0x06, 0x1d, 0xc1, 0x30, 0x51, 0x36, 0xc6, 0xc2, 0x2b, 0x8b, 0xaf, 0x0c, 0x01, 0x27, 0xa9};

  SERVICES_cryptocell_mbedtls_poly1305_crypt(services_handle, &error_code,
      (uint32_t)poly_key, (uint32_t)poly_dataIn, sizeof(poly_dataIn), (uint32_t)poly_macBuf);
  TEST_print(services_handle,
             "** TEST SERVICES_cryptocell_mbedtls_poly1305_crypt     service_resp=0x%08X\n",
             error_code);

  fill_print_buf(poly_macBuf, 32); // truncated
  TEST_print(services_handle, "Poly MAC output (truncated):\n        %s...\n", s_print_buf);
}

/**
 * @fn void test_crypto(uint32_t)
 * @brief
 * @param services_handle
 */
void test_crypto(uint32_t services_handle)
{
  test_mbedtls_aes(services_handle);
  test_mbedtls_sha(services_handle);
  test_mbedtls_cmac(services_handle);
  test_mbedtls_ccm(services_handle);
  test_mbedtls_ccm_star(services_handle);
  test_mbedtls_gcm(services_handle);
  test_mbedtls_chachapoly(services_handle);
}
