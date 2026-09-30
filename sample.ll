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
declare ptr @calloc(i64, i64)
declare void @free(ptr)
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

define %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable @flowx.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable(%flowx.list.primitive.i32.errorable %input0, %flowx.list.primitive.float.errorable %input1, %flowx.list.primitive.i32.nullerrorable %input2, %flowx.list.primitive.float.nullerrorable %input3, i64 %length, ptr %output0, ptr %output1, ptr %output2, ptr %output3, ptr %output4) {
entry:
    %nonempty = icmp ne i64 %length, 0
    %v1 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v2 = icmp eq ptr %v1, null
    %v3 = and i1 %v2, %nonempty
    br i1 %v3, label %allocationFailed, label %allocated3

allocated3:
    br label %check4

check4:
    %v5 = phi i64 [ 0, %allocated3 ], [ %v6, %row4 ]
    %v7 = icmp ult i64 %v5, %length
    br i1 %v7, label %row4, label %exit4

row4:
    %v8 = getelementptr i32, ptr %v1, i64 %v5
    store i32 10, ptr %v8
    %v6 = add i64 %v5, 1
    br label %check4

exit4:
    %v9 = insertvalue %flowx.list.i32 poison, ptr %v1, 0
    %v10 = insertvalue %flowx.list.i32 %v9, i64 %length, 1
    %v11 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v12 = icmp eq ptr %v11, null
    %v13 = and i1 %v12, %nonempty
    br i1 %v13, label %allocationFailed, label %allocated13

allocated13:
    br label %check14

check14:
    %v15 = phi i64 [ 0, %allocated13 ], [ %v16, %row14 ]
    %v17 = icmp ult i64 %v15, %length
    br i1 %v17, label %row14, label %exit14

row14:
    %v18 = getelementptr float, ptr %v11, i64 %v15
    store float 0x3FF8000000000000, ptr %v18
    %v16 = add i64 %v15, 1
    br label %check14

exit14:
    %v19 = insertvalue %flowx.list.float poison, ptr %v11, 0
    %v20 = insertvalue %flowx.list.float %v19, i64 %length, 1
    %v21 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v22 = icmp eq ptr %v21, null
    %v23 = and i1 %v22, %nonempty
    br i1 %v23, label %allocationFailed, label %allocated23

allocated23:
    br label %check24

check24:
    %v25 = phi i64 [ 0, %allocated23 ], [ %v26, %row24 ]
    %v27 = icmp ult i64 %v25, %length
    br i1 %v27, label %row24, label %exit24

row24:
    %v28 = getelementptr i32, ptr %v21, i64 %v25
    store i32 20, ptr %v28
    %v26 = add i64 %v25, 1
    br label %check24

exit24:
    %v29 = insertvalue %flowx.list.i32 poison, ptr %v21, 0
    %v30 = insertvalue %flowx.list.i32 %v29, i64 %length, 1
    %v31 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v32 = icmp eq ptr %v31, null
    %v33 = and i1 %v32, %nonempty
    br i1 %v33, label %allocationFailed, label %allocated33

allocated33:
    br label %check34

check34:
    %v35 = phi i64 [ 0, %allocated33 ], [ %v36, %row34 ]
    %v37 = icmp ult i64 %v35, %length
    br i1 %v37, label %row34, label %exit34

row34:
    %v38 = getelementptr float, ptr %v31, i64 %v35
    store float 0x4004000000000000, ptr %v38
    %v36 = add i64 %v35, 1
    br label %check34

exit34:
    %v39 = insertvalue %flowx.list.float poison, ptr %v31, 0
    %v40 = insertvalue %flowx.list.float %v39, i64 %length, 1
    %v41 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v42 = icmp eq ptr %v41, null
    %v43 = and i1 %v42, %nonempty
    br i1 %v43, label %allocationFailed, label %allocated43

allocated43:
    br label %check44

check44:
    %v45 = phi i64 [ 0, %allocated43 ], [ %v46, %row44 ]
    %v47 = icmp ult i64 %v45, %length
    br i1 %v47, label %row44, label %exit44

row44:
    %v48 = getelementptr i32, ptr %v41, i64 %v45
    store i32 30, ptr %v48
    %v46 = add i64 %v45, 1
    br label %check44

exit44:
    %v49 = insertvalue %flowx.list.i32 poison, ptr %v41, 0
    %v50 = insertvalue %flowx.list.i32 %v49, i64 %length, 1
    %v51 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v52 = icmp eq ptr %v51, null
    %v53 = and i1 %v52, %nonempty
    br i1 %v53, label %allocationFailed, label %allocated53

allocated53:
    br label %check54

check54:
    %v55 = phi i64 [ 0, %allocated53 ], [ %v56, %row54 ]
    %v57 = icmp ult i64 %v55, %length
    br i1 %v57, label %row54, label %exit54

row54:
    %v58 = getelementptr float, ptr %v51, i64 %v55
    store float 0x400C000000000000, ptr %v58
    %v56 = add i64 %v55, 1
    br label %check54

exit54:
    %v59 = insertvalue %flowx.list.float poison, ptr %v51, 0
    %v60 = insertvalue %flowx.list.float %v59, i64 %length, 1
    %v61 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v62 = icmp eq ptr %v61, null
    %v63 = and i1 %v62, %nonempty
    br i1 %v63, label %allocationFailed, label %allocated63

allocated63:
    br label %check64

check64:
    %v65 = phi i64 [ 0, %allocated63 ], [ %v66, %row64 ]
    %v67 = icmp ult i64 %v65, %length
    br i1 %v67, label %row64, label %exit64

row64:
    %v68 = getelementptr i32, ptr %v61, i64 %v65
    store i32 40, ptr %v68
    %v66 = add i64 %v65, 1
    br label %check64

exit64:
    %v69 = insertvalue %flowx.list.i32 poison, ptr %v61, 0
    %v70 = insertvalue %flowx.list.i32 %v69, i64 %length, 1
    %v71 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v72 = icmp eq ptr %v71, null
    %v73 = and i1 %v72, %nonempty
    br i1 %v73, label %allocationFailed, label %allocated73

allocated73:
    br label %check74

check74:
    %v75 = phi i64 [ 0, %allocated73 ], [ %v76, %row74 ]
    %v77 = icmp ult i64 %v75, %length
    br i1 %v77, label %row74, label %exit74

row74:
    %v78 = getelementptr float, ptr %v71, i64 %v75
    store float 0x4012000000000000, ptr %v78
    %v76 = add i64 %v75, 1
    br label %check74

exit74:
    %v79 = insertvalue %flowx.list.float poison, ptr %v71, 0
    %v80 = insertvalue %flowx.list.float %v79, i64 %length, 1
    %v81 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v82 = icmp eq ptr %v81, null
    %v83 = and i1 %v82, %nonempty
    br i1 %v83, label %allocationFailed, label %allocated83

allocated83:
    br label %check84

check84:
    %v85 = phi i64 [ 0, %allocated83 ], [ %v86, %row84 ]
    %v87 = icmp ult i64 %v85, %length
    br i1 %v87, label %row84, label %exit84

row84:
    %v88 = getelementptr i32, ptr %v81, i64 %v85
    store i32 50, ptr %v88
    %v86 = add i64 %v85, 1
    br label %check84

exit84:
    %v89 = insertvalue %flowx.list.i32 poison, ptr %v81, 0
    %v90 = insertvalue %flowx.list.i32 %v89, i64 %length, 1
    %v91 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v92 = icmp eq ptr %v91, null
    %v93 = and i1 %v92, %nonempty
    br i1 %v93, label %allocationFailed, label %allocated93

allocated93:
    br label %check94

check94:
    %v95 = phi i64 [ 0, %allocated93 ], [ %v96, %row94 ]
    %v97 = icmp ult i64 %v95, %length
    br i1 %v97, label %row94, label %exit94

row94:
    %v98 = getelementptr float, ptr %v91, i64 %v95
    store float 0x4016000000000000, ptr %v98
    %v96 = add i64 %v95, 1
    br label %check94

exit94:
    %v99 = insertvalue %flowx.list.float poison, ptr %v91, 0
    %v100 = insertvalue %flowx.list.float %v99, i64 %length, 1
    %v101 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v102 = icmp eq ptr %v101, null
    %v103 = and i1 %v102, %nonempty
    br i1 %v103, label %allocationFailed, label %allocated103

allocated103:
    br label %check104

check104:
    %v105 = phi i64 [ 0, %allocated103 ], [ %v106, %row104 ]
    %v107 = icmp ult i64 %v105, %length
    br i1 %v107, label %row104, label %exit104

row104:
    %v108 = getelementptr i32, ptr %v101, i64 %v105
    store i32 2, ptr %v108
    %v106 = add i64 %v105, 1
    br label %check104

exit104:
    %v109 = insertvalue %flowx.list.i32 poison, ptr %v101, 0
    %v110 = insertvalue %flowx.list.i32 %v109, i64 %length, 1
    %v111 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v112 = icmp eq ptr %v111, null
    %v113 = and i1 %v112, %nonempty
    br i1 %v113, label %allocationFailed, label %allocated113

allocated113:
    br label %check114

check114:
    %v115 = phi i64 [ 0, %allocated113 ], [ %v116, %row114 ]
    %v117 = icmp ult i64 %v115, %length
    br i1 %v117, label %row114, label %exit114

row114:
    %v118 = getelementptr i32, ptr %v111, i64 %v115
    store i32 1, ptr %v118
    %v116 = add i64 %v115, 1
    br label %check114

exit114:
    %v119 = insertvalue %flowx.list.i32 poison, ptr %v111, 0
    %v120 = insertvalue %flowx.list.i32 %v119, i64 %length, 1
    %v121 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v122 = icmp eq ptr %v121, null
    %v123 = and i1 %v122, %nonempty
    br i1 %v123, label %allocationFailed, label %allocated123

allocated123:
    br label %check124

check124:
    %v125 = phi i64 [ 0, %allocated123 ], [ %v126, %row124 ]
    %v127 = icmp ult i64 %v125, %length
    br i1 %v127, label %row124, label %exit124

row124:
    %v128 = getelementptr i32, ptr %v121, i64 %v125
    store i32 3, ptr %v128
    %v126 = add i64 %v125, 1
    br label %check124

exit124:
    %v129 = insertvalue %flowx.list.i32 poison, ptr %v121, 0
    %v130 = insertvalue %flowx.list.i32 %v129, i64 %length, 1
    %v131 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v132 = icmp eq ptr %v131, null
    %v133 = and i1 %v132, %nonempty
    br i1 %v133, label %allocationFailed, label %allocated133

allocated133:
    br label %check134

check134:
    %v135 = phi i64 [ 0, %allocated133 ], [ %v136, %row134 ]
    %v137 = icmp ult i64 %v135, %length
    br i1 %v137, label %row134, label %exit134

row134:
    %v138 = getelementptr float, ptr %v131, i64 %v135
    store float 0x4000000000000000, ptr %v138
    %v136 = add i64 %v135, 1
    br label %check134

exit134:
    %v139 = insertvalue %flowx.list.float poison, ptr %v131, 0
    %v140 = insertvalue %flowx.list.float %v139, i64 %length, 1
    %v141 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v142 = icmp eq ptr %v141, null
    %v143 = and i1 %v142, %nonempty
    br i1 %v143, label %allocationFailed, label %allocated143

allocated143:
    br label %check144

check144:
    %v145 = phi i64 [ 0, %allocated143 ], [ %v146, %row144 ]
    %v147 = icmp ult i64 %v145, %length
    br i1 %v147, label %row144, label %exit144

row144:
    %v148 = getelementptr float, ptr %v141, i64 %v145
    store float 0x3FF0000000000000, ptr %v148
    %v146 = add i64 %v145, 1
    br label %check144

exit144:
    %v149 = insertvalue %flowx.list.float poison, ptr %v141, 0
    %v150 = insertvalue %flowx.list.float %v149, i64 %length, 1
    %v151 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v152 = icmp eq ptr %v151, null
    %v153 = and i1 %v152, %nonempty
    br i1 %v153, label %allocationFailed, label %allocated153

allocated153:
    br label %check154

check154:
    %v155 = phi i64 [ 0, %allocated153 ], [ %v156, %row154 ]
    %v157 = icmp ult i64 %v155, %length
    br i1 %v157, label %row154, label %exit154

row154:
    %v158 = getelementptr float, ptr %v151, i64 %v155
    store float 0x4008000000000000, ptr %v158
    %v156 = add i64 %v155, 1
    br label %check154

exit154:
    %v159 = insertvalue %flowx.list.float poison, ptr %v151, 0
    %v160 = insertvalue %flowx.list.float %v159, i64 %length, 1
    %v161 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v162 = icmp eq ptr %v161, null
    %v163 = and i1 %v162, %nonempty
    br i1 %v163, label %allocationFailed, label %allocated163

allocated163:
    br label %check164

check164:
    %v165 = phi i64 [ 0, %allocated163 ], [ %v166, %row164 ]
    %v167 = icmp ult i64 %v165, %length
    br i1 %v167, label %row164, label %exit164

row164:
    %v168 = getelementptr i1, ptr %v161, i64 %v165
    store i1 true, ptr %v168
    %v166 = add i64 %v165, 1
    br label %check164

exit164:
    %v169 = insertvalue %flowx.list.i1 poison, ptr %v161, 0
    %v170 = insertvalue %flowx.list.i1 %v169, i64 %length, 1
    %v171 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v172 = icmp eq ptr %v171, null
    %v173 = and i1 %v172, %nonempty
    br i1 %v173, label %allocationFailed, label %allocated173

allocated173:
    br label %check174

check174:
    %v175 = phi i64 [ 0, %allocated173 ], [ %v176, %row174 ]
    %v177 = icmp ult i64 %v175, %length
    br i1 %v177, label %row174, label %exit174

row174:
    %v178 = getelementptr i1, ptr %v171, i64 %v175
    store i1 false, ptr %v178
    %v176 = add i64 %v175, 1
    br label %check174

exit174:
    %v179 = insertvalue %flowx.list.i1 poison, ptr %v171, 0
    %v180 = insertvalue %flowx.list.i1 %v179, i64 %length, 1
    %v181 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v182 = icmp eq ptr %v181, null
    %v183 = and i1 %v182, %nonempty
    br i1 %v183, label %allocationFailed, label %allocated183

allocated183:
    br label %check184

check184:
    %v185 = phi i64 [ 0, %allocated183 ], [ %v186, %row184 ]
    %v187 = icmp ult i64 %v185, %length
    br i1 %v187, label %row184, label %exit184

row184:
    %v188 = getelementptr i1, ptr %v181, i64 %v185
    store i1 true, ptr %v188
    %v186 = add i64 %v185, 1
    br label %check184

exit184:
    %v189 = insertvalue %flowx.list.i1 poison, ptr %v181, 0
    %v190 = insertvalue %flowx.list.i1 %v189, i64 %length, 1
    %v191 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v192 = icmp eq ptr %v191, null
    %v193 = and i1 %v192, %nonempty
    br i1 %v193, label %allocationFailed, label %allocated193

allocated193:
    br label %check194

check194:
    %v195 = phi i64 [ 0, %allocated193 ], [ %v196, %row194 ]
    %v197 = icmp ult i64 %v195, %length
    br i1 %v197, label %row194, label %exit194

row194:
    %v198 = getelementptr i1, ptr %v191, i64 %v195
    store i1 false, ptr %v198
    %v196 = add i64 %v195, 1
    br label %check194

exit194:
    %v199 = insertvalue %flowx.list.i1 poison, ptr %v191, 0
    %v200 = insertvalue %flowx.list.i1 %v199, i64 %length, 1
    %v201 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v202 = icmp eq ptr %v201, null
    %v203 = and i1 %v202, %nonempty
    br i1 %v203, label %allocationFailed, label %allocated203

allocated203:
    br label %check204

check204:
    %v205 = phi i64 [ 0, %allocated203 ], [ %v206, %row204 ]
    %v207 = icmp ult i64 %v205, %length
    br i1 %v207, label %row204, label %exit204

row204:
    %v208 = getelementptr i1, ptr %v201, i64 %v205
    store i1 true, ptr %v208
    %v206 = add i64 %v205, 1
    br label %check204

exit204:
    %v209 = insertvalue %flowx.list.i1 poison, ptr %v201, 0
    %v210 = insertvalue %flowx.list.i1 %v209, i64 %length, 1
    %v211 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v212 = icmp eq ptr %v211, null
    %v213 = and i1 %v212, %nonempty
    br i1 %v213, label %allocationFailed, label %allocated213

allocated213:
    br label %check214

check214:
    %v215 = phi i64 [ 0, %allocated213 ], [ %v216, %row214 ]
    %v217 = icmp ult i64 %v215, %length
    br i1 %v217, label %row214, label %exit214

row214:
    %v218 = getelementptr i1, ptr %v211, i64 %v215
    store i1 true, ptr %v218
    %v216 = add i64 %v215, 1
    br label %check214

exit214:
    %v219 = insertvalue %flowx.list.i1 poison, ptr %v211, 0
    %v220 = insertvalue %flowx.list.i1 %v219, i64 %length, 1
    %v221 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v222 = icmp eq ptr %v221, null
    %v223 = and i1 %v222, %nonempty
    br i1 %v223, label %allocationFailed, label %allocated223

allocated223:
    br label %check224

check224:
    %v225 = phi i64 [ 0, %allocated223 ], [ %v226, %row224 ]
    %v227 = icmp ult i64 %v225, %length
    br i1 %v227, label %row224, label %exit224

row224:
    %v228 = getelementptr i1, ptr %v221, i64 %v225
    store i1 true, ptr %v228
    %v226 = add i64 %v225, 1
    br label %check224

exit224:
    %v229 = insertvalue %flowx.list.i1 poison, ptr %v221, 0
    %v230 = insertvalue %flowx.list.i1 %v229, i64 %length, 1
    %v231 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v232 = icmp eq ptr %v231, null
    %v233 = and i1 %v232, %nonempty
    br i1 %v233, label %allocationFailed, label %allocated233

allocated233:
    br label %check234

check234:
    %v235 = phi i64 [ 0, %allocated233 ], [ %v236, %row234 ]
    %v237 = icmp ult i64 %v235, %length
    br i1 %v237, label %row234, label %exit234

row234:
    %v238 = getelementptr i1, ptr %v231, i64 %v235
    store i1 true, ptr %v238
    %v236 = add i64 %v235, 1
    br label %check234

exit234:
    %v239 = insertvalue %flowx.list.i1 poison, ptr %v231, 0
    %v240 = insertvalue %flowx.list.i1 %v239, i64 %length, 1
    %v241 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v242 = icmp eq ptr %v241, null
    %v243 = and i1 %v242, %nonempty
    br i1 %v243, label %allocationFailed, label %allocated243

allocated243:
    br label %check244

check244:
    %v245 = phi i64 [ 0, %allocated243 ], [ %v246, %row244 ]
    %v247 = icmp ult i64 %v245, %length
    br i1 %v247, label %row244, label %exit244

row244:
    %v248 = getelementptr i1, ptr %v241, i64 %v245
    store i1 true, ptr %v248
    %v246 = add i64 %v245, 1
    br label %check244

exit244:
    %v249 = insertvalue %flowx.list.i1 poison, ptr %v241, 0
    %v250 = insertvalue %flowx.list.i1 %v249, i64 %length, 1
    %v251 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v252 = icmp eq ptr %v251, null
    %v253 = and i1 %v252, %nonempty
    br i1 %v253, label %allocationFailed, label %allocated253

allocated253:
    br label %check254

check254:
    %v255 = phi i64 [ 0, %allocated253 ], [ %v256, %row254 ]
    %v257 = icmp ult i64 %v255, %length
    br i1 %v257, label %row254, label %exit254

row254:
    %v258 = getelementptr i1, ptr %v251, i64 %v255
    store i1 false, ptr %v258
    %v256 = add i64 %v255, 1
    br label %check254

exit254:
    %v259 = insertvalue %flowx.list.i1 poison, ptr %v251, 0
    %v260 = insertvalue %flowx.list.i1 %v259, i64 %length, 1
    %v261 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v262 = icmp eq ptr %v261, null
    %v263 = and i1 %v262, %nonempty
    br i1 %v263, label %allocationFailed, label %allocated263

allocated263:
    br label %check264

check264:
    %v265 = phi i64 [ 0, %allocated263 ], [ %v266, %row264 ]
    %v267 = icmp ult i64 %v265, %length
    br i1 %v267, label %row264, label %exit264

row264:
    %v268 = getelementptr i32, ptr %v261, i64 %v265
    store i32 100, ptr %v268
    %v266 = add i64 %v265, 1
    br label %check264

exit264:
    %v269 = insertvalue %flowx.list.i32 poison, ptr %v261, 0
    %v270 = insertvalue %flowx.list.i32 %v269, i64 %length, 1
    %v271 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v272 = icmp eq ptr %v271, null
    %v273 = and i1 %v272, %nonempty
    br i1 %v273, label %allocationFailed, label %allocated273

allocated273:
    br label %check274

check274:
    %v275 = phi i64 [ 0, %allocated273 ], [ %v276, %row274 ]
    %v277 = icmp ult i64 %v275, %length
    br i1 %v277, label %row274, label %exit274

row274:
    %v278 = getelementptr i1, ptr %v271, i64 %v275
    store i1 false, ptr %v278
    %v276 = add i64 %v275, 1
    br label %check274

exit274:
    %v279 = insertvalue %flowx.list.i1 poison, ptr %v271, 0
    %v280 = insertvalue %flowx.list.i1 %v279, i64 %length, 1
    %v281 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v282 = icmp eq ptr %v281, null
    %v283 = and i1 %v282, %nonempty
    br i1 %v283, label %allocationFailed, label %allocated283

allocated283:
    br label %check284

check284:
    %v285 = phi i64 [ 0, %allocated283 ], [ %v286, %row284 ]
    %v287 = icmp ult i64 %v285, %length
    br i1 %v287, label %row284, label %exit284

row284:
    %v288 = getelementptr float, ptr %v281, i64 %v285
    store float 0x4059000000000000, ptr %v288
    %v286 = add i64 %v285, 1
    br label %check284

exit284:
    %v289 = insertvalue %flowx.list.float poison, ptr %v281, 0
    %v290 = insertvalue %flowx.list.float %v289, i64 %length, 1
    %v291 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.i32.nullable, ptr null, i64 1) to i64))
    %v292 = icmp eq ptr %v291, null
    %v293 = and i1 %v292, %nonempty
    br i1 %v293, label %allocationFailed, label %allocated293

