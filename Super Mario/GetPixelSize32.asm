.386
.model flat,C

.code

GetPixelSize proc ID2D1Bitmap:ptr dword,D2D1_SIZE_U:ptr dword
mov eax,D2D1_SIZE_U
push eax
mov eax,ID2D1Bitmap
push eax
mov eax,dword ptr [eax]
add eax,14H
call dword ptr [eax]
ret
GetPixelSize endp

end