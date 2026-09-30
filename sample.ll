%flowx.primitive.i32.errorable = type { i1, i32 }
%flowx.list.primitive.i32.errorable = type { ptr, i64 }
%flowx.primitive.float.errorable = type { i1, float }
%flowx.list.primitive.float.errorable = type { ptr, i64 }
%flowx.primitive.i32.nullerrorable = type { i1, i1, i32 }
%flowx.list.primitive.i32.nullerrorable = type { ptr, i64 }
%flowx.primitive.float.nullerrorable = type { i1, i1, float }
%flowx.list.primitive.float.nullerrorable = type { ptr, i64 }
%flowx.list.i32 = type { ptr, i64 }
%flowx.list.float = type { ptr, i64 }
%flowx.list.i1 = type { ptr, i64 }
%flowx.primitive.i32.nullable = type { i1, i32 }
%flowx.list.primitive.i32.nullable = type { ptr, i64 }
%flowx.primitive.float.nullable = type { i1, float }
%flowx.list.primitive.float.nullable = type { ptr, i64 }
%flowx.struct.Pair = type { i32, float }
%flowx.list.struct.Pair = type { ptr, i64 }
%flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable = type { %flowx.list.i32, %flowx.list.float, %flowx.list.i1, %flowx.list.i32, %flowx.list.float }
%flowx.return.user.combine.int4.int4 = type { %flowx.list.i32 }
declare void @llvm.trap()

%flowx.return.panic.int4.nullerrorable = type { %flowx.list.primitive.i32.nullable }

define %flowx.return.panic.int4.nullerrorable @flowx.panic.int4.nullerrorable(%flowx.list.primitive.i32.nullerrorable %input, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 0
    %length = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %copy ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.nullerrorable, ptr %inputData, i64 %index
    %inputValue = load %flowx.primitive.i32.nullerrorable, ptr %inputPtr
    %error = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 1
    br i1 %error, label %panic, label %copy

panic:
    call void @llvm.trap()
    unreachable

copy:
    %original = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 2
    %null = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 0
    %flags = insertvalue %flowx.primitive.i32.nullable poison, i1 %null, 0
    %value = insertvalue %flowx.primitive.i32.nullable %flags, i32 %original, 1
    %resultPtr = getelementptr %flowx.primitive.i32.nullable, ptr %result, i64 %index
    store %flowx.primitive.i32.nullable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.i32.nullable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.i32.nullable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.panic.int4.nullerrorable poison, %flowx.list.primitive.i32.nullable %list, 0
    ret %flowx.return.panic.int4.nullerrorable %returnValue
}

%flowx.return.panic.float4.nullerrorable = type { %flowx.list.primitive.float.nullable }

define %flowx.return.panic.float4.nullerrorable @flowx.panic.float4.nullerrorable(%flowx.list.primitive.float.nullerrorable %input, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.nullerrorable %input, 0
    %length = extractvalue %flowx.list.primitive.float.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %copy ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.nullerrorable, ptr %inputData, i64 %index
    %inputValue = load %flowx.primitive.float.nullerrorable, ptr %inputPtr
    %error = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 1
    br i1 %error, label %panic, label %copy

panic:
    call void @llvm.trap()
    unreachable

copy:
    %original = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 2
    %null = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 0
    %flags = insertvalue %flowx.primitive.float.nullable poison, i1 %null, 0
    %value = insertvalue %flowx.primitive.float.nullable %flags, float %original, 1
    %resultPtr = getelementptr %flowx.primitive.float.nullable, ptr %result, i64 %index
    store %flowx.primitive.float.nullable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.float.nullable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.float.nullable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.panic.float4.nullerrorable poison, %flowx.list.primitive.float.nullable %list, 0
    ret %flowx.return.panic.float4.nullerrorable %returnValue
}

%flowx.return.errorToValue.int4.errorable.int4 = type { %flowx.list.i32 }

define %flowx.return.errorToValue.int4.errorable.int4 @flowx.errorToValue.int4.errorable.int4(%flowx.list.primitive.i32.errorable %input, %flowx.list.i32 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.errorable %input, 0
    %replacementData = extractvalue %flowx.list.i32 %replacement, 0
    %length = extractvalue %flowx.list.primitive.i32.errorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.errorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i32, ptr %replacementData, i64 %index
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.i32.errorable, ptr %inputPtr
    %replacementValue = load i32, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.i32.errorable %inputValue, 0
    %original = extractvalue %flowx.primitive.i32.errorable %inputValue, 1
    %selected = select i1 %replace, i32 %replacementValue, i32 %original
    store i32 %selected, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.errorToValue.int4.errorable.int4 poison, %flowx.list.i32 %list, 0
    ret %flowx.return.errorToValue.int4.errorable.int4 %returnValue
}