allocated293:
    %v294 = call %flowx.return.panic.int4.nullerrorable @flowx.panic.int4.nullerrorable(%flowx.list.primitive.i32.nullerrorable %input2, ptr %v291)
    %v295 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.float.nullable, ptr null, i64 1) to i64))
    %v296 = icmp eq ptr %v295, null
    %v297 = and i1 %v296, %nonempty
    br i1 %v297, label %allocationFailed, label %allocated297

allocated297:
    %v298 = call %flowx.return.panic.float4.nullerrorable @flowx.panic.float4.nullerrorable(%flowx.list.primitive.float.nullerrorable %input3, ptr %v295)
    %v299 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v300 = icmp eq ptr %v299, null
    %v301 = and i1 %v300, %nonempty
    br i1 %v301, label %allocationFailed, label %allocated301

allocated301:
    %v302 = call %flowx.return.errorToValue.int4.errorable.int4 @flowx.errorToValue.int4.errorable.int4(%flowx.list.primitive.i32.errorable %input0, %flowx.list.i32 %v10, ptr %v299)
    %v303 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v304 = icmp eq ptr %v303, null
    %v305 = and i1 %v304, %nonempty
    br i1 %v305, label %allocationFailed, label %allocated305

allocated305:
    %v306 = call %flowx.return.errorToValue.float4.errorable.float4 @flowx.errorToValue.float4.errorable.float4(%flowx.list.primitive.float.errorable %input1, %flowx.list.float %v20, ptr %v303)
    %v307 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.i32.nullable, ptr null, i64 1) to i64))
    %v308 = icmp eq ptr %v307, null
    %v309 = and i1 %v308, %nonempty
    br i1 %v309, label %allocationFailed, label %allocated309

