#ifndef _CRC32_H_
#define _CRC32_H_

class CRC32 {
  public:
    CRC32() {
        InitCRC32Table();
        Reset();
    }

    virtual ~CRC32() {}

    void Reset() {
        m_currentCRC32 = 0xFFFFFFFF;
    }

    void ProcessBuffer(unsigned char* pBuffer, int bufferSize, int stride) {
        // Perform the algorithm on each character
        // in the string, using the lookup table values.
        while(bufferSize > 0) {
            m_currentCRC32 = (m_currentCRC32 >> 8) ^ m_crc32Table[(m_currentCRC32 & 0xFF) ^ *pBuffer];

            pBuffer += stride;
            bufferSize -= stride;
        }
    }

    unsigned int GetCRC() {
        // Exclusive OR the result with the beginning value.
        return m_currentCRC32 ^ 0xFFFFFFFF;
    }

  private:

    // This is the official polynomial used by CRC-32
    // in PKZip, WinZip and Ethernet.
    static const unsigned int Polynomial = 0x04c11db7;

    static const unsigned int CRC32TableSize = 256;

    unsigned int m_crc32Table[CRC32TableSize];  // Lookup table array

    unsigned int m_currentCRC32;

    void InitCRC32Table() {
        // 256 values representing ASCII character codes.
        for(int i = 0; i <= CRC32TableSize; i++) {
            m_crc32Table[i] = Reflect(i, 8) << 24;

            for (unsigned int j = 0; j < 8; j++) {
                m_crc32Table[i] = (m_crc32Table[i] << 1) ^ (m_crc32Table[i] & (1 << 31) ? Polynomial : 0);
            }

            m_crc32Table[i] = Reflect(m_crc32Table[i], 32);
        }
    }

    unsigned int Reflect(unsigned int ref, unsigned char numBits) {
        unsigned int value = 0;

        // Swap bit 0 for bit 7
        // bit 1 for bit 6, etc.
        for(unsigned char i = 1; i < (numBits + 1); i++) {
            if(ref & 1) {
                value |= 1 << (numBits - i);
            }

            ref >>= 1;
        }

        return value;
    }
};

#endif