%flowx.return.errorToValue.float4.errorable.float4 = type { %flowx.list.float }

define %flowx.return.errorToValue.float4.errorable.float4 @flowx.errorToValue.float4.errorable.float4(%flowx.list.primitive.float.errorable %input, %flowx.list.float %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.errorable %input, 0
    %replacementData = extractvalue %flowx.list.float %replacement, 0
    %length = extractvalue %flowx.list.primitive.float.errorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.errorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr float, ptr %replacementData, i64 %index
    %resultPtr = getelementptr float, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.float.errorable, ptr %inputPtr
    %replacementValue = load float, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.float.errorable %inputValue, 0
    %original = extractvalue %flowx.primitive.float.errorable %inputValue, 1
    %selected = select i1 %replace, float %replacementValue, float %original
    store float %selected, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.errorToValue.float4.errorable.float4 poison, %flowx.list.float %list, 0
    ret %flowx.return.errorToValue.float4.errorable.float4 %returnValue
}

%flowx.return.errorToValue.int4.nullerrorable.int4 = type { %flowx.list.primitive.i32.nullable }

define %flowx.return.errorToValue.int4.nullerrorable.int4 @flowx.errorToValue.int4.nullerrorable.int4(%flowx.list.primitive.i32.nullerrorable %input, %flowx.list.i32 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 0
    %replacementData = extractvalue %flowx.list.i32 %replacement, 0
    %length = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.nullerrorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i32, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.i32.nullable, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.i32.nullerrorable, ptr %inputPtr
    %replacementValue = load i32, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 1
    %original = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 2
    %selected = select i1 %replace, i32 %replacementValue, i32 %original
    %oldFlag = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 0
    %remainingFlag = select i1 %replace, i1 false, i1 %oldFlag
    %flags = insertvalue %flowx.primitive.i32.nullable poison, i1 %remainingFlag, 0
    %value = insertvalue %flowx.primitive.i32.nullable %flags, i32 %selected, 1
    store %flowx.primitive.i32.nullable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.i32.nullable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.i32.nullable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.errorToValue.int4.nullerrorable.int4 poison, %flowx.list.primitive.i32.nullable %list, 0
    ret %flowx.return.errorToValue.int4.nullerrorable.int4 %returnValue
}

%flowx.return.errorToValue.float4.nullerrorable.float4 = type { %flowx.list.primitive.float.nullable }

define %flowx.return.errorToValue.float4.nullerrorable.float4 @flowx.errorToValue.float4.nullerrorable.float4(%flowx.list.primitive.float.nullerrorable %input, %flowx.list.float %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.nullerrorable %input, 0
    %replacementData = extractvalue %flowx.list.float %replacement, 0
    %length = extractvalue %flowx.list.primitive.float.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.nullerrorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr float, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.float.nullable, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.float.nullerrorable, ptr %inputPtr
    %replacementValue = load float, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 1
    %original = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 2
    %selected = select i1 %replace, float %replacementValue, float %original
    %oldFlag = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 0
    %remainingFlag = select i1 %replace, i1 false, i1 %oldFlag
    %flags = insertvalue %flowx.primitive.float.nullable poison, i1 %remainingFlag, 0
    %value = insertvalue %flowx.primitive.float.nullable %flags, float %selected, 1
    store %flowx.primitive.float.nullable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.float.nullable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.float.nullable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.errorToValue.float4.nullerrorable.float4 poison, %flowx.list.primitive.float.nullable %list, 0
    ret %flowx.return.errorToValue.float4.nullerrorable.float4 %returnValue
}

%flowx.return.nullToValue.int4.nullerrorable.int4 = type { %flowx.list.primitive.i32.errorable }