allocated309:
    %v310 = call %flowx.return.errorToValue.int4.nullerrorable.int4 @flowx.errorToValue.int4.nullerrorable.int4(%flowx.list.primitive.i32.nullerrorable %input2, %flowx.list.i32 %v30, ptr %v307)
    %v311 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.float.nullable, ptr null, i64 1) to i64))
    %v312 = icmp eq ptr %v311, null
    %v313 = and i1 %v312, %nonempty
    br i1 %v313, label %allocationFailed, label %allocated313

allocated313:
    %v314 = call %flowx.return.errorToValue.float4.nullerrorable.float4 @flowx.errorToValue.float4.nullerrorable.float4(%flowx.list.primitive.float.nullerrorable %input3, %flowx.list.float %v40, ptr %v311)
    %v315 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.i32.errorable, ptr null, i64 1) to i64))
    %v316 = icmp eq ptr %v315, null
    %v317 = and i1 %v316, %nonempty
    br i1 %v317, label %allocationFailed, label %allocated317

allocated317:
    %v318 = call %flowx.return.nullToValue.int4.nullerrorable.int4 @flowx.nullToValue.int4.nullerrorable.int4(%flowx.list.primitive.i32.nullerrorable %input2, %flowx.list.i32 %v70, ptr %v315)
    %v319 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.float.errorable, ptr null, i64 1) to i64))
    %v320 = icmp eq ptr %v319, null
    %v321 = and i1 %v320, %nonempty
    br i1 %v321, label %allocationFailed, label %allocated321

