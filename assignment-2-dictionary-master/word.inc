%include "colon.inc"

section .rodata

repeat_str 255, "A"
colon j, fifth_word
db "255 A explanation", 0

colon "56asdf6134rafj0s0f-03qk4fdf/sdf]s#sdf#mn", fourth_word
db "56asdf6134rafj0s0f-03qk4fdf/sdf]s#sdf#mn explanation", 0

colon "ABC", third_word
db "ABC explanation", 0

colon "second word", second_word
db "second word explanation", 0 

colon "first word", first_word
db "first word explanation", 0 