## Auto-generated file, transpiled with RSPL
#include <rsp_queue.inc>

.set noreorder
.set noat
.set nomacro

#undef zero
#undef at
#undef v0
#undef v1
#undef a0
#undef a1
#undef a2
#undef a3
#undef t0
#undef t1
#undef t2
#undef t3
#undef t4
#undef t5
#undef t6
#undef t7
#undef s0
#undef s1
#undef s2
#undef s3
#undef s4
#undef s5
#undef s6
#undef s7
#undef t8
#undef t9
#undef k0
#undef k1
#undef gp
#undef sp
#undef fp
#undef ra
.equ hex.$zero, 0
.equ hex.$at, 1
.equ hex.$v0, 2
.equ hex.$v1, 3
.equ hex.$a0, 4
.equ hex.$a1, 5
.equ hex.$a2, 6
.equ hex.$a3, 7
.equ hex.$t0, 8
.equ hex.$t1, 9
.equ hex.$t2, 10
.equ hex.$t3, 11
.equ hex.$t4, 12
.equ hex.$t5, 13
.equ hex.$t6, 14
.equ hex.$t7, 15
.equ hex.$s0, 16
.equ hex.$s1, 17
.equ hex.$s2, 18
.equ hex.$s3, 19
.equ hex.$s4, 20
.equ hex.$s5, 21
.equ hex.$s6, 22
.equ hex.$s7, 23
.equ hex.$t8, 24
.equ hex.$t9, 25
.equ hex.$k0, 26
.equ hex.$k1, 27
.equ hex.$gp, 28
.equ hex.$sp, 29
.equ hex.$fp, 30
.equ hex.$ra, 31
#define vco 0
#define vcc 1
#define vce 2

.data
  RSPQ_BeginOverlayHeader
    RSPQ_DefineCommand BlendCmd_SetSource, 8
    RSPQ_DefineCommand BlendCmd_ProcessLine, 4
  RSPQ_EndOverlayHeader

  RSPQ_BeginSavedState
    STATE_MEM_START:
    .align 2
    SRC_RDRAM: .ds.b 4
    .align 1
    SRC_WIDTH: .ds.b 2
    .align 1
    SRC_HEIGHT: .ds.b 2
    STATE_MEM_END:
  RSPQ_EndSavedState

.bss
  TEMP_STATE_MEM_START:
    .align 4
    SRC_BUF: .ds.b 1280
    .align 4
    DST_BUF: .ds.b 1280
  TEMP_STATE_MEM_END:

.text
OVERLAY_CODE_START:

BlendCmd_SetSource:
  srl $t0, $a1, 16                                   ## L:16   |      ^ | u16<$t0> sizeX = sourceSize >> 16;
  andi $t1, $t0, 7                                   ## L:17   |      2 | u16<$t1> isDivEight = sizeX & 0x7;
  bgtz $t1, assertion_failed                         ## L:19   |      3 | if(isDivEight > 0) {
  lui $at, 4112                                      ## L:20   |     *5 | assert(0x01010);
  LABEL_BlendCmd_SetSource_0001:
  sltiu $at, $t0, 641                                ## L:23   |      6 | if(sizeX > 640) {
  beq $at, $zero, assertion_failed                   ## L:23   |      7 | if(sizeX > 640) {
  lui $at, 4112                                      ## L:24   |     *9 | assert(0x01010);
  LABEL_BlendCmd_SetSource_0002:
  sll $t0, $t0, 1                                    ## L:27   |     10 | sizeX = sizeX * 2;
  andi $s7, $a1, 65535                               ## L:28   |     11 | u16 sizeY = sourceSize & 0xFFFF;
  sh $s7, %lo(SRC_HEIGHT)($zero)                     ## L:30   |     12 | store(sizeY, SRC_HEIGHT);
  sw $a0, %lo(SRC_RDRAM)($zero)                      ## L:31   |     13 | store(source_pointer, SRC_RDRAM);
  j RSPQ_Loop                                        ## L:32   |     14 | }
  sh $t0, %lo(SRC_WIDTH)($zero)                      ## L:29   |    *16 | store(sizeX, SRC_WIDTH);