allocated321:
    %v322 = call %flowx.return.nullToValue.float4.nullerrorable.float4 @flowx.nullToValue.float4.nullerrorable.float4(%flowx.list.primitive.float.nullerrorable %input3, %flowx.list.float %v80, ptr %v319)
    %v323 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v324 = icmp eq ptr %v323, null
    %v325 = and i1 %v324, %nonempty
    br i1 %v325, label %allocationFailed, label %allocated325

allocated325:
    %v326 = call %flowx.return.and.bool.bool @flowx.and.bool.bool(%flowx.list.i1 %v170, %flowx.list.i1 %v180, ptr %v323)
    %v327 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v328 = icmp eq ptr %v327, null
    %v329 = and i1 %v328, %nonempty
    br i1 %v329, label %allocationFailed, label %allocated329

allocated329:
    %v330 = call %flowx.return.less.bool.bool @flowx.less.bool.bool(%flowx.list.i1 %v200, %flowx.list.i1 %v210, ptr %v327)
    %v331 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v332 = icmp eq ptr %v331, null
    %v333 = and i1 %v332, %nonempty
    br i1 %v333, label %allocationFailed, label %allocated333

allocated333:
    %v334 = call %flowx.return.lessEqual.bool.bool @flowx.lessEqual.bool.bool(%flowx.list.i1 %v220, %flowx.list.i1 %v230, ptr %v331)
    %v335 = extractvalue %flowx.return.panic.int4.nullerrorable %v294, 0
    %v336 = extractvalue %flowx.return.panic.float4.nullerrorable %v298, 0
    %v337 = extractvalue %flowx.return.errorToValue.int4.errorable.int4 %v302, 0
    %v338 = extractvalue %flowx.return.errorToValue.float4.errorable.float4 %v306, 0
    %v339 = extractvalue %flowx.return.errorToValue.int4.nullerrorable.int4 %v310, 0
    %v340 = extractvalue %flowx.return.errorToValue.float4.nullerrorable.float4 %v314, 0
    %v341 = extractvalue %flowx.return.nullToValue.int4.nullerrorable.int4 %v318, 0
    %v342 = extractvalue %flowx.return.nullToValue.float4.nullerrorable.float4 %v322, 0
    %v343 = extractvalue %flowx.return.and.bool.bool %v326, 0
    %v344 = extractvalue %flowx.return.less.bool.bool %v330, 0
    %v345 = extractvalue %flowx.return.lessEqual.bool.bool %v334, 0
    %v346 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v347 = icmp eq ptr %v346, null
    %v348 = and i1 %v347, %nonempty
    br i1 %v348, label %allocationFailed, label %allocated348

