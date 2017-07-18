/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          forward_packet.h
 * @brief         Forward packet code 
 *
 * Author:        Atsushi Kasuya
 *
 */

/**
 * Send a packet to an NBI port. Notifies sequencer and frees the packet.
 * @param isl           Island of the CTM packet
 * @param pnum          Packet number of the CTM packet
 * @param msi           Modification script info required for transmission
 * @param len           Length of the packet from the start of the packet data
 *                      (which immediately follows the rewrite script
 *                      plus padding)
 * @param nbi           NBI TM to send the packet to
 * @param txq           NBI TM TX queue to send the packet to
 * @param seqr          NBI TM sequencer to send the packet to
 * @param seq           NBI TM sequence number of the packet
 * @param ctm_buf_size  Encoded CTM buffer size
 */
__intrinsic void pkt_nbi_send(unsigned char isl, unsigned int pnum,
                              __gpr const struct pkt_ms_info *msi,
                              unsigned int len, unsigned int nbi,
                              unsigned int txq, unsigned int seqr,
                              unsigned int seq,
                              enum PKT_CTM_SIZE ctm_buf_size);
/**
* Structure describing the NBI/CTM PE metadata for a packet delivered by
* the "catamaran" picoengine load.  This load provides packet
* pre-classification metadata and should be the NBI load used for flow
* processing of packets.
* See EDD-Catamaran 1.0 NPFW for details. PS5 build 1908 and later.
*
* Packet Metadata Format Catamaran
* Bit    3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
* -----\ 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
* Word  +---------------------------------------------------------------+
*    0  | CTM Island| Packet Number     |Bls|     Packet Length         |
*       +---------------------------------------------------------------+
*    1  |S|Rsv|                   MU Pointer                            |
*       +---------------------------------------------------------------+
*    2  |      Sequence Number          |NFP|  R  | Seq |P|Mtype|V| BP  |
*       +---------------------------------------------------------------+
*    3  |    Reserved             |E|TLD| OL4   | OL3  |  OL2     | R   |
*       +---------------------------------------------------------------+
*    4  |    Port        |     HP-Off1  |     HP-Off0    |    Misc      |
*       +---------------------------------------------------------------+
*    5  |P|I|W|S| VC |I|T|      O-Off   |     LIF+Mode / Match          |
*       +---------------------------------------------------------------+
*/
struct nbi_meta_catamaran {
   union {
       struct {
           struct nbi_meta_pkt_info pkt_info;
           unsigned int seq:16;               /**< Packet sequence number */
           unsigned int nfp:2;                /**< Packet NFP number */
           unsigned int __reserved_12:3;      /**< Reserved */
           unsigned int seqr:3;               /**< Packet sequencer */
           unsigned int prot_err:1;           /**< Packet protocol error */
           unsigned int meta_type:3;          /**< Packet meta type nbi_id */
           unsigned int meta_valid:1;         /**< Packet meta valid */
           unsigned int buffer_pool:3;        /**< Buffer pool */

           unsigned int __reserved_19:13;     /**< Reserved */
           unsigned int e_term:1;             /**< Early termination of pkt */
           unsigned int tunnel_depth:2;       /**< Packet tunnel layer depth */
           unsigned int outer_l4_prot_type:4; /**< Packet outer L4 protocol */
           unsigned int outer_l3_prot_type:4; /**< Packet outer L3 protocol */
           unsigned int outer_l2_prot_type:5; /**< Packet outer L2 protocol */
           unsigned int __reserved_1:3;       /**< Reserved */

           unsigned int port:8;               /**< Port number */
           unsigned int hp_off1:8;            /**< Packet header parse offset
                                                * 1 Start of L4 payload
                                                * relative to PKT_NBI_OFFSET
                                                * */
           unsigned int hp_off0:8;            /**< Packet header parse offset
                                                * 2 Start of L4 relative to
                                                * PKT_NBI_OFFSET */
           unsigned int misc:8;               /**< Miscellaneous packet flags */

           unsigned int pe:1;                 /**< Packet error */
           unsigned int ie:1;                 /**< Interface error */
           unsigned int pw:1;                 /**< Packet warning */
           unsigned int sp:1;                 /**< Special handling indicator */
           unsigned int vlan_cnt:2;           /**< VLAN tag count */
           unsigned int it:1;                 /**< TCAM tag indicator */
           unsigned int ts:1;                 /**< Table set indicator */
           unsigned int outer_off:8;          /**< Packet outer offset */
           unsigned int lif:16;               /**< Logical interface id */
       };
       uint32_t __raw[6];
   };
};



