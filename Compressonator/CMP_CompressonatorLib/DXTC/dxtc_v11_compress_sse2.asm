;===============================================================================
; Copyright (c) 2004-2006 ATI Technologies Inc.
;===============================================================================
; Assemble with:
; ml   /DX64=0 /W3 /Fo $(OutDir)\$(InputName).obj /c /coff /Cp /Zi $(InputPath)
; ml64 /DX64=1 /W3 /Fo $(OutDir)\$(InputName).obj /c /Cp /Zi $(InputPath)


; Performance / test / tuning options

WEIGHTING                equ 1        ; Allows (limited) weighting of RGB components (currently 2 : 4 : 1)
UNROUNDING                equ 1        ; Costs ~1%; improves MSE slightly
PROG_THRESHOLD            equ 1        ; Enable to skip prog on small-signal blocks; meaningless drag with no_prog

NO_PROG                    equ 0        ; Note that weighting is not relevant if NO_PROG is enabled
USE_34                    equ 0        ; Enable 3-colour blocks. Costs ~10% on Phenom with X64, but more like 20% on older chips with win32, helps most images little but fixes some challenging blocks

COUNT_PROG_STEPS        equ 0        ; Generates a false-colour image; black is 2 prog steps, rising accordingly as with Spectrum palette
SHOW_BLOCK_TYPES        equ 0        ; USE_34 must be enabled

; This is the dial controlling the point at which prog turns on. The tipping point is somewhere between 8 and
; 12; increases above 12 start to add noise and hit MSE noticeably; below 8 there are few gains.
; For a typical image 8 provides about a 2.5% gain, with 12 providing about 7.5% gain in performance; 16 only
; adds another 2%. In the end 12 seems acceptable; the image is very similar and the shape of noise is not
; significantly changed, while the diminishing returns above this point seem to make it unwise.
prog_threshold            equ 12*32    ; X is in 9.5 format

; Increasing stepsize suffers from decreasing performance returns (due to a minimum of 2 prog steps being
; required), but decreasing it suffers from increasing inaccuracy once a certain point is crossed. Selecting
; the optimum step size is therefore an important performance/quality tradeoff; excellent results are
; possible with high performance if this is correctly tuned.
; If weighting is off, larger step sizes appear to make less difference in quality.
step                    equ 3*32    ; X is in 9.5 format; we want to step somewhere between 1.0 and 4.0
;step                    equ    2*32
;step                    equ    4*32
;step                    equ    8*32

; We could make step programmable to trade off performance and image quality


IF X64 EQ 0
    .686
    .MMX
    .XMM
    .MODEL FLAT
ENDIF




.DATA

white                DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0ffffffh
                    DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0ffffffh
                    DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0ffffffh
                    DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0ffffffh


whiteblack            DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0ffffffh
                    DWORD 0ffffffh, 0000000h, 0000000h, 0ffffffh
                    DWORD 0ffffffh, 0ffffffh, 0000000h, 0000000h
                    DWORD 0000000h, 0000000h, 0000000h, 0000000h

redblack            DWORD 0ff0000h, 0ff0000h, 0ff0000h, 0ff0000h
                    DWORD 0ff0000h, 0000000h, 0000000h, 0ff0000h
                    DWORD 0ff0000h, 0ff0000h, 0000000h, 0000000h
                    DWORD 0000000h, 0000000h, 0000000h, 0000000h

redsblack            DWORD 0ff0000h, 0aa0000h, 0aa0000h, 0ff0000h
                    DWORD 0ff0000h, 0550000h, 0550000h, 0ff0000h
                    DWORD 0ff0000h, 0ff0000h, 0000000h, 0000000h
                    DWORD 0000000h, 0000000h, 0000000h, 0000000h

redsblack_prog        DWORD 0ff0000h, 0990000h, 0990000h, 0990000h
                    DWORD 0990000h, 0660000h, 0660000h, 0990000h
                    DWORD 0990000h, 0990000h, 0660000h, 0000000h
                    DWORD 0660000h, 0660000h, 0660000h, 0660000h

redsblack2            DWORD 0e00000h, 0e00000h, 0e00000h, 0e00000h
                    DWORD 0c00000h, 0c00000h, 0c00000h, 0c00000h
                    DWORD 0a00000h, 0a00000h, 0a00000h, 0a00000h
                    DWORD 0800000h, 0800000h, 0800000h, 0800000h

greenred            DWORD 0ff0000h, 0ff0000h, 0ff0000h, 0ff0000h
                    DWORD 0ff0000h, 0ff0000h, 0ff0000h, 0ff0000h
                    DWORD 000ff00h, 000ff00h, 000ff00h, 000ff00h
                    DWORD 000ff00h, 000ff00h, 000ff00h, 000ff00h

bluered                DWORD 0ff0000h, 0ff0000h, 0ff0000h, 0ff0000h
                    DWORD 0ff0000h, 0ff0000h, 0ff0000h, 0ff0000h
                    DWORD 00000ffh, 00000ffh, 00000ffh, 00000ffh
                    DWORD 00000ffh, 00000ffh, 00000ffh, 00000ffh

fade                DWORD 0ffffffh, 0ffffffh, 0b0b0b0h, 0707070h
                    DWORD 0ffffffh, 0ffffffh, 0b0b0b0h, 0707070h
                    DWORD 0ffffffh, 0ffffffh, 0d0d0d0h, 0909090h
                    DWORD 0ffffffh, 0ffffffh, 0ffffffh, 0e0e0e0h

col_table            DWORD 000000000h, 0001f001fh, 0f800f800h, 0f81ff81fh
                    DWORD 007e007e0h, 007ff07ffh, 0ffe0ffe0h, 0ffffffffh
                    DWORD 084108410h

ALIGN 16

testval                    equ 0400h
test_val                WORD testval, testval, testval, testval, testval, testval, testval, testval

one_third                equ 65536/3


AVG_FRAC_BITS            equ 4                            ; 0-4

mask_rgb                QWORD 000ffffff00ffffffh, 000ffffff00ffffffh
mask_rg                    QWORD 000ffff0000ffff00h, 000ffff0000ffff00h
mask_rb                    QWORD 000ff00ff00ff00ffh, 000ff00ff00ff00ffh
mask_gb                    QWORD 00000ffff0000ffffh, 00000ffff0000ffffh
mask_r                    QWORD 000ff000000ff0000h, 000ff000000ff0000h
mask_b                    QWORD 0000000ff000000ffh, 0000000ff000000ffh
mask_low_qword            QWORD 0ffffffffffffffffh, 0
mask_low_dword            QWORD 000000000ffffffffh, 0
mask_low_word            QWORD 0000000000000ffffh, 0
mask_third_word            QWORD 00000ffff00000000h, 0
dword_word_mask            QWORD 00000ffff0000ffffh, 00000ffff0000ffffh
quad_word_mask            QWORD 0000000000000ffffh, 0000000000000ffffh
quad_dword_mask            QWORD 000000000ffffffffh, 000000000ffffffffh
quad_upper_dword_mask    QWORD 0ffffffff00000000h, 0ffffffff00000000h
max_sint32                DWORD 07fffffffh, 07fffffffh, 07fffffffh, 07fffffffh

scale_one_third            WORD one_third,one_third,one_third,one_third,one_third,one_third,one_third,one_third
stepsize                WORD step,step,step,step,step,step,step,step
prog_threshold_size        WORD prog_threshold,prog_threshold,prog_threshold,prog_threshold,prog_threshold,prog_threshold,prog_threshold,prog_threshold

IF WEIGHTING
WEIGHTING_BITS            equ 4                ; If this equals AVG_FRAC_BITS a few extra optimisations kick in
weighting                WORD 4,16,8,0, 4,16,8,0
unweighting                WORD 4,1,2,0
round_565_weighted        WORD 0010h, 0020h, 0020h, 0
ELSE
WEIGHTING_BITS            equ 0
ENDIF

IF UNROUNDING
round_565                WORD 0040h,0020h,0040h,0    ; This is applied to the average, which is in 8.4 format, so it's 0.5 in 5.7, 6.6, 5.7 format
scale_to_round            WORD 2048,1024,2048,0        ; This is a 5 6 5 right shift in 8.4 format
round_mask                WORD 0fff0h, 0fff0h, 0fff0h, 0
ENDIF


scale_8_4_to_565        WORD 32*16,    64*16, 32*16,    0
clamp_565                WORD 31,63,31,0

_0000000055555555        QWORD 00000000055555555h

_0707070707070707        QWORD 00707070707070707h
_0f0f0f0f0f0f0f0f        QWORD 00f0f0f0f0f0f0f0fh
_000f000f000f000f        QWORD 0000f000f000f000fh
_00f000f000f000f0        QWORD 000f000f000f000f0h

ALIGN 16

; The axis ordering table. This is a table of XMMWORD 4-float values that set the axis signs
; using the index made up of the RB,GB,GR negative correlation and zero signal bits
; In addition, the scaling factor to convert from -1,1 range to 1.15 fixed point is baked in
scale_1_15                equ 32768.0

AXIS_ORDER_ENTRY        MACRO rb_neg, gb_zero, gb_neg, gr_zero, gr_neg
    IF (gr_zero NE 0) AND (gb_zero NE 0)
        ; use rb on B
        IF (rb_neg NE 0)
            REAL4 -scale_1_15, scale_1_15, scale_1_15, 0.0
        ELSE
            REAL4 scale_1_15, scale_1_15, scale_1_15, 0.0
        ENDIF
    ELSE
        ; use gb_pos on B and gr_pos on R
        IF (gb_neg NE 0)
            REAL4 -scale_1_15, scale_1_15
        ELSE
            REAL4 scale_1_15, scale_1_15
        ENDIF
        IF (gr_neg NE 0)
            REAL4 -scale_1_15, 0.0
        ELSE
            REAL4 scale_1_15, 0.0
        ENDIF
    ENDIF