allocated348:
    %v349 = call %flowx.return.user.combine.int4.int4 @flowx.user.combine.int4.int4(%flowx.list.i32 %v337, %flowx.list.i32 %v110, i64 %length, ptr %v346)
    %v350 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v351 = icmp eq ptr %v350, null
    %v352 = and i1 %v351, %nonempty
    br i1 %v352, label %allocationFailed, label %allocated352

allocated352:
    %v353 = call %flowx.return.add.float4.float4 @flowx.add.float4.float4(%flowx.list.float %v338, %flowx.list.float %v140, ptr %v350)
    %v354 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v355 = icmp eq ptr %v354, null
    %v356 = and i1 %v355, %nonempty
    br i1 %v356, label %allocationFailed, label %allocated356

allocated356:
    %v357 = call %flowx.return.nullToValue.int4.nullable.int4 @flowx.nullToValue.int4.nullable.int4(%flowx.list.primitive.i32.nullable %v339, %flowx.list.i32 %v50, ptr %v354)
    %v358 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v359 = icmp eq ptr %v358, null
    %v360 = and i1 %v359, %nonempty
    br i1 %v360, label %allocationFailed, label %allocated360

allocated360:
    %v361 = call %flowx.return.nullToValue.float4.nullable.float4 @flowx.nullToValue.float4.nullable.float4(%flowx.list.primitive.float.nullable %v340, %flowx.list.float %v60, ptr %v358)
    %v362 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v363 = icmp eq ptr %v362, null
    %v364 = and i1 %v363, %nonempty
    br i1 %v364, label %allocationFailed, label %allocated364

allocated364:
    %v365 = call %flowx.return.errorToValue.int4.errorable.int4 @flowx.errorToValue.int4.errorable.int4(%flowx.list.primitive.i32.errorable %v341, %flowx.list.i32 %v90, ptr %v362)
    %v366 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v367 = icmp eq ptr %v366, null
    %v368 = and i1 %v367, %nonempty
    br i1 %v368, label %allocationFailed, label %allocated368

allocated368:
    %v369 = call %flowx.return.errorToValue.float4.errorable.float4 @flowx.errorToValue.float4.errorable.float4(%flowx.list.primitive.float.errorable %v342, %flowx.list.float %v100, ptr %v366)
    %v370 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v371 = icmp eq ptr %v370, null
    %v372 = and i1 %v371, %nonempty
    br i1 %v372, label %allocationFailed, label %allocated372

allocated372:
    %v373 = call %flowx.return.or.bool.bool @flowx.or.bool.bool(%flowx.list.i1 %v343, %flowx.list.i1 %v190, ptr %v370)
    %v374 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v375 = icmp eq ptr %v374, null
    %v376 = and i1 %v375, %nonempty
    br i1 %v376, label %allocationFailed, label %allocated376