define %flowx.return.nullToValue.int4.nullerrorable.int4 @flowx.nullToValue.int4.nullerrorable.int4(%flowx.list.primitive.i32.nullerrorable %input, %flowx.list.i32 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 0
    %replacementData = extractvalue %flowx.list.i32 %replacement, 0
    %length = extractvalue %flowx.list.primitive.i32.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.nullerrorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i32, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.i32.errorable, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.i32.nullerrorable, ptr %inputPtr
    %replacementValue = load i32, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 0
    %original = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 2
    %selected = select i1 %replace, i32 %replacementValue, i32 %original
    %oldFlag = extractvalue %flowx.primitive.i32.nullerrorable %inputValue, 1
    %remainingFlag = select i1 %replace, i1 false, i1 %oldFlag
    %flags = insertvalue %flowx.primitive.i32.errorable poison, i1 %remainingFlag, 0
    %value = insertvalue %flowx.primitive.i32.errorable %flags, i32 %selected, 1
    store %flowx.primitive.i32.errorable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.i32.errorable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.i32.errorable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.nullToValue.int4.nullerrorable.int4 poison, %flowx.list.primitive.i32.errorable %list, 0
    ret %flowx.return.nullToValue.int4.nullerrorable.int4 %returnValue
}

%flowx.return.nullToValue.float4.nullerrorable.float4 = type { %flowx.list.primitive.float.errorable }

define %flowx.return.nullToValue.float4.nullerrorable.float4 @flowx.nullToValue.float4.nullerrorable.float4(%flowx.list.primitive.float.nullerrorable %input, %flowx.list.float %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.nullerrorable %input, 0
    %replacementData = extractvalue %flowx.list.float %replacement, 0
    %length = extractvalue %flowx.list.primitive.float.nullerrorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.nullerrorable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr float, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.float.errorable, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.float.nullerrorable, ptr %inputPtr
    %replacementValue = load float, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 0
    %original = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 2
    %selected = select i1 %replace, float %replacementValue, float %original
    %oldFlag = extractvalue %flowx.primitive.float.nullerrorable %inputValue, 1
    %remainingFlag = select i1 %replace, i1 false, i1 %oldFlag
    %flags = insertvalue %flowx.primitive.float.errorable poison, i1 %remainingFlag, 0
    %value = insertvalue %flowx.primitive.float.errorable %flags, float %selected, 1
    store %flowx.primitive.float.errorable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.float.errorable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.float.errorable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.nullToValue.float4.nullerrorable.float4 poison, %flowx.list.primitive.float.errorable %list, 0
    ret %flowx.return.nullToValue.float4.nullerrorable.float4 %returnValue
}

%flowx.return.and.bool.bool = type { %flowx.list.i1 }

define %flowx.return.and.bool.bool @flowx.and.bool.bool(%flowx.list.i1 %left, %flowx.list.i1 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %rightData = extractvalue %flowx.list.i1 %right, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %rightPtr = getelementptr i1, ptr %rightData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %y = load i1, ptr %rightPtr
    %value = and i1 %x, %y
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.and.bool.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.and.bool.bool %returnValue
}

%flowx.return.less.bool.bool = type { %flowx.list.i1 }

define %flowx.return.less.bool.bool @flowx.less.bool.bool(%flowx.list.i1 %left, %flowx.list.i1 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %rightData = extractvalue %flowx.list.i1 %right, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %rightPtr = getelementptr i1, ptr %rightData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %y = load i1, ptr %rightPtr
    %value = icmp ult i1 %x, %y
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.less.bool.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.less.bool.bool %returnValue
}

%flowx.return.lessEqual.bool.bool = type { %flowx.list.i1 }

define %flowx.return.lessEqual.bool.bool @flowx.lessEqual.bool.bool(%flowx.list.i1 %left, %flowx.list.i1 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %rightData = extractvalue %flowx.list.i1 %right, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %rightPtr = getelementptr i1, ptr %rightData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %y = load i1, ptr %rightPtr
    %value = icmp ule i1 %x, %y
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.lessEqual.bool.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.lessEqual.bool.bool %returnValue
}

%flowx.return.add.int4.int4 = type { %flowx.list.i32 }

define %flowx.return.add.int4.int4 @flowx.add.int4.int4(%flowx.list.i32 %left, %flowx.list.i32 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i32 %left, 0
    %rightData = extractvalue %flowx.list.i32 %right, 0
    %length = extractvalue %flowx.list.i32 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i32, ptr %leftData, i64 %index
    %rightPtr = getelementptr i32, ptr %rightData, i64 %index
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    %x = load i32, ptr %leftPtr
    %y = load i32, ptr %rightPtr
    %value = add i32 %x, %y
    store i32 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.add.int4.int4 poison, %flowx.list.i32 %list, 0
    ret %flowx.return.add.int4.int4 %returnValue
}

