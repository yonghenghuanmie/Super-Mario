.code

GetPixelSize proc
mov rax,qword ptr [rcx]
add rax,28h
sub rsp,28h
call qword ptr [rax]
add rsp,28h
ret
GetPixelSize endp

end