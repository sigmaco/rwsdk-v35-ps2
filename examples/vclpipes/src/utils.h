#define MAXQWC      (65535)
#define DMATAG_REFE (0x0l)
#define DMATAG_CNT  (0x1l)
#define DMATAG_NEXT (0x2l)
#define DMATAG_REF  (0x3l)
#define DMATAG_REFS (0x4l)
#define DMATAG_CALL (0x5l)
#define DMATAG_RET  (0x6l)
#define DMATAG_END  (0x7l)

#define MAXVIFCODES        (2+4*MAXQWC)
#define VIFCMD_NOP         (0x00l)
#define VIFCMD_STCYCL      (0x01l)
#define VIFCMD_OFFSET      (0x02l)
#define VIFCMD_BASE        (0x03l)
#define VIFCMD_ITOP        (0x04l)
#define VIFCMD_STMOD       (0x05l)
#define VIFCMD_MSKPATH3    (0x06l)
#define VIFCMD_MARK        (0x07l)
#define VIFCMD_FLUSHE      (0x10l)
#define VIFCMD_FLUSH       (0x11l)
#define VIFCMD_FLUSHA      (0x13l)
#define VIFCMD_MSCAL       (0x14l)
#define VIFCMD_MSCALF      (0x15l)
#define VIFCMD_MSCNT       (0x17l)
#define VIFCMD_STMASK      (0x20l)
#define VIFCMD_STROW       (0x30l)
#define VIFCMD_STCOL       (0x31l)
#define VIFCMD_MPG         (0x4al)
#define VIFCMD_DIRECT      (0x50l)
#define VIFCMD_DIRECTHL    (0x51l)
#define VIFCMD_UNPACKS_32  (0x60l)
#define VIFCMD_UNPACKS_16  (0x61l)
#define VIFCMD_UNPACKS_8   (0x62l)
#define VIFCMD_UNPACKV2_32 (0x64l)
#define VIFCMD_UNPACKV2_16 (0x65l)
#define VIFCMD_UNPACKV2_8  (0x66l)
#define VIFCMD_UNPACKV3_32 (0x68l)
#define VIFCMD_UNPACKV3_16 (0x69l)
#define VIFCMD_UNPACKV3_8  (0x6al)
#define VIFCMD_UNPACKV4_32 (0x6cl)
#define VIFCMD_UNPACKV4_16 (0x6dl)
#define VIFCMD_UNPACKV4_8  (0x6el)
#define VIFCMD_UNPACKV4_5  (0x6fl)

#define ADDTOPKT(_a, _b) { RwUInt128 _c; MAKE128(_c, _a, _b); RWDMA_ADD_TO_PKT(_c); }
#define ADDTOPKT32(_a, _b, _c, _d)                                      \
MACRO_START                                                             \
{                                                                       \
    RwUInt128 _t1, _t2, _t3;                                            \
    __asm__ ( "ppacw %0, %1, %2" : "=r" (_t1) : "r" (_d), "r" (_c));    \
    __asm__ ( "ppacw %0, %1, %2" : "=r" (_t2) : "r" (_b), "r" (_a));    \
    __asm__ ( "ppacw %0, %1, %2" : "=r" (_t3) : "r" (_t1), "r" (_t2));  \
    RWDMA_ADD_TO_PKT(_t3);                                              \
}                                                                       \
MACRO_STOP

extern void SetGSReg(RwUInt64 reg, RwUInt64 value);
