; Return combined state of Controller 1 and 3 in an integer (16 bits)

.export _pad_poll_4score_1_3
.export _pad_poll_4score_2_4

_pad_poll_4score_1_3:

    lda #1
    sta $4016           ; Strobe latch
    lda #0
    sta $4016           ; Release latch

    ; Read 8 bits for Controller 1
    ldx #8
    lda #0              ; Clear accumulator for Controller 1 state

@poll_controller1:
    lda $4016
    lsr a
    rol PAD1_STATE      ; Rotate into buffer for Controller 1
    dex
    bne @poll_controller1

    ; Store Controller 1 state in the result
    lda PAD1_STATE      ; Load the state of Controller 1
    sta <PAD_RESULT     ; Store low byte of the result

    ; Read 8 bits for Controller 3
    ldx #8
    lda #0              ; Clear accumulator for Controller 3 state

@poll_controller3:
    lda $4016
    lsr a
    rol PAD3_STATE      ; Rotate into buffer for Controller 3
    dex
    bne @poll_controller3

    ; Store Controller 3 state in the result
    lda PAD3_STATE      ; Load the state of Controller 3
    sta <PAD_RESULT+1   ; Store high byte of the result

    ; Combine low and high bytes into a 16-bit result
    lda <PAD_RESULT     ; Load low byte
    tax                 ; Transfer to X
    lda <PAD_RESULT+1   ; Load high byte
    tay                 ; Transfer to Y
    rts                 ; Return; A holds low byte, Y holds high byte




; int __fastcall__ pad_poll_4score_2_4();

_pad_poll_4score_2_4:

    lda #1
    sta $4016           ; Strobe latch
    lda #0
    sta $4016           ; Release latch

    ; Read 8 bits for Controller 2 from $4017
    ldx #8
    lda #0              ; Clear accumulator for Controller 2 state

@poll_controller2:
    lda $4017
    lsr a
    rol PAD2_STATE      ; Rotate into buffer for Controller 2
    dex
    bne @poll_controller2

    lda PAD2_STATE      ; Store Controller 2 state in the low byte
    sta <PAD_RESULT

    ; Read 8 bits for Controller 4 from $4017
    ldx #8
    lda #0              ; Clear accumulator for Controller 4 state

@poll_controller4:
    lda $4017
    lsr a
    rol PAD4_STATE      ; Rotate into buffer for Controller 4
    dex
    bne @poll_controller4

    lda PAD4_STATE      ; Shift Controller 4 state into high byte
    sta <PAD_RESULT+1   ; Store high byte in PAD_RESULT+1

    ; Combine low and high bytes into a 16-bit result
    lda <PAD_RESULT
    tax
    lda <PAD_RESULT+1
    tay
    rts                 ; A holds 16-bit result in (Y:A)
