#ifndef __MS_DDS_bandplan_h__
#define __MS_DDS_bandplan_h__

/* modes = combine to get more allowed modes */
#define CW _BV(0) /* CW */
#define SSB _BV(1) /* SSB */
#define DIGI _BV(2) /* any digi */
#define OTHER _BV(3) /* other transmissions allowed */

/* usages = combine to get more specific restriction */
#define NARROW _BV(4) /* narrow band modes only */
#define UNATTENDED _BV(5) /* beacons, bbs and repeaters */
#define QRP _BV(6) /* QRP activity */
#define DX _BV(7) /* DX activity */

/* helper macro to define all modes as allowed */
#define ALLM (CW | SSB | DIGI | OTHER)

#define BANDTX (CW) /* what flags have to be present for TX to be allowed */
#define BANDRX (UNATTENDED) /* what flags signalize RX mode only */

/* macro to implement "TX allowed" logic */
#define TXOK(flags) ((BANDTX & (flags)) && !(BANDRX & (flags)))

uint8_t bandplan(uint16_t kHz, const uint8_t **desc);

#endif
