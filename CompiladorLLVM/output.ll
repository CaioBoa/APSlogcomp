; ModuleID = 'my_module'
source_filename = "my_module"

@strtmp = private unnamed_addr constant [6 x i8] c"gato \00", align 1
@strtmp.1 = private unnamed_addr constant [10 x i8] c"cachorro \00", align 1
@fmt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
  %x = alloca i8*, align 8
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @strtmp, i32 0, i32 0), i8** %x, align 8
  %y = alloca i8*, align 8
  store i8* getelementptr inbounds ([10 x i8], [10 x i8]* @strtmp.1, i32 0, i32 0), i8** %y, align 8
  %x1 = load i8*, i8** %x, align 8
  %y2 = load i8*, i8** %y, align 8
  %cmptmp = icmp eq i8* %x1, %y2
  %bool_ext = zext i1 %cmptmp to i32
  %printf_call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @fmt, i32 0, i32 0), i32 %bool_ext)
  ret i32 0
}

declare i32 @printf(i8*, ...)