ENDM


axis_order_table    LABEL XMMWORD
    axis_order_count = 0
    WHILE (axis_order_count LT 32)
        AXIS_ORDER_ENTRY (axis_order_count AND 1), (axis_order_count AND 2), (axis_order_count AND 4), (axis_order_count AND 8), (axis_order_count AND 010h)
        axis_order_count = axis_order_count + 1
    ENDM




; A few helper macros to make shift code cleaner
AVG_SHIFT_BITS            equ WEIGHTING_BITS + (4-AVG_FRAC_BITS)
IF AVG_SHIFT_BITS
avg_round_val            equ 1 SHL (AVG_SHIFT_BITS-1)        ; 0.5 correction
avg_round                WORD avg_round_val,avg_round_val,avg_round_val,avg_round_val,avg_round_val,avg_round_val,avg_round_val,avg_round_val
ENDIF

INPUT_SHIFT_BITS        equ WEIGHTING_BITS - AVG_FRAC_BITS        ; input has been left shifted by weight, needs to be left shifted by frac

INPUT_SHIFT            MACRO reg
    IF INPUT_SHIFT_BITS GT 0
        psrlw reg,INPUT_SHIFT_BITS
    ELSEIF INPUT_SHIFT_BITS LT 0
        psllw reg,-INPUT_SHIFT_BITS
    ENDIF
    
    ENDM


; Macro to generate the correct value for use in the various shuffle ops
SHUFFLE_SELECT        MACRO a,b,c,d
    LOCAL Value
    IF (a gt 3) or (b gt 3) or (c gt 3) or (d gt 3)
        .ERR
        EXITM <0>
    ENDIF
    Value = ((a) OR (b SHL 2) OR (c SHL 4) OR (d SHL 6))
    EXITM %Value
ENDM



.CODE

; The DXTC compressor is a six-step process:
; 1. AVERAGE:    find the average value of the block
; 2. AXIS:        compute the compression axis
; 3. POS:        calculate the position on the axis of each data value in the block
; 4. PROG:        progressively refine the selected endpoints to find an error minima
; 5. COLOUR:    generate output colours from the endpoints, axis and average
; 6. CLUSTER:    cluster the data values into the appropriate clusters

; Each of these is implemented by macros to allow the composition of different components
; to form a particular block compressor; this also eases portability to x64. For example,
; changing AVERAGE would allow different structured input, and changing AVERAGE, AXIS and POS
; allows e.g. 2D vs. 3D block types.

; Macros make the code harder to debug, because MASM generates the error messages with line
; numbers corresponding to the invocation of the macro rather than inside the macro. Enabling
; /W3 helps, it then shows the line offset inside the macro. Short macros are therefore a
; lot easier to debug than long ones.

; CLUSTER has been subsumed into pos, prog and colour in this implementation


; Temporaries
TMP_TMP                equ    (8*16)            ; Basic tmp area - 8 XMMWORD each contains two WORD RGB0 source values
TMP_AVG                equ TMP_TMP + 16
TMP_AXIS            equ TMP_AVG + 16
TMP_POS                equ TMP_AXIS + (2*16)
TMP_CLUSTER            equ TMP_POS + 16            ; 2 dwords; centre and split
TMP_BEST            equ TMP_CLUSTER + 16
TMP_CENTRE            equ TMP_BEST + 16
TMP_CURRENT            equ TMP_CENTRE + 16


; Prologue / epilog / register management
IF X64
    ; We can use volatile registers for some of the scratch regs we need so we don't need to save them
    rfx                equ r8
    efx                equ r8d
    tmp_base_reg    equ rbp        ; Tried r9 but faster with rbp - REX prefix overhead perhaps?
    source_reg        equ rcx
    dest_reg        equ rdx
    stride_reg        equ rax
    egx                equ r10d
    ehx                equ r11d

    main_proc_name    equ DXTCV11CompressBlockSSE2

    SAVED_REGS            equ 8
    TMP_REGSAVE            equ TMP_CENTRE + (SAVED_REGS*16)


    SAVE_REG        MACRO reg, n
        IF n LE SAVED_REGS
            movdqa [tmp_base_reg-TMP_REGSAVE+(n*16)], reg
        ENDIF
    ENDM

    RESTORE_REG        MACRO reg, n
        IF n LE SAVED_REGS
            movdqa reg, [tmp_base_reg-TMP_REGSAVE+(n*16)]
        ENDIF
    ENDM

    SAVE_REGS        MACRO
;        push rsi    
;        push rdi
;        push rbx
        push rbp

        ; Set up a 16-byte aligned storage space pointer
        mov tmp_base_reg, rsp
        and tmp_base_reg, NOT 0fh

        ; Any xmm regs over 5 need to be saved here as well
        SAVE_REG xmm6,1
        SAVE_REG xmm7,2
        SAVE_REG xmm8,3
        SAVE_REG xmm9,4
        SAVE_REG xmm10,5
        SAVE_REG xmm11,6
        SAVE_REG xmm12,7
        SAVE_REG xmm13,8
        SAVE_REG xmm14,9
        SAVE_REG xmm15,10
    ENDM

    RESTORE_REGS    MACRO
        RESTORE_REG xmm6,1
        RESTORE_REG xmm7,2
        RESTORE_REG xmm8,3
        RESTORE_REG xmm9,4
        RESTORE_REG xmm10,5
        RESTORE_REG xmm11,6
        RESTORE_REG xmm12,7
        RESTORE_REG xmm13,8
        RESTORE_REG xmm14,9
        RESTORE_REG xmm15,10

        pop rbp
;        pop rbx
;        pop rdi
;        pop rsi
    ENDM

ELSE    ; X64

    ; We don't have spare volatile regs
    efx                equ ebx
    tmp_base_reg    equ ebp
    source_reg        equ ecx
    dest_reg        equ edx
    stride_reg        equ eax
    IF USE_34
        egx                equ esi
        ehx                equ edi
    ENDIF

    main_proc_name    equ _DXTCV11CompressBlockSSE2

    SAVE_REGS        MACRO
        IF USE_34
            push esi
            push edi
        ENDIF

        push ebx
        push ebp

        ; Set up the tmp pointer aligned on a 16-byte boundary
        mov ebp, esp
        and ebp, NOT 0fh
    ENDM

    RESTORE_REGS    MACRO
        pop ebp
        pop ebx
        IF USE_34
            pop edi
            pop esi
        ENDIF

        emms        
    ENDM

ENDIF    ; X64



COLOURSPACE_TRANSFORM_SETUP    MACRO
    IF WEIGHTING
        movdqa xmm5, xmmword ptr [weighting]
    ELSE
        movdqa xmm5, xmmword ptr [mask_rgb]
    ENDIF
ENDM

COLOURSPACE_TRANSFORM    MACRO reg1, reg2
    IF WEIGHTING
        ; Weighting doesn't need masking - the weighting mul clears out the A component
    ELSE
        pand reg1,xmm5
        pand reg2,xmm5
    ENDIF
ENDM



AVERAGE_RGB        MACRO src_reg, stride_reg

        COLOURSPACE_TRANSFORM_SETUP

        ; Read source data - use DQU since we can't be certain it's aligned, the
        ; overhead is not important given the size of the rest of the calculation
        movdqu xmm0,[src_reg]
        movdqu xmm2,[src_reg+stride_reg]
        pxor xmm7,xmm7

        COLOURSPACE_TRANSFORM xmm0,xmm2

        movdqa xmm1,xmm0
        movdqa xmm3,xmm2
        punpcklbw xmm0,xmm7        ; expand to two 0RGB values in each XMM, 8 XMMs total
        punpckhbw xmm1,xmm7
        punpcklbw xmm2,xmm7
        punpckhbw xmm3,xmm7

    IF WEIGHTING
        pmullw xmm0,xmm5
        pmullw xmm1,xmm5
        pmullw xmm2,xmm5
        pmullw xmm3,xmm5
    ENDIF

        ; Write unpacked values (8 bits) to the scratch buffer: they're needed
        ; again in the axis calculation and we don't have the registers to keep
        ; them around (we probably can on x64)
        movdqa [tmp_base_reg-TMP_TMP],xmm0
        movdqa [tmp_base_reg-TMP_TMP+16],xmm1
        paddw xmm0,xmm1                        ; Start accumulating the result
        movdqa [tmp_base_reg-TMP_TMP+32],xmm2
        movdqa [tmp_base_reg-TMP_TMP+48],xmm3
        paddw xmm2,xmm3

        lea src_reg,[src_reg+2*stride_reg]
        paddw xmm0,xmm2

        movdqu xmm2,[src_reg]
        movdqu xmm4,[src_reg+stride_reg]

        COLOURSPACE_TRANSFORM xmm2,xmm4

    IF X64
            movdqa xmm6,xmm2
            movdqa xmm9,xmm4
            punpcklbw xmm2,xmm7
            punpckhbw xmm6,xmm7
            punpcklbw xmm4,xmm7
            punpckhbw xmm9,xmm7
        IF WEIGHTING
            pmullw xmm2,xmm5
            pmullw xmm6,xmm5
            pmullw xmm4,xmm5
            pmullw xmm9,xmm5
        ENDIF

            ; 7, 6, 8 and 9 are the inputs to the axis comp (so no need to write to the scratchpad)
            movdqa xmm7,xmm2
            movdqa xmm8,xmm4
            paddw xmm2,xmm6
            paddw xmm4,xmm9
    ELSE    ; X64
            movdqa xmm1,xmm2
            movdqa xmm6,xmm4
            punpcklbw xmm2,xmm7
            punpckhbw xmm1,xmm7
            punpcklbw xmm4,xmm7
            punpckhbw xmm6,xmm7
        IF WEIGHTING
            pmullw xmm2,xmm5
            pmullw xmm1,xmm5
            pmullw xmm4,xmm5
            pmullw xmm6,xmm5
        ENDIF

            ; 7 and 6 are the inputs to the axis comp (so no need to write to the scratchpad)
            movdqa [tmp_base_reg-TMP_TMP+64],xmm2
            movdqa [tmp_base_reg-TMP_TMP+80],xmm1
            paddw xmm2,xmm1
            movdqa xmm7,xmm4
            paddw xmm4,xmm6
    ENDIF    ; X64

        paddw xmm0,xmm2

        paddw xmm0,xmm4            ; xmm0 has 8 value sums in each qword

        pshufd xmm1,xmm0,SHUFFLE_SELECT(2,3,0,1)    ; swap so result is in both qwords
        paddw xmm0,xmm1

        ; Convert the average to have the correct fractional bit count
        ; This is some combination of a real or virtual divide by 16 in total to generate the average, so
        ; xmm0 now has the average in low 4 words 0,R,G,B in 8.0 to 8.4 fixed point format

    IF AVG_SHIFT_BITS
        paddw xmm0,[avg_round]
        psrlw xmm0,AVG_SHIFT_BITS
    ENDIF
        movdqa [tmp_base_reg-TMP_AVG],xmm0