%flowx.return.add.float4.float4 = type { %flowx.list.float }

define %flowx.return.add.float4.float4 @flowx.add.float4.float4(%flowx.list.float %left, %flowx.list.float %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.float %left, 0
    %rightData = extractvalue %flowx.list.float %right, 0
    %length = extractvalue %flowx.list.float %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr float, ptr %leftData, i64 %index
    %rightPtr = getelementptr float, ptr %rightData, i64 %index
    %resultPtr = getelementptr float, ptr %result, i64 %index
    %x = load float, ptr %leftPtr
    %y = load float, ptr %rightPtr
    %value = fadd float %x, %y
    store float %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.add.float4.float4 poison, %flowx.list.float %list, 0
    ret %flowx.return.add.float4.float4 %returnValue
}

%flowx.return.nullToValue.int4.nullable.int4 = type { %flowx.list.i32 }

define %flowx.return.nullToValue.int4.nullable.int4 @flowx.nullToValue.int4.nullable.int4(%flowx.list.primitive.i32.nullable %input, %flowx.list.i32 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.nullable %input, 0
    %replacementData = extractvalue %flowx.list.i32 %replacement, 0
    %length = extractvalue %flowx.list.primitive.i32.nullable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.nullable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i32, ptr %replacementData, i64 %index
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.i32.nullable, ptr %inputPtr
    %replacementValue = load i32, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.i32.nullable %inputValue, 0
    %original = extractvalue %flowx.primitive.i32.nullable %inputValue, 1
    %selected = select i1 %replace, i32 %replacementValue, i32 %original
    store i32 %selected, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.nullToValue.int4.nullable.int4 poison, %flowx.list.i32 %list, 0
    ret %flowx.return.nullToValue.int4.nullable.int4 %returnValue
}

%flowx.return.nullToValue.float4.nullable.float4 = type { %flowx.list.float }

define %flowx.return.nullToValue.float4.nullable.float4 @flowx.nullToValue.float4.nullable.float4(%flowx.list.primitive.float.nullable %input, %flowx.list.float %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.nullable %input, 0
    %replacementData = extractvalue %flowx.list.float %replacement, 0
    %length = extractvalue %flowx.list.primitive.float.nullable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.nullable, ptr %inputData, i64 %index
    %replacementPtr = getelementptr float, ptr %replacementData, i64 %index
    %resultPtr = getelementptr float, ptr %result, i64 %index
    %inputValue = load %flowx.primitive.float.nullable, ptr %inputPtr
    %replacementValue = load float, ptr %replacementPtr
    %replace = extractvalue %flowx.primitive.float.nullable %inputValue, 0
    %original = extractvalue %flowx.primitive.float.nullable %inputValue, 1
    %selected = select i1 %replace, float %replacementValue, float %original
    store float %selected, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.nullToValue.float4.nullable.float4 poison, %flowx.list.float %list, 0
    ret %flowx.return.nullToValue.float4.nullable.float4 %returnValue
}

%flowx.return.or.bool.bool = type { %flowx.list.i1 }

define %flowx.return.or.bool.bool @flowx.or.bool.bool(%flowx.list.i1 %left, %flowx.list.i1 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %rightData = extractvalue %flowx.list.i1 %right, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %rightPtr = getelementptr i1, ptr %rightData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %y = load i1, ptr %rightPtr
    %value = or i1 %x, %y
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.or.bool.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.or.bool.bool %returnValue
}

%flowx.return.equal.bool.bool = type { %flowx.list.i1 }

define %flowx.return.equal.bool.bool @flowx.equal.bool.bool(%flowx.list.i1 %left, %flowx.list.i1 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %rightData = extractvalue %flowx.list.i1 %right, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %rightPtr = getelementptr i1, ptr %rightData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %y = load i1, ptr %rightPtr
    %value = icmp eq i1 %x, %y
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.equal.bool.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.equal.bool.bool %returnValue
}

%flowx.return.not.bool = type { %flowx.list.i1 }

