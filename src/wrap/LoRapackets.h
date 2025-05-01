#include <sys/types.h>
#include <stdint.h>
#ifndef LORA_PAKETS_H
#define LORA_PAKETS_H

#define SMALL_PACKET_T  0x01
#define MEDIUM_PACKET_T 0x02
#define BIG_PACKET_T    0x03
#define SYNC_PACKET_T   0x04

inline uint16_t encodeParams(uint8_t packetType, uint8_t sf, uint8_t cr, uint8_t bw, uint8_t txPower) {
  uint8_t sfBits = (sf - 7) & 0x07;         // SF7–SF12 → 0–5
  uint8_t crBits = (cr - 5) & 0x03;         // CR 1–4 → 0–3
  uint8_t bwBits = bw & 0x03;              // BW index
  uint8_t powerBits = (txPower / 6) & 0x03; // e.g., 0, 6, 12, 18 dBm

  return ((packetType & 0x07) << 13) |
         ((sfBits & 0x07) << 10) |
         ((crBits & 0x03) << 8) |
         ((bwBits & 0x03) << 6) |
         ((powerBits & 0x03) << 4);
}

#pragma pack(push,1)
inline void decodeParams(uint16_t params, uint8_t &packetType, uint8_t &sf, uint8_t &cr, uint8_t &bw, uint8_t &txPower) {
  packetType = (params >> 13) & 0x07;
  sf = ((params >> 10) & 0x07) + 7;
  cr = ((params >> 8) & 0x03) + 5;
  bw = (params >> 6) & 0x03;
  txPower = ((params >> 4) & 0x03) * 6;
}
#pragma parck(pop)

struct __attribute__((packed)) SyncLoRaPacket12{
  uint8_t test_id;
  uint16_t comm_id;
  uint16_t packet_id;
  uint32_t timestamp_sender;
  uint16_t params;

  void setParams(uint8_t tst_id,uint16_t com_id, uint16_t pkt_id,uint8_t packetType, uint32_t timestamp, uint8_t sf, uint8_t cr, uint8_t bw, uint8_t txPower) {
    test_id = tst_id;
    comm_id = com_id;
    packet_id = pkt_id;
    timestamp = timestamp_sender;
    params = encodeParams(packetType,sf, cr, bw, txPower);
  }
};

#pragma pack(push,1)
struct Medium_packet {
  SyncLoRaPacket12 * data;
  char padding[100];
};

struct Flat_Medium_packet {
  SyncLoRaPacket12 data;
  char padding[100];
};
#pragma pack(pop)

#pragma pack(push,1)
struct Big_packet {
  SyncLoRaPacket12 * data;
  char padding[243];
};

struct Flat_big_packet {
  SyncLoRaPacket12 data;
  char padding[244];
};
#pragma pack(pop)



struct DynamicPacket {
  uint8_t * data;
  int length;
};

#endif