allocated376:
    %v377 = call %flowx.return.equal.bool.bool @flowx.equal.bool.bool(%flowx.list.i1 %v344, %flowx.list.i1 %v345, ptr %v374)
    %v378 = extractvalue %flowx.return.user.combine.int4.int4 %v349, 0
    %v379 = extractvalue %flowx.return.add.float4.float4 %v353, 0
    %v380 = extractvalue %flowx.return.nullToValue.int4.nullable.int4 %v357, 0
    %v381 = extractvalue %flowx.return.nullToValue.float4.nullable.float4 %v361, 0
    %v382 = extractvalue %flowx.return.errorToValue.int4.errorable.int4 %v365, 0
    %v383 = extractvalue %flowx.return.errorToValue.float4.errorable.float4 %v369, 0
    %v384 = extractvalue %flowx.return.or.bool.bool %v373, 0
    %v385 = extractvalue %flowx.return.equal.bool.bool %v377, 0
    %v386 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v387 = icmp eq ptr %v386, null
    %v388 = and i1 %v387, %nonempty
    br i1 %v388, label %allocationFailed, label %allocated388

allocated388:
    %v389 = call %flowx.return.subtract.int4.int4 @flowx.subtract.int4.int4(%flowx.list.i32 %v378, %flowx.list.i32 %v120, ptr %v386)
    %v390 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v391 = icmp eq ptr %v390, null
    %v392 = and i1 %v391, %nonempty
    br i1 %v392, label %allocationFailed, label %allocated392

allocated392:
    %v393 = call %flowx.return.subtract.float4.float4 @flowx.subtract.float4.float4(%flowx.list.float %v379, %flowx.list.float %v150, ptr %v390)
    %v394 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.i32.errorable, ptr null, i64 1) to i64))
    %v395 = icmp eq ptr %v394, null
    %v396 = and i1 %v395, %nonempty
    br i1 %v396, label %allocationFailed, label %allocated396

allocated396:
    %v397 = call %flowx.return.assert.int4.bool @flowx.assert.int4.bool(%flowx.list.i32 %v380, %flowx.list.i1 %v260, ptr %v394)
    %v398 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.float.errorable, ptr null, i64 1) to i64))
    %v399 = icmp eq ptr %v398, null
    %v400 = and i1 %v399, %nonempty
    br i1 %v400, label %allocationFailed, label %allocated400

allocated400:
    %v401 = call %flowx.return.assert.float4.bool @flowx.assert.float4.bool(%flowx.list.float %v381, %flowx.list.i1 %v280, ptr %v398)
    %v402 = extractvalue %flowx.list.i32 %v382, 0
    br label %check403

check403:
    %v404 = phi i64 [ 0, %allocated400 ], [ %v405, %row403 ]
    %v406 = icmp ult i64 %v404, %length
    br i1 %v406, label %row403, label %exit403

row403:
    %v407 = getelementptr i32, ptr %v402, i64 %v404
    %v408 = load i32, ptr %v407
    %v409 = getelementptr i32, ptr %output3, i64 %v404
    store i32 %v408, ptr %v409
    %v405 = add i64 %v404, 1
    br label %check403

exit403:
    %v410 = insertvalue %flowx.list.i32 poison, ptr %output3, 0
    %v411 = insertvalue %flowx.list.i32 %v410, i64 %length, 1
    %v412 = extractvalue %flowx.list.float %v383, 0
    br label %check413

check413:
    %v414 = phi i64 [ 0, %exit403 ], [ %v415, %row413 ]
    %v416 = icmp ult i64 %v414, %length
    br i1 %v416, label %row413, label %exit413

row413:
    %v417 = getelementptr float, ptr %v412, i64 %v414
    %v418 = load float, ptr %v417
    %v419 = getelementptr float, ptr %output4, i64 %v414
    store float %v418, ptr %v419
    %v415 = add i64 %v414, 1
    br label %check413

exit413:
    %v420 = insertvalue %flowx.list.float poison, ptr %output4, 0
    %v421 = insertvalue %flowx.list.float %v420, i64 %length, 1
    %v422 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v423 = icmp eq ptr %v422, null
    %v424 = and i1 %v423, %nonempty
    br i1 %v424, label %allocationFailed, label %allocated424

allocated424:
    %v425 = call %flowx.return.not.bool @flowx.not.bool(%flowx.list.i1 %v384, ptr %v422)
    %v426 = extractvalue %flowx.return.subtract.int4.int4 %v389, 0
    %v427 = extractvalue %flowx.return.subtract.float4.float4 %v393, 0
    %v428 = extractvalue %flowx.return.assert.int4.bool %v397, 0
    %v429 = extractvalue %flowx.return.assert.float4.bool %v401, 0
    %v430 = extractvalue %flowx.return.not.bool %v425, 0
    %v431 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v432 = icmp eq ptr %v431, null
    %v433 = and i1 %v432, %nonempty
    br i1 %v433, label %allocationFailed, label %allocated433

allocated433:
    %v434 = call %flowx.return.multiply.int4.int4 @flowx.multiply.int4.int4(%flowx.list.i32 %v426, %flowx.list.i32 %v130, ptr %v431)
    %v435 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v436 = icmp eq ptr %v435, null
    %v437 = and i1 %v436, %nonempty
    br i1 %v437, label %allocationFailed, label %allocated437

allocated437:
    %v438 = call %flowx.return.multiply.float4.float4 @flowx.multiply.float4.float4(%flowx.list.float %v427, %flowx.list.float %v160, ptr %v435)
    %v439 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v440 = icmp eq ptr %v439, null
    %v441 = and i1 %v440, %nonempty
    br i1 %v441, label %allocationFailed, label %allocated441

allocated441:
    %v442 = call %flowx.return.errorToValue.int4.errorable.int4 @flowx.errorToValue.int4.errorable.int4(%flowx.list.primitive.i32.errorable %v428, %flowx.list.i32 %v270, ptr %v439)
    %v443 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v444 = icmp eq ptr %v443, null
    %v445 = and i1 %v444, %nonempty
    br i1 %v445, label %allocationFailed, label %allocated445

allocated445:
    %v446 = call %flowx.return.errorToValue.float4.errorable.float4 @flowx.errorToValue.float4.errorable.float4(%flowx.list.primitive.float.errorable %v429, %flowx.list.float %v290, ptr %v443)
    %v447 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i1, ptr null, i64 1) to i64))
    %v448 = icmp eq ptr %v447, null
    %v449 = and i1 %v448, %nonempty
    br i1 %v449, label %allocationFailed, label %allocated449

