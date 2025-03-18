%define second 0
%macro colon 2
%ifnstr %1
%error "Error: macro first argument is not a string."
%endif
%ifnid %2
%error "Error: macro second argument is not a label."
%endif
%2:
    dq  second
    db  %1, 0
%define second %2
%endmacro



%define j ""
%macro repeat_str 2		
%define j ""
%rep %1
%strcat j j, %2
%endrep
%endmacro