ENDM    ; AVERAGE_RGB



IF X64

    ; For X64, the ability to 4-way interleave is slightly faster even taking into account the (significant)
    ; overhead of saving the registers (each register saved is more than 1 MB/s off the rate)
AXIS_3C_I_64        MACRO offset, first, last
    IF first
        pxor xmm2,xmm2
    ELSE
        movdqa xmm7,[tmp_base_reg-offset]        ; fetch two RGBs
        movdqa xmm6,[tmp_base_reg-offset+16]
        movdqa xmm8,[tmp_base_reg-offset+32]
        movdqa xmm9,[tmp_base_reg-offset+48]
    ENDIF
        INPUT_SHIFT xmm7            ; Convert to 8.x fixed point. Note we don't need bit replication (avg of 16 255s is 255.0)
        INPUT_SHIFT xmm6            ; This is a nop if the weight and avg fractional bits match
        INPUT_SHIFT xmm8
        INPUT_SHIFT xmm9
            pxor xmm3,xmm3
            movdqa xmm1,xmm3    ; move is cheaper than repeated xors
            movdqa xmm10,xmm3
            movdqa xmm11,xmm3
        psubw xmm7,xmm0            ; subtract avg
        psubw xmm6,xmm0
        psubw xmm8,xmm0
        psubw xmm9,xmm0
;    IF last EQ 0        ; Oddly this appears to be slower on X64?
            movdqa [tmp_base_reg-offset],xmm7        ; write RGB-avg back as it will be reused later
            movdqa [tmp_base_reg-offset+16],xmm6
;    ENDIF
            movdqa [tmp_base_reg-offset+32],xmm8
            movdqa [tmp_base_reg-offset+48],xmm9
        pshuflw xmm5,xmm7,SHUFFLE_SELECT(2,0,1,3)    ; R B G 0; lines up with B G R 0 to produce RB / BG / GR axis ordering info
        pshuflw xmm4,xmm6,SHUFFLE_SELECT(2,0,1,3)
        pshuflw xmm12,xmm8,SHUFFLE_SELECT(2,0,1,3)
        pshuflw xmm13,xmm9,SHUFFLE_SELECT(2,0,1,3)
            psubw xmm3,xmm7            ; -axis
            psubw xmm1,xmm6
            psubw xmm10,xmm8
            psubw xmm11,xmm9
        pshufhw xmm5,xmm5,SHUFFLE_SELECT(2,0,1,3)
        pshufhw xmm4,xmm4,SHUFFLE_SELECT(2,0,1,3)
        pshufhw xmm12,xmm12,SHUFFLE_SELECT(2,0,1,3)
        pshufhw xmm13,xmm13,SHUFFLE_SELECT(2,0,1,3)
            pmaxsw xmm3,xmm7        ; abs(axis)
            pmaxsw xmm1,xmm6
            pmaxsw xmm10,xmm8
            pmaxsw xmm11,xmm9
        psraw xmm5,16                ; state of sign bit
        psraw xmm4,16
        psraw xmm12,16
        psraw xmm13,16
            paddw xmm1,xmm3        ; accumulate axis
            paddw xmm10,xmm11
            paddw xmm1,xmm10
    IF first EQ 0
            paddw xmm1,[tmp_base_reg-TMP_AXIS]
    ENDIF
        pandn xmm5,xmm7
        pandn xmm4,xmm6
        pandn xmm12,xmm8
        pandn xmm13,xmm9
    IF last EQ 0
            movdqa [tmp_base_reg-TMP_AXIS], xmm1
    ENDIF
        paddw xmm5,xmm4
        paddw xmm12,xmm13

        paddw xmm2,xmm5            ; accumulate order
        paddw xmm2,xmm12
ENDM    ; AXIS_3C_I_64

ELSE    ; X64

AXIS_3C_I        MACRO offset, first, last
    IF first
        pxor xmm2,xmm2
    ELSE
        movdqa xmm7,[tmp_base_reg-offset]        ; fetch two RGBs
        movdqa xmm6,[tmp_base_reg-offset+16]
    ENDIF
        INPUT_SHIFT xmm7            ; Convert to 8.x fixed point. Note we don't need bit replication (avg of 16 255s is 255.0)
        INPUT_SHIFT xmm6            ; This is a nop if the weight and avg fractional bits match
            pxor xmm3,xmm3
            pxor xmm1,xmm1
        psubw xmm7,xmm0            ; subtract avg
        psubw xmm6,xmm0
    IF last EQ 0
            movdqa [tmp_base_reg-offset],xmm7        ; write RGB-avg back as it will be reused later
            movdqa [tmp_base_reg-offset+16],xmm6
    ENDIF
        pshuflw xmm5,xmm7,SHUFFLE_SELECT(2,0,1,3)    ; R B G 0; lines up with B G R 0 to produce RB / BG / GR axis ordering info
        pshuflw xmm4,xmm6,SHUFFLE_SELECT(2,0,1,3)
            psubw xmm3,xmm7            ; -axis
            psubw xmm1,xmm6
        pshufhw xmm5,xmm5,SHUFFLE_SELECT(2,0,1,3)
        pshufhw xmm4,xmm4,SHUFFLE_SELECT(2,0,1,3)
            pmaxsw xmm3,xmm7        ; abs(axis)
            pmaxsw xmm1,xmm6
        psraw xmm5,16                ; state of sign bit
        psraw xmm4,16
            paddw xmm1,xmm3        ; accumulate axis
    IF first EQ 0
            paddw xmm1,[tmp_base_reg-TMP_AXIS]
    ENDIF
        pandn xmm5,xmm7
        pandn xmm4,xmm6
    IF last EQ 0
            movdqa [tmp_base_reg-TMP_AXIS], xmm1
    ENDIF
        paddw xmm2,xmm5            ; accumulate order
        paddw xmm2,xmm4
ENDM    ; AXIS_3C_I

ENDIF