define %flowx.return.not.bool @flowx.not.bool(%flowx.list.i1 %left, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i1 %left, 0
    %length = extractvalue %flowx.list.i1 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i1, ptr %leftData, i64 %index
    %resultPtr = getelementptr i1, ptr %result, i64 %index
    %x = load i1, ptr %leftPtr
    %value = xor i1 %x, true
    store i1 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i1 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i1 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.not.bool poison, %flowx.list.i1 %list, 0
    ret %flowx.return.not.bool %returnValue
}

%flowx.return.subtract.int4.int4 = type { %flowx.list.i32 }

define %flowx.return.subtract.int4.int4 @flowx.subtract.int4.int4(%flowx.list.i32 %left, %flowx.list.i32 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i32 %left, 0
    %rightData = extractvalue %flowx.list.i32 %right, 0
    %length = extractvalue %flowx.list.i32 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i32, ptr %leftData, i64 %index
    %rightPtr = getelementptr i32, ptr %rightData, i64 %index
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    %x = load i32, ptr %leftPtr
    %y = load i32, ptr %rightPtr
    %value = sub i32 %x, %y
    store i32 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.subtract.int4.int4 poison, %flowx.list.i32 %list, 0
    ret %flowx.return.subtract.int4.int4 %returnValue
}

%flowx.return.subtract.float4.float4 = type { %flowx.list.float }

define %flowx.return.subtract.float4.float4 @flowx.subtract.float4.float4(%flowx.list.float %left, %flowx.list.float %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.float %left, 0
    %rightData = extractvalue %flowx.list.float %right, 0
    %length = extractvalue %flowx.list.float %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr float, ptr %leftData, i64 %index
    %rightPtr = getelementptr float, ptr %rightData, i64 %index
    %resultPtr = getelementptr float, ptr %result, i64 %index
    %x = load float, ptr %leftPtr
    %y = load float, ptr %rightPtr
    %value = fsub float %x, %y
    store float %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.subtract.float4.float4 poison, %flowx.list.float %list, 0
    ret %flowx.return.subtract.float4.float4 %returnValue
}

%flowx.return.assert.int4.bool = type { %flowx.list.primitive.i32.errorable }

define %flowx.return.assert.int4.bool @flowx.assert.int4.bool(%flowx.list.i32 %input, %flowx.list.i1 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.i32 %input, 0
    %replacementData = extractvalue %flowx.list.i1 %replacement, 0
    %length = extractvalue %flowx.list.i32 %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr i32, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i1, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.i32.errorable, ptr %result, i64 %index
    %inputValue = load i32, ptr %inputPtr
    %replacementValue = load i1, ptr %replacementPtr
    %error = xor i1 %replacementValue, true
    %flags = insertvalue %flowx.primitive.i32.errorable poison, i1 %error, 0
    %value = insertvalue %flowx.primitive.i32.errorable %flags, i32 %inputValue, 1
    store %flowx.primitive.i32.errorable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.i32.errorable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.i32.errorable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.assert.int4.bool poison, %flowx.list.primitive.i32.errorable %list, 0
    ret %flowx.return.assert.int4.bool %returnValue
}

%flowx.return.assert.float4.bool = type { %flowx.list.primitive.float.errorable }

define %flowx.return.assert.float4.bool @flowx.assert.float4.bool(%flowx.list.float %input, %flowx.list.i1 %replacement, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.float %input, 0
    %replacementData = extractvalue %flowx.list.i1 %replacement, 0
    %length = extractvalue %flowx.list.float %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr float, ptr %inputData, i64 %index
    %replacementPtr = getelementptr i1, ptr %replacementData, i64 %index
    %resultPtr = getelementptr %flowx.primitive.float.errorable, ptr %result, i64 %index
    %inputValue = load float, ptr %inputPtr
    %replacementValue = load i1, ptr %replacementPtr
    %error = xor i1 %replacementValue, true
    %flags = insertvalue %flowx.primitive.float.errorable poison, i1 %error, 0
    %value = insertvalue %flowx.primitive.float.errorable %flags, float %inputValue, 1
    store %flowx.primitive.float.errorable %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.primitive.float.errorable poison, ptr %result, 0
    %list = insertvalue %flowx.list.primitive.float.errorable %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.assert.float4.bool poison, %flowx.list.primitive.float.errorable %list, 0
    ret %flowx.return.assert.float4.bool %returnValue
}

