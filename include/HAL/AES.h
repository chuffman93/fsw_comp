// /*! \file AES.h
//  *  \brief Header file for the AES interface
//  *
//  *  This file contains the definition for the HAL AES interface.
//  */
// 
// #ifndef _AES_H_
// #define _AES_H_
// 
// #include "core/StdTypes.h"
// 
// namespace AllStar
// {
//     namespace HAL
//     {
//         /*! \brief Initialize OS elements for AES interface
//          *
//          *  Creates semaphores which are used in other AES functions.
//          *
//          *  \note This function must be called before any other AES
//          *  functions and may only be called exactly once.
//          */
//         void AESOSInit(void);
// 
//         /*! \brief Initialize the AES device
//          *
//          *  Initializes the AES hardware.
//          *
//          *  \note This function must be called before accessing the
//          *  device for the first time. It may be called multiple times
//          *  to reinitialize the hardware with different settings.
//          *
//          *  \param key 256-bit key to use for encryption/decryption.
//          *  \return true if device successfully initialized, false otherwise.
//          */
//         bool AESInit(const unsigned int *key);
// 
//         /*! \brief Encrypt data
//          *
//          *  Encrypts a string of data using the key given in AESInit.
//          *
//          *  \param in Data to be encrypted.
//          *  \param out Where to store encrypted result.
//          *  \param len Length (in 32-bit words) of data to encrypt.
//          *  \return true if data successfully encrypted, false otherwise.
//          */
//         bool AESEncrypt(const unsigned int *in, unsigned int *out, std::size_t len);
// 
//         /*! \brief Decrypt data
//          *
//          *  Decrypts a string of data using the key given in AESInit.
//          *
//          *  \param in Data to be decrypted.
//          *  \param out Where to store decrypted result.
//          *  \param len Length (in 32-bit words) of data to decrypt.
//          *  \return true if data successfully decrypted, false otherwise.
//          */
//         bool AESDecrypt(const unsigned int *in, unsigned int *out, std::size_t len);
//     }
// }
// 
// #endif /* _AES_H_ */