; Calculate the axis vector
AXIS_3COMPONENT        MACRO no_axis
        ; Expects:    TMP_TMP to have expanded RGB values
        ;            xmm0 is the average in the form R G B 0 R G B 0
        ; Is expected to:    set TMP_TMP to RGB-avg
        ;                    set xmm7 to the axis

        ; G is the priority axis and the axis ordering info is set accordingly. The method used
        ; is far from perfect, but suffices for most cases.
    IF X64
        AXIS_3C_I_64 TMP_TMP+64, 1, 0
        AXIS_3C_I_64 TMP_TMP   , 0, 1
    ELSE
        AXIS_3C_I TMP_TMP+96, 1, 0
        AXIS_3C_I TMP_TMP+32, 0, 0
        AXIS_3C_I TMP_TMP+64, 0, 0
        AXIS_3C_I TMP_TMP   , 0, 1
    ENDIF

        ; parallel add xmm1 and xmm2 to get final absolute axis info
        pshufd xmm3,xmm1,SHUFFLE_SELECT(2,3,0,1)
        pshufd xmm4,xmm2,SHUFFLE_SELECT(2,3,0,1)
        pxor xmm5,xmm5
        paddw xmm1,xmm3            ; Final summed absolute axis
        paddw xmm2,xmm4            ; Final summed pos
        movdqa xmm0,xmm5

        ; axis is in 8.8 fixed point - it needs normalisation and ordering (the signs set correctly)
        ; By default, G is the major axis, and BG and RG pos define the orders of the B and R axes
        ; If G isn't the major axis (i.e. it is 0) R becomes the major axis and RB pos defines the
        ; order of the B axis. If RB_pos is also 0, B is the major axis and is positive.

        ; If the axis is 0 we have a constant-colour block - we must catch this here (division by
        ; zero in normalisation otherwise)

        pcmpeqw xmm5,xmm1
        pmovmskb eax,xmm2        ; eax is the sign-flip bitvector (axis_neg)
        pcmpeqw xmm2,xmm0

        pmovmskb ecx,xmm5        ; ecx is the axis equals 0 bitvector
        pmovmskb efx,xmm2        ; efx is the order equals 0 bitvector (axis_order_zero)

        ; Finish up the no-axis check.
        and ecx,02ah
        cmp ecx,02ah
        je no_axis

        ; We need to normalise the axis below, and negation is easier in float, so do it there
        punpcklwd xmm1,xmm0                            ; The axis is always positive so we can unpack with 0

            ; Axis ordering: we have put two bitvectors into eax and efx.
            ; The bits are 1 RB, 3 GB, 5 GR - 0,2,4,6+ are junk, so we mask them off
            ; 5 values in here affect the axis ordering:
            ; If GR and GB == 0, then we need to apply RB's sign to B
            ; Otherwise, we apply GR to R and GB to B
            ; See the AXIS_ORDER_ENTRY macro for how we create the 512-byte table of negation bits
            and eax,02ah
            and efx,028h        ; We don't need rb of axis_zero


        ; Promote both to float
        cvtdq2ps xmm2,xmm1
            xorps xmm5,xmm5

        ; We can do the normalisation and set the axis signs in parallel
        movaps xmm1,xmm2
        mulps xmm2,xmm2

            shl eax,3            ; x8, with empty lower bit, is a 16-byte aligned pointer to the 32 entry table
            shl efx,2            ; One shift less to slot into the gaps

        ; 3D parallel add
        movaps xmm4,xmm2
        movhlps xmm0,xmm2        ; note that xmm0 is still 0 from the pxor above which cleans things up a little
        shufps xmm2,xmm2,SHUFFLE_SELECT(1,1,1,1)
        addss xmm4,xmm0
            or eax,efx            ; bits low to high: 0,0,0,0, rb_neg, gb_zero, gb_neg, gr_zero, gr_neg
        addss xmm2,xmm4
    IF X64
            lea rfx,axis_order_table
    ENDIF

        ; low of xmm7 is the DP result
        ; We know that this cannot be 0 in the int implementation
        ; - the axis was known to be nonzero on at least one component
        ; - this is known to be representable exactly in float as it's less than 24 bits in magnitude
        ; - the square cannot be small, exponent is positive and it also gives positive results in each component
        ; - they cannot therefore sum to 0
        rsqrtss xmm2, xmm2        ; No need for Newton-Raphson, ~15 bits of precision is fine

            ; Apply the axis ordering; we also need the result in the correct 1.15 format when we
            ; send it back, so this scaling factor is baked into the axis order table
    IF X64
            mulps xmm1, [rfx + rax]
    ELSE
            mulps xmm1, [axis_order_table + eax]
    ENDIF
            
        shufps xmm2, xmm2, SHUFFLE_SELECT(0, 0, 0, 0)

        ; Normalise, apply axis order, and scale to 1.15
        mulps xmm2, xmm1

        ; Get it back into int
        cvtps2dq xmm1,xmm2
        packssdw xmm1,xmm1                ; This duplicates as well, which is what we want
        movdqa [tmp_base_reg-TMP_AXIS], xmm1
ENDM    ; AXIS_3COMPONENT





POS_3C_8_VALUES    MACRO offset, first
        movdqa xmm0,xmm1        ; Copy across the axis. It's a faster to copy then multiply-load, one reason
        movdqa xmm2,xmm1        ; is likely because the maddwd issues at 1/clock at best while movdqa can
        movdqa xmm3,xmm1        ; parallelise up to 3 per clock, so there's more breathing room for the loads

    IF first AND (X64 EQ 0)        ; This optimisation appears to be oddly slower on X64
        pmaddwd xmm0,xmm7
        pmaddwd xmm1,xmm6
    ELSE
        pmaddwd xmm0,[tmp_base_reg-offset]    
        pmaddwd xmm1,[tmp_base_reg-offset+16]
    ENDIF
        pmaddwd xmm2,[tmp_base_reg-offset+32]
        pmaddwd xmm3,[tmp_base_reg-offset+48]

        movdqa xmm4,xmm0        ; We could do this with shift or pshufd. Shift has lower code density
        movdqa xmm5,xmm1        ; but is fast on more CPUs, and pshufd shows little if any gain
        movdqa xmm6,xmm2
        movdqa xmm7,xmm3
        psllq xmm0,32
        psllq xmm1,32
        psllq xmm2,32
        psllq xmm3,32

        paddd xmm0,xmm4            ; We now have 2 results in dwords 1 and 3 of the XMM. This is important below...
        paddd xmm1,xmm5            ; The result is in 9.(15+AVG_FRAC_BITS) format
        paddd xmm2,xmm6
        paddd xmm3,xmm7
        psrad xmm0,10+AVG_FRAC_BITS    ; Scale to the desired 9.5 format
        psrad xmm1,10+AVG_FRAC_BITS    ; (9.5 is convenient because it becomes 8.4 when multiplied by axis)
        psrad xmm2,10+AVG_FRAC_BITS
        psrad xmm3,10+AVG_FRAC_BITS
        packssdw xmm0,xmm1        ; Pack so we now have 4 word results in words 1, 3, 5 and 7
        packssdw xmm2,xmm3

    IF first
        movdqa xmm1, [tmp_base_reg-TMP_AXIS]    ; Reload the axis we corrupted above
    ENDIF

        psrad xmm0,16            ; Because we cunningly arranged the results to be in the high word of
        psrad xmm2,16            ; each dword, a sign-preserving shift puts it right for the final pack

        packssdw xmm0,xmm2        ; 8 results in xmm0
ENDM    ; POS_3C_8_VALUES





; These should be reusable whatever the axis and average format are
POS_MINMAX        MACRO 
        ; using the xmm0 and xmm2 results we calculate a final minmax
        movdqa xmm3,xmm2
        pmaxsw xmm2,xmm0
        pminsw xmm3,xmm0
        pshufd xmm4,xmm2,SHUFFLE_SELECT(2,3,0,1)    ; dword halves
        pshufd xmm5,xmm3,SHUFFLE_SELECT(2,3,0,1)
        pmaxsw xmm2,xmm4
        pminsw xmm3,xmm5
        pshuflw xmm4,xmm2,SHUFFLE_SELECT(2,3,0,1)    ; word halves
        pshuflw xmm5,xmm3,SHUFFLE_SELECT(2,3,0,1)
        pmaxsw xmm2,xmm4                            ; Final max
        pminsw xmm3,xmm5                            ; Final min
        pshuflw xmm4,xmm2,SHUFFLE_SELECT(1,0,1,0)    ; word quarters
        pshuflw xmm5,xmm3,SHUFFLE_SELECT(1,0,1,0)
        pmaxsw xmm2,xmm4                            ; Final max
        pminsw xmm3,xmm5                            ; Final min

        ; if min == max == 0 then we have a single-colour block. This shouldn't be able to
        ; happen, as these should be caught when there is no axis instead.
ENDM


POS_CENTRE        MACRO 
        ; Calculate centre
        ; Centre = (A+B)/2    (and then min = -max)

        ; Replicate low and high halves; note shuffles above leave halves already the same
        pshufd xmm7,xmm2,SHUFFLE_SELECT(0,0,0,0)    ; max
        pshufd xmm2,xmm3,SHUFFLE_SELECT(0,0,0,0)

        paddw xmm2,xmm7
    IF PROG_THRESHOLD
            movdqa xmm4, xmmword ptr [prog_threshold_size]
    ENDIF
        psraw xmm2,1                    ; xmm2 = offset; arithmetic shift to preserve sign

        psubw xmm7,xmm2                    ; offset max to new centre

        ; save offset centre (9.5 format) for later correction of refined X
        movdqa [tmp_base_reg-TMP_CENTRE], xmm2
ENDM


POS_OFFSET        MACRO reread
        ; Offset points to centre, forming array of abs(P-centre) and centre-side cluster bits
    IF reread
        movdqa xmm0,[tmp_base_reg-TMP_POS]
        movdqa xmm1,[tmp_base_reg-TMP_POS+16]
    ENDIF
    IF PROG_THRESHOLD
            pcmpgtw xmm4,xmm7
    ENDIF
        psubw xmm0,xmm2
        psubw xmm1,xmm2
        pxor xmm2,xmm2
        pxor xmm3,xmm3
        pmovmskb eax,xmm0                ; Save clustering
        pmovmskb efx,xmm1
        psubw xmm2,xmm0
        psubw xmm3,xmm1
        shl efx,15
        shr eax,1
        or eax,efx
        and eax,055555555h
        mov [tmp_base_reg-TMP_CLUSTER],eax

        pmaxsw xmm0,xmm2                ; abs(P-centre)
        pmaxsw xmm1,xmm3
    IF NO_PROG EQ 0
        movdqa [tmp_base_reg-TMP_POS],xmm0
        movdqa [tmp_base_reg-TMP_POS+16],xmm1
    ENDIF
    IF PROG_THRESHOLD
            pmovmskb ecx,xmm4
    ENDIF

ENDM





POS_3COMPONENT            MACRO no_prog
        ; Expects:    TMP_TMP to have expanded RGB-average values
        ;            xmm1 == axis in form R G B 0 R G B 0
        ; Is expected to:    put abs(P-centre) in TMP_POS
        ;                    put 'centre side' info in TMP_CLUSTER_CENTRE

        ; We need to generate 16 values, but we don't have enough XMMs, so we do two halves
        POS_3C_8_VALUES TMP_TMP, 1
        movdqa [tmp_base_reg-TMP_POS],xmm0
        POS_3C_8_VALUES (TMP_TMP-64), 0
        movdqa [tmp_base_reg-TMP_POS+16],xmm0

        movdqa xmm2,[tmp_base_reg-TMP_POS]
        POS_MINMAX
        POS_CENTRE
        POS_OFFSET 1

ENDM    ; POS_3COMPONENT