BlendCmd_ProcessLine:
  lhu $t0, %lo(SRC_WIDTH + 0)                        ## L:37   |      ^ | u16<$t0> size = load(SRC_WIDTH);
  addiu $t0, $t0, -1                                 ## L:38   |    **4 | dma_in_async(SRC_BUF, s, size);
  ori $s4, $zero, %lo(SRC_BUF)                       ## L:38   |      5 | dma_in_async(SRC_BUF, s, size);
  or $t2, $zero, $zero                               ## L:38   |      6 | dma_in_async(SRC_BUF, s, size);
  jal DMAExec                                        ## L:38   |      7 | dma_in_async(SRC_BUF, s, size); ## Args: $t0, $t1, $s0, $s4, $t2
  ori $s0, $zero, %lo(SRC_RDRAM)                     ## L:36   |     *9 | u32<$s0> s = SRC_RDRAM;
  addiu $t0, $t0, -1                                 ## L:42   |     10 | dma_in(DST_BUF, target_pointer, size);
  ori $s4, $zero, %lo(DST_BUF)                       ## L:42   |     11 | dma_in(DST_BUF, target_pointer, size);
  addiu $t2, $zero, 12                               ## L:42   |     12 | dma_in(DST_BUF, target_pointer, size);
  jal DMAExec                                        ## L:42   |     13 | dma_in(DST_BUF, target_pointer, size); ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a0                                 ## L:42   |    *15 | dma_in(DST_BUF, target_pointer, size);
  lw $s3, %lo(SRC_BUF + 0)                           ## L:44   |     16 | u32<$s3> src_ptr = load(SRC_BUF);
  lw $s4, %lo(DST_BUF + 0)                           ## L:45   |     17 | u32<$s4> dst_ptr = load(DST_BUF);
  lhu $t7, %lo(SRC_WIDTH + 0)                        ## L:46   |     18 | u16<$t7> width = load(SRC_WIDTH);
  addiu $at, $zero, 31                               ## L:48   |     19 | konstants.x = 0x1F;
  mtc2 $at, $v28.e0                                  ## L:48   |    *21 | konstants.x = 0x1F;
  LABEL_BlendCmd_ProcessLine_0003:
  bltz $t7, LABEL_BlendCmd_ProcessLine_0004          ## L:50   |     22 | while (width >= 0) {
  nop                                                ## L:50   |    *24 | while (width >= 0) {
  lqv $v01, 0, 0, $s3                                ## L:52   |     25 | vec16<$v01> vsrc16 = load(src_ptr);
  lqv $v05, 0, 0, $s4                                ## L:53   |  ***29 | vec16<$v05> vdst16 = load(dst_ptr);
  vmudm $v02, $v01, $v31.e0                          ## L:62   |      ^ | vec16<$v02> vsrcr16 = vsrc16 >> 1;
  vmudn $v03, $v01, $v30.e3                          ## L:63   |     30 | vec16<$v03> vsrcg16 = vsrc16 << 4;
  vmudn $v04, $v01, $v31.e6                          ## L:64   |     31 | vec16<$v04> vsrcb16 = vsrc16 << 9;
  vmudm $v06, $v05, $v31.e0                          ## L:66   |    *33 | vec16<$v06> vdstr16 = vdst16 >> 1;
  vmudn $v07, $v05, $v30.e3                          ## L:67   |     34 | vec16<$v07> vdstg16 = vdst16 << 4;
  vmudn $v08, $v05, $v31.e6                          ## L:68   |     35 | vec16<$v08> vdstb16 = vdst16 << 9;
  vand $v02, $v02, $v28.e0                           ## L:70   |     36 | vsrcr16 &= konstants.x;
  vand $v04, $v04, $v28.e0                           ## L:71   |     37 | vsrcb16 &= konstants.x;
  vand $v03, $v03, $v28.e0                           ## L:72   |     38 | vsrcg16 &= konstants.x;
  vand $v06, $v06, $v28.e0                           ## L:74   |     39 | vdstr16 &= konstants.x;
  vand $v08, $v08, $v28.e0                           ## L:75   |     40 | vdstb16 &= konstants.x;
  vand $v07, $v07, $v28.e0                           ## L:76   |     41 | vdstg16 &= konstants.x;
  vaddc $v06, $v06, $v02.v                           ## L:78   |    *43 | vdstr16 += vsrcr16;
  vaddc $v07, $v07, $v03.v                           ## L:79   |    *45 | vdstg16 += vsrcg16;
  vaddc $v08, $v08, $v04.v                           ## L:80   |     46 | vdstb16 += vsrcb16;
  vmudn $v06, $v06, $v30.e6                          ## L:83   |     47 | vdstr16 = vdstr16 << 1;
  vmudm $v07, $v07, $v31.e3                          ## L:84   |    *49 | vdstg16 = vdstg16 >> 4;
  vmudm $v08, $v08, $v30.e0                          ## L:85   |     50 | vdstb16 = vdstb16 >> 9;
  vor $v05, $v06, $v07.v                             ## L:87   |   **53 | vdst16 = vdstr16 | vdstg16;
  vor $v05, $v05, $v08.v                             ## L:88   |  ***57 | vdst16 |= vdstb16;
  sqv $v05, 0, 0, $s4                                ## L:90   |  ***61 | store(vdst16, dst_ptr);
  addiu $s3, $s3, 16                                 ## L:92   |     62 | src_ptr += 16;
  addiu $t7, $t7, 65520                              ## L:93   |     63 | width -= 16;
  j LABEL_BlendCmd_ProcessLine_0003                  ## L:93   |     64 | width -= 16;
  addiu $s4, $s4, 16                                 ## L:91   |    *66 | dst_ptr += 16;
  LABEL_BlendCmd_ProcessLine_0004:
  ori $t0, $zero, 1279                               ## L:96   |     67 | dma_out(DST_BUF, target_pointer);
  ori $s4, $zero, %lo(DST_BUF)                       ## L:96   |     68 | dma_out(DST_BUF, target_pointer);
  addiu $t2, $zero, -32756                           ## L:96   |     69 | dma_out(DST_BUF, target_pointer);
  jal DMAExec                                        ## L:96   |     70 | dma_out(DST_BUF, target_pointer); ## Args: $t0, $t1, $s0, $s4, $t2
  or $s0, $zero, $a0                                 ## L:96   |    *72 | dma_out(DST_BUF, target_pointer);
  j RSPQ_Loop                                        ## L:98   |     73 | }
  sw $s3, %lo(SRC_RDRAM)($zero)                      ## L:97   |    *75 | store(src_ptr, SRC_RDRAM);

OVERLAY_CODE_END:

#define zero $0
#define v0 $2
#define v1 $3
#define a0 $4
#define a1 $5
#define a2 $6
#define a3 $7
#define t0 $8
#define t1 $9
#define t2 $10
#define t3 $11
#define t4 $12
#define t5 $13
#define t6 $14
#define t7 $15
#define s0 $16
#define s1 $17
#define s2 $18
#define s3 $19
#define s4 $20
#define s5 $21
#define s6 $22
#define s7 $23
#define t8 $24
#define t9 $25
#define k0 $26
#define k1 $27
#define gp $28
#define sp $29
#define fp $30
#define ra $31

.set at
.set macro