allocated449:
    %v450 = call %flowx.return.or.bool.bool @flowx.or.bool.bool(%flowx.list.i1 %v430, %flowx.list.i1 %v385, ptr %v447)
    %v451 = extractvalue %flowx.return.multiply.int4.int4 %v434, 0
    %v452 = extractvalue %flowx.return.multiply.float4.float4 %v438, 0
    %v453 = extractvalue %flowx.return.errorToValue.int4.errorable.int4 %v442, 0
    %v454 = extractvalue %flowx.return.errorToValue.float4.errorable.float4 %v446, 0
    %v455 = extractvalue %flowx.return.or.bool.bool %v450, 0
    %v456 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.i32.errorable, ptr null, i64 1) to i64))
    %v457 = icmp eq ptr %v456, null
    %v458 = and i1 %v457, %nonempty
    br i1 %v458, label %allocationFailed, label %allocated458

allocated458:
    %v459 = call %flowx.return.assert.int4.bool @flowx.assert.int4.bool(%flowx.list.i32 %v451, %flowx.list.i1 %v240, ptr %v456)
    %v460 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.primitive.float.errorable, ptr null, i64 1) to i64))
    %v461 = icmp eq ptr %v460, null
    %v462 = and i1 %v461, %nonempty
    br i1 %v462, label %allocationFailed, label %allocated462

allocated462:
    %v463 = call %flowx.return.assert.float4.bool @flowx.assert.float4.bool(%flowx.list.float %v452, %flowx.list.i1 %v250, ptr %v460)
    %v464 = extractvalue %flowx.list.i1 %v455, 0
    br label %check465

check465:
    %v466 = phi i64 [ 0, %allocated462 ], [ %v467, %row465 ]
    %v468 = icmp ult i64 %v466, %length
    br i1 %v468, label %row465, label %exit465

row465:
    %v469 = getelementptr i1, ptr %v464, i64 %v466
    %v470 = load i1, ptr %v469
    %v471 = getelementptr i1, ptr %output2, i64 %v466
    store i1 %v470, ptr %v471
    %v467 = add i64 %v466, 1
    br label %check465

exit465:
    %v472 = insertvalue %flowx.list.i1 poison, ptr %output2, 0
    %v473 = insertvalue %flowx.list.i1 %v472, i64 %length, 1
    %v474 = extractvalue %flowx.return.assert.int4.bool %v459, 0
    %v475 = extractvalue %flowx.return.assert.float4.bool %v463, 0
    %v476 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v477 = icmp eq ptr %v476, null
    %v478 = and i1 %v477, %nonempty
    br i1 %v478, label %allocationFailed, label %allocated478

allocated478:
    %v479 = call %flowx.return.panic.int4.errorable @flowx.panic.int4.errorable(%flowx.list.primitive.i32.errorable %v474, ptr %v476)
    %v480 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v481 = icmp eq ptr %v480, null
    %v482 = and i1 %v481, %nonempty
    br i1 %v482, label %allocationFailed, label %allocated482

allocated482:
    %v483 = call %flowx.return.panic.float4.errorable @flowx.panic.float4.errorable(%flowx.list.primitive.float.errorable %v475, ptr %v480)
    %v484 = extractvalue %flowx.return.panic.int4.errorable %v479, 0
    %v485 = extractvalue %flowx.return.panic.float4.errorable %v483, 0
    %v486 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (%flowx.struct.Pair, ptr null, i64 1) to i64))
    %v487 = icmp eq ptr %v486, null
    %v488 = and i1 %v487, %nonempty
    br i1 %v488, label %allocationFailed, label %allocated488

allocated488:
    %v489 = extractvalue %flowx.list.i32 %v484, 0
    %v490 = extractvalue %flowx.list.float %v485, 0
    br label %check491

check491:
    %v492 = phi i64 [ 0, %allocated488 ], [ %v493, %row491 ]
    %v494 = icmp ult i64 %v492, %length
    br i1 %v494, label %row491, label %exit491

row491:
    %v495 = getelementptr i32, ptr %v489, i64 %v492
    %v496 = load i32, ptr %v495
    %v497 = getelementptr float, ptr %v490, i64 %v492
    %v498 = load float, ptr %v497
    %v499 = insertvalue %flowx.struct.Pair poison, i32 %v496, 0
    %v500 = insertvalue %flowx.struct.Pair %v499, float %v498, 1
    %v501 = getelementptr %flowx.struct.Pair, ptr %v486, i64 %v492
    store %flowx.struct.Pair %v500, ptr %v501
    %v493 = add i64 %v492, 1
    br label %check491

exit491:
    %v502 = insertvalue %flowx.list.struct.Pair poison, ptr %v486, 0
    %v503 = insertvalue %flowx.list.struct.Pair %v502, i64 %length, 1
    %v504 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v505 = icmp eq ptr %v504, null
    %v506 = and i1 %v505, %nonempty
    br i1 %v506, label %allocationFailed, label %allocated506

allocated506:
    %v507 = extractvalue %flowx.list.struct.Pair %v503, 0
    br label %check508

check508:
    %v509 = phi i64 [ 0, %allocated506 ], [ %v510, %row508 ]
    %v511 = icmp ult i64 %v509, %length
    br i1 %v511, label %row508, label %exit508

row508:
    %v512 = getelementptr %flowx.struct.Pair, ptr %v507, i64 %v509
    %v513 = load %flowx.struct.Pair, ptr %v512
    %v514 = extractvalue %flowx.struct.Pair %v513, 0
    %v515 = getelementptr i32, ptr %v504, i64 %v509
    store i32 %v514, ptr %v515
    %v510 = add i64 %v509, 1
    br label %check508

exit508:
    %v516 = insertvalue %flowx.list.i32 poison, ptr %v504, 0
    %v517 = insertvalue %flowx.list.i32 %v516, i64 %length, 1
    %v518 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (float, ptr null, i64 1) to i64))
    %v519 = icmp eq ptr %v518, null
    %v520 = and i1 %v519, %nonempty
    br i1 %v520, label %allocationFailed, label %allocated520

allocated520:
    %v521 = extractvalue %flowx.list.struct.Pair %v503, 0
    br label %check522

check522:
    %v523 = phi i64 [ 0, %allocated520 ], [ %v524, %row522 ]
    %v525 = icmp ult i64 %v523, %length
    br i1 %v525, label %row522, label %exit522

row522:
    %v526 = getelementptr %flowx.struct.Pair, ptr %v521, i64 %v523
    %v527 = load %flowx.struct.Pair, ptr %v526
    %v528 = extractvalue %flowx.struct.Pair %v527, 1
    %v529 = getelementptr float, ptr %v518, i64 %v523
    store float %v528, ptr %v529
    %v524 = add i64 %v523, 1
    br label %check522