NOPROG_CLUSTER            MACRO
        movdqa xmm6,xmm7
        paddw xmm6,xmm6
        pmulhw xmm6, xmmword ptr [scale_one_third]

        psubw xmm1,xmm6
        psubw xmm0,xmm6                    ; abs(P-centre) - 2/3x

        pmovmskb efx,xmm1
        pmovmskb eax,xmm0                ; 4-block clustering

        ; Save the split point clustering data
        shl efx,16
        or eax,efx
        and eax,0aaaaaaaah
        mov [tmp_base_reg-TMP_CLUSTER+4],eax
    ENDM




; Attempt a (simple) progressive refinement step to reduce noise in the
; output image by trying to find a better overall match for the endpoints
; than the first-guess solution (the extremities of the input signal)

; The method is to move the endpoints inwards until a local MSE minima is found.

PROG            MACRO 
        LOCAL next_refinement_loop
        LOCAL refinement_done
        LOCAL no_prog

        ; Expects:    xmm7 (all words) is the initial max value
        ;            TMP_POS has been set up with the array of 16 words

    IF COUNT_PROG_STEPS
        xor eax,eax
    ENDIF

    IF PROG_THRESHOLD
        ; If we're below the prog threshold, we can use the no-refinement clustering
        ; (which doesn't have to calculate MSE)
        test ecx,2
        je doprog
        NOPROG_CLUSTER
        jmp no_prog
    doprog:
    ENDIF

        movdqa [tmp_base_reg-TMP_BEST],xmm7
        movq mm1,mmword ptr [max_sint32]    ; Initialise max error; scalar, leverage the MMX unit

        next_refinement_loop:
            movdqa xmm6,xmm7        ; Save the current X (since we corrupt it)

            pmulhw xmm7, xmmword ptr [scale_one_third]

            ; Calculate E4 (4-colour block MSE)
            ; xmm0 and 1 are already abs(P-centre)
            pxor xmm2,xmm2
            pxor xmm3,xmm3
            psubw xmm0,xmm7            ; Since it all parallelises nicely, it's faster to subtract twice (and uses a register less)
            psubw xmm1,xmm7
            psubw xmm0,xmm7            ; abs(P-centre) - 2/3x
            psubw xmm1,xmm7
    IF COUNT_PROG_STEPS EQ 0
            pmovmskb eax,xmm0        ; 4-block clustering
            pmovmskb efx,xmm1
    ENDIF

;            jmp refinement_done

            psubw xmm2,xmm0
            psubw xmm3,xmm1
            pmaxsw xmm0,xmm2
            pmaxsw xmm1,xmm3        ; abs(abs(P-centre)-2/3x)

            psubw xmm0,xmm7            ; abs(abs(P-centre)-2/3x) - 1/3x
            psubw xmm1,xmm7

            pmaddwd xmm0,xmm0        ; 4 mean-square-error values, doing part of the parallel add
            pmaddwd xmm1,xmm1

            paddd xmm0,xmm1

            pshufd xmm1,xmm0,SHUFFLE_SELECT(2,3,0,1)    ; This is a big cost
            paddd xmm0,xmm1

            ; Move to MMX for this last bit; faster on all but the most recent CPUs
            movdq2q mm0,xmm0
            movdq2q mm2,xmm0
            psrlq mm0,32
            paddd mm0,mm2

            ; Compare E4 with current minimum error and choose result
            pcmpgtd mm1,mm0

            pmovmskb ecx,mm1

            ; Pause here while that result becomes available, so read these up for go-around
            movdqa xmm0,[tmp_base_reg-TMP_POS]        ; Read up the source values of abs(P-centre)
            movdqa xmm1,[tmp_base_reg-TMP_POS+16]

            test ecx,8
            jz refinement_done

            movq mm1,mm0            ; Save the better max error

            ; Go around
        go_around:
            movdqa xmm7,xmm6
            movdqa [tmp_base_reg-TMP_BEST],xmm6

    IF COUNT_PROG_STEPS
            add eax,1
    ELSE
            ; Save the split point clustering data
            shl efx,16
            or eax,efx
            and eax,0aaaaaaaah
            mov [tmp_base_reg-TMP_CLUSTER+4],eax
    ENDIF

            ; Fixed step size, tunable
            psubw xmm7,[stepsize]

            ; Used to check for a negative stepsize here, but stopped that because I think it's
            ; impossible that the error could be less than it was the previous time and the
            ; movmsk/test combination is expensive.
            jmp next_refinement_loop


    refinement_done:
        movdqa xmm7,[tmp_base_reg-TMP_BEST]
    no_prog:

ENDM    ; PROG




PROG_34            MACRO 
        LOCAL next_refinement_loop
        LOCAL refinement_done
        LOCAL no_prog

        ; Expects:    xmm7 (all words) is the initial max value
        ;            TMP_POS has been set up with the array of 16 words

    IF COUNT_PROG_STEPS
        xor eax,eax
    ENDIF

    IF PROG_THRESHOLD
        ; If we're below the prog threshold, we can use the no-refinement clustering
        ; XXX - this means that all threshold blocks get 4-colour clustering
        test ecx,2
        je doprog
        NOPROG_CLUSTER
        xor ecx,ecx        ; 4-colour block
        jmp no_prog
    doprog:
    ENDIF

        movdqa [tmp_base_reg-TMP_BEST],xmm7
        movq mm1,mmword ptr [max_sint32]    ; Initialise max error; scalar, leverage the MMX unit

        next_refinement_loop:
            movdqa [tmp_base_reg-TMP_CURRENT],xmm7
                movdqa xmm6,xmm7
            pmulhw xmm7, xmmword ptr [scale_one_third]
                psrlw xmm6,1        ; 1/2 x


            ; Calculate E3 and E4 (3 and 4-colour block MSE)
            ; xmm0 and 1 are already abs(P-centre)

                movdqa xmm4,xmm0
                movdqa xmm5,xmm1

IF X64
            ; This X64 path is a surprisingly marginal gain, probably because we can get decent
            ; loop-to-loop parallelism here
            pxor xmm2,xmm2
            movdqa xmm3,xmm2
            movdqa xmm8,xmm2
            movdqa xmm9,xmm2
            psubw xmm0,xmm7            ; Since it all parallelises nicely, it's faster to subtract twice (and uses a register less)
            psubw xmm1,xmm7
            psubw xmm4,xmm6            ; abs(P-centre) - 1/2x
            psubw xmm5,xmm6
            psubw xmm0,xmm7            ; abs(P-centre) - 2/3x
            psubw xmm1,xmm7
    IF COUNT_PROG_STEPS EQ 0
            pmovmskb egx,xmm4        ; 3-block clustering
            pmovmskb ehx,xmm5
            pmovmskb eax,xmm0        ; 4-block clustering
            pmovmskb efx,xmm1
    ENDIF

            psubw xmm2,xmm0
            psubw xmm3,xmm1
            psubw xmm8,xmm4
            psubw xmm9,xmm5
            pmaxsw xmm0,xmm2
            pmaxsw xmm1,xmm3        ; abs(abs(P-centre)-2/3x)
            pmaxsw xmm4,xmm8
            pmaxsw xmm5,xmm9        ; abs(abs(P-centre)-1/2x)
ELSE
            ; Calculate E4 (4-colour block MSE)
            ; xmm0 and 1 are already abs(P-centre)
            pxor xmm2,xmm2
            pxor xmm3,xmm3
            psubw xmm0,xmm7            ; Since it all parallelises nicely, it's faster to subtract twice (and uses a register less)
            psubw xmm1,xmm7
            psubw xmm0,xmm7            ; abs(P-centre) - 2/3x
            psubw xmm1,xmm7
    IF COUNT_PROG_STEPS EQ 0
            pmovmskb eax,xmm0        ; 4-block clustering
            pmovmskb efx,xmm1
    ENDIF

                psubw xmm4,xmm6        ; abs(P-centre)-2/3x)
                psubw xmm5,xmm6
                pmovmskb egx,xmm4    ; 3-block clustering
                pmovmskb ehx,xmm5

            psubw xmm2,xmm0
            psubw xmm3,xmm1
            pmaxsw xmm0,xmm2
            pmaxsw xmm1,xmm3        ; abs(abs(P-centre)-2/3x)

                pxor xmm2,xmm2
                pxor xmm3,xmm3
                psubw xmm2,xmm4
                psubw xmm3,xmm5
                pmaxsw xmm4,xmm2
                pmaxsw xmm5,xmm3    ; abs(abs(P-centre)-1/2x)
ENDIF