%flowx.return.multiply.int4.int4 = type { %flowx.list.i32 }

define %flowx.return.multiply.int4.int4 @flowx.multiply.int4.int4(%flowx.list.i32 %left, %flowx.list.i32 %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.i32 %left, 0
    %rightData = extractvalue %flowx.list.i32 %right, 0
    %length = extractvalue %flowx.list.i32 %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr i32, ptr %leftData, i64 %index
    %rightPtr = getelementptr i32, ptr %rightData, i64 %index
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    %x = load i32, ptr %leftPtr
    %y = load i32, ptr %rightPtr
    %value = mul i32 %x, %y
    store i32 %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.multiply.int4.int4 poison, %flowx.list.i32 %list, 0
    ret %flowx.return.multiply.int4.int4 %returnValue
}

%flowx.return.multiply.float4.float4 = type { %flowx.list.float }

define %flowx.return.multiply.float4.float4 @flowx.multiply.float4.float4(%flowx.list.float %left, %flowx.list.float %right, ptr %result) {
entry:
    %leftData = extractvalue %flowx.list.float %left, 0
    %rightData = extractvalue %flowx.list.float %right, 0
    %length = extractvalue %flowx.list.float %left, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %body ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %leftPtr = getelementptr float, ptr %leftData, i64 %index
    %rightPtr = getelementptr float, ptr %rightData, i64 %index
    %resultPtr = getelementptr float, ptr %result, i64 %index
    %x = load float, ptr %leftPtr
    %y = load float, ptr %rightPtr
    %value = fmul float %x, %y
    store float %value, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.multiply.float4.float4 poison, %flowx.list.float %list, 0
    ret %flowx.return.multiply.float4.float4 %returnValue
}

%flowx.return.panic.int4.errorable = type { %flowx.list.i32 }

define %flowx.return.panic.int4.errorable @flowx.panic.int4.errorable(%flowx.list.primitive.i32.errorable %input, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.i32.errorable %input, 0
    %length = extractvalue %flowx.list.primitive.i32.errorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %copy ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.i32.errorable, ptr %inputData, i64 %index
    %inputValue = load %flowx.primitive.i32.errorable, ptr %inputPtr
    %error = extractvalue %flowx.primitive.i32.errorable %inputValue, 0
    br i1 %error, label %panic, label %copy

panic:
    call void @llvm.trap()
    unreachable

copy:
    %original = extractvalue %flowx.primitive.i32.errorable %inputValue, 1
    %resultPtr = getelementptr i32, ptr %result, i64 %index
    store i32 %original, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.i32 poison, ptr %result, 0
    %list = insertvalue %flowx.list.i32 %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.panic.int4.errorable poison, %flowx.list.i32 %list, 0
    ret %flowx.return.panic.int4.errorable %returnValue
}

%flowx.return.panic.float4.errorable = type { %flowx.list.float }

define %flowx.return.panic.float4.errorable @flowx.panic.float4.errorable(%flowx.list.primitive.float.errorable %input, ptr %result) {
entry:
    %inputData = extractvalue %flowx.list.primitive.float.errorable %input, 0
    %length = extractvalue %flowx.list.primitive.float.errorable %input, 1
    br label %check

check:
    %index = phi i64 [ 0, %entry ], [ %next, %copy ]
    %hasNext = icmp ult i64 %index, %length
    br i1 %hasNext, label %body, label %exit

body:
    %inputPtr = getelementptr %flowx.primitive.float.errorable, ptr %inputData, i64 %index
    %inputValue = load %flowx.primitive.float.errorable, ptr %inputPtr
    %error = extractvalue %flowx.primitive.float.errorable %inputValue, 0
    br i1 %error, label %panic, label %copy

panic:
    call void @llvm.trap()
    unreachable

copy:
    %original = extractvalue %flowx.primitive.float.errorable %inputValue, 1
    %resultPtr = getelementptr float, ptr %result, i64 %index
    store float %original, ptr %resultPtr
    %next = add i64 %index, 1
    br label %check

exit:
    %listData = insertvalue %flowx.list.float poison, ptr %result, 0
    %list = insertvalue %flowx.list.float %listData, i64 %length, 1
    %returnValue = insertvalue %flowx.return.panic.float4.errorable poison, %flowx.list.float %list, 0
    ret %flowx.return.panic.float4.errorable %returnValue
}
