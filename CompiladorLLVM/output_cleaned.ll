; ModuleID = 'my_module'
source_filename = "my_module"
@strtmp = private unnamed_addr constant [36 x i8] c"acaraje poder crocodilo felicidade \00", align 1
@fmt = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
define i32 @main() {
entry:
%jovem = alloca i8*, align 8
store i8* getelementptr inbounds ([36 x i8], [36 x i8]* @strtmp, i32 0, i32 0), i8** %jovem, align 8
%adulto = alloca i32, align 4
store i32 2, i32* %adulto, align 4
%jovem1 = load i8*, i8** %jovem, align 8
%adulto2 = load i32, i32* %adulto, align 4
%addtmp = add i8* %jovem1, i32 %adulto2
%printf_call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @fmt, i32 0, i32 0), i8* %addtmp)
ret i32 0
}
declare i32 @printf(i8*, ...)