;            jmp refinement_done

            psubw xmm0,xmm7            ; abs(abs(P-centre)-2/3x) - 1/3x
            psubw xmm1,xmm7
                psubw xmm4,xmm6        ; abs(abs(P-centre)-1/2x) - 1/2x
                psubw xmm5,xmm6

            pmaddwd xmm0,xmm0        ; 4 mean-square-error values, doing part of the parallel add
            pmaddwd xmm1,xmm1
                pmaddwd xmm4,xmm4
                pmaddwd xmm5,xmm5

            paddd xmm0,xmm1
                paddd xmm4,xmm5

            pshufd xmm1,xmm0,SHUFFLE_SELECT(2,3,0,1)    ; This is a big cost
                pshufd xmm5,xmm4,SHUFFLE_SELECT(2,3,0,1)
            paddd xmm0,xmm1
                paddd xmm4,xmm5

            ; Move to MMX for this last bit; faster on all but the most recent CPUs
            movdq2q mm0,xmm0
                movdq2q mm4,xmm4
            movdq2q mm2,xmm0
                movdq2q mm6,xmm4
            psrlq mm0,32
                psrlq mm4,32
            paddd mm0,mm2
                paddd mm4,mm6

            ; Compare E4 (and E3 if present) with current minimum error and choose result
                movq mm5,mm1
            pcmpgtd mm1,mm0

            pmovmskb ecx,mm1

            ; Pause here while that result becomes available, so read these up for go-around
            movdqa xmm0,[tmp_base_reg-TMP_POS]        ; Read up the source values of abs(P-centre)
            movdqa xmm1,[tmp_base_reg-TMP_POS+16]

            test ecx,8
            jnz e4_good
                ; e4 is not better, is e3?
                pcmpgtd mm5,mm4
                pmovmskb ecx,mm5
                test ecx,8
                jz refinement_done
                ; e3 is the new best
        e3_best:
                movq mm1,mm4
                mov ecx,0ffffffffh    ; 3-colour block
                mov eax,egx
                mov efx,ehx
                jmp go_around

        e4_good:
                ; is e3 better than e4?
                movq mm5,mm0
                pcmpgtd mm5,mm4
                pmovmskb ecx,mm5
                test ecx,8
                jnz e3_best
                xor ecx,ecx            ; 4-colour block

            movq mm1,mm0            ; Save the better max error

            ; Go around
        go_around:
            mov [tmp_base_reg-TMP_CLUSTER+8],ecx
            movdqa xmm7,[tmp_base_reg-TMP_CURRENT]
            movdqa [tmp_base_reg-TMP_BEST],xmm7

    IF COUNT_PROG_STEPS
            add eax,1
    ELSE
            ; Save the split point clustering data
            shl efx,16
            or eax,efx
            and eax,0aaaaaaaah
            mov [tmp_base_reg-TMP_CLUSTER+4],eax
    ENDIF

            ; Fixed step size
            psubw xmm7,[stepsize]

            jmp next_refinement_loop


    refinement_done:
        movdqa xmm7,[tmp_base_reg-TMP_BEST]
    no_prog:
    
ENDM    ; PROG_34







COLOUR_AVERAGE            MACRO output_reg, use_3component

        ; Output the average if there is no axis

        ; input is:
        ; average (8.4)

        movq mm0,[tmp_base_reg-TMP_AVG]
    IF AVG_FRAC_BITS LT 4
        psllw mm0,4-AVG_FRAC_BITS            ; Convert average to 8.4 format
    ENDIF

    IF WEIGHTING AND use_3component
        pmullw mm0,[unweighting]
    ENDIF
    IF UNROUNDING
        paddw mm0, mmword ptr [round_565]
        movq mm3, mmword ptr [scale_to_round]
        pmulhw mm3,mm0
        pand mm3,mmword ptr [round_mask]
        psubw mm0,mm3
    ENDIF

        pmulhw mm0,mmword ptr [scale_8_4_to_565]                ; 0 R G B in 565 format
        pminsw mm0,mmword ptr [clamp_565]
        movq mm2,mm0
    IF use_3component
        movq mm4,mm0
        pand mm0, [mask_third_word]
        psrlq mm0,(32-11)
        psrlq mm2,(16-5)
        por mm2,mm4
    ELSE
        psrlq mm2,(16-5)
    ENDIF
        por mm0,mm2

        ; Duplicate
        punpcklwd mm0,mm0
        pand mm0,[mask_low_dword]
        movq [output_reg],mm0
ENDM    ; COLOUR




COLOUR        MACRO output_reg, use_3component
        ; input is:
        ; x (9.5, always > 0) in xmm7 (all words)
        ; centre (9.5)
        ; axis (1.15)
        ; average (8.AVG_FRAC_BITS)

    IF COUNT_PROG_STEPS
        sub eax,1
        test eax,0ffffff8h
        jz noclamp
        mov eax,8
    noclamp:
        mov eax,[col_table+eax*4]
        mov [edx],eax
        mov dword ptr [edx+4],0
    ELSEIF SHOW_BLOCK_TYPES
        mov ecx,[col_table+4]
        mov efx,[col_table+8]
        mov eax,[tmp_base_reg-TMP_CLUSTER+8]
        cmp eax,3
        cmove ecx,efx
        mov [edx],ecx
        mov dword ptr [edx+4],0
    ELSE