exit522:
    %v530 = insertvalue %flowx.list.float poison, ptr %v518, 0
    %v531 = insertvalue %flowx.list.float %v530, i64 %length, 1
    %v532 = extractvalue %flowx.list.i32 %v517, 0
    br label %check533

check533:
    %v534 = phi i64 [ 0, %exit522 ], [ %v535, %row533 ]
    %v536 = icmp ult i64 %v534, %length
    br i1 %v536, label %row533, label %exit533

row533:
    %v537 = getelementptr i32, ptr %v532, i64 %v534
    %v538 = load i32, ptr %v537
    %v539 = getelementptr i32, ptr %output0, i64 %v534
    store i32 %v538, ptr %v539
    %v535 = add i64 %v534, 1
    br label %check533

exit533:
    %v540 = insertvalue %flowx.list.i32 poison, ptr %output0, 0
    %v541 = insertvalue %flowx.list.i32 %v540, i64 %length, 1
    %v542 = extractvalue %flowx.list.float %v531, 0
    br label %check543

check543:
    %v544 = phi i64 [ 0, %exit533 ], [ %v545, %row543 ]
    %v546 = icmp ult i64 %v544, %length
    br i1 %v546, label %row543, label %exit543

row543:
    %v547 = getelementptr float, ptr %v542, i64 %v544
    %v548 = load float, ptr %v547
    %v549 = getelementptr float, ptr %output1, i64 %v544
    store float %v548, ptr %v549
    %v545 = add i64 %v544, 1
    br label %check543

exit543:
    %v550 = insertvalue %flowx.list.float poison, ptr %output1, 0
    %v551 = insertvalue %flowx.list.float %v550, i64 %length, 1
    call void @free(ptr %v1)
    call void @free(ptr %v11)
    call void @free(ptr %v21)
    call void @free(ptr %v31)
    call void @free(ptr %v41)
    call void @free(ptr %v51)
    call void @free(ptr %v61)
    call void @free(ptr %v71)
    call void @free(ptr %v81)
    call void @free(ptr %v91)
    call void @free(ptr %v101)
    call void @free(ptr %v111)
    call void @free(ptr %v121)
    call void @free(ptr %v131)
    call void @free(ptr %v141)
    call void @free(ptr %v151)
    call void @free(ptr %v161)
    call void @free(ptr %v171)
    call void @free(ptr %v181)
    call void @free(ptr %v191)
    call void @free(ptr %v201)
    call void @free(ptr %v211)
    call void @free(ptr %v221)
    call void @free(ptr %v231)
    call void @free(ptr %v241)
    call void @free(ptr %v251)
    call void @free(ptr %v261)
    call void @free(ptr %v271)
    call void @free(ptr %v281)
    call void @free(ptr %v291)
    call void @free(ptr %v295)
    call void @free(ptr %v299)
    call void @free(ptr %v303)
    call void @free(ptr %v307)
    call void @free(ptr %v311)
    call void @free(ptr %v315)
    call void @free(ptr %v319)
    call void @free(ptr %v323)
    call void @free(ptr %v327)
    call void @free(ptr %v331)
    call void @free(ptr %v346)
    call void @free(ptr %v350)
    call void @free(ptr %v354)
    call void @free(ptr %v358)
    call void @free(ptr %v362)
    call void @free(ptr %v366)
    call void @free(ptr %v370)
    call void @free(ptr %v374)
    call void @free(ptr %v386)
    call void @free(ptr %v390)
    call void @free(ptr %v394)
    call void @free(ptr %v398)
    call void @free(ptr %v422)
    call void @free(ptr %v431)
    call void @free(ptr %v435)
    call void @free(ptr %v439)
    call void @free(ptr %v443)
    call void @free(ptr %v447)
    call void @free(ptr %v456)
    call void @free(ptr %v460)
    call void @free(ptr %v476)
    call void @free(ptr %v480)
    call void @free(ptr %v486)
    call void @free(ptr %v504)
    call void @free(ptr %v518)
    %v552 = insertvalue %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable poison, %flowx.list.i32 %v541, 0
    %v553 = insertvalue %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable %v552, %flowx.list.float %v551, 1
    %v554 = insertvalue %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable %v553, %flowx.list.i1 %v473, 2
    %v555 = insertvalue %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable %v554, %flowx.list.i32 %v411, 3
    %v556 = insertvalue %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable %v555, %flowx.list.float %v421, 4
    ret %flowx.return.user.main.int4.errorable.float4.errorable.int4.nullerrorable.float4.nullerrorable %v556

allocationFailed:
    call void @llvm.trap()
    unreachable
}

define %flowx.return.user.combine.int4.int4 @flowx.user.combine.int4.int4(%flowx.list.i32 %input0, %flowx.list.i32 %input1, i64 %length, ptr %output0) {
entry:
    %nonempty = icmp ne i64 %length, 0
    %v1 = call ptr @calloc(i64 %length, i64 ptrtoint (ptr getelementptr (i32, ptr null, i64 1) to i64))
    %v2 = icmp eq ptr %v1, null
    %v3 = and i1 %v2, %nonempty
    br i1 %v3, label %allocationFailed, label %allocated3

allocated3:
    %v4 = call %flowx.return.add.int4.int4 @flowx.add.int4.int4(%flowx.list.i32 %input0, %flowx.list.i32 %input1, ptr %v1)
    %v5 = extractvalue %flowx.return.add.int4.int4 %v4, 0
    %v6 = extractvalue %flowx.list.i32 %v5, 0
    br label %check7

check7:
    %v8 = phi i64 [ 0, %allocated3 ], [ %v9, %row7 ]
    %v10 = icmp ult i64 %v8, %length
    br i1 %v10, label %row7, label %exit7

row7:
    %v11 = getelementptr i32, ptr %v6, i64 %v8
    %v12 = load i32, ptr %v11
    %v13 = getelementptr i32, ptr %output0, i64 %v8
    store i32 %v12, ptr %v13
    %v9 = add i64 %v8, 1
    br label %check7

exit7:
    %v14 = insertvalue %flowx.list.i32 poison, ptr %output0, 0
    %v15 = insertvalue %flowx.list.i32 %v14, i64 %length, 1
    call void @free(ptr %v1)
    %v16 = insertvalue %flowx.return.user.combine.int4.int4 poison, %flowx.list.i32 %v15, 0
    ret %flowx.return.user.combine.int4.int4 %v16

allocationFailed:
    call void @llvm.trap()
    unreachable
}