;        pxor xmm7,xmm7    ; to force the refined X to 0

        ; This is mostly done in MMX code, which is faster on some machines and (slightly)
        ; slower only on very recent chips as there are no independent chains to
        ; execute in the gaps

        ; The two colours are avg +- ((x+-centre)*axis)
        movq mm0,[tmp_base_reg-TMP_AVG]
    IF AVG_FRAC_BITS LT 4
        psllw mm0,4-AVG_FRAC_BITS            ; Convert average to 8.4 format
    ENDIF
        movdq2q mm1,xmm7                    ; 9.5 format x
        movdq2q mm2,xmm7
        movq mm7,[tmp_base_reg-TMP_CENTRE]    ; 9.5 format centre offset
        movq mm5,[tmp_base_reg-TMP_AXIS]
        paddw mm1,mm7                        ; 9.5 format x+-centre
        psubw mm2,mm7
    IF UNROUNDING
        IF WEIGHTING AND use_3component
            paddw mm0, mmword ptr [round_565_weighted]
        ELSE
            paddw mm0, mmword ptr [round_565]    ; 8.4 format average
        ENDIF
    ENDIF
        pmulhw mm1,mm5                        ; 8.4 format (x+-centre)*axis
        pmulhw mm2,mm5
        pxor mm4,mm4
        paddw mm1,mm0                        ; avg + axis offset
        psubw mm0,mm2                        ; avg - axis offset
        pmaxsw mm0,mm4                        ; Clamp to positive range (can't use addusw, axis is signed)
        pmaxsw mm1,mm4

    IF WEIGHTING AND use_3component
        pmullw mm0,mmword ptr [unweighting]
        pmullw mm1,mmword ptr [unweighting]
    ENDIF

    IF UNROUNDING
        ; The (canonical) DXTC decompressor uses (should use) bit replication to generate
        ; 888 colour values from the 565 input. We therefore need to tweak our colours here
        ; to take account of this. The procedure is to add 0.5 and then subtract between
        ; 0 and 1 depending on the value of the input. We aren't in the 565 colourspace yet,
        ; still 8.4 fixed, but we can scale accordingly as long as we mask out the bits we don't want
        ; to contribute (consider 1F->FF, 1E->F7, 1D->EF, 1C->E7 but 1B->DE, we must only
        ; apply the top three bits in the unrounding process for R/B and the top two for G).
        movq mm3, mmword ptr [scale_to_round]    ; This is 5 6 5 right shifts
        movq mm4,mm0
        movq mm5,mm1
        movq mm6, mmword ptr [round_mask]
        pmulhw mm4,mm3
        pmulhw mm5,mm3
        pand mm4,mm6            ; Truncate off bits below the threshold point at which they influence the result
        pand mm5,mm6
        psubw mm0,mm4
        psubw mm1,mm5
    ENDIF

        movq mm3, mmword ptr [scale_8_4_to_565]        ; Encodes the appropriate shifts
        pmulhw mm0,mm3            ; 0 R G B in 565 format
        pmulhw mm1,mm3

        movq mm5,mmword ptr [clamp_565]
        pminsw mm0,mm5
        pminsw mm1,mm5

    IF use_3component
        pshufw mm4,mm0,SHUFFLE_SELECT(1,3,3,3)
        pshufw mm5,mm1,SHUFFLE_SELECT(1,3,3,3)
        pshufw mm2,mm0,SHUFFLE_SELECT(2,3,3,3)
        pshufw mm3,mm1,SHUFFLE_SELECT(2,3,3,3)
        psllw mm4,5
        psllw mm5,5
        psllw mm2,11
        psllw mm3,11
        por mm0,mm4
        por mm1,mm5
    ELSE
        movq mm4,mmword ptr [mask_low_dword]
        pand mm0,mm4 
        pand mm1,mm4 
        pshufw mm2,mm0,SHUFFLE_SELECT(1,3,3,3)
        pshufw mm3,mm1,SHUFFLE_SELECT(1,3,3,3)
        psllw mm2,5
        psllw mm3,5
    ENDIF
        por mm0,mm2
        por mm1,mm3

        ; mm0 and mm1 are c0 and c1

        ; Read up the cluster information we'll need
        movd mm7,dword ptr [tmp_base_reg-TMP_CLUSTER]        ; low/high (low bit)
        movd mm6,dword ptr [tmp_base_reg-TMP_CLUSTER+4]        ; endpoint/splitpoint (high bit)
    IF USE_34
        movd mm5,dword ptr [tmp_base_reg - TMP_CLUSTER+8]    ; 3-colour flag
    ELSE
        movq mm5,[_0000000055555555]
    ENDIF

        ; Compare and write in correct order

        ; rdx contains the destination DXTC block (dx == DXTC)

        ; We need to use dword compares - compares are signed and we want 32 result bits anyway
        pshufw mm2,mm0,SHUFFLE_SELECT(0,3,3,3)        ; Word 3 is zero, saves a read up cf. an AND
        pshufw mm1,mm1,SHUFFLE_SELECT(0,3,3,3)
        movq mm4,mm2

        ; Create the mask to say which way round the colours are
        pcmpgtd mm2, mm1            ; mm2 is the swap mask

    IF USE_34        
        pxor mm2,mm5    ; swap the swap flag
        pand mm5,mm6    ; bits are set if 3-colour and endpoint/splitpoint bit is set
        psrld mm5,1
        pxor mm5, [_0000000055555555]    ; Form mask
        pand mm7,mm5    ; mask low/high
    ENDIF

        movq mm3,mm0
            pand mm5,mm2            ; mm5 is the cluster swap bit pattern

        pcmpeqd mm4, mm1            ; Set the equality flag for clustering info
            pxor mm7,mm5            ; Apply the cluster swap or not

        punpcklwd mm0, mm1            ; the two 565 colours in normal order
        punpcklwd mm1, mm3            ; the two 565 colours in reversed order
            por mm7,mm6                ; merge endpoint / splitpoint cluster with low/high cluster

        pand mm0, mm2
        pandn mm2, mm1

            pandn mm4,mm7            ; apply zero mask flag to cluster bits

        por mm0, mm2                ; one of the two colour sets as selected by mm5

        punpckldq mm0,mm4            ; merge the colour and the cluster bits
        movq [output_reg],mm0        ; and write

        ; 0 is the low word endpoint
        ; 1 is the high word endpoint
        ; 2 is the split point near 0
        ; 3 is the split point near 1

        ; For transparent blocks
        ; 0 is the low word endpoint
        ; 1 is the high word endpoint
        ; 2 is the interpolated point

    ENDIF    ; COUNT_PROG_STEPS


ENDM    ; COLOUR





IF 0

AVERAGE_RGB_RB        MACRO src_reg, stride_reg
        movdqa xmm0, xmmword ptr [mask_rb]

        ; Read up all 16 values
        ; Use DQU since we can't be certain they're aligned
        movdqu xmm4,[src_reg]
        movdqu xmm5,[src_reg+stride_reg]
        lea src_reg,[src_reg+2*stride_reg]
        movdqu xmm6,[src_reg]
        movdqu xmm7,[src_reg+stride_reg]

        ; RB is hugely convenient - we just need the AND and we have 4 (B R) pairs in each register
        pand xmm4,xmm0
        pand xmm5,xmm0
        pand xmm6,xmm0
        pand xmm7,xmm0

        ; We can keep the unpacked values in the scratch regs for now
        movdqa xmm0,xmm4
        movdqa xmm1,xmm5
        paddw xmm0,xmm6
        paddw xmm1,xmm7
        paddw xmm0,xmm1

        pshufd xmm1,xmm0,SHUFFLE_SELECT(2,3,0,1)    ; dword halves
        paddw xmm0,xmm1
        pshuflw xmm1,xmm0,SHUFFLE_SELECT(2,3,0,1)    ; word halves; do both sides, so we get the whole reg populated cheaper than a post shufd
        pshufhw xmm1,xmm1,SHUFFLE_SELECT(2,3,0,1)
        paddw xmm0,xmm1

        ; We now do a 'virtual divide' by 16 to generate the average, so
        ; xmm0 now has the average in all word pairs R,B in 8.4 fixed point format
        movdqa [tmp_base_reg-TMP_AVG],xmm0
    ENDM    ; AVERAGE_RGB_RB


ACCUMULATE_AXIS_2C    MACRO reg, lastreg, destination
        psllw reg,4
            pxor xmm4,xmm4
        psubw reg,xmm0
            movdqa destination,reg
        pshuflw xmm2,reg,SHUFFLE_SELECT(1,0,3,2)    ; reverse R and B for axis ordering calc
            psubw xmm4,reg        ; -axis
        pshufhw xmm2,xmm2,SHUFFLE_SELECT(1,0,3,2)
            pmaxsw xmm4,reg        ; abs(axis)
        psraw xmm2,16                                ; state of sign bit
            paddw xmm1,xmm4
        pandn reg,xmm2
        paddw reg,lastreg
    ENDM

    ; We need to specify the reg to be used for indexing on the macro call (for easy portability to x64)
AXIS_2COMPONENT        MACRO no_axis

        ; Expects:    xmm4-7 have expanded RB values
        ;            xmm0 is the average in the form R B R B R B R B
        ; Is expected to:    xmm1 to the absolute axis
        ;                    xmm2 to the axis ordering info (for each axis pair sum of one value when the other value is positive or zero)

        psllw xmm4,4
            pxor xmm1,xmm1
        psubw xmm4,xmm0
            movdqa [tmp_base_reg-TMP_TMP],xmm4
        pshuflw xmm3,xmm4,SHUFFLE_SELECT(1,0,3,2)    ; reverse R and B for axis ordering calc
            psubw xmm1,xmm4        ; -axis
        pshufhw xmm3,xmm3,SHUFFLE_SELECT(1,0,3,2)
            pmaxsw xmm1,xmm4    ; abs(axis)
        psraw xmm3,16                                ; state of sign bit
        pandn xmm3,xmm4                                ; pos

        ACCUMULATE_AXIS_2C xmm5, xmm3, [tmp_base_reg-TMP_TMP+16]
        ACCUMULATE_AXIS_2C xmm6, xmm5, [tmp_base_reg-TMP_TMP+32]
        ACCUMULATE_AXIS_2C xmm7, xmm6, [tmp_base_reg-TMP_TMP+48]

        ; xmm7 is now the axis ordering info

        ; parallel add the 4 results in xmm1 and xmm7->xmm2 to get final absolute axis info
        pshufd xmm3,xmm1,SHUFFLE_SELECT(2,3,0,1)    ; dword halves
        pshufd xmm2,xmm7,SHUFFLE_SELECT(2,3,0,1)
        paddw xmm1,xmm3
        paddw xmm2,xmm7
        pshuflw xmm3,xmm1,SHUFFLE_SELECT(2,3,0,1)    ; word halves
        pshuflw xmm7,xmm2,SHUFFLE_SELECT(2,3,0,1)
        paddw xmm1,xmm3            ; Final summed absolute axis
        paddw xmm2,xmm7            ; Final summed pos

        ; If the axis is 0 we have a constant-colour block - we must catch this here (division by zero otherwise)
        pxor xmm3,xmm3
        pcmpeqw xmm3,xmm1
        pmovmskb ecx,xmm3
        and ecx,0ah
        cmp ecx,0ah
        je no_axis
    ENDM    ; AXIS_2COMPONENT


AXIS_NORM_2COMPONENT        MACRO
        ; Expects: xmm1 to be the axis and xmm2 to be the ordering info
        ; Is expected to:    set TMP_TMP to RB-avg
        ;                    set xmm7 to the axis

        ; axis is in 8.8 fixed point - it needs normalisation and ordering (the signs set correctly)

        ; We need to normalise the axis below, and negation is easier in float, so promote to float
        pand xmm2, xmmword ptr [dword_word_mask]        ; We only need the B part of the sign to be meaningful (the R-B axis)
        pxor xmm4,xmm4
        punpcklwd xmm1,xmm4                            ; The axis is always positive so we can unpack with 0
        punpcklwd xmm4,xmm2                            ; Only the sign matters

        ; Promote both to float
        cvtdq2ps xmm7,xmm1
        cvtdq2ps xmm5,xmm4

        ; We can start the normalisation while the axis signs are being set
        movaps xmm1, xmm7
        xorps xmm2,xmm2

        ; magnitude is a 2D dot product
        mulps xmm7, xmm7

            cmpltps xmm5,xmm2            ; Signs of the axis ordering

        movaps xmm4,xmm7
        shufps xmm7,xmm7,SHUFFLE_SELECT(1,0,1,0)
        addss xmm7,xmm4

            andps xmm5,[b_sign_bit]

        ; low of xmm7 is the DP result
        ; If this is 0 we haven't actually got an axis, and we can't rsq it,
        ; so mask the output to 0 in this case. This generates an acceptable result
        ; It may also be important the top bits of the register used for the and stay as 0.
        ; Otherwise, it ands together two floating-point-messes...
        cmpneqss xmm2,xmm7                ; xmm2 was still zero to this point

            xorps xmm1,xmm5                ; Flip the sign of the axis if the r/g or b/g tests indicate a negative slope

        ; This skips the Newton-Raphson. It's half to 1% faster, and it seems to make very little difference to the
        ; int compressor (which is what I'd expect - we're only working at around 15 bits of precision anyway)
        rsqrtss xmm7, xmm7
        andps xmm7, xmm2                ; zero mask

            ; We also need the result in the correct 1.15 format when we send it back, so
            ; multiply that through here while we're waiting for the rcp to finish
            mulps xmm1, xmmword ptr [scale_1_15]

        shufps xmm7, xmm7, SHUFFLE_SELECT(0, 0, 0, 0)
            shufps xmm1,xmm1, SHUFFLE_SELECT(0,1,0,1)

        ; Normalise
        mulps xmm7, xmm1

        ; Get it back into int
        cvtps2dq xmm7,xmm7
        movdqa xmm1,xmm7
        packssdw xmm7,xmm7                ; This duplicates as well, which is what we want
        movdqa [tmp_base_reg-TMP_AXIS], xmm7

    ENDM    ; AXIS_NORM_2COMPONENT







POS_2COMPONENT            MACRO

        ; Expects:    TMP_TMP to have expanded RGB-average values
        ;            xmm7 == axis in form R G R G R G R G
        ; Is expected to:    put abs(P-centre) in TMP_POS
        ;                    put 'centre side' info in TMP_CLUSTER_CENTRE

        movdqa xmm0,[tmp_base_reg-TMP_TMP]        ; Four (RB-avg) values in 9.4 (-255.0 to +255.0)
        movdqa xmm1,[tmp_base_reg-TMP_TMP+16]
        movdqa xmm2,[tmp_base_reg-TMP_TMP+32]
        movdqa xmm3,[tmp_base_reg-TMP_TMP+48]
        pmaddwd xmm0,xmm7                        ; Multiply by axis and do 2D dot product: four 32-bit results in 9.19 format
        pmaddwd xmm1,xmm7
        pmaddwd xmm2,xmm7
        pmaddwd xmm3,xmm7

        psrad xmm0,10+AXIS_SHIFT_BITS            ; Convert to desired 9.5 format (preserving sign)
        psrad xmm1,10+AXIS_SHIFT_BITS
        psrad xmm2,10+AXIS_SHIFT_BITS
        psrad xmm3,10+AXIS_SHIFT_BITS

        packssdw xmm0,xmm1        ; 8 results in xmm0
        packssdw xmm2,xmm3

        movdqa [tmp_base_reg-TMP_POS],xmm0
        movdqa [tmp_base_reg-TMP_POS+16],xmm2

        POS_MINMAX
        POS_CENTRE
        POS_OFFSET 1

    ENDM    ; POS_2AXIS

ENDIF



IF 0

DXTCV11CompressAlphaBlockSSE2 PROC

        ; The alpha compressor is somewhat simpler. There is no need to find an
        ; axis, and the min and max values serve to determine pos_minmax from
        ; which centre is generated.
        
        ; Progressive refinement is also less important (as there are more interpolated
        ; values available and it is more important that the endpoints be represented
        ; correctly)

        movd mm4, dword ptr [ecx]
        movd mm5, dword ptr [ecx+eax]
        lea ecx,[ecx+2*eax]
        movq mm0,mm4
        movq mm1,mm4
        movd mm6, dword ptr [ecx]
        movd mm7, dword ptr [ecx+eax]
        pmaxub mm0,mm5
        pminub mm1,mm5
        pmaxub mm0,mm6
        pminub mm1,mm6
        pmaxub mm0,mm7
        pminub mm1,mm7

        ; Parallel minmax to finish up
        pshufw mm2,mm0,SHUFFLE_SELECT(1,0,3,3)
        pshufw mm3,mm1,SHUFFLE_SELECT(1,0,3,3)
        pmaxub mm0,mm2
        pminub mm1,mm3
        movq mm2,mm0
        movq mm3,mm1
        psrlw mm0,8
        psrlw mm1,8
        pmaxub mm0,mm2
        pminub mm1,mm3

        ; Promote to SSE2 and convert to words
        punpckldq mm4,mm5
        punpckldq mm6,mm7
        pxor xmm5,xmm5
        movq2dq xmm0,mm5
        movq2dq xmm1,mm6
        punpcklbw xmm0,xmm5
        punpcklbw xmm1,xmm5

        ; Calculate centre
        ; Could use pavgb for this, but let's keep the precision
        movq2dq xmm2,mm0
        movq2dq xmm3,mm1
        punpcklbw xmm2,xmm5
        punpcklbw xmm3,xmm5
        paddw xmm2,xmm3            ; 8.1 format centre
        pshuflw xmm2,xmm2,SHUFFLE_SELECT(0,0,0,0)
        pshufd xmm2,xmm2,SHUFFLE_SELECT(0,0,0,0)
        movdqa [tmp_base_reg-TMP_CENTRE], xmm2        ; offset centre (9.5 format) for later correction of refined X

        psllw xmm0,1            ; 8.1 format values
        psllw xmm1,1

        POS_OFFSET ebp, 0


        ; Select an endpoint which is exactly representable
        ; Any step can then be of 1.0


        ; Round all interpolated positions to integer values before calculating E8



        ; Given n, find the output value
        ; 0 2 3 4 5 6 7 1 is the output for given values on n

        movdqa xmm1,[bytes_7]
        pxor xmm2,xmm2
        pcmpeqb xmm3,xmm3        ; all 1s

        pcmpeqb xmm1,xmm0        ; mask set if 7
        pcmpeqb xmm2,xmm0        ; mask set if 0
        pxor xmm3,xmm1
        pxor xmm3,xmm2            ; mask set if 1-6

        pand xmm2,[bytes_1]
        psubb xmm0,[bytes_1]
        pand xmm3,xmm0
        por xmm3,xmm2            ; final result in unpacked form

        movdqa xmm0,xmm3
        pand xmm3,[_07000700s]    ; alternate mids
        pand xmm0,[_00070007s]    ; alternate lows
        psrlw xmm3,5
        por xmm0,xmm3            ; 8 sets of 6-bit pairs of results
        movdqa xmm3,xmm0
        pand xmm0,[_003f0000003f0000s]    ; alternate mids
        pand xmm3,[_0000003f0000003fs]    ; alternate lows
        psrld xmm0,12
        por xmm0,xmm3                    ; 4 sets of 12-bit 4 results

        movdqa xmm3,xmm0
        pand xmm0,[_00000fff00000000s]    ; alternate mids
        pand xmm3,[_0000000000000fffs]    ; alternate lows
        psrlq xmm0,20
        por xmm0,xmm3                    ; 2 sets of 24-bit 8 results

        movdqa xmm3,xmm0
        psrldq xmm0,10                    ; 10-byte shift
        por xmm3,xmm0                    ; Result

        ; or-in endpoint values



DXTCV11CompressAlphaBlockSSE2 ENDP

ENDIF




;void __fastcall DXTCV11CompressExplicitAlphaBlockMMX(BYTE block_8[16], DWORD block_dxtc[2]);
    IF X64
DXTCV11CompressExplicitAlphaBlockMMX PROC
        movq mm0,[rcx]
        movq mm1,[rcx+8]
    ELSE
@DXTCV11CompressExplicitAlphaBlockMMX@8 PROC
        movq mm0,[ecx]
        movq mm1,[ecx+8]
    ENDIF

        ; We have to adjust the values because of the derounding operation in the decode
        ; We need to add (7 - top nybble) to the lower nybble
        movq mm4,[_0707070707070707]
        movq mm5,mm4
        movq mm2,mm0
        movq mm3,mm1
        psrlq mm2,4
        psrlq mm3,4
        movq mm6,[_0f0f0f0f0f0f0f0f]
        pand mm2,mm6
        pand mm3,mm6
        psubb mm4,mm2       ; This is a signed value
        psubb mm5,mm3
        paddusb mm0,mm4     ; ... which is added or subtracted using unsigned saturation on the result to clamp to 0/255
        paddusb mm1,mm5

        ; We need to pack into a single 64-bit word, discarding the lower bits

        movq mm2,mm0
        movq mm3,mm1
        psrlq mm0,4      ; mm0 has x7x6x5x4x3x2x1x0
        psrlq mm1,4
        psrlq mm2,8      ; mm2 has 0x7x6x5x4x3x2x1x
        psrlq mm3,8

        movq mm4, [_000f000f000f000f]
        movq mm5, [_00f000f000f000f0]
        pand mm0,mm4        ; ...6...4...2...0
        pand mm1,mm4
        pand mm2,mm5        ; ..7...5...3...1.
        pand mm3,mm5
        por mm0,mm2         ; ..76..54..32..10
        por mm1,mm3

        packuswb mm0,mm1    ; fedcba98 76543210

    IF X64
        movq [rdx],mm0
    ELSE
        movq [edx],mm0
    ENDIF

        emms
        ret

IF X64
DXTCV11CompressExplicitAlphaBlockMMX ENDP
ELSE
@DXTCV11CompressExplicitAlphaBlockMMX@8 ENDP
ENDIF





main_proc_name PROC 
        ; Fetch stride and source according to calling conventions
        mov stride_reg,16                ; Packed data input at the moment
IF X64
        ; rcx and rdx are already the correct source and destination
ELSE
        ; Fetch stride and source according to calling conventions
        mov source_reg,[esp+4]
        mov dest_reg,[esp+8]
ENDIF
;lea source_reg, white
;lea source_reg, whiteblack
;lea source_reg, redblack
;lea source_reg, redsblack_prog
;lea source_reg, redsblack2
;lea source_reg, greenred

        SAVE_REGS

        AVERAGE_RGB source_reg, stride_reg
;        jmp no_axis                        ; Insert this to test averaging

        AXIS_3COMPONENT no_axis
        POS_3COMPONENT
    IF NO_PROG
        NOPROG_CLUSTER
    ELSEIF USE_34
        PROG_34
    ELSE
        PROG
    ENDIF
colour:
        COLOUR edx, 1

exit:
        RESTORE_REGS
        ret

no_axis:
        COLOUR_AVERAGE dest_reg, 1
        jmp exit

main_proc_name     ENDP







IF 0
        mov eax,16                ; Packed data input at the moment
        mov ecx,[esp+4]
        mov edx,[esp+8]
;lea ecx, bluered
;lea ecx,fade

        SAVE_REGS

        AVERAGE_RGB_RB ecx, eax
;        jmp no_axis                        ; Insert this to test averaging

        AXIS_2COMPONENT no_axis
        AXIS_NORM_2COMPONENT
        POS_2COMPONENT
        PROG
        COLOUR edx, 0

exit:
        RESTORE_REGS
        ret

no_axis:
        COLOUR_AVERAGE edx, 0
        jmp exit


ELSE
ENDIF






IF X64 EQ 0

; Prototype for fastcall with stride...
;void __fastcall DXTCV11CompressBlockSSE2Strided(DWORD *block_32, DWORD *block_dxtc, DWORD input_stride);
@DXTCV11CompressBlockSSE2Strided@12 PROC 
        ; Fetch stride and source according to calling conventions
        mov eax,[esp+4]
        SAVE_REGS

        AVERAGE_RGB ecx, eax
;        jmp no_axis                        ; Insert this to test averaging

        AXIS_3COMPONENT no_axis
        POS_3COMPONENT
        PROG
        COLOUR edx, 1

exit:
        RESTORE_REGS
        ret

no_axis:
        COLOUR_AVERAGE edx, 1
        jmp exit

@DXTCV11CompressBlockSSE2Strided@12     ENDP

ENDIF



END



; x64 uses register calling conventions
; The first four parameters are put in rcx, rdx, r8, r9 (floats would be in xmm0-3)
; rax, r10, r11, xmm4 and xmm5 are volatile in addition to the above - all others must be saved

; void __cdecl DXTCCompressBlockSSE(DWORD *block_32, DWORD *block_dxtc);
; block_dxtc == rdx, how convenient!